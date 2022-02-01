#include "PCSetting.hpp"
#include "PCIniVar.hpp"
#include "PCResources.hpp"
#include "PCGraphicsDevice.hpp"

#include "File/PCFileAccess.hpp"
#include "PCTypedefs.hpp"


/*static*/ CPCSetting::VIDEOMODE CPCSetting::CPCSetting::m_videomode = { 640, 480, 32 };
/*static*/ int32 CPCSetting::m_iDispMode = CPCGraphicsDevice::DISPLAYMODE_FULLSCREEN;
/*static*/ int32 CPCSetting::m_iMonitorNo = 0;
/*static*/ bool CPCSetting::m_bFlagClassicPad = false;
/*static*/ bool CPCSetting::m_bFlagFontEffect = true;
/*static*/ bool CPCSetting::m_bFlagAutosave = true;
/*static*/ bool CPCSetting::m_bFlagHelp = true;
/*static*/ int32 CPCSetting::m_iDifficulty = 0;


namespace
{
    template<class T>
    void ReadIniVar(T& t, CIniVar& ini, const char* segment, const char* name)
    {
        try
        {
            t = ini[segment][name];
        }
        catch (CIniVar::exception& e)
        {
			REF(e);
            OUTPUT("[SYS-PC] %s --> %s\n", __FUNCTION__, e.what());
        };
    };


    template<class T>
    void WriteIniVar(T& t, CIniVar& ini, const char* segment, const char* name)
    {
        try
        {
            ini[segment][name] = t;
        }
        catch (CIniVar::exception& e)
        {
			REF(e);
            OUTPUT("[SYS-PC] %s --> %s\n", __FUNCTION__, e.what());
        };
    };


    void LoadIniVar(const char* buffer, int32 bufferSize)
    {
        ASSERT(buffer);
        ASSERT(bufferSize);

        try
        {
            CIniVar ini(buffer, bufferSize, CIniVar::access_read);

            ReadIniVar(CPCSetting::m_videomode.m_iWidth,    ini, "SYSTEM", "gx_width");
            ReadIniVar(CPCSetting::m_videomode.m_iHeight,   ini, "SYSTEM", "gx_height");
            ReadIniVar(CPCSetting::m_videomode.m_iDepth,    ini, "SYSTEM", "gx_depth");
            ReadIniVar(CPCSetting::m_iDispMode,             ini, "SYSTEM", "gx_dispmode");
            ReadIniVar(CPCSetting::m_iMonitorNo,            ini, "SYSTEM", "gx_monitor");

            ReadIniVar(CPCSetting::m_bFlagClassicPad,       ini, "GAME", "gm_classicpad");
            ReadIniVar(CPCSetting::m_bFlagAutosave,         ini, "GAME", "gm_autosave");
            ReadIniVar(CPCSetting::m_bFlagHelp,             ini, "GAME", "gm_help");
            ReadIniVar(CPCSetting::m_bFlagFontEffect,       ini, "GAME", "gm_fontfx");
            ReadIniVar(CPCSetting::m_iDifficulty,           ini, "GAME", "gm_difficulty");
        }
        catch (CIniVar::exception& e)
        {
			REF(e);
            OUTPUT("[SYS-PC] %s --> %s\n", __FUNCTION__, e.what());
        };
    };


    std::string SaveIniVarCommon(CIniVar& ini)
    {
        std::string result;

        try
        {
            WriteIniVar(CPCSetting::m_videomode.m_iWidth,   ini, "SYSTEM", "gx_width");
            WriteIniVar(CPCSetting::m_videomode.m_iHeight,  ini, "SYSTEM", "gx_height");
            WriteIniVar(CPCSetting::m_videomode.m_iDepth,   ini, "SYSTEM", "gx_depth");
            WriteIniVar(CPCSetting::m_iDispMode,            ini, "SYSTEM", "gx_dispmode");
            WriteIniVar(CPCSetting::m_iMonitorNo,           ini, "SYSTEM", "gx_monitor");

            WriteIniVar(CPCSetting::m_bFlagClassicPad,      ini, "GAME", "gm_classicpad");
            WriteIniVar(CPCSetting::m_bFlagAutosave,        ini, "GAME", "gm_autosave");
            WriteIniVar(CPCSetting::m_bFlagHelp,            ini, "GAME", "gm_help");
            WriteIniVar(CPCSetting::m_bFlagFontEffect,      ini, "GAME", "gm_fontfx");
            WriteIniVar(CPCSetting::m_iDifficulty,          ini, "GAME", "gm_difficulty");

            result = ini.save();
        }
        catch (CIniVar::exception& e)
        {
			REF(e);
            OUTPUT("[SYS-PC] %s --> %s\n", __FUNCTION__, e.what());
        };

        return result;
    };

    
    std::string SaveIniVar(const char* buffer, int32 bufferSize)
    {
        std::string result;        

        try
        {
            CIniVar ini(buffer, bufferSize, CIniVar::access_write);
            result = SaveIniVarCommon(ini);
        }
        catch (CIniVar::exception& e)
        {
			REF(e);
            OUTPUT("[SYS-PC] %s --> %s\n", __FUNCTION__, e.what());
        };

        return result;
    };


    std::string SaveIniVar(void)
    {
        std::string result;

        try
        {
            CIniVar ini;
            result = SaveIniVarCommon(ini);
        }
        catch (CIniVar::exception& e)
        {
			REF(e);
            OUTPUT("[SYS-PC] %s --> %s\n", __FUNCTION__, e.what());
        };

        return result;
    };
};


/*static*/ void CPCSetting::Initialize(void)
{
    TCHAR szModulePath[MAX_PATH];
    szModulePath[0] = TEXT('\0');

    GetModuleFileName(NULL, szModulePath, sizeof(szModulePath));
    TCHAR* pszResult = _tcsrchr(szModulePath, TEXT('\\'));
    *pszResult = TEXT('\0');
    SetCurrentDirectory(szModulePath);

    Load();
};


/*static*/ void CPCSetting::Terminate(void)
{
    Save();
};


/*static*/ void CPCSetting::Load(void)
{
    const char* pszFilename = "TMNT2.ini";

    //
    //  Load base 
    //
    CPCResFileAccess resfile;
    if (resfile.Read(IDF_DBG_CFG))
    {
        LoadIniVar((char*)resfile.Data(), (int32)resfile.Size());
        resfile.Clear();
    }
    else
    {
        OUTPUT("[SYS-PC] Reading resource configuration file %s failed!\n", pszFilename);
    };
    
    //
    //  Load overrides if exists
    //
    if (CPCPhysicalFileAccess::IsExists(pszFilename))
    {
        CPCPhysicalFileAccess phyfile;
        if (phyfile.Read(pszFilename))
        {
            LoadIniVar((char*)phyfile.Data(), (int32)phyfile.Size());
            phyfile.Clear();
        }
        else
        {
            OUTPUT("[SYS-PC] Reading physical configuration file %s failed!\n", pszFilename);
        };
    };
};


/*static*/ void CPCSetting::Save(void)
{
    const char* pszFilename = "TMNT2.ini";

    {
        CPCPhysicalFileAccess phyfile;
        
		std::string result = SaveIniVar();
        if (!phyfile.Write(pszFilename, result.data(), result.size()))
            OUTPUT("[SYS-PC] Writing physical configuration file %s failed!\n", pszFilename);
        
        phyfile.Clear();
    }    
};