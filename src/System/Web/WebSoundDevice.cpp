#include "WebSoundDevice.hpp"

#include "System/Common/Sound/SdDrv.hpp"

#include <emscripten.h>


CWebSoundDevice::CWebSoundDevice(void)
{
    ;
};


CWebSoundDevice::~CWebSoundDevice(void)
{
    ;
};


bool CWebSoundDevice::Initialize(void)
{
    return true;
};


void CWebSoundDevice::Terminate(void)
{

};


bool CWebSoundDevice::StartupFramework(void)
{
    char szPath[256];
    szPath[0] = '\0';

    uint32 OptFlag = GetFrameworkOptFlags();
    void* HeapPtr = nullptr;
    uint32 HeapSize = 0;

    if (!SdDrvInitialize(0, szPath, HeapPtr, HeapSize, OptFlag))
        return false;

    SdDrvFirstLoad();

    while (SdDrvGetLoadInfo())
    {
        SdDrvTask();
        emscripten_sleep(0);
    };

    return true;
};


void CWebSoundDevice::ShutdownFramework(void)
{
    SdDrvTerminate();
};