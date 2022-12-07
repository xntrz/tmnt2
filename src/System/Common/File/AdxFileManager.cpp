#include "AdxFileManager.hpp"
#include "AdxFileAccess.hpp"
#include "AdxRwFileSystem.hpp"
#include "FileID.hpp"
#include "Filename.hpp"
#include "RwFileManager.hpp"

#include "cri_adxt.h"


/*static*/ CAdxFileManager& CAdxFileManager::Instance(void)
{
    return (CAdxFileManager&)CFileManager::Instance();
};


CAdxFileManager::CAdxFileManager(void)
: m_pAdxRwFileSystem(nullptr)
, m_paAdxFileAccess(nullptr)
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

    m_pAdxRwFileSystem = (CAdxRwFileSystem*)RwMalloc(sizeof(CAdxRwFileSystem), rwMEMHINTDUR_GLOBAL);
    if (!m_pAdxRwFileSystem->Initialize())
        return false;
    CRwFileManager::Regist(m_pAdxRwFileSystem, true);

    m_paAdxFileAccess = new CAdxFileAccess[32];
    if (!m_paAdxFileAccess)
        return false;

    for (int32 i = 0; i < 32; ++i)
        m_listAdxFileAccessPool.push_back(&m_paAdxFileAccess[i]);

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
    for (CAdxFileAccess& it : m_listAdxFileAccessAlloc)
        it.Clear();
    
    GarbageCollection();

    ADXT_Finish();

    ShutdownThreadSystem();
    ShutdownFileSystem();

    if (m_paAdxFileAccess)
    {
        delete[] m_paAdxFileAccess;
        m_paAdxFileAccess = nullptr;
    };

    if (m_pAdxRwFileSystem)
    {
        m_pAdxRwFileSystem->Terminate();
        CRwFileManager::Remove(m_pAdxRwFileSystem);
        m_pAdxRwFileSystem = nullptr;
    };

    CFileManager::Stop();
};


void CAdxFileManager::Sync(void)
{
    CFileManager::Sync();    
    GarbageCollection();
    ADXM_ExecMain();
};


void CAdxFileManager::Reset(void)
{
    bool bResult = LoadPartitionCommon();
    ASSERT(bResult != false);
    bResult = LoadPartitionLang();
    ASSERT(bResult != false);
};


CFileAccess* CAdxFileManager::AllocRequest(int32 nID, int32 nLabel)
{
    if (nLabel != LABELADX_ID)
        return CFileManager::AllocRequest(nID, nLabel);

    if (m_listAdxFileAccessPool.empty())
        return nullptr;

    CAdxFileAccess* pAccess = m_listAdxFileAccessPool.front();
    ASSERT(pAccess);
    if (!pAccess)
        return nullptr;
    
    m_listAdxFileAccessPool.erase(pAccess);
    m_listAdxFileAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_nLabel = nLabel;    
    request.m_type = REQUEST::TYPE_ID;
    request.m_data.id = nID;
    RegistRequest(request);
    
    return pAccess;
};


CFileAccess* CAdxFileManager::AllocRequest(const char* pszName, int32 nLabel)
{
    if (nLabel != LABELADX_NAME)
        return CFileManager::AllocRequest(pszName, nLabel);

    if (m_listAdxFileAccessPool.empty())
        return nullptr;

    CAdxFileAccess* pAccess = m_listAdxFileAccessPool.front();
    ASSERT(pAccess);
    if (!pAccess)
        return nullptr;
    
    m_listAdxFileAccessPool.erase(pAccess);
    m_listAdxFileAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_nLabel = nLabel;
    request.m_type = REQUEST::TYPE_ID;
    request.SetName(pszName);
    RegistRequest(request);

    return pAccess;
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
    while (ADXF_LoadPartitionNw(PtId, FName, NULL, PtInfo))
        Error("Loading partition no wait error");

    while (true)
    {        
        int32 PtStat = ADXF_GetPtStat(PtId);
        if (PtStat == ADXF_STAT_READEND)
        {
			break;
        }
        else if (PtStat == ADXF_STAT_READING)
        {
			Sync();
        }
        else
        {
			Error("Reading partition error");
        };        
    };

    return true;
};


void CAdxFileManager::GarbageCollection(void)
{
    if (m_listAdxFileAccessAlloc.empty())
        return;

    auto it = m_listAdxFileAccessAlloc.begin();
    while (it)
    {
        CAdxFileAccess* pAccess = &(*it);
        if (pAccess->Status() == CAdxFileAccess::STATUS_NOREAD)
        {
            it = m_listAdxFileAccessAlloc.erase(it);
            m_listAdxFileAccessPool.push_back(pAccess);
        }
        else
        {
            ++it;
        };
    };
};