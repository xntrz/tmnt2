#include "Configure.hpp"


static char** s_argv = nullptr;
static int32 s_argc = 0;
static char s_argvvdelim = '=';
static char s_argkey = '-';


/*static*/ TYPEDEF::CONFIG_LAUNCH CConfigure::m_launchmode = TYPEDEF::CONFIG_LAUNCH_NORMAL;
/*static*/ TYPEDEF::CONFIG_LANG CConfigure::m_language;
/*static*/ TYPEDEF::CONFIG_TV CConfigure::m_tvmode;


/*static*/ void CConfigure::InitArgs(int32 argc, char* argv[])
{
    s_argv = argv;
    s_argc = argc;
};


/*static*/ void CConfigure::InitLanguange(void)
{
    ;
};


/*static*/ void CConfigure::InitTVMode(void)
{
    ;
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
            if (!std::strcmp(&arg[1], pszArg))
                return true;
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
    ASSERT(
        (launch == TYPEDEF::CONFIG_LAUNCH_NORMAL)   ||
        (launch == TYPEDEF::CONFIG_LAUNCH_ARCADE)   ||
        (launch == TYPEDEF::CONFIG_LAUNCH_TVCHANGE) ||
        (launch == TYPEDEF::CONFIG_LAUNCH_DASHBOARD)
    );
    
    m_launchmode = launch;
};


/*static*/ TYPEDEF::CONFIG_LAUNCH CConfigure::GetLaunchMode(void)
{
    return m_launchmode;
};


/*static*/ TYPEDEF::CONFIG_LANG CConfigure::GetLanguage(void)
{
    return m_language;
};


/*static*/ TYPEDEF::CONFIG_TV CConfigure::GetTVMode(void)
{
    return m_tvmode;
};