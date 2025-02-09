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
    static_assert(sizeof(m_ptinfoCommon) == 868, "multilang eu build COMMON partition should be 868 bytes size");
    static_assert(sizeof(m_ptinfoLang)   == 300, "multilang eu build LANG partition should be 300 bytes size");
#else
    static_assert(COUNT_OF(m_apszPartitionLangDataFilename) == 1, "name table incorrect");    
    static_assert(sizeof(m_ptinfoCommon) == 756, "en build COMMON partition should be 756 bytes size");
    static_assert(sizeof(m_ptinfoLang)   == 284, "en build LANG partition should be 284 bytes size");
#endif
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