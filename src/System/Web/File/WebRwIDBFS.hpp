#pragma once

#include "WebRwStdFS.hpp"


class CWebRwIDBFS : public CWebRwStdFS
{
public:
    CWebRwIDBFS(void);
    virtual ~CWebRwIDBFS(void);
    virtual bool Initialize(const char* pszFSName, const char* pszDeviceName, const char* pszMountPath);
    virtual void Terminate(void) override;
    virtual bool Sync(void) override;
    virtual RtFileSystemError rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags) override;
    virtual void rwClose(RtFile* file) override;
    virtual RwBool rwExists(RtFileSystem* fs, const RwChar* filename) override;
    const RwChar* PathCorrection(const RwChar* filename) const;
    
private:
    bool m_bSyncRequest;
    bool m_bResyncRequest;
};