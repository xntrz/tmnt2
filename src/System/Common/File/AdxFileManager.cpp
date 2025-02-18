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


/*static*/ bool CAdxFileManager::LoadPartition(int32 ptid, const char* fname, void* dir, void* ptinfo)
{
    return static_cast<CAdxFileManager&>(Instance()).LoadPartition(ptid, fname, ptinfo);
};


CAdxFileManager::CAdxFileManager(void)
{
    std::memset(&m_ptinfoCommon[0], 0x00, sizeof(m_ptinfoCommon));
    std::memset(&m_ptinfoLang[0],   0x00, sizeof(m_ptinfoLang));

    /* checkouts with idb */
#ifdef TMNT2_BUILD_EU
    static_assert(COUNT_OF(m_apszPartitionLangDataFilename) == TYPEDEF::CONFIG_LANG_NUM, "name table incorrect");    
    static_assert(sizeof(m_ptinfoCommon) == 868, "EU build COMMON partition should be 868 bytes size");
    static_assert(sizeof(m_ptinfoLang)   == 300, "EU build LANG partition should be 300 bytes size");
#else /* TMNT2_BUILD_EU */
    static_assert(COUNT_OF(m_apszPartitionLangDataFilename) == 1, "name table incorrect");    
    static_assert(sizeof(m_ptinfoCommon) == 756, "NA build COMMON partition should be 756 bytes size");
    static_assert(sizeof(m_ptinfoLang)   == 284, "NA build LANG partition should be 284 bytes size");
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

    CheckForProperBuild();

    if (!LoadPartitionCommon())
        return false;

    TYPEDEF::CONFIG_LANG lang = CConfigure::GetLanguage();
    if (!LoadPartitionLang(lang))
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


void CAdxFileManager::Reset(void)
{
    bool bResult = LoadPartitionCommon();
    ASSERT(bResult != false);

    TYPEDEF::CONFIG_LANG lang = CConfigure::GetLanguage();
    bResult = LoadPartitionLang(lang);
    ASSERT(bResult != false);
};


CFileAccess* CAdxFileManager::AllocRequest(int32 nType, void* pTypeData)
{
    if (nType != FILETYPE_ID)
        return nullptr;

    int32 fileId = reinterpret_cast<int32>(pTypeData);
    ASSERT(fileId >= 0);
    ASSERT(fileId < FILEID::ID_MAX);

    CRequest req(fileId, &m_aAdxFileAccess[fileId]);
    RegistRequest(req);
    
    return &m_aAdxFileAccess[fileId];
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
    return LoadPartition(0, m_apszPartitionCommonDataFilename[0], m_ptinfoCommon);
};


bool CAdxFileManager::LoadPartitionLang(TYPEDEF::CONFIG_LANG lang)
{
    ASSERT(lang >= 0);
    ASSERT(lang < COUNT_OF(m_apszPartitionLangDataFilename));

    return LoadPartition(1, m_apszPartitionLangDataFilename[lang], m_ptinfoLang);
};


bool CAdxFileManager::LoadPartition(int32 PtId, const char* FName, void* PtInfo)
{
    if (!ADXF_LoadPartitionNw(PtId, FName, NULL, PtInfo))
    {
        bool bLoopFlag = true;

        do
        {
            int32 stat = ADXF_GetPtStat(PtId);
            switch (stat)
            {
            case ADXF_STAT_READEND:
                return true;

            case ADXF_STAT_READING:
                Sync();
                break;

            default:
                bLoopFlag = false;
                break;
            };
        } while (bLoopFlag);
    };

    char buff[1024];
    buff[0] = '\0';

    std::sprintf(buff, "Load partition \"%s\" failed.", FName);

    Error(buff);

    return false;
};


void CAdxFileManager::CheckForProperBuild(void)
{
    const char* pszPartitionName = nullptr;

    /* check common partition */
    int32 fileCountCommon = GetAFSFileCountFromFilename(m_apszPartitionCommonDataFilename[0]);
    if (fileCountCommon != FILEID::COMMONMAX)
        pszPartitionName = m_apszPartitionCommonDataFilename[0];

    /* check lang partition */
    if (!pszPartitionName)
    {
        TYPEDEF::CONFIG_LANG lang = CConfigure::GetLanguage();
        int32 fileCountLang = GetAFSFileCountFromFilename(m_apszPartitionLangDataFilename[lang]);        
        if (fileCountLang != FILEID::LANGMAX)
            pszPartitionName = m_apszPartitionLangDataFilename[lang];
    };

    /* disp fatal err if mismatch */
    if (pszPartitionName)
    {
        char szFatalErrorBuff[1024];
        szFatalErrorBuff[0] = '\0';

        std::sprintf(szFatalErrorBuff,
                     "%s partition file count mismatch!\n\n"
                     "This may happens if you are using wrong assets files for this build "
                     "or wrong specified assets game files path, or damaged assets!",
                     pszPartitionName);

        Error(szFatalErrorBuff);
    };
};


int32 CAdxFileManager::GetAFSFileCountFromFilename(const char* pszFilename) const
{
    int32 fileCount = 0;

    FILE* hFile = fopen(pszFilename, "rb");
    if (hFile)
    {
        uint8 AFSHeaderBuff[8];
        std::memset(AFSHeaderBuff, 0, sizeof(AFSHeaderBuff));

        size_t cbReaded = fread(AFSHeaderBuff, sizeof(uint8), sizeof(AFSHeaderBuff), hFile);

        if (cbReaded == sizeof(AFSHeaderBuff))
            fileCount = ADXF_GetNumFilesFromAfs(AFSHeaderBuff);

        fclose(hFile);
        hFile = nullptr;
    };
    
    return fileCount;
};