#pragma once

#include "rtfsyst.h"


class CWebRwFS
{
public:
    struct RtFileSystemEx : public RtFileSystem
    {
        CWebRwFS* cppobj;
    };

public:
    CWebRwFS(void);
    virtual ~CWebRwFS(void);
    virtual bool Initialize(const char* pszFSName, const char* pszDeviceName, int32 fileNum);
    virtual void Terminate(void);
    virtual bool Sync(void);
    virtual void rwFileSystemClose(RtFileSystem* fs) = 0;
    virtual RwChar* rwFileSystemGetDeviceName(RtFileSystem* fs) = 0;
    virtual RtFile* rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index) = 0;
    virtual RtFileSystemError rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags) = 0;
    virtual void rwClose(RtFile* file) = 0;
    virtual RwUInt32 rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes) = 0;
    virtual RwUInt32 rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes) = 0;
    virtual RtInt64 rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition) = 0;
    virtual RtFileStatus rwSync(RtFile* file, RwBool block) = 0;
    virtual RwBool rwAbort(RtFile* file) = 0;
    virtual RwBool rwEof(RtFile* file) = 0;
    virtual RtFileStatus rwGetStatus(RtFile* file) = 0;
    virtual RwBool rwExists(RtFileSystem* fs, const RwChar* filename) = 0;
    RtFileSystem* GetRtFS(void);
    
protected:
    char            m_szFSName[RTFS_MAX_NAME_LENGTH];
    char            m_szDeviceName[16];
    RtFileSystemEx* m_pFileSystem;
    bool            m_bRegist;
};