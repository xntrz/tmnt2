#include "AfsFileManager.hpp"
#include "AfsFileSystem.hpp"
#include "AfsFileAccess.hpp"
#include "AfsFileID.hpp"
#include "AfsFilename.hpp"
#include "AfsRwFileSystem.hpp"
#include "RwFileManager.hpp"


static const int32 AFS_ACCESS_NUM = AFSFILEID::ID_MAX;
static const int32 AFS_PT_DATA = 0;
static const int32 AFS_PT_LANG = 1;
static const char* s_pszPartitionRootPath = "";


static const char* s_apszPartitionFilename[] =
{
    "TMNT.DAT",
    "TMNTE.DAT",
};


CAfsFileManager::CAfsFileManager(void)
: m_pAfsRwFS(nullptr)
, m_paAfsFileAccess(nullptr)
, m_uPtLoadMask(0)
{
    ;
};


CAfsFileManager::~CAfsFileManager(void)
{
    ;
};


bool CAfsFileManager::Start(void)
{
    if (!CFileManager::Start())
        return false;

    m_pAfsRwFS = (CAfsRwFileSystem*)RwMalloc(sizeof(CAfsRwFileSystem), rwMEMHINTDUR_GLOBAL);
    ASSERT(m_pAfsRwFS);
    m_pAfsRwFS->Initialize();
    CRwFileManager::Regist(m_pAfsRwFS, true);

    CAfsFileSystem::Initialize(s_pszPartitionRootPath);

    if (!MountPartition(AFS_PT_DATA, s_apszPartitionFilename[AFS_PT_DATA]))
        return false;

    if (!MountPartition(AFS_PT_LANG, s_apszPartitionFilename[AFS_PT_LANG]))
        return false;

    m_paAfsFileAccess = new CAfsFileAccess[AFS_ACCESS_NUM];
    ASSERT(m_paAfsFileAccess);
    if (!m_paAfsFileAccess)
        return false;

    for (int32 i = 0; i < AFS_ACCESS_NUM; ++i)
        m_listAfsAccessPool.push_back(&m_paAfsFileAccess[i]);

    return true;
};


void CAfsFileManager::Stop(void)
{
	GarbageCollection();

    for (CAfsFileAccess& it : m_listAfsAccessAlloc)
        it.Clear();
    
    if (m_paAfsFileAccess)
    {
        delete[] m_paAfsFileAccess;
        m_paAfsFileAccess = nullptr;
    };
    
    UnmountPartition(AFS_PT_LANG);
    UnmountPartition(AFS_PT_DATA);
    
    CAfsFileSystem::Terminate();

    if (m_pAfsRwFS)
    {
		m_pAfsRwFS->Terminate();

        CRwFileManager::Remove(m_pAfsRwFS);
        m_pAfsRwFS = nullptr;
    };

    CFileManager::Stop();
};


void CAfsFileManager::Sync(void)
{
    CFileManager::Sync();    
    GarbageCollection();
};


void CAfsFileManager::Reset(void)
{
    ASSERT(false);
};


CFileAccess* CAfsFileManager::AllocRequest(int32 nID, int32 nLabel)
{
    if (nLabel != LABELAFS_ID)
        return CFileManager::AllocRequest(nID, nLabel);

    if (m_listAfsAccessPool.empty())
        return nullptr;

    CAfsFileAccess* pAccess = m_listAfsAccessPool.front();
    ASSERT(pAccess);
    m_listAfsAccessPool.erase(pAccess);
    m_listAfsAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_nLabel = nLabel;    
    request.m_type = REQUEST::TYPE_ID;
    request.m_data.id = nID;
    RegistRequest(request);
    
    return pAccess;
};


CFileAccess* CAfsFileManager::AllocRequest(const char* pszName, int32 nLabel)
{
    if (nLabel != LABELAFS_NAME)
        return CFileManager::AllocRequest(pszName, nLabel);

    if (m_listAfsAccessPool.empty())
        return nullptr;

    CAfsFileAccess* pAccess = m_listAfsAccessPool.front();
    ASSERT(pAccess);
    m_listAfsAccessPool.erase(pAccess);
    m_listAfsAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_nLabel = nLabel;
    request.m_type = REQUEST::TYPE_ID;
    request.SetName(pszName);
    RegistRequest(request);

    return pAccess;
};


bool CAfsFileManager::MountPartition(int32 iPartitionID, const char* pszArchive)
{
    ASSERT(pszArchive);
    ASSERT(iPartitionID < BITSOF(m_uPtLoadMask));

    if (CAfsFileSystem::PartitionMount(iPartitionID, pszArchive))
    {
        BIT_SET(m_uPtLoadMask, iPartitionID);
        return true;
    }
    else
    {
        char szBuffer[256];
        szBuffer[0] = '\0';

        std::sprintf(szBuffer, "Failed to open game archive: %s. File not exist or corrupted", pszArchive);

        Error(szBuffer);
    };

    return false;    
};


void CAfsFileManager::UnmountPartition(int32 iPartitionID)
{
    ASSERT(iPartitionID < BITSOF(m_uPtLoadMask));

    if (IS_BIT_SET(m_uPtLoadMask, iPartitionID))
    {
        CAfsFileSystem::PartitionUnmount(iPartitionID);
        BIT_CLEAR(m_uPtLoadMask, iPartitionID);
    };
};


void CAfsFileManager::GarbageCollection(void)
{
    if (m_listAfsAccessAlloc.empty())
		return;

    auto it = m_listAfsAccessAlloc.begin();
    while (it)
    {
        CAfsFileAccess* pAccess = &(*it);
        if (pAccess->Status() == CAfsFileAccess::STATUS_NOREAD)
        {
            it = m_listAfsAccessAlloc.erase(it);
            m_listAfsAccessPool.push_back(pAccess);
        }
        else
        {
            ++it;
        };
    };
};