#include "PCTypedefs.hpp"
#include "PCDebug.hpp"
#include "PCSpecific.hpp"

#include "System/Common/main.hpp"

#define VLD_CHECK 1

#ifdef VLD_CHECK
#include "vld.h"
#endif

#ifdef _TARGET_PC
int32 APIENTRY
_tWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInstance,
    LPTSTR		lpCmdLine,
    int32		iCmdShow
)
{
#ifdef VLD_CHECK    
    VLDEnable();
#endif
    
#ifdef _DEBUG
    CPCDebug::Initialize();
#endif
    CPCSpecific::m_hInstance = hInstance;

    int32 iResult = CommonMain(__argc, __argv);

#ifdef _DEBUG
    CPCDebug::Terminate();
#endif

    return iResult;
};
#endif