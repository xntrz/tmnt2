#pragma once

#include "System/Common/File/RwFileSystem.hpp"


class CPCResRwFileSystem final : public CRwFileSystem
{
private:
    static const int32 MAX_FILES = 8;
    class CFile;

public:
    CPCResRwFileSystem(void);
    ~CPCResRwFileSystem(void);
    bool Initialize(void);
    void Terminate(void);

private:
    static void FsClose(RtFileSystem* fs);
    static RwChar* FsGetDeviceName(RtFileSystem* fs);
    static RtFile* FsGetFile(RtFileSystem* fs, RwUInt32 index);
    static RtFileSystemError FOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags);
    static void FClose(RtFile* file);
    static RwUInt32 FRead(RtFile* file, void* pBuffer, RwUInt32 nBytes);
    static RwUInt32 FWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes);
    static RtInt64 FSetPos(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition);
    static RtFileStatus FSync(RtFile* fie, RwBool block);
    static RwBool FAbort(RtFile* file);
    static RwBool FEOF(RtFile* file);
    static RtFileStatus FStatus(RtFile* fie);
    static RwBool FExists(RtFileSystem* fs, const RwChar* filename);
    static bool ConvertPath(char* pszPathBuffer, int32 nPathBufferSize, const char* pszPath);

private:
    CFile* m_paFile;
};