#include "afs.hpp"

#pragma warning(disable : 4200)
#pragma warning(disable : 4996)

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <Windows.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <exception>


#define ALIGN_U(value, align)      (unsigned)(unsigned(value) + (align - 1) & ~(align - 1))
#define ALIGN_D(value, align)      (unsigned)(unsigned(value) & ~(align - 1))
#define ADJUST_U(value, align)     (unsigned)(align - (unsigned(value) & (align - 1)))
#define ADJUST_D(value, align)     (unsigned)(unsigned(value) & (align - 1))
#define IS_ALIGNED(value, align)   ((unsigned(value) & (align - 1)) == 0)


struct AFS_FDATA
{
    unsigned m_offset;
    unsigned m_size;
};


struct AFS_FINFO
{
    static const unsigned NAME_MAX = 32;
    
    char m_szName[NAME_MAX];
    unsigned short m_nYear;
    unsigned short m_nMonth;
    unsigned short m_nDay;
    unsigned short m_nHour;
    unsigned short m_nMinute;
    unsigned short m_nSecond;
    unsigned unk0;
};


struct AFS_HDR
{
    static const unsigned MAGIC = '\0SFA';
    static const unsigned DEFAULT_ALIGN = 2048;

    unsigned m_Magic;
    unsigned m_uFileNum;
};


struct AFS
{
    AFS_HDR m_header;
    AFS_FDATA* m_pFileData;
    AFS_FINFO* m_pFileInfo;
	FILE* m_hFile;
};


namespace PARAMETER
{
	const char EXCLUDE_DELIMITER = ';';
    bool RECURSIVE = true;
    std::vector<std::string> EXCLUDE;
    unsigned ALIGN = 2048;
    afs_note_callback NOTE_CALLBACK = nullptr;
};


class CBuffer
{
public:
    CBuffer(void)
    : m_pBuffer(nullptr)
    , m_uBufferSize(0)
    {
        ;
    };


    ~CBuffer(void)
    {
        if (m_pBuffer)
        {
            delete [] m_pBuffer;
            m_pBuffer = nullptr;
        };
    };
    

    char* Alloc(unsigned uBufferSize)
    {
        if (uBufferSize > m_uBufferSize)
        {
            delete [] m_pBuffer;
            m_pBuffer = nullptr;
        };

        m_uBufferSize = uBufferSize;
        m_pBuffer = new char[m_uBufferSize];
        assert(m_pBuffer);
        return m_pBuffer;
    };
    

    char* Pointer(void) const
    {
        return m_pBuffer;
    };


    unsigned Size(void) const
    {
        return m_uBufferSize;
    };

    
private:
    char* m_pBuffer;
    unsigned m_uBufferSize;
};


static void adxf_note(const char* fn, int line, afs_note_type type, const char* format, ...)
{
    if (!PARAMETER::NOTE_CALLBACK)
        return;
    
    char buff[4096];
    buff[0] = '\0';

    int writeoffset = sprintf(buff, "%s(%d): ", fn, line);
    if (writeoffset)
    {
        va_list vl;
        va_start(vl, format);
        vsprintf_s(&buff[ writeoffset ], sizeof(buff) - writeoffset, format, vl);
        va_end(vl);        
    };

    PARAMETER::NOTE_CALLBACK(type, buff);
};


#define ADXF_NOTE(type, format, ...) adxf_note(__FUNCTION__, __LINE__, type, format, ##__VA_ARGS__);


static void adxf_clearname(std::string& str, const char* charsMask)
{
    for (unsigned long i = 0; i < std::strlen(charsMask); i++)
        str.erase(std::remove(str.begin(), str.end(), charsMask[ i ]), str.end());
};


static bool adxf_check_attribute(const std::string& path, DWORD attribute)
{
    DWORD dwAttributes = 0;

    if ((dwAttributes = GetFileAttributesA(path.c_str())) == INVALID_FILE_ATTRIBUTES)
        dwAttributes = 0;

    return ((dwAttributes & attribute) == attribute);
};


static bool adxf_is_directory(const std::string& path)
{
    return adxf_check_attribute(path, FILE_ATTRIBUTE_DIRECTORY);
};


