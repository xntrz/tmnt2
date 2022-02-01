#include "PCFilename.hpp"

#include "System/PC/PCTypedefs.hpp"


static const char* s_apszResFilename[] =
{
    "TMNT2.ini",
};


/*static*/ bool CPCFilename::RCFileID(const char* pszFilename, int32& nResID)
{
    for (int32 i = 0; i < COUNT_OF(s_apszResFilename); ++i)
    {
        if (!std::strcmp(s_apszResFilename[i], pszFilename))
        {
            nResID = (200 + i);
            return true;
        };
    };

    return false;
};