#include "PCRwFileSystem.hpp"

#include "../PCTypedefs.hpp"

#include "rtfsystwin32.h"


bool CPCRwFileSystem::Initialize(void)
{
    char szDrive[MAX_PATH];
    szDrive[0] = '\0';

    char szDirectory[MAX_PATH];
    szDirectory[0] = '\0';

    GetModulePathSplit(szDrive, szDirectory, nullptr, nullptr);
    
    RtFileSystem* pWinFS = RtWinFSystemInit(4, szDrive, "PC");
	if (!pWinFS)
		return false;

	return (RtFSManagerRegister(pWinFS) > 0);
};


void CPCRwFileSystem::Terminate(void)
{
    ;
};