#include "PCTypedefs.hpp"
#include "PCDebug.hpp"
#include "PCSpecific.hpp"
#include "PCSetting.hpp"
#include "PCFramework.hpp"

#include "System/Common/Configure.hpp"


#define VLDCHECK
#undef VLDCHECK

#ifdef VLDCHECK
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
#ifdef VLDCHECK    
    VLDEnable();
#endif

#ifdef _DEBUG
    CPCDebug::Initialize();
#endif
    CPCSetting::Initialize();
    CPCSpecific::m_hInstance = hInstance;
    CPCSpecific::Initialize();

    CConfigure::InitLaunchmode(CConfigure::LAUNCHMODE_NORMAL);
    CConfigure::InitArgs(__argc, __argv);
    
    bool bResult = CPCFramework::StartAndRun();

    CPCSpecific::Terminate();
    CPCSetting::Terminate();
#ifdef _DEBUG
    CPCDebug::Terminate();
#endif

    return (!!bResult);
};
#endif