#pragma once

#include "System/Common/File/AfsFileManager.hpp"


class CPCResRwFileSystem;
class CPCPhysicalFileAccess;
class CPCResFileAccess;


class CPCFileManager final : public CAfsFileManager
{
public:
    enum
    {
        LABELPCBEGIN = LABELAFS_EXTEND,
        
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

private:
    CFileAccess* AllocFileResource(int32 nID);
    CFileAccess* AllocFilePhysical(const char* pszFilename);

private:
    CPCResRwFileSystem* m_pResRwFS;    
    CPCPhysicalFileAccess* m_paPCPhyFileAccess;
    CList<CPCPhysicalFileAccess> m_listPCPhyFileAccessPool;
    CList<CPCPhysicalFileAccess> m_listPCPhyFileAccessAlloc;
    CPCResFileAccess* m_paPCResFileAccess;
    CList<CPCResFileAccess> m_listPCResFileAccessPool;
    CList<CPCResFileAccess> m_listPCResFileAccessAlloc;
    bool m_bAfsOverrideFlag;
};