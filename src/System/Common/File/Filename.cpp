#include "Filename.hpp"
#include "FileTypes.hpp"

#include "System/Common/Configure.hpp"


#define FILETABLE_IMPL


#if (defined(TARGET_PC))

    #if (defined(TMNT2_BUILD_EU))
        #include "FnameTable_PC_EU.hpp"
    #else
        #include "FnameTable_PC_NA.hpp"
    #endif

#else 

#error "Not implemented for current target"

#endif


/*static*/ const char* CFilename::m_apszLanguageDataPath[] =
{
    "language/english/",
#ifdef TMNT2_BUILD_EU
    "language/german/",
    "language/french/",
    "language/spanish/",
    "language/italian/",
#endif /* TMNT2_BUILD_EU */
};


/*static*/ const char* CFilename::m_pszCommonDataPath = "common/";


/*static*/ int32 CFilename::ID(const char* pszFilename)
{
    char szFilename[FILETYPES::FILE_NAME_MAX];
    szFilename[0] = '\0';
    
    std::strcpy(szFilename, pszFilename);
    ConvPathTable(szFilename);
#if defined(TMNT2_BUILD_EU) && !defined(TMNT2_IDFSYST)
    ConvTablePathLanguage(szFilename);
#endif /* defined(TMNT2_BUILD_EU) && !defined(TMNT2_IDFSYST) */

    size_t len = std::strlen(m_pszCommonDataPath);
    if (!std::strncmp(szFilename, m_pszCommonDataPath, len))
    {
        for (int32 i = FILEID::COMMON_BEGIN; i < FILEID::COMMON_END; ++i)
        {
            if (!std::strcmp(&szFilename[len], m_apszFilename[i]))
                return i;
        };
    };

    TYPEDEF::CONFIG_LANG lang = TYPEDEF::CONFIG_LANG_ENGLISH;
#ifdef TMNT2_BUILD_EU
    lang = CConfigure::GetLanguage();
#endif /* TMNT2_BUILD_EU */

    len = std::strlen(m_apszLanguageDataPath[lang]);
    if (!std::strncmp(szFilename, m_apszLanguageDataPath[lang], len))
    {
        for (int32 i = FILEID::LANGUAGE_BEGIN; i < FILEID::LANGUAGE_END; ++i)
        {
            if (!std::strcmp(&szFilename[len], m_apszFilename[i]))
                return i;
        };
    };

    return -1;
};


/*static*/ const char* CFilename::FullName(int32 id)
{
    static char s_szFilename[FILETYPES::FILE_NAME_MAX];

    ASSERT(id >= 0);
    ASSERT(id < FILEID::FILEID_MAX);

    if ((id >= FILEID::COMMON_BEGIN) &&
        (id < FILEID::COMMON_END))
    {
        std::strcpy(s_szFilename, "common/");
    }
    else if ((id >= FILEID::LANGUAGE_BEGIN) &&
             (id < FILEID::LANGUAGE_END))
    {
        std::strcpy(s_szFilename, "language/");
    };

    std::strcat(s_szFilename, m_apszFilename[id]);

    return s_szFilename;
};


/*static*/ void CFilename::ConvPathTable(char* pszPath)
{
    while (*pszPath)
    {
        if (*pszPath == '\\')
        {
            *pszPath = '/';
        }
        else if ((*pszPath >= 'A') &&
                 (*pszPath <= 'Z'))
        {
            *pszPath = (*pszPath + 0x20); // to lower case
        };

        ++pszPath;
    };
};


/*static*/ void CFilename::ConvPathPlatform(char* pszPath)
{
    while (*pszPath)
    {
#ifdef TARGET_PC
        if (*pszPath == '/')
            *pszPath = '\\';
#else /* TARGET_PC */
        if (*Path == '\\')
            *Path = '/';
#endif /* TARGET_PC */
        ++pszPath;
    };
};


/*static*/ void CFilename::ConvTablePathLanguage(char* pszTablePath)
{
    const char szDefaultLanguagePath[] = "language/english/";

    if (std::strstr(pszTablePath, szDefaultLanguagePath))
    {
        char szNewFilename[FILETYPES::FILE_NAME_MAX];
        szNewFilename[0] = '\0';

        const char* pszCurrentLanguagePath = m_apszLanguageDataPath[CConfigure::GetLanguage()];

        std::strcpy(szNewFilename, pszCurrentLanguagePath);
        std::strcat(szNewFilename, &pszTablePath[sizeof(szDefaultLanguagePath) - 1]);

        std::strcpy(pszTablePath, szNewFilename);
    };
};