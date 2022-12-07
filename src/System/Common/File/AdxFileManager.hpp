#pragma once

#include "FileManager.hpp"
#include "cri_adxf.h"


class CAdxRwFileSystem;
class CAdxFileAccess;


class CAdxFileManager : public CFileManager
{
public:
    enum
    {
        LABELADX_ID = 0,
        LABELADX_NAME,
        LABELADX_EXTEND,        
    };
    
public:
    static CAdxFileManager& Instance(void);
    
    CAdxFileManager(void);
    virtual ~CAdxFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Sync(void) override;
    virtual void Reset(void) override;
    virtual CFileAccess* AllocRequest(int32 nID, int32 nLabel) override;
    virtual CFileAccess* AllocRequest(const char* pszName, int32 nLabel) override;
    virtual bool SetupThreadSystem(void);
    virtual void ShutdownThreadSystem(void);
    virtual bool LoadPartitionCommon(void);
    virtual bool LoadPartitionLang(void);
    virtual bool LoadPartition(int32 PrId, const char* FName, void* PtInfo);
    virtual void GarbageCollection(void);
    virtual bool SetupFileSystem(void) = 0;
    virtual void ShutdownFileSystem(void) = 0;

private:
    CAdxRwFileSystem* m_pAdxRwFileSystem;
    CAdxFileAccess* m_paAdxFileAccess;
    CList<CAdxFileAccess> m_listAdxFileAccessPool;
    CList<CAdxFileAccess> m_listAdxFileAccessAlloc;
    char m_ptinfoCommon[ADXF_CALC_PTINFO_REAL_SIZE(237)];
    char m_ptinfoLang[ADXF_CALC_PTINFO_REAL_SIZE(1)];
};