#include "Configure.hpp"


static char** s_argv = nullptr;
static int32 s_argc = 0;
static char s_argvvdelim = '=';
static char s_argkey = '-';


/*static*/ TYPEDEF::CONFIG_LAUNCH CConfigure::m_launchMode  = TYPEDEF::CONFIG_LAUNCH_NORMAL;
/*static*/ TYPEDEF::CONFIG_LANG CConfigure::m_language      = TYPEDEF::CONFIG_LANG_ENGLISH;
/*static*/ TYPEDEF::CONFIG_TV CConfigure::m_tvMode          = TYPEDEF::CONFIG_TV_NTSC;


/*static*/ void CConfigure::InitArgs(int32 argc, char* argv[])
{
    s_argv = argv;
    s_argc = argc;
};


/*static*/ bool CConfigure::CheckArg(const char* pszArg)
{
    if (!s_argc || !s_argv)
        return false;

    char** argv = s_argv;
    int32 argc = s_argc;

    for (int32 i = 0; i < argc; ++i)
    {
        const char* arg = argv[i];

        if ((arg[0] == s_argkey) && arg[1])
        {
            const char* end = std::strchr(&arg[1], s_argvvdelim);
            
            if (end)
            {
                size_t lenArg = std::strlen(pszArg);
                size_t lenArgV = static_cast<size_t>(end - &arg[1]);
                if (!std::strncmp(&arg[1], pszArg, Min(lenArg, lenArgV)))
                    return true;
            }
            else
            {
                if (!std::strcmp(&arg[1], pszArg))
                    return true;
            };            
        };
    };

    return false;
};


/*static*/ bool CConfigure::CheckArgValue(const char* pszArg, const char** ppszValue)
{
    if (!s_argc || !s_argv)
        return false;

    char** argv = s_argv;
    int32 argc = s_argc;

    for (int32 i = 0; i < argc; ++i)
    {
        const char* arg = argv[i];

        if ((arg[0] == s_argkey) && arg[1])
        {
            const char* argstart = std::strstr(&arg[1], pszArg);
            if(argstart)
            {
                const char* argvalstart = std::strchr(argstart, s_argvvdelim);
                if (argvalstart)
                {
                    ++argvalstart;                        
                    *ppszValue = argvalstart;
                    return true;
                };
            };
        };
    };

    return false;
};


/*static*/ void CConfigure::SetLaunchMode(TYPEDEF::CONFIG_LAUNCH launch)
{
    ASSERT((launch == TYPEDEF::CONFIG_LAUNCH_NORMAL)   ||
           (launch == TYPEDEF::CONFIG_LAUNCH_ARCADE)   ||
           (launch == TYPEDEF::CONFIG_LAUNCH_TVCHANGE) ||
           (launch == TYPEDEF::CONFIG_LAUNCH_DASHBOARD));
    
    m_launchMode = launch;
};


/*static*/ TYPEDEF::CONFIG_LAUNCH CConfigure::GetLaunchMode(void)
{
    return m_launchMode;
};


/*static*/ void CConfigure::SetLanguage(TYPEDEF::CONFIG_LANG lang)
{
    ASSERT((lang == TYPEDEF::CONFIG_LANG_ENGLISH) ||
           (lang == TYPEDEF::CONFIG_LANG_GERMAN)  ||
           (lang == TYPEDEF::CONFIG_LANG_FRENCH)  ||
           (lang == TYPEDEF::CONFIG_LANG_SPANISH) ||
           (lang == TYPEDEF::CONFIG_LANG_ITALIAN));

    m_language = lang;
};


/*static*/ TYPEDEF::CONFIG_LANG CConfigure::GetLanguage(void)
{    
    return m_language;
};


/*static*/ void CConfigure::SetTVMode(TYPEDEF::CONFIG_TV tvMode)
{
    ASSERT((tvMode == TYPEDEF::CONFIG_TV_NTSC) ||
           (tvMode == TYPEDEF::CONFIG_TV_PAL));

    m_tvMode = tvMode;
};


/*static*/ TYPEDEF::CONFIG_TV CConfigure::GetTVMode(void)
{
    return m_tvMode;
};