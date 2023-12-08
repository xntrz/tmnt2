#include "Screen.hpp"
#include "Sprite.hpp"
#include "System2D.hpp"
#include "GraphicsDevice.hpp"


static float s_fFramerate = 60.0f;
static float s_fTimer = 0.0f;
static float s_fTimerStride = (1.0f / s_fFramerate);
static int32 s_iWidth = 0;
static int32 s_iHeight = 0;
static int32 s_iDepth = 0;
static CGraphicsDevice* s_pDevice = nullptr;


/*static*/ void CScreen::AttachDevice(CGraphicsDevice* pDevice)
{
    ASSERT(!s_pDevice);
    
    s_pDevice = pDevice;
    s_fTimer = 0.0f;
    
    DeviceChanged();
};


/*static*/ void CScreen::DetachDevice(void)
{   
    s_pDevice = nullptr;
};


/*static*/ void CScreen::DeviceChanged(void)
{
    if (s_pDevice)
    {
        s_iWidth = s_pDevice->ScreenWidth();
        s_iHeight = s_pDevice->ScreenHeight();
        s_iDepth = s_pDevice->ScreenDepth();

        int32 nVsyncCount = s_pDevice->GetFlipInternval();        
        if (nVsyncCount == 0)
            nVsyncCount = 1;
        
        s_fFramerate = 60.0f / float(nVsyncCount);
        ASSERT(s_fFramerate > 0.0f);
        
        s_fTimerStride = 1.0f / s_fFramerate;
    };
};


/*static*/ void CScreen::SetFlipEnable(bool bEnable)
{
    ASSERT(s_pDevice);

    s_pDevice->SetFlipEnable(bEnable);
};


/*static*/ void CScreen::SetFlipInterval(int32 nVsyncCount)
{
    ASSERT(s_pDevice);
    
    s_pDevice->SetFlipInterval(nVsyncCount);
};


/*static*/ void CScreen::SetClearColor(const RwRGBA* pColor)
{
    ASSERT(s_pDevice);

    s_pDevice->SetClearColor(pColor);
};


/*static*/ void CScreen::SetClearColor(const RwRGBA& rColor)
{
    SetClearColor(&rColor);
};


/*static*/ void CScreen::ResetClearColor(void)
{
    ASSERT(s_pDevice);

    s_pDevice->ResetClearColor();
};


/*static*/ void CScreen::OnFlip(void)
{
    s_fTimer += s_fTimerStride;
};


/*static*/ float CScreen::Framerate(void)
{
    return s_fFramerate;
};


/*static*/ float CScreen::TimerStride(void)
{
    return s_fTimerStride;
};


/*static*/ float CScreen::TimeElapsed(void)
{
    return s_fTimer;
};


/*static*/ int32 CScreen::Width(void)
{
    return s_iWidth;
};


/*static*/ int32 CScreen::Height(void)
{
    return s_iHeight;
};