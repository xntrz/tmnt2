#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>


namespace PC
{
    struct VIDEOMODE
    {
        int32 w;
        int32 h;
        int32 d;

        inline VIDEOMODE(int32 _w, int32 _h, int32 _d) : w(_w), h(_h), d(_d) {};
        inline VIDEOMODE(void) : w(0), h(0), d(0) {};
    };
};


#define GetModulePathSplit(Drive, Dir, FName, FExt)                                 \
do                                                                                  \
{                                                                                   \
    char ModulePath[MAX_PATH];                                                      \
    GetModuleFileNameA(GetModuleHandleA(NULL), ModulePath, COUNT_OF(ModulePath));   \
    _splitpath(ModulePath, Drive, Dir, FName, FExt);                                \
} while (0)


#define GetModulePath(Path)                                 \
do                                                          \
{                                                           \
    char Drive[MAX_PATH];                                   \
    char Directory[MAX_PATH];                               \
                                                            \
    GetModulePathSplit(Drive, Directory, nullptr, nullptr); \
    std::sprintf(Path, "%s%s", Drive, Directory);           \
} while (0)

