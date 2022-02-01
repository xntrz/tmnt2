#include "PCFileManager.hpp"
#include "PCFileAccess.hpp"
#include "PCFilename.hpp"
#include "PCResRwFileSystem.hpp"
#include "PCFileStd.hpp"

#include "System/PC/PCTypedefs.hpp"

#include "System/Common/File/FileTypes.hpp"
#include "System/Common/File/AfsFilename.hpp"
#include "System/Common/File/RwFileManager.hpp"


CPCFileManager::CPCFileManager(void)
: m_pResRwFS(nullptr)
, m_paPCPhyFileAccess(nullptr)
, m_paPCResFileAccess(nullptr)
, m_bAfsOverrideFlag(true)
{
    ;
};


CPCFileManager::~CPCFileManager(void)
{
    ASSERT(!m_pResRwFS);
    ASSERT(!m_paPCPhyFileAccess);
    ASSERT(!m_paPCResFileAccess);
};


bool CPCFileManager::Start(void)
{
    stdf_init();
    
    if (!CAfsFileManager::Start())
        return false;

    m_pResRwFS = (CPCResRwFileSystem*)RwMalloc(sizeof(CPCResRwFileSystem), rwMEMHINTDUR_GLOBAL);
    ASSERT(m_pResRwFS);
    m_pResRwFS->Initialize();    
    CRwFileManager::Regist(m_pResRwFS);

    m_paPCPhyFileAccess = new CPCPhysicalFileAccess[32];
    ASSERT(m_paPCPhyFileAccess);
    for (int32 i = 0; i < 32; ++i)
        m_listPCPhyFileAccessPool.push_back(&m_paPCPhyFileAccess[i]);

    m_paPCResFileAccess = new CPCResFileAccess[32];
    ASSERT(m_paPCResFileAccess);
    for (int32 i = 0; i < 32; ++i)
        m_listPCResFileAccessPool.push_back(&m_paPCResFileAccess[i]);

    return true;
};


void CPCFileManager::Stop(void)
{
    GarbageCollection();
    
    for (CPCResFileAccess& it : m_listPCResFileAccessAlloc)
        it.Clear();

    if (m_paPCResFileAccess)
    {
        delete[] m_paPCResFileAccess;
        m_paPCResFileAccess = nullptr;
    };
    
    for (CPCPhysicalFileAccess& it : m_listPCPhyFileAccessAlloc)
        it.Clear();

    if (m_paPCPhyFileAccess)
    {
        delete[] m_paPCPhyFileAccess;
        m_paPCPhyFileAccess = nullptr;
    };

    if (m_pResRwFS)
    {
        m_pResRwFS->Terminate();
        CRwFileManager::Remove(m_pResRwFS);
        m_pResRwFS = nullptr;
    };

    CAfsFileManager::Stop();
    
    stdf_term();
};


void CPCFileManager::Error(const char* pszDescription)
{
    MessageBoxA(NULL, pszDescription, "File error", MB_OK | MB_ICONERROR);
};


CFileAccess* CPCFileManager::AllocRequest(int32 nID, int32 nLabel)
{
    CFileAccess* pResult = nullptr;
    
    switch (nLabel)
    {
    case LABELAFS_ID:
        {
            if (m_bAfsOverrideFlag)
            {
                char szFilename[FILETYPES::FILE_NAME_MAX];
                szFilename[0] = '\0';

                CAfsFilename::Filename(szFilename, nID);

                if (CPCPhysicalFileAccess::IsExists(szFilename))
                    pResult = AllocFilePhysical(szFilename);
            };

            if (!pResult)
                pResult = CAfsFileManager::AllocRequest(nID, nLabel);
        }
        break;

    case LABELPC_RESID:
        {
            pResult = AllocFileResource(nID);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return pResult;
};


CFileAccess* CPCFileManager::AllocRequest(const char* pszName, int32 nLabel)
{
    CFileAccess* pResult = nullptr;
    
    switch (nLabel)
    {
    case LABELAFS_NAME:
        {
            pResult = CAfsFileManager::AllocRequest(pszName, nLabel);
        }
        break;
        
    default:
        ASSERT(false);
        break;
    };

    return pResult;
};


void CPCFileManager::GarbageCollection(void)
{
    CAfsFileManager::GarbageCollection();

    {
        auto it = m_listPCResFileAccessAlloc.begin();
        while (it)
        {
            CPCResFileAccess* pAccess = &(*it);
            if (pAccess->Status() == CPCResFileAccess::STATUS_NOREAD)
            {
                it = m_listPCResFileAccessAlloc.erase(it);
                m_listPCResFileAccessPool.push_back(pAccess);
            }
            else
            {
                ++it;
            };
        };
    }

    {
        auto it = m_listPCPhyFileAccessAlloc.begin();
        while (it)
        {
            CPCPhysicalFileAccess* pAccess = &(*it);
            if (pAccess->Status() == CPCPhysicalFileAccess::STATUS_NOREAD)
            {
                it = m_listPCPhyFileAccessAlloc.erase(it);
                m_listPCPhyFileAccessPool.push_back(pAccess);
            }
            else
            {
                ++it;
            };
        };
    }
};


CFileAccess* CPCFileManager::AllocFileResource(int32 nID)
{
    if (m_listPCResFileAccessPool.empty())
        return nullptr;

    CPCResFileAccess* pAccess = m_listPCResFileAccessPool.front();
    ASSERT(pAccess);
    m_listPCResFileAccessPool.erase(pAccess);
    m_listPCResFileAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_type = REQUEST::TYPE_ID;
    request.m_data.id = nID;
    RegistRequest(request);

    return pAccess;
};


CFileAccess* CPCFileManager::AllocFilePhysical(const char* pszFilename)
{
    if (m_listPCPhyFileAccessPool.empty())
        return nullptr;

    CPCPhysicalFileAccess* pAccess = m_listPCPhyFileAccessPool.front();
    ASSERT(pAccess);
    m_listPCPhyFileAccessPool.erase(pAccess);
    m_listPCPhyFileAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_type = REQUEST::TYPE_NAME;
    request.SetName(pszFilename);
    RegistRequest(request);

    return pAccess;
};