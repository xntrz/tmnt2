#pragma once


#if (defined(TARGET_PC))

    #if (defined(TMNT2_BUILD_EU))
        #include "FnameTable_PC_EU.hpp"
    #else
        #include "FnameTable_PC_NA.hpp"
    #endif

#else 

#error "Not implemented for current target"

#endif


class CFilename
{
public:
    static const char* m_apszFilename[];
    static const char* m_apszLanguageDataPath[];
    static const char* m_pszCommonDataPath;

public:
    static int32 ID(const char* pszFilename);
    static const char* FullName(int32 id);
    static void ConvPathTable(char* pszPath);
    static void ConvPathPlatform(char* pszPath);
    static void ConvTablePathLanguage(char* pszTablePath);
};