static bool adxf_is_excluded(const std::string& path)
{
    std::size_t posBegin = path.find_last_of('.');
    if (posBegin == std::string::npos)
        return false;
    
    std::string FileFormat = path.substr(++posBegin);
    if (FileFormat.empty())
        return false;
    
    for (const auto& it : PARAMETER::EXCLUDE)
    {
        if (it == FileFormat)
            return true;
    };

    return false;
};


static std::vector<std::string> adxf_count_files(const std::string& path)
{
    std::vector<std::string> result;
    std::string FindPath;
    WIN32_FIND_DATAA FindData = { 0 };

    FindPath += path;
    FindPath += "\\*";
    
    HANDLE hFind = FindFirstFileA(FindPath.c_str(), &FindData);
    
    if (hFind)
    {
        do
        {
            if (FindData.cFileName[0] == '.')
                continue;

            std::string FilePath = path + "\\" + FindData.cFileName;
            if (adxf_is_excluded(FilePath))
                continue;

            if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (PARAMETER::RECURSIVE)
                {
                    std::vector<std::string> subResult = adxf_count_files(FilePath);
                    if (!subResult.empty())
                        result.insert(result.end(), subResult.begin(), subResult.end());
                };
            }
            else
            {
                result.push_back(FilePath);
            };
        } while (FindNextFileA(hFind, &FindData));
    };

    FindClose(hFind);
    hFind = NULL;

    return result;
};


static void adxf_write_data(FILE* File, const void* data, unsigned datalen)
{
    assert(File);
    
    if (!fwrite(data, datalen, 1, File))
        throw std::exception("Failed to write data to file");
};


static void adxf_read_data(FILE* File, void* buffer, unsigned bufferlen)
{
    assert(File);

    if (!fread(buffer, bufferlen, 1, File))
        throw std::exception("Failed to read data from file");
};


static void adxf_alignfptr(FILE* File)
{
    assert(File);
    
    int fptr = ftell(File);
    int adjust = ADJUST_U(fptr, PARAMETER::ALIGN);
    fseek(File, adjust, SEEK_CUR);
};


static void adxf_setfptr(FILE* File, int pos)
{
    assert(File);
    
    fseek(File, pos, SEEK_SET);
};


static int adxf_getfptr(FILE* File)
{
    assert(File);
    
    return ftell(File);
};


