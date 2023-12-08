#include "AdxFileManager.hpp"

#include "cri_adxt.h"


/*static*/ bool CAdxFileManager::LoadPartition(int32 ptid, const char* fname, void* dir, void* ptinfo)
{
    return ((CAdxFileManager&)Instance()).LoadPartition(ptid, fname, ptinfo);
};


CAdxFileManager::CAdxFileManager(void)
{
    std::memset(&m_ptinfoCommon[0], 0x00, sizeof(m_ptinfoCommon));
    std::memset(&m_ptinfoLang[0], 0x00, sizeof(m_ptinfoLang));      
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

    if (!LoadPartitionLang())
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
    
    bResult = LoadPartitionLang();
    ASSERT(bResult != false);
};


CFileAccess* CAdxFileManager::AllocRequest(int32 nType, const void* pTypeData)
{
    if (nType != FILETYPE_ID)
        return nullptr;

    int32 FileID = int32(pTypeData);
    ASSERT( (FileID >= 0) && (FileID < FILEID::ID_MAX) );

	CRequest req(FileID, &m_aAdxFileAccess[FileID]);
	RegistRequest(req);
    
    return &m_aAdxFileAccess[FileID];
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
    return LoadPartition(0, "TMNT.DAT", &m_ptinfoCommon[0]);
};


bool CAdxFileManager::LoadPartitionLang(void)
{
    return LoadPartition(1, "TMNTE.DAT", &m_ptinfoLang[0]);
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