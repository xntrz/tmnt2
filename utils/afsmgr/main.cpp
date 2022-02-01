#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <Windows.h>

#include "afs.hpp"


static void afs_msg_endpoint(const char* msg);
static void cmd_setup_flags(int& argc, char** argv);
static void cmd_new(int argc, char** argv);
static void cmd_list(int argc, char** argv);
static void cmd_extract(int argc, char** argv);
static void cmd_help(int argc, char** argv);


struct TOOLCMD
{
	const char* m_pszCmdName;
	const char* m_pszCmdArgs;
	const char* m_pszCmdDescription;
	void(*m_pfnCmdHandler)(int argc, char** argv);
};


struct TOOLFLAG
{
	const char* m_pszFlag;
	const char* m_pszFlagDescription;
};


static const TOOLCMD s_aToolCmd[] =
{
	{ "help", 		"", 									"Shows help information", 		cmd_help 	},
	{ "new",  		"<dirpath> [-flag]", 					"Create and pack AFS archive", 	cmd_new 	},
	{ "extract", 	"<afspath> <fileid> <dirpath> [-flag]", "Extract from AFS archive", 	cmd_extract },
	{ "list", 		"<afspath> [-flag]", 					"List AFS archive files", 		cmd_list	},
};


static const TOOLFLAG s_aToolFlag[] =
{
	{ "-sct <integer>", 							"Set afs sector alignmnent. By default its 2048" 									},
	{ "-recursive <boolean>", 						"Set flag should explore subfolder while reading or writing" 						},
	{ "-exclude <string> (delimited with \';\')", 	"Set string with formats that should be excluded while reading or writing archive" 	},
};


static void afs_msg_endpoint(afs_note_type type, const char* msg)
{
	printf("%s\n", msg);
};


static void cmd_setup_flags(int& argc, char** argv)
{
	int iHandled = 0;
	
	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp(argv[i], "sct"))
		{
			afs_opt_align(std::atoi(argv[i + 1]));
			++iHandled;
			argv[i]  = nullptr;
		}
		else if (!strcmp(argv[i], "recursive"))
		{
			afs_opt_recursive(std::atoi(argv[i + 1]) > 0);
			++iHandled;
			argv[i] = nullptr;
		}
		else if (!strcmp(argv[i], "exclude"))
		{
			if (argv[i + 1])
			{
				afs_opt_excludemask(argv[i + 1]);
				++iHandled;
				argv[i] = nullptr;
			};
		};
	};

	argc -= iHandled;
};


static void cmd_new(int argc, char** argv)
{
	assert(argv[0]);
	
	void* hAfs = afs_new(argv[0]);
	if (hAfs)
		afs_close(hAfs);
};


static void cmd_list(int argc, char** argv)
{
	assert(argv[0]);

	void* hAfs = afs_open(argv[0]);
	if (hAfs)
	{
		printf("Id\tFile name\n");
		printf("-------------------\n");
		auto fnEnumCallback = [](int idx, const char* name, unsigned size, unsigned offset) -> void
		{
			printf("%d)\t%s\n", idx, name);
		};

		afs_enum(hAfs, fnEnumCallback);
		afs_close(hAfs);
	};
};


static void cmd_extract(int argc, char** argv)
{
	assert(argv[ 0 ]);

	const char* pszArchivePath = argv[0];
	const char* pszFilePath = argv[2];
	int iFileID = std::atoi(argv[1]);
	
	void* hAfs = afs_open(pszArchivePath);
	if (hAfs)
	{
		afs_extract(hAfs, pszFilePath, iFileID);
		afs_close(hAfs);
	};
};


static void cmd_help(int argc, char** argv)
{
	printf(" AFS archive tools. Allows to extract files from AFS archive or create new one\n");
	printf(" Supported commands:\n");
	for (int i = 0; i < sizeof(s_aToolCmd) / sizeof(s_aToolCmd[0]); ++i)
	{
		if (!strcmp(s_aToolCmd[i].m_pszCmdName, "help"))
			continue;

		printf("\t%s %s\n\t\t%s\n\n", 
			s_aToolCmd[i].m_pszCmdName, 
			s_aToolCmd[i].m_pszCmdArgs, 
			s_aToolCmd[i].m_pszCmdDescription
		);
	};
};


void toolcmd_execute(int argc, char** argv)
{
	void(*CmdHandler)(int argc, char** argv) = nullptr;
	const char* CmdName = nullptr;

	if (argc <= 1)
	{
		CmdName = "help";
		++argc;
	}
	else
	{
		CmdName = argv[1];
	};

	for (int i = 0; i < sizeof(s_aToolCmd) / sizeof(s_aToolCmd[ 0 ]); ++i)
	{
		if (!strcmp(s_aToolCmd[i].m_pszCmdName, CmdName))
		{
			CmdHandler = s_aToolCmd[i].m_pfnCmdHandler;
			break;
		};
	};

	argc -= 2;
	argv += 2;
	
	if (CmdHandler)
	{
		afs_opt_note(afs_msg_endpoint);
		cmd_setup_flags(argc, argv);
		CmdHandler(argc, argv);
	};
};


void setup_path(void)
{
	char szModulePath[MAX_PATH];
	szModulePath[0] = '\0';
	GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath));
	char* result = strrchr(szModulePath, '\\');
	if (result)
		*result = '\0';
	SetCurrentDirectoryA(szModulePath);
};


int main(int argc, char** argv)
{
	setup_path();
	toolcmd_execute(argc, argv);
	return 0;
};