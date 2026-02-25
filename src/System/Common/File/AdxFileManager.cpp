#include "AdxFileManager.hpp"

#include "System/Common/Configure.hpp"

#include "cri_adxt.h"


/*static*/ const char* CAdxFileManager::m_apszPartitionCommonDataFilename[] =
{
    "TMNT.DAT",
};


/*static*/ const char* CAdxFileManager::m_apszPartitionLangDataFilename[] =
{
    "TMNTE.DAT",
#ifdef TMNT2_BUILD_EU
    "TMNTG.DAT",
    "TMNTF.DAT",
    "TMNTS.DAT",
    "TMNTI.DAT",
#endif /* TMNT2_BUILD_EU */
};


/*static*/ bool CAdxFileManager::LoadPt(int32 ptid, const char* fname, void* dir, void* ptinfo)
{
    return static_cast<CAdxFileManager&>(Instance()).LoadPartition(ptid, fname, dir, ptinfo);
};


CAdxFileManager::CAdxFileManager(void)
: m_ptinfoCommon()
, m_ptinfoLang()
{
    std::memset(&m_ptinfoCommon[0], 0, sizeof(m_ptinfoCommon));
    std::memset(&m_ptinfoLang[0], 0, sizeof(m_ptinfoLang));

#ifdef TMNT2_BUILD_EU
    static_assert(COUNT_OF(m_apszPartitionLangDataFilename) == TYPEDEF::CONFIG_LANG_NUM, "name table incorrect");
#endif /* TMNT2_BUILD_EU */
};


CAdxFileManager::~CAdxFileManager(void)
{
    ;
};


bool CAdxFileManager::Start(void)
{
    if (!CFileManager::Start())
        return false;

    if (!SetupFileSystem())
        return false;

    if (!SetupThreadSystem())
        return false;

    ADXT_Init();

    if (!LoadPartitionCommon())
        return false;

    if (!LoadPartitionLang(CConfigure::GetLanguage()))
        return false;

    return true;
};


void CAdxFileManager::Stop(void)
{
    ADXT_Finish();

    ShutdownThreadSystem();
    ShutdownFileSystem();

    CFileManager::Stop();
};


void CAdxFileManager::Sync(void)
{
    CFileManager::Sync();
    ADXM_ExecMain();
};


void CAdxFileManager::ResetData(void)
{
    bool bResult = false;

    bResult = LoadPartitionCommon();
    ASSERT(bResult != false);

    bResult = LoadPartitionLang(CConfigure::GetLanguage());
    ASSERT(bResult != false);
};


bool CAdxFileManager::SetupThreadSystem(void)
{
    ADXM_SetupThrd(NULL);
    return true;
};


void CAdxFileManager::ShutdownThreadSystem(void)
{
    ADXM_ShutdownThrd();
};


bool CAdxFileManager::LoadPartitionCommon(void)
{
    return LoadPartition(0, m_apszPartitionCommonDataFilename[0], NULL, m_ptinfoCommon);
};


bool CAdxFileManager::LoadPartitionLang(TYPEDEF::CONFIG_LANG lang)
{
    ASSERT(lang >= 0);
    ASSERT(lang < COUNT_OF(m_apszPartitionLangDataFilename));

    return LoadPartition(1, m_apszPartitionLangDataFilename[lang], NULL, m_ptinfoLang);
};


bool CAdxFileManager::LoadPartition(int32 ptid, const char* fname, void* dir, void* ptinfo)
{
    char szErrBuf[256];
    szErrBuf[0] = '\0';

    while (ADXF_LoadPartitionNw(ptid, fname, dir, ptinfo))
    {
        std::sprintf(szErrBuf, "Failed to load partition: %s (%d)", fname, ptid);
        Error(szErrBuf);
    };

    while (ADXF_GetPtStat(ptid) == ADXF_STAT_READING)
        Sync();

    int32 stat = ADXF_GetPtStat(ptid);
    if (stat == ADXF_STAT_ERROR)
    {
        std::sprintf(szErrBuf, "Failed to read partition: %s (%d)", fname, ptid);
        Error(szErrBuf);
    };

    return true;
};