#include "PCRwFileSystem.hpp"

#include "System/Common/Configure.hpp"
#include "System/PC/PCTypedefs.hpp"

#include "rtfsystwin32.h"


bool CPCRwFileSystem::Initialize(void)
{
    char szDrive[MAX_PATH];
    szDrive[0] = '\0';

    char szDirectory[MAX_PATH];
    szDirectory[0] = '\0';

    const char* pszAfsPath = nullptr;
    if (CConfigure::CheckArgValue("afspath", &pszAfsPath))
        _splitpath(pszAfsPath, szDrive, nullptr, nullptr, nullptr);
    else
        GetModulePathSplit(szDrive, szDirectory, nullptr, nullptr);

    char fsName[] = "PC";
    RtFileSystem* pWinFS = RtWinFSystemInit(4, szDrive, fsName);
    if (!pWinFS)
        return false;

    return (RtFSManagerRegister(pWinFS) > 0);
};


void CPCRwFileSystem::Terminate(void)
{
    ;
};