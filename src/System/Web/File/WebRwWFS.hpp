#pragma once

#include "WebRwFS.hpp"


class CWebRwWFS : public CWebRwFS
{
public:
    struct RtFileX : public RtFile
    {
        int32         id;
        int32         opid;
        char          path[256];
        bool          isopen;
        void*         ioaddr;
        size_t        iosize;
        size_t        ioofst;
        volatile bool iorun;
    };

public:
    CWebRwWFS(void);
    virtual ~CWebRwWFS(void);
    virtual bool Initialize(const char* pszFSName, const char* pszDeviceName, const char* pszMountPath);
    virtual void Terminate(void) override;
    virtual bool Sync(void) override;
    virtual void rwFileSystemClose(RtFileSystem* fs) override;
    virtual RwChar* rwFileSystemGetDeviceName(RtFileSystem* fs) override;
    virtual RtFile* rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index) override;
    virtual RtFileSystemError rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags) override;
    virtual void rwClose(RtFile* file) override;
    virtual RwUInt32 rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes) override;
    virtual RwUInt32 rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes) override;
    virtual RtInt64 rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition) override;
    virtual RtFileStatus rwSync(RtFile* file, RwBool block) override;
    virtual RwBool rwAbort(RtFile* file) override;
    virtual RwBool rwEof(RtFile* file) override;
    virtual RtFileStatus rwGetStatus(RtFile* file) override;
    virtual RwBool rwExists(RtFileSystem* fs, const RwChar* filename) override;
    const RwChar* PathCorrection(const RwChar* filename) const;
    bool CheckFileExistInMEMFS(const RwChar* filename) const;

private:
    char    m_szMountPath[256];
    RtFileX m_aFile[12];
    int32   m_numOpen;
};