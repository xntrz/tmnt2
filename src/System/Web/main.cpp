#include "WebDebug.hpp"
#include "WebFramework.hpp"
#include "WebSpecific.hpp"

#include "System/Common/Configure.hpp"

#include <emscripten.h>


int32 main(int32 argc, char** argv)
{
    CWebSpecific::WaitForMainModuleReady();
    //CWebSpecific::WaitUserActive();

    EM_ASM({ Error.stackTraceLimit = 64; });

    printf("EMSDK VER: %d.%d.%d\n", __EMSCRIPTEN_MAJOR__,
                                    __EMSCRIPTEN_MINOR__,
                                    __EMSCRIPTEN_TINY__);
    
    printf("BUILD VER: %s -- %s\n", __DATE__,
                                    __TIME__);

#if defined(NDEBUG)
    CDebug::Fatal = CWebDebug::Fatal;
#elif defined(_DEBUG)
    CWebDebug::Initialize();
#endif

    CConfigure::SetLaunchMode(TYPEDEF::CONFIG_LAUNCH_NORMAL);
    CConfigure::InitArgs(argc, argv);

    CWebFramework::StartAndRun();

#if defined(NDEBUG)
    CDebug::Fatal = nullptr;
#elif defined(_DEBUG)
    CWebDebug::Terminate();
#endif

    return EXIT_SUCCESS;
};