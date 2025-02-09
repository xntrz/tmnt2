#include "Screen.hpp"
#include "Sprite.hpp"
#include "System2D.hpp"
#include "GraphicsDevice.hpp"
#include "Configure.hpp"


static float            s_fFramerate = 60.0f;
static float            s_fTimer = 0.0f;
static float            s_fTimerStride = (1.0f / s_fFramerate);
static int32            s_iWidth = 0;
static int32            s_iHeight = 0;
static int32            s_iDepth = 0;
static CGraphicsDevice* s_pDevice = nullptr;


/*static*/ void CScreen::AttachDevice(CGraphicsDevice* pDevice)
{
    ASSERT(s_pDevice == nullptr);
    
    s_pDevice = pDevice;
    s_fTimer  = 0.0f;
    
    DeviceChanged();
};


/*static*/ void CScreen::DetachDevice(void)
{
    ASSERT(s_pDevice != nullptr);
    
    s_pDevice = nullptr;
};


/*static*/ void CScreen::DeviceChanged(void)
{
    if (s_pDevice)
    {
        s_iWidth  = Device().ScreenWidth();
        s_iHeight = Device().ScreenHeight();
        s_iDepth  = Device().ScreenDepth();

        int32 nVsyncCount = Device().GetFlipInternval();        
        if (nVsyncCount < 1)
            nVsyncCount = 1;

        float fRefreshRate = 60.0f;
#ifdef TMNT2_BUILD_EU
        if ((CConfigure::GetTVMode() == TYPEDEF::CONFIG_TV_PAL) && Device().IsPalMode())
            fRefreshRate = 50.0f;
#endif /* TMNT2_BUILD_EU */        

        s_fFramerate = (fRefreshRate / static_cast<float>(nVsyncCount));
        ASSERT(s_fFramerate > 0.0f);
        
        s_fTimerStride = (1.0f / s_fFramerate);
    };
};


/*static*/ void CScreen::SetFlipEnable(bool bEnable)
{
    Device().SetFlipEnable(bEnable);
};


/*static*/ void CScreen::SetPalMode(bool bEnable)
{
#ifdef TMNT2_BUILD_EU
    Device().SetPalMode(bEnable);
    DeviceChanged();
#endif /* TMNT2_BUILD_EU */ 
};


/*static*/ void CScreen::SetFlipInterval(int32 nVsyncCount)
{
    Device().SetFlipInterval(nVsyncCount);
};


/*static*/ void CScreen::SetClearColor(const RwRGBA* pColor)
{
    Device().SetClearColor(pColor);
};


/*static*/ void CScreen::SetClearColor(const RwRGBA& rColor)
{
    SetClearColor(&rColor);
};


/*static*/ void CScreen::ResetClearColor(void)
{
    Device().ResetClearColor();
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


/*static*/ CGraphicsDevice& CScreen::Device(void)
{
    ASSERT(s_pDevice != nullptr);
    return *s_pDevice;
};