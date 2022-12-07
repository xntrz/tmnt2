#pragma once

#include "System/Common/File/AdxFileManager.hpp"

class CPCPhyFileAccess;
class CPCRcFileAccess;


class CPCFileManager final : public CAdxFileManager
{
public:
    enum
    {
        LABELPCBEGIN = LABELADX_EXTEND,
        LABELPC_RESID,
    };
    
public:
    CPCFileManager(void);
    virtual ~CPCFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Error(const char* pszDescription) override;
    virtual CFileAccess* AllocRequest(int32 nID, int32 nLabel) override;
    virtual CFileAccess* AllocRequest(const char* pszName, int32 nLabel) override;
    virtual void GarbageCollection(void) override;
    virtual bool SetupFileSystem(void) override;
    virtual void ShutdownFileSystem(void) override;

private:
    CFileAccess* AllocFileRc(int32 nID);
    CFileAccess* AllocFilePhy(const char* pszFilename);
    void SetupWorkingDir(void);

private:    
    CPCPhyFileAccess* m_paPCPhyFileAccess;
    CList<CPCPhyFileAccess> m_listPCPhyFileAccessPool;
    CList<CPCPhyFileAccess> m_listPCPhyFileAccessAlloc;
    CPCRcFileAccess* m_paPCRcFileAccess;
    CList<CPCRcFileAccess> m_listPCRcFileAccessPool;
    CList<CPCRcFileAccess> m_listPCRcFileAccessAlloc;
    bool m_bAfsOverrideFlag;
    void* m_adxfic;
    char m_szPath[256];
};