static bool adxf_write_recursive(AFS* afs, const std::string& path)
{
    bool bResult = false;
    
    std::vector<std::string> FileList = adxf_count_files(path);
    if (FileList.empty())
        return bResult;

    CBuffer InputFileBuffer;    
    FILE* InputFile = nullptr;
    FILE* OutputFile = nullptr;

    try
    {
        //
        //  Prepeare main afs structures
        //
		afs->m_header.m_Magic = AFS_HDR::MAGIC;
        afs->m_header.m_uFileNum = FileList.size();
        afs->m_pFileData = new AFS_FDATA[FileList.size() + 1]();
        afs->m_pFileInfo = new AFS_FINFO[FileList.size()]();
        
        if (!afs->m_pFileData)
            throw std::exception("Failed to allcoate file data");

        if (!afs->m_pFileInfo)
            throw std::exception("Failed to allocate file info");

        //
        //  Create output file - afs archive
        //
        std::string OutputFilePath;
        OutputFilePath += path;
        OutputFilePath += ".dat";
        OutputFile = fopen(OutputFilePath.c_str(), "wb");
        if (!OutputFile)
            throw std::exception("Failed to open output file for writing");

        afs->m_hFile = OutputFile;
        
        //
        //  Write main header and blank file data array for align
        //
        adxf_write_data(OutputFile, &afs->m_header, sizeof(afs->m_header));
        adxf_write_data(OutputFile, afs->m_pFileData, sizeof(afs->m_pFileData[ 0 ]) * (FileList.size() + 1));
        adxf_alignfptr(OutputFile);            

        //
        //  Iterate over all files and write each to AFS archive
        //
        int i = 0;
        for (const std::string& it : FileList)
        {
            //
            //  Open an input file
            //
            InputFile = fopen(it.c_str(), "rb");
            if (!InputFile)
                throw std::exception("Failed to open input filed for reading");

            //
            //  Getting input file size
            //
            fseek(InputFile, 0, SEEK_END);
            int InputFileSize = ftell(InputFile);
            fseek(InputFile, 0, SEEK_SET);

            //
            //  Realloc buffer for input file if required
            //
            if (!InputFileBuffer.Alloc(InputFileSize))
                throw std::exception("Failed to alloc io buffer for input file");

            //
            //  Initialize current file data
            //
            afs->m_pFileData[ i ].m_offset = adxf_getfptr(OutputFile);
            afs->m_pFileData[ i ].m_size = InputFileSize;

            //
            //  Initialize current file info
            //
            std::size_t posBegin = it.find_last_of('\\');
            if (posBegin == std::string::npos)
                throw std::exception("Failed to extract filename");

            std::string FileName = it.substr(++posBegin);
            assert(!FileName.empty());
            assert(FileName.size() < sizeof(AFS_FINFO::m_szName));
            
            strcpy(afs->m_pFileInfo[ i ].m_szName, &FileName[ 0 ]);
            
            SYSTEMTIME SystemTime = { 0 };
            GetLocalTime(&SystemTime);
            
            afs->m_pFileInfo[ i ].m_nYear   = SystemTime.wYear;
            afs->m_pFileInfo[ i ].m_nMonth  = SystemTime.wMonth;
            afs->m_pFileInfo[ i ].m_nDay    = SystemTime.wDay;
            afs->m_pFileInfo[ i ].m_nHour   = SystemTime.wHour;
            afs->m_pFileInfo[ i ].m_nMinute = SystemTime.wMinute;
            afs->m_pFileInfo[ i ].m_nSecond = SystemTime.wSecond;

            //
            //  Copy input file to output file and align output file ptr by sector alignment
            //
            adxf_read_data(InputFile, InputFileBuffer.Pointer(), InputFileBuffer.Size());
            adxf_write_data(OutputFile, InputFileBuffer.Pointer(), InputFileBuffer.Size());
            adxf_alignfptr(OutputFile);

            //
            //  Close input file
            //
            fclose(InputFile);
            InputFile = nullptr;

			++i;
        };

        //
        //  Write data for file info table
        //
        afs->m_pFileData[FileList.size()].m_size = sizeof(afs->m_pFileInfo[0]) * FileList.size();
        afs->m_pFileData[FileList.size()].m_offset = adxf_getfptr(OutputFile);
        
        //
        //  Rewrite blank file data arary with valid data
        //
        adxf_setfptr(OutputFile, sizeof(afs->m_header));
        adxf_write_data(OutputFile, afs->m_pFileData, sizeof(afs->m_pFileData[0]) * (FileList.size() + 1));

        //
        //  Write file info table
        //
        int FileInfoTableSize = sizeof(afs->m_pFileInfo[ 0 ]) * FileList.size();
        adxf_setfptr(OutputFile, afs->m_pFileData[ FileList.size() ].m_offset);
        adxf_write_data(OutputFile, afs->m_pFileInfo, FileInfoTableSize);

		std::vector<char> padding;
		padding.resize(PARAMETER::ALIGN);
        adxf_write_data(OutputFile, &padding[ 0 ], ADJUST_U(FileInfoTableSize, PARAMETER::ALIGN));

        afs->m_hFile = OutputFile;

        bResult = true;
    }
    catch (std::exception& e)
    {
        ADXF_NOTE(afs_note_error, e.what());
    };

    if (InputFile)
    {
        fclose(InputFile);
        InputFile = nullptr;
    };

    return bResult;
};


