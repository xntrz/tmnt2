#include "WebGraphicsDevice.hpp"
#include "WebSpecific.hpp"
#include "WebError.hpp"

#include "SDL3/SDL.h"


static RwInt32
RwDrvGetWindowHeightFunc(void)
{
    int32 w = 0;
    int32 h = 0;
    CWebSpecific::GetCanvasSize(&w, &h);
    
    return h;
};


static RwInt32
RwDrvGetVideoModeNumFunc(void)
{
    return 1;
};


static RwBool
RwDrvGetVideoModeInfoFunc(RwInt32 modeIndex, RwVideoMode* info)
{
    if (modeIndex == 0)
    {
        int32 w = 0;
        int32 h = 0;
        CWebSpecific::GetCanvasSize(&w, &h);

        std::memset(info, 0, sizeof(*info));
        info->width = w;
        info->height = h;
        info->refRate = 60;

        return TRUE;
    };

    return FALSE;
};


static RwBool
RwDrvChangeVideoModeFunc(RwInt32 modeIndex)
{
    //
    //  Web is always in windowed mode
    //  so do nothing - return always TRUE
    // 

    return TRUE;
};


CWebGraphicsDevice::CWebGraphicsDevice(void)
{
    ;
};


CWebGraphicsDevice::~CWebGraphicsDevice(void)
{
    ;
};


bool CWebGraphicsDevice::Initialize(void)
{
    RwEngineDrvCallbacks callbacks = {};
    callbacks.fpGetWindowHeight = &RwDrvGetWindowHeightFunc;
    callbacks.fpGetVideoModeNum = &RwDrvGetVideoModeNumFunc;
    callbacks.fpGetVideoModeInfo= &RwDrvGetVideoModeInfoFunc;
    callbacks.fpChangeVideoMode = &RwDrvChangeVideoModeFunc;

    RwEngineSetDrvCallbacks(&callbacks);

    if (!CGraphicsDevice::Initialize())
    {
        CWebError::ShowNoRet("Video Initialization Failed");
        return false;
    };

    return true;
};


void CWebGraphicsDevice::Terminate(void)
{
    CGraphicsDevice::Terminate();
};


bool CWebGraphicsDevice::Start(void)
{
    if (!CGraphicsDevice::Start())
    {
        CWebError::ShowNoRet("Video Start Failed");
        return false;
    };

    RwImageSetGamma(1.2f);
    
    return true;
};


void CWebGraphicsDevice::Flip(void)
{
    CGraphicsDevice::Flip();
};


int32 CWebGraphicsDevice::ScreenWidth(void)
{
    int32 w = 0;
    int32 h = 0;
    CWebSpecific::GetCanvasSize(&w, &h);

    return w;
};


int32 CWebGraphicsDevice::ScreenHeight(void)
{
    int32 w = 0;
    int32 h = 0;
    CWebSpecific::GetCanvasSize(&w, &h);
    
    return h;
};


int32 CWebGraphicsDevice::ScreenDepth(void)
{
    return 24;
};


void* CWebGraphicsDevice::Configure(void)
{
    return nullptr;
};


int32 CWebGraphicsDevice::Subsystem(void)
{
    return 0;
};


int32 CWebGraphicsDevice::Videomode(void)
{
    return 0;
};


bool CWebGraphicsDevice::CreateFrameBuffer(void)
{
    return CGraphicsDevice::CreateFrameBuffer();
};


void CWebGraphicsDevice::DestroyFrameBuffer(void)
{
    CGraphicsDevice::DestroyFrameBuffer();
};