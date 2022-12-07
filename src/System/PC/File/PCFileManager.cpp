#include "PCFileManager.hpp"
#include "PCFilename.hpp"
#include "PCFileStd.hpp"
#include "PCFileAccess.hpp"

#include "System/PC/PCTypedefs.hpp"
#include "System/PC/PCSpecific.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/File/FileTypes.hpp"
#include "System/Common/File/Filename.hpp"
#include "System/Common/File/RwFileManager.hpp"

#include <mmsystem.h>
#include "adx_pc.h"


CPCFileManager::CPCFileManager(void)
: m_bAfsOverrideFlag(true)
, m_paPCPhyFileAccess(nullptr)
, m_paPCRcFileAccess(nullptr)
, m_adxfic(0)
{
    ;
};


CPCFileManager::~CPCFileManager(void)
{
    ;
};


bool CPCFileManager::Start(void)
{
    if (!CAdxFileManager::Start())
        return false;
    
    SetupWorkingDir();
    stdf_init();

    m_paPCPhyFileAccess = new CPCPhyFileAccess[32];
    m_paPCRcFileAccess = new CPCRcFileAccess[32];
    if ((!m_paPCPhyFileAccess) || (!m_paPCRcFileAccess))
        return false;

    for (int32 i = 0; i < 32; ++i)
        m_listPCPhyFileAccessPool.push_back(&m_paPCPhyFileAccess[i]);

    for (int32 i = 0; i < 32; ++i)
        m_listPCRcFileAccessPool.push_back(&m_paPCRcFileAccess[i]);

    return true;
};


void CPCFileManager::Stop(void)
{
    for (CPCRcFileAccess& it : m_listPCRcFileAccessAlloc)
        it.Clear();

    for (CPCPhyFileAccess& it : m_listPCPhyFileAccessAlloc)
        it.Clear();

    GarbageCollection();

    if (m_paPCRcFileAccess)
    {
        delete[] m_paPCRcFileAccess;
        m_paPCRcFileAccess = nullptr;
    };

    if (m_paPCPhyFileAccess)
    {
        delete[] m_paPCPhyFileAccess;
        m_paPCPhyFileAccess = nullptr;
    };

    stdf_term();
    
    CAdxFileManager::Stop();
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
    case LABELADX_ID:
        {
#ifdef _DEBUG            
            if (m_bAfsOverrideFlag)
            {
                char szFilename[256];
                szFilename[0] = '\0';
                CFilename::Filename(szFilename, nID);
                
                char szFilepath[256];
                szFilepath[0] = '\0';
                std::strcpy(szFilepath, m_szPath);
                std::strcat(szFilepath, szFilename);

                if (CPCPhyFileAccess::IsExists(szFilepath))
                    pResult = AllocFilePhy(szFilepath);
            };
#endif            

            if (!pResult)
                pResult = CAdxFileManager::AllocRequest(nID, nLabel);
        }
        break;

    case LABELPC_RESID:
        {
            pResult = AllocFileRc(nID);
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
    case LABELADX_NAME:
        {
            pResult = CAdxFileManager::AllocRequest(pszName, nLabel);
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
    CAdxFileManager::GarbageCollection();

    {
        auto it = m_listPCRcFileAccessAlloc.begin();
        while (it)
        {
            CPCRcFileAccess* pAccess = &(*it);
            if (pAccess->Status() == CPCRcFileAccess::STATUS_NOREAD)
            {
                it = m_listPCRcFileAccessAlloc.erase(it);
                m_listPCRcFileAccessPool.push_back(pAccess);
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
            CPCPhyFileAccess* pAccess = &(*it);
            if (pAccess->Status() == CPCPhyFileAccess::STATUS_NOREAD)
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


bool CPCFileManager::SetupFileSystem(void)
{
    m_szPath[0] = '\0';
    
    const char* pszValue = nullptr;
    if (CConfigure::CheckArgValue("afspath", &pszValue))
    {
        std::strcpy(m_szPath, pszValue);
    }
    else
    {
        CPCSpecific::GetModulePath(m_szPath, COUNT_OF(m_szPath));
        char szDrive[4];
        char szDir[MAX_PATH];
        _splitpath(m_szPath, szDrive, szDir, nullptr, nullptr);
        std::sprintf(m_szPath, "%s%s", szDrive, szDir);
    };

    m_bAfsOverrideFlag = CConfigure::CheckArg("afsover");

    AdxpcSprmFs sprmfs;
    sprmfs.rtdir = m_szPath;
    ADXPC_SetupFileSystem(&sprmfs);
    
    m_adxfic = ADXFIC_Create(m_szPath, 0, 0, 0);
    return (m_adxfic != 0);
};


void CPCFileManager::ShutdownFileSystem(void)
{
    if (m_adxfic)
    {
        ADXFIC_Destroy(m_adxfic);
        m_adxfic = 0;
    };
    
    ADXPC_ShutdownFileSystem();
};


CFileAccess* CPCFileManager::AllocFileRc(int32 nID)
{
    if (m_listPCRcFileAccessPool.empty())
        return nullptr;

    CPCRcFileAccess* pAccess = m_listPCRcFileAccessPool.front();    
    ASSERT(pAccess);
    if (!pAccess)
        return nullptr;
    
    m_listPCRcFileAccessPool.erase(pAccess);
    m_listPCRcFileAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_type = REQUEST::TYPE_ID;
    request.m_data.id = nID;
    RegistRequest(request);

    return pAccess;
};


CFileAccess* CPCFileManager::AllocFilePhy(const char* pszFilename)
{
    if (m_listPCPhyFileAccessPool.empty())
        return nullptr;

    CPCPhyFileAccess* pAccess = m_listPCPhyFileAccessPool.front();
    ASSERT(pAccess);
    if (!pAccess)
        return nullptr;
    
    m_listPCPhyFileAccessPool.erase(pAccess);
    m_listPCPhyFileAccessAlloc.push_back(pAccess);

    REQUEST request;
    request.m_pAccessData = pAccess;
    request.m_type = REQUEST::TYPE_NAME;
    request.SetName(pszFilename);
    RegistRequest(request);

    return pAccess;
};


void CPCFileManager::SetupWorkingDir(void)
{
    char szPath[MAX_PATH];
    CPCSpecific::GetModulePath(szPath, COUNT_OF(szPath));
    
    char szDrive[4];
    char szDir[MAX_PATH];
    _splitpath(szPath, szDrive, szDir, nullptr, nullptr);
    std::sprintf(szPath, "%s%s", szDrive, szDir);
    
    SetCurrentDirectoryA(szPath);
};