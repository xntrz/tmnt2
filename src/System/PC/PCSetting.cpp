#include "PCSetting.hpp"
#include "PCSpecific.hpp"
#include "PCGraphicsDevice.hpp"

#include "System/Common/Configure.hpp"


/*static*/ const PC::VIDEOMODE CPCSetting::VIDEOMODE_DEFAULT = { 640, 480, 32 };
/*static*/ PC::VIDEOMODE CPCSetting::m_videomode;
/*static*/ bool CPCSetting::m_bWindowMode;


/*static*/ void CPCSetting::Initialize(void)
{
    m_videomode = VIDEOMODE_DEFAULT;
#ifdef _DEBUG    
    m_bWindowMode = true;
#else
    m_bWindowMode = CConfigure::CheckArg("wnd");
#endif    

    Load();
};


/*static*/ void CPCSetting::Terminate(void)
{
    Save();
};


/*static*/ void CPCSetting::Load(void)
{
    std::string Path;
    GetIniPath(Path);

    char szBuff[256];
    szBuff[0] = '\0';

    if (GetPrivateProfileStringA("SCREEN", "WIDTH", "640", szBuff, COUNT_OF(szBuff), Path.c_str()))
        m_videomode.w = std::atol(szBuff);

    if (GetPrivateProfileStringA("SCREEN", "HEIGHT", "480", szBuff, COUNT_OF(szBuff), Path.c_str()))
        m_videomode.h = std::atol(szBuff);

    if (GetPrivateProfileStringA("SCREEN", "DEPTH", "32", szBuff, COUNT_OF(szBuff), Path.c_str()))
        m_videomode.d = std::atol(szBuff);
};


/*static*/ void CPCSetting::Save(void)
{
    std::string Path;
    GetIniPath(Path);

    char szBuff[256];
    szBuff[0] = '\0';

    std::sprintf(szBuff, "%d", m_videomode.w);
    WritePrivateProfileStringA("SCREEN", "WIDTH", szBuff, Path.c_str());

    std::sprintf(szBuff, "%d", m_videomode.h);
    WritePrivateProfileStringA("SCREEN", "HEIGHT", szBuff, Path.c_str());

    std::sprintf(szBuff, "%d", m_videomode.d);
    WritePrivateProfileStringA("SCREEN", "DEPTH", szBuff, Path.c_str());
};


/*static*/ void CPCSetting::GetIniPath(std::string& Path)
{
    char szDrive[MAX_PATH];
    szDrive[0] = '\0';
    
    char szDir[MAX_PATH];
    szDir[0] = '\0';
    
    char szFname[MAX_PATH];
    szFname[0] = '\0';
    
    GetModulePathSplit(szDrive, szDir, szFname, nullptr);

    Path.clear();
    Path += szDrive;
    Path += szDir;
    Path += szFname;
    Path += ".ini";
};