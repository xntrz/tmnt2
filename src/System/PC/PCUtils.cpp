#include "PCUtils.hpp"
#include "PCTypedefs.hpp"


/*static*/ void CPCUtils::GetModulePath(char* pszModulePathBuff, int32 buffsize)
{
    GetModuleFileNameA(GetModuleHandle(NULL), pszModulePathBuff, buffsize);
};


/*static*/ void CPCUtils::DisplayCursor(bool bDispFlag)
{
    if (bDispFlag)
    {
        while (ShowCursor(TRUE) < 0)
            ;
    }
    else
    {
        while (ShowCursor(FALSE) >= 0)
            ;
    };
};