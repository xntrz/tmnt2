#pragma once


class CConfigure
{
public:    
    static void InitArgs(int32 argc, char* argv[]);
    static void InitLanguange(void);
    static void InitTVMode(void);
    static bool CheckArg(const char* pszArg);
    static bool CheckArgValue(const char* pszArg, const char** ppszValue);
    static void SetLaunchMode(TYPEDEF::CONFIG_LAUNCH launch);
    static TYPEDEF::CONFIG_LAUNCH GetLaunchMode(void);
    static TYPEDEF::CONFIG_LANG GetLanguage(void);
    static TYPEDEF::CONFIG_TV GetTVMode(void);

private:
    static TYPEDEF::CONFIG_LAUNCH m_launchmode;
    static TYPEDEF::CONFIG_LANG m_language;
    static TYPEDEF::CONFIG_TV m_tvmode;
};