static bool adxf_read_recursive(AFS* afs, const std::string& path)
{
    assert(afs->m_header.m_Magic != AFS_HDR::MAGIC);
    assert(!afs->m_pFileData);
    assert(!afs->m_pFileInfo);
    
    bool bResult = false;
    FILE* InputFile = nullptr;
    CBuffer InputFileBuffer;

    try
    {
        FILE* InputFile = fopen(path.c_str(), "rb");
        if (!InputFile)
            throw std::exception("Failed to open input file for reading");

        afs->m_hFile = InputFile;

        adxf_read_data(InputFile, &afs->m_header, sizeof(afs->m_header));

        if (afs->m_header.m_Magic != AFS_HDR::MAGIC)
            throw std::exception("AFS signature mismatch");

        afs->m_pFileData = new AFS_FDATA[ afs->m_header.m_uFileNum + 1 ]();
        afs->m_pFileInfo = new AFS_FINFO[afs->m_header.m_uFileNum]();

        if (!afs->m_pFileData)
            throw std::exception("Failed to allcoate file data");

        if (!afs->m_pFileInfo)
            throw std::exception("Failed to allocate file info");

        adxf_read_data(InputFile, afs->m_pFileData, sizeof(afs->m_pFileData[0]) * (afs->m_header.m_uFileNum + 1));
        adxf_setfptr(InputFile, afs->m_pFileData[ afs->m_header.m_uFileNum ].m_offset);
        adxf_read_data(InputFile, afs->m_pFileInfo, sizeof(afs->m_pFileInfo[0]) * (afs->m_header.m_uFileNum));

        bResult = true;
    }
    catch (std::exception& e)
    {
        ADXF_NOTE(afs_note_error, e.what());
    };
    
	return bResult;
};


static void* afs_create_common(const char* path, bool bRead = false)
{
    bool(*adxf_init_fn)(AFS* afs, const std::string& path);

    if (bRead)
        adxf_init_fn = adxf_read_recursive;
    else
        adxf_init_fn = adxf_write_recursive;

    AFS* afs = new AFS();
    if (!adxf_init_fn(afs, path))
    {
        if (afs->m_hFile)
        {
            fclose(afs->m_hFile);
            afs->m_hFile = nullptr;
        };

        if (afs->m_pFileInfo)
        {
            delete [] afs->m_pFileInfo;
            afs->m_pFileInfo = nullptr;
        };

        if (afs->m_pFileData)
        {
            delete [] afs->m_pFileData;
            afs->m_pFileData = nullptr;
        };

        delete afs;
        afs = nullptr;
    };

    return afs;
};


void afs_opt_excludemask(const char* mask)
{
    std::string result;
    std::stringstream stream(mask);
    while (std::getline(stream, result, PARAMETER::EXCLUDE_DELIMITER))
        PARAMETER::EXCLUDE.push_back(result);    
};


void afs_opt_align(unsigned alignment)
{
    PARAMETER::ALIGN = alignment;
};


void afs_opt_recursive(bool value)
{
    PARAMETER::RECURSIVE = value;
};


void afs_opt_note(afs_note_callback cb)
{
    PARAMETER::NOTE_CALLBACK = cb;
};


void* afs_new(const char* path)
{
    return afs_create_common(path, false);
};


void* afs_open(const char* path)
{
    return afs_create_common(path, true);
};


void afs_close(void* handle)
{
    AFS* afs = (AFS*)handle;
    assert(afs);
    
    if (afs->m_hFile)
    {
        fclose(afs->m_hFile);
        afs->m_hFile = nullptr;
    };

    if (afs->m_pFileInfo)
    {
        delete [] afs->m_pFileInfo;
        afs->m_pFileInfo = nullptr;
    };

    if (afs->m_pFileData)
    {
        delete [] afs->m_pFileData;
        afs->m_pFileData = nullptr;
    };

	delete afs;
};


bool afs_read(void* handle, int idx, void* buffer, unsigned bufferSize)
{
	AFS* afs = (AFS*)handle;
	assert(afs);
	assert(afs->m_pFileData);
	assert(afs->m_pFileInfo);
	assert(idx >= 0 && idx < afs->m_header.m_uFileNum);
    assert(afs->m_hFile);
    assert(bufferSize >= afs->m_pFileData[idx].m_size);

    bool bResult = false;
    
    try
    {
		adxf_setfptr(afs->m_hFile, afs->m_pFileData[idx].m_offset);
        adxf_read_data(afs->m_hFile, buffer, bufferSize);
        bResult = true;
    }
    catch(std::exception& e)
    {
        ADXF_NOTE(afs_note_error, e.what());
    }
        
    return bResult;
};


