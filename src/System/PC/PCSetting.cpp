#include "PCSetting.hpp"
#include "PCSpecific.hpp"
#include "PCResources.hpp"
#include "PCGraphicsDevice.hpp"

#include "PCTypedefs.hpp"


/*static*/ CPCSetting::VIDEOMODE CPCSetting::CPCSetting::m_videomode = { 640, 480, 32 };
/*static*/ int32 CPCSetting::m_iDispMode = CPCGraphicsDevice::DISPLAYMODE_FULLSCREEN;
/*static*/ int32 CPCSetting::m_iMonitorNo = 0;
///*static*/ bool CPCSetting::m_bFlagClassicPad = false;
///*static*/ bool CPCSetting::m_bFlagFontEffect = true;
///*static*/ bool CPCSetting::m_bFlagAutosave = true;
///*static*/ bool CPCSetting::m_bFlagHelp = true;
///*static*/ int32 CPCSetting::m_iDifficulty = 0;


/*static*/ void CPCSetting::Initialize(void)
{
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
        m_videomode.m_iWidth = std::atol(szBuff);

    if (GetPrivateProfileStringA("SCREEN", "HEIGHT", "480", szBuff, COUNT_OF(szBuff), Path.c_str()))
        m_videomode.m_iHeight = std::atol(szBuff);

    if (GetPrivateProfileStringA("SCREEN", "DEPTH", "32", szBuff, COUNT_OF(szBuff), Path.c_str()))
        m_videomode.m_iDepth = std::atol(szBuff);

    if (GetPrivateProfileStringA("SCREEN", "MODE", "FULLSCREEN", szBuff, COUNT_OF(szBuff), Path.c_str()))
    {
        if (!std::strcmp(szBuff, "FULLSCREEN"))
        {
            m_iDispMode = CPCGraphicsDevice::DISPLAYMODE_FULLSCREEN;
        }
        else if (!std::strcmp(szBuff, "BORDERLESS"))
        {
            m_iDispMode = CPCGraphicsDevice::DISPLAYMODE_FULLWINDOW;
        }
        else if (!std::strcmp(szBuff, "WINDOW"))
        {
            m_iDispMode = CPCGraphicsDevice::DISPLAYMODE_WINDOW;
        };
    };

    if (GetPrivateProfileStringA("SCREEN", "MONITOR", "-1", szBuff, COUNT_OF(szBuff), Path.c_str()))
        m_iMonitorNo = std::atol(szBuff);

    //m_iDispMode = CPCGraphicsDevice::DISPLAYMODE_WINDOW;
};


/*static*/ void CPCSetting::Save(void)
{
    std::string Path;
    GetIniPath(Path);

    char szBuff[256];
    szBuff[0] = '\0';

    std::sprintf(szBuff, "%d", m_videomode.m_iWidth);
    WritePrivateProfileStringA("SCREEN", "WIDTH", szBuff, Path.c_str());

    std::sprintf(szBuff, "%d", m_videomode.m_iHeight);
    WritePrivateProfileStringA("SCREEN", "HEIGHT", szBuff, Path.c_str());

    std::sprintf(szBuff, "%d", m_videomode.m_iDepth);
    WritePrivateProfileStringA("SCREEN", "DEPTH", szBuff, Path.c_str());

    static const char* s_apszDispMode[] = { "FULLSCREEN", "BORDERLESS", "WINDOW", };
    WritePrivateProfileStringA("SCREEN", "MODE", s_apszDispMode[m_iDispMode], Path.c_str());

    std::sprintf(szBuff, "%d", m_iMonitorNo);
    WritePrivateProfileStringA("SCREEN", "MONITOR", szBuff, Path.c_str());
};


/*static*/ void CPCSetting::GetIniPath(std::string& Path)
{
    char szBuff[MAX_PATH];
    szBuff[0] = '\0';    
    CPCSpecific::GetModulePath(szBuff, COUNT_OF(szBuff));

    char szDrive[32];
    char szDir[256];
    char szFname[128];
    szDrive[0] = '\0';
    szDir[0] = '\0';
    szFname[0] = '\0';    
    _splitpath(szBuff, szDrive, szDir, szFname, nullptr);

    Path += szDrive;
    Path += szDir;
    Path += szFname;
    Path += ".ini";
};