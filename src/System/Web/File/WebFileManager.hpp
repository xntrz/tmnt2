#pragma once

#include "WebRwIDBFS.hpp"
#include "WebRwRFS.hpp"
#include "WebRwStdFS.hpp"
#include "WebRwWFS.hpp"

#include "System/Common/File/AdxFileManager.hpp"


class CWebFileManager : public CAdxFileManager
{
public:
    static CWebFileManager& Instance(void);
    static const char* MakePath(const char* pszFilepath);

    CWebFileManager(void);
    virtual ~CWebFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Sync(void) override;
    virtual void Error(const char* pszDescription) override;
    virtual bool SetupFileSystem(void) override;
    virtual void ShutdownFileSystem(void) override;
    virtual void SyncWait(void);
    bool IsAdxWork(void) const;

private:
    static bool m_bRFS;
    CWebRwIDBFS m_idbfs;
    CWebRwRFS   m_rfs;
    CWebRwStdFS m_stdfs;
    CWebRwWFS   m_wfs;
    bool        m_bRun;
};