unsigned afs_fsize(void* handle, int idx)
{
	AFS* afs = (AFS*)handle;
	assert(afs);
	assert(afs->m_pFileData);
	assert(afs->m_pFileInfo);
    assert(idx >= 0 && idx < afs->m_header.m_uFileNum);
    
	return afs->m_pFileData[idx].m_size;
};


void afs_enum(void* handle, afs_enum_callback cb)
{
    AFS* afs = (AFS*)handle;
    assert(afs);
    assert(afs->m_pFileData);
    assert(afs->m_pFileInfo);

    for (unsigned i = 0; i < afs->m_header.m_uFileNum; ++i)
    {
        cb(
            int(i),
            afs->m_pFileInfo[i].m_szName,
            afs->m_pFileData[i].m_size,
            afs->m_pFileData[i].m_offset
        );
    };
};


bool afs_extract(void* handle, const char* path, int idx)
{
	AFS* afs = (AFS*)handle;
	assert(afs);
	assert(afs->m_pFileData);
	assert(afs->m_pFileInfo);
	assert(afs->m_hFile);

    bool bResult = false;
    FILE* OutputFile = nullptr;
    CBuffer InputFileBuffer;
    FILE* InputFile = afs->m_hFile;

    try
    {
        //
        //  Calc range for extracting
        //
        int iFileBegin = (idx == -1 ? 0 : idx);
        int iFileEnd = (idx == -1 ? afs->m_header.m_uFileNum : idx + 1);
        
        for (int i = iFileBegin; i < iFileEnd; i++)
        {
            // 
            //  Construct output file path
            // 
            std::string OutputFilePath;
			if (path)
			{
				OutputFilePath += path;
				OutputFilePath += '\\';
			};
            OutputFilePath += afs->m_pFileInfo[i].m_szName;

            //
            //  Create new file
            // 
            assert(!OutputFile);
            OutputFile = fopen(OutputFilePath.c_str(), "wb");
            if (!OutputFile)
                throw std::exception("Failed to create output file");

            //
            //  Set archive file pointer to an output file
            //
			adxf_setfptr(InputFile, afs->m_pFileData[i].m_offset);

            //
            //  Allocate temp buffer for input file
            //
			if (!InputFileBuffer.Alloc(afs->m_pFileData[i].m_size))
				throw std::exception("Faild to allocate temp file buffer");

            //
            //  Copy data from input file to output
            //
            adxf_read_data(InputFile, InputFileBuffer.Pointer(), InputFileBuffer.Size());
            adxf_write_data(OutputFile, InputFileBuffer.Pointer(), InputFileBuffer.Size());

            //
            //  Correct timedata for output file
            //
            HANDLE hOutputFile = HANDLE(_fileno(OutputFile));
            SYSTEMTIME SystemTime = { 0 };
            FILETIME FileTime = { 0 };

			SystemTime.wYear	= afs->m_pFileInfo[i].m_nYear;
			SystemTime.wMonth	= afs->m_pFileInfo[i].m_nMonth;
			SystemTime.wDay		= afs->m_pFileInfo[i].m_nDay;
			SystemTime.wHour	= afs->m_pFileInfo[i].m_nHour;
			SystemTime.wMinute	= afs->m_pFileInfo[i].m_nMinute;
			SystemTime.wSecond	= afs->m_pFileInfo[i].m_nSecond;

            SystemTimeToFileTime(&SystemTime, &FileTime);
            
            SetFileTime(hOutputFile, &FileTime, &FileTime, &FileTime);

            //
            //  Save output file
            //
            fclose(OutputFile);
            OutputFile = nullptr;
        };

		bResult = true;
    }
    catch (std::exception& e)
    {
        ADXF_NOTE(afs_note_error, e.what());
    };

	if (OutputFile)
	{
		fclose(OutputFile);
		OutputFile = nullptr;
	};

    return bResult;
};