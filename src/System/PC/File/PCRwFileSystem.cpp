#include "PCRwFileSystem.hpp"

#include "System/Common/Configure.hpp"
#include "System/PC/PCTypedefs.hpp"

#include "rtfsyst.h"

/* check if "rtfsystwin32.h" already inlined in "rtfsyst.h" */
#if !(defined(WIN_FS_MAX_PATH_LENGTH) || defined(WIN_SECTOR_SIZE))
#include "rtfsystwin32.h"
#endif /* !(defined(WIN_FS_MAX_PATH_LENGTH) || defined(WIN_SECTOR_SIZE)) */


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