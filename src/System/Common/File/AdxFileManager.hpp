#pragma once

#include "FileManager.hpp"
#include "Filename.hpp"

#include "cri_adxf.h"


class CAdxFileManager : public CFileManager
{
public:
    static bool LoadPt(int32 ptid, const char* fname, void* dir, void* ptinfo);
    
    CAdxFileManager(void);
    virtual ~CAdxFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Sync(void) override;
    virtual void ResetData(void) override;
    virtual bool SetupThreadSystem(void);
    virtual void ShutdownThreadSystem(void);
    virtual bool LoadPartitionCommon(void);
    virtual bool LoadPartitionLang(TYPEDEF::CONFIG_LANG lang);
    virtual bool LoadPartition(int32 ptid, const char* fname, void* dir, void* ptinfo);
    virtual bool SetupFileSystem(void) = 0;
    virtual void ShutdownFileSystem(void) = 0;
    
protected:
    static const char* m_apszPartitionCommonDataFilename[];
    static const char* m_apszPartitionLangDataFilename[];

    char m_ptinfoCommon[ADXF_CALC_PTINFO_REAL_SIZE(FILEID::COMMON_MAX)];
    char m_ptinfoLang[ADXF_CALC_PTINFO_REAL_SIZE(FILEID::LANGUAGE_MAX)];
};