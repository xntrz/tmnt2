#pragma once

#include "StdFile.hpp"


class CRwFileSystem final : public RtFileSystem
{
private:
    static const int32 MAX_FILES = 6;

    class CRwFile final : public RtFile
    {
    public:
        CStdFile m_extFile;
    };

public:
    static void rwFileSystemClose(RtFileSystem* fs);
    static RwChar* rwFileSystemGetDeviceName(RtFileSystem* fs);
    static RtFile* rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index);
    static RtFileSystemError rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags);
    static void rwClose(RtFile* file);
    static RwUInt32 rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes);
    static RwUInt32 rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes);
    static RtInt64 rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition);
    static RtFileStatus rwSync(RtFile* fie, RwBool block);
    static RwBool rwAbort(RtFile* file);
    static RwBool rwEof(RtFile* file);
    static RtFileStatus rwGetStatus(RtFile* file);
    static RwBool rwExists(RtFileSystem* fs, const RwChar* filename);

    CRwFileSystem(void);
    ~CRwFileSystem(void);
    bool Initialize(void);
    void Terminate(void);

private:
    CRwFile* m_pFile;
};