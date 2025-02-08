#include "PCFileManager.hpp"

#include "../PCTypedefs.hpp"
#include "../PCError.hpp"

#include "System/Common/Configure.hpp"
#include "System/Common/File/Filename.hpp"

#include <mmsystem.h>
#include "adx_pc.h"


/*static*/ bool CPCFileManager::IsExist(const char* pszFilename)
{
    ASSERT(pszFilename != nullptr);
    
    CPCFileManager& filemanager = static_cast<CPCFileManager&>(Instance());

    char szFilepath[256];
    szFilepath[0] = '\0';
    std::strcpy(szFilepath, filemanager.m_szAfsPath);
    std::strcat(szFilepath, "/");
    std::strcat(szFilepath, pszFilename);

    return CPCPhyFileAccess::IsExists(szFilepath);
};


CPCFileManager::CPCFileManager(void)
: m_rwFileSystemPC()
, m_aPhyFAccessPool()
, m_listPhyFAccessFree()
, m_listPhyFAccessAlloc()
, m_adxfic(nullptr)
, m_bAfsOverrideFlag(false)
, m_szAfsPath()
{
    m_szAfsPath[0] = '\0';
};


CPCFileManager::~CPCFileManager(void)
{
    ;
};


bool CPCFileManager::Start(void)
{
    SetCwd();
    SetAfsPath();
    SetAfsOverrideFlag();
    
    if (!CAdxFileManager::Start())
        return false;

    if (!m_rwFileSystemPC.Initialize())
        return false;
    
    PhyAccessInit();

    return true;
};


void CPCFileManager::Stop(void)
{
    PhyAccessTerm();
    
    m_rwFileSystemPC.Terminate();
    
    CAdxFileManager::Stop();
};


void CPCFileManager::Sync(void)
{
    CAdxFileManager::Sync();
    PhyAccessSync();
};


void CPCFileManager::Error(const char* pszDescription)
{
    CPCError::ShowNoRet(pszDescription);
};


CFileAccess* CPCFileManager::AllocRequest(int32 nType, void* pTypeData)
{
    if (nType == FILETYPE_PC)        
    {
        const char* fname = static_cast<const char*>(pTypeData);

        char szFilepath[256];
        szFilepath[0] = '\0';
        std::strcpy(szFilepath, m_szAfsPath);
        std::strcat(szFilepath, "/");
        std::strcat(szFilepath, fname);

        return PhyAccessAlloc(szFilepath);
    };

    if (nType != CAdxFileManager::FILETYPE_ID)
        return CAdxFileManager::AllocRequest(nType, pTypeData);

    if (m_bAfsOverrideFlag)
    {
        int32 id = reinterpret_cast<int32>(pTypeData);

        char szFilename[256];
        szFilename[0] = '\0';
        CFilename::Filename(szFilename, id);

        char szFilepath[256];
        szFilepath[0] = '\0';
        std::strcpy(szFilepath, m_szAfsPath);
		std::strcat(szFilepath, "/");
        std::strcat(szFilepath, szFilename);

        if (CPCPhyFileAccess::IsExists(szFilepath))
            return PhyAccessAlloc(szFilepath);
    };

    return CAdxFileManager::AllocRequest(nType, pTypeData);
};


bool CPCFileManager::SetupFileSystem(void)
{
    AdxpcSprmFs sprmfs;
    sprmfs.rtdir = m_szAfsPath;
    ADXPC_SetupFileSystem(&sprmfs);
    
    m_adxfic = ADXFIC_Create(m_szAfsPath, 0, 0, 0);
    
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


void CPCFileManager::SetAfsPath(void)
{
    const char* pszArgValue = nullptr;
    
    if (CConfigure::CheckArgValue("afspath", &pszArgValue))
    {
        std::strcpy(m_szAfsPath, pszArgValue);
        //CFilename::ConvPathPlatform(m_szAfsPath);
    }
    else
    {
        char szCwd[MAX_PATH];
        szCwd[0] = '\0';
        
        GetCurrentDirectoryA(COUNT_OF(szCwd), szCwd);

        std::strcpy(m_szAfsPath, szCwd);
    };
};


void CPCFileManager::SetAfsOverrideFlag(void)
{
    m_bAfsOverrideFlag = CConfigure::CheckArg("afsover");
};


void CPCFileManager::SetCwd(void)
{
    char szMyPath[MAX_PATH];
    szMyPath[0] = '\0';

    GetModulePath(szMyPath);

    SetCurrentDirectoryA(szMyPath);
};


void CPCFileManager::PhyAccessInit(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aPhyFAccessPool); ++i)
        m_listPhyFAccessFree.push_back(&m_aPhyFAccessPool[i]);
};


void CPCFileManager::PhyAccessTerm(void)
{
    for (auto it : m_listPhyFAccessAlloc)
        it->Clear();

    PhyAccessSync();
};


void CPCFileManager::PhyAccessSync(void)
{
    auto it = m_listPhyFAccessAlloc.begin();
    while (it != m_listPhyFAccessAlloc.end())
    {
        CPCPhyFileAccess* pAccess = (*it);

        if (pAccess->Stat() == CPCPhyFileAccess::STAT_NOREAD)
        {
            it = m_listPhyFAccessAlloc.erase(it);
            m_listPhyFAccessFree.push_back(pAccess);
        }
        else
        {
            ++it;
        };
    };
};


CPCPhyFileAccess* CPCFileManager::PhyAccessAlloc(const char* pszFilename)
{
    ASSERT(!m_listPhyFAccessFree.empty());

    if (m_listPhyFAccessFree.empty())
        return nullptr;

    CPCPhyFileAccess* pAccess = m_listPhyFAccessFree.front();
    m_listPhyFAccessFree.pop_front();
    m_listPhyFAccessAlloc.push_back(pAccess);
    
	CRequest req(pszFilename, pAccess);
	RegistRequest(req);

    return pAccess;
};