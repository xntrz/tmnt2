#pragma once

#include "WebRwFS.hpp"


class CWebRwStdFS : public CWebRwFS
{
protected:
    struct RtFileX : public RtFile
    {
        FILE* fptr;
    };

public:
    CWebRwStdFS(void);
    virtual ~CWebRwStdFS(void);
    virtual bool Initialize(const char* pszFSName, const char* pszDeviceName);
    virtual void Terminate(void) override;
    virtual void rwFileSystemClose(RtFileSystem* fs) override;
    virtual RwChar* rwFileSystemGetDeviceName(RtFileSystem* fs) override;
    virtual RtFile* rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index) override;
    virtual RtFileSystemError rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags) override;
    virtual void rwClose(RtFile* file) override;
    virtual RwUInt32 rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes) override;
    virtual RwUInt32 rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes) override;
    virtual RtInt64 rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition) override;
    virtual RtFileStatus rwSync(RtFile* fie, RwBool block) override;
    virtual RwBool rwAbort(RtFile* file) override;
    virtual RwBool rwEof(RtFile* file) override;
    virtual RtFileStatus rwGetStatus(RtFile* file) override;
    virtual RwBool rwExists(RtFileSystem* fs, const RwChar* filename) override;
    virtual const char* GetFOpenMode(RwUInt32 flags) const;
    void SetRedirectFromAfs(void);
    void ClrRedirectFromAfs(void);
    
private:
    RtFileX m_aFile[4];
    int32 m_numOpen;
};