#include "PCFileManager.hpp"

#include "../PCTypedefs.hpp"
#include "../PCError.hpp"

#include "System/Common/Configure.hpp"
#include "System/Common/File/Filename.hpp"

#include <mmsystem.h>
#include "adx_pc.h"


CPCFileManager::CPCFileManager(void)
: m_rwFileSystemPC()
, m_adxfic(NULL)
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
    
    if (!CAdxFileManager::Start())
        return false;

    if (!m_rwFileSystemPC.Initialize())
        return false;

    return true;
};


void CPCFileManager::Stop(void)
{
    m_rwFileSystemPC.Terminate();
    
    CAdxFileManager::Stop();
};


void CPCFileManager::Error(const char* pszDescription)
{
    CPCError::ShowNoRet(pszDescription);
};


bool CPCFileManager::SetupFileSystem(void)
{
    ADXPC_SPRM_WINFS prm;
    prm.rtdir = m_szAfsPath;
    ADXPC_SetupWinFs(&prm);

    m_adxfic = ADXFIC_Create(m_szAfsPath, ADXFIC_MODE_CURRENT, NULL, 0);
    
    return (m_adxfic != NULL);
};


void CPCFileManager::ShutdownFileSystem(void)
{
    if (m_adxfic)
    {
        ADXFIC_Destroy(m_adxfic);
        m_adxfic = NULL;
    };

    ADXPC_ShutdownWinFs();
};


void CPCFileManager::SetAfsPath(void)
{
    const char* pszArgValue = nullptr;
    
    if (CConfigure::CheckArgValue("afspath", &pszArgValue))
    {
        std::strcpy(m_szAfsPath, pszArgValue);
        CFilename::ConvPathPlatform(m_szAfsPath);
    }
    else
    {
        char szCwd[MAX_PATH];
        szCwd[0] = '\0';
        
        GetCurrentDirectory(COUNT_OF(szCwd), szCwd);

        std::strcpy(m_szAfsPath, szCwd);
    };
};


void CPCFileManager::SetCwd(void)
{
    char szMyPath[MAX_PATH];
    szMyPath[0] = '\0';

    const char* pszAfsPath = nullptr;
    if (CConfigure::CheckArgValue("afspath", &pszAfsPath))
        std::strcpy(szMyPath, pszAfsPath);
    else
        GetModulePath(szMyPath);

    SetCurrentDirectoryA(szMyPath);
};


//
// *********************************************************************************
//


CPCFileManagerReadLog::CPCFileManagerReadLog(void)
: m_readLog(nullptr)
{
    ;
};


bool CPCFileManagerReadLog::Start(void)
{
    bool bResult = CPCFileManager::Start();

    m_readLog = std::fopen("readlog.txt", "w");

    return bResult;
};


void CPCFileManagerReadLog::Stop(void)
{
    if (m_readLog != nullptr)
    {
        std::fclose(m_readLog);
        m_readLog = nullptr;
    };

    CPCFileManager::Stop();
};


void CPCFileManagerReadLog::ReadEnd(CFileAccess* pData)
{
    if (m_readLog != nullptr)
        std::fprintf(m_readLog, "%s\n", m_pAccessData->GetLastFilename());
};