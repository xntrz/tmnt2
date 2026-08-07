#include "WebFileManager.hpp"

#include "System/Common/File/RwFileSystem.hpp"
#include "System/Common/Configure.hpp"
#include "System/Web/WebSpecific.hpp"
#include "System/Web/WebError.hpp"

#include "rtfsyst.h"
#include "adx_pc.h"

#include <emscripten.h>


#define WEBFILE_BASE_URL "/data"

#ifdef TMNT2_TRIAL
    #define WEBFILE_BUILD_URL (WEBFILE_BASE_URL"/trial")
#else
    #ifdef TMNT2_BUILD_EU
        #define WEBFILE_BUILD_URL (WEBFILE_BASE_URL"/eu")
    #else
        #define WEBFILE_BUILD_URL (WEBFILE_BASE_URL"/na")
    #endif
#endif

#define WEBFILE_CDN_URL WEBFILE_BUILD_URL


/*static*/ bool CWebFileManager::m_bRFS = false;


/*static*/ CWebFileManager& CWebFileManager::Instance(void)
{
    return static_cast<CWebFileManager&>(CAdxFileManager::Instance());
};


/*static*/ const char* CWebFileManager::MakePath(const char* pszFilepath)
{
    static char s_szFilepath[256];
    s_szFilepath[0] = '\0';
    
    std::strcpy(s_szFilepath, m_bRFS ? "rfs:" : "wfs:");
    std::strcat(s_szFilepath, pszFilepath);

    return s_szFilepath;
};


CWebFileManager::CWebFileManager(void)
: m_idbfs()
, m_rfs()
, m_stdfs()
, m_wfs()
, m_bRun(false)
{
    ;
};


CWebFileManager::~CWebFileManager(void)
{
    ;
};


bool CWebFileManager::Start(void)
{
    m_bRFS = CConfigure::CheckArg("rfs");

    if (!CAdxFileManager::Start())
        return false;

    m_bRun = true;

    return true;
};


void CWebFileManager::Stop(void)
{
    CAdxFileManager::Stop();
};


void CWebFileManager::Sync(void)
{
    if (m_idbfs.Sync())
    {
        m_stdfs.Sync();
        m_rfs.Sync();
        m_wfs.Sync();

        if (IsAdxWork() || !m_bRun)
            CAdxFileManager::Sync();
    };
};


void CWebFileManager::Error(const char* pszDescription)
{
    CWebError::ShowNoRet(pszDescription);
};


bool CWebFileManager::SetupFileSystem(void)
{
    if (!m_stdfs.Initialize("std", "std:"))
    {
        OUTPUT("STDFS init failed!\n");
        return false;
    };

    m_stdfs.SetRedirectFromAfs();

    if (!m_idbfs.Initialize("ifs", "ifs:", "/ifs"))
    {
        OUTPUT("IDBFS init failed!\n");
        return false;
    };

    if (!m_rfs.Initialize("rfs", "rfs:", WEBFILE_CDN_URL))
    {
        OUTPUT("REMOTE fs init failed!\n");
        return false;
    };

    const char* pszMountPathWFS = "/wfs/";

    /* redirect WFS to MEMFS if RFS enabled or if trial build */
    if (m_bRFS)
        pszMountPathWFS = nullptr;

#ifdef TMNT2_TRIAL
    pszMountPathWFS = nullptr;
#endif /* TMNT2_TRIAL */

    if (!m_wfs.Initialize("wfs", "wfs:", pszMountPathWFS))
    {
        OUTPUT("WORKER fs init failed!\n");
        return false;
    };

    char rtdir[256];
    std::strcpy(rtdir, m_bRFS ? "rfs:" : "wfs:");

    ADXPC_SPRM_WINFS prm;
    prm.rtdir = rtdir;

    ADXPC_SetupWinFs(&prm);

    return true;
};


void CWebFileManager::ShutdownFileSystem(void)
{
    ADXPC_ShutdownWinFs();

    m_wfs.Terminate();
    m_rfs.Terminate();    
    m_idbfs.Terminate();
    m_stdfs.ClrRedirectFromAfs();
    m_stdfs.Terminate();
};


void CWebFileManager::SyncWait(void)
{
    m_stdfs.Sync();
    m_rfs.Sync();
    m_wfs.Sync();
    CAdxFileManager::Sync();
};


bool CWebFileManager::IsAdxWork(void) const
{
    return (!m_readQueue.is_empty()) ||
           (m_pAccessData != nullptr) ||
           (m_stat == STAT_BUSY);
};