#pragma once


class CConfigure
{
public:
    enum LAUNCHMODE
    {
        LAUNCHMODE_NORMAL = 0,
        LAUNCHMODE_ARCADE,
        LAUNCHMODE_TVCHANGE,
        LAUNCHMODE_DASHBOARD,
    };
    
public:
    static LAUNCHMODE m_launchmode;
    
    static void InitLaunchmode(LAUNCHMODE launcmode);
    static void InitArgs(int32 argc, char* argv[]);
    static bool CheckArg(const char* pszArg);
    static bool CheckArgValue(const char* pszArg, const char** ppszValue);
};