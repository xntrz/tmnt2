#pragma once

#include "FileManager.hpp"
#include "FileID.hpp"

#include "AdxFileAccess.hpp"

#include "cri_adxf.h"


class CAdxFileManager : public CFileManager
{
public:
    enum FILETYPE
    {
        FILETYPE_ID = 0,
        
        FILETYPE_EXTEND,
    };
    
public:
    static bool LoadPartition(int32 ptid, const char* fname, void* dir, void* ptinfo);
    
    CAdxFileManager(void);
    virtual ~CAdxFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Sync(void) override;
    virtual void Reset(void) override;
    virtual CFileAccess* AllocRequest(int32 nType, void* pTypeData) override;
    virtual bool SetupThreadSystem(void);
    virtual void ShutdownThreadSystem(void);
    virtual bool LoadPartitionCommon(void);
    virtual bool LoadPartitionLang(void);
    virtual bool LoadPartition(int32 PrId, const char* FName, void* PtInfo);
    virtual bool SetupFileSystem(void) = 0;
    virtual void ShutdownFileSystem(void) = 0;

private:
    CAdxFileAccess m_aAdxFileAccess[FILEID::ID_MAX];  
    char m_ptinfoCommon[ADXF_CALC_PTINFO_REAL_SIZE(237)];
    char m_ptinfoLang[ADXF_CALC_PTINFO_REAL_SIZE(1)];
};