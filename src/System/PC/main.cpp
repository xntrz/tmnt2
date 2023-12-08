#include "PCTypedefs.hpp"
#include "PCDebug.hpp"
#include "PCSpecific.hpp"
#include "PCSetting.hpp"
#include "PCFramework.hpp"
#include "rweval.hpp"

#include "System/Common/Configure.hpp"


#define VLDCHECK
#undef VLDCHECK

#ifdef VLDCHECK
#include "vld.h"
#endif


#ifdef TARGET_PC
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
    
    CConfigure::SetLaunchMode(TYPEDEF::CONFIG_LAUNCH_NORMAL);
    CConfigure::InitArgs(__argc, __argv);

    if(CConfigure::CheckArg("noeval"))
        rwevalInitialize();
    
    CPCSetting::Initialize();
	CPCSpecific::m_hInstance = hInstance;
    
    bool bResult = CPCFramework::StartAndRun();

    CPCSetting::Terminate();

    if (CConfigure::CheckArg("noeval"))
        rwevalTerminate();

#ifdef _DEBUG
    CPCDebug::Terminate();
#endif

    return (bResult ? EXIT_SUCCESS : EXIT_FAILURE);
};
#endif