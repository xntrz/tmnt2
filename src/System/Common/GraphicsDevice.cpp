#include "GraphicsDevice.hpp"
#include "Memory.hpp"
#include "Screen.hpp"
#include "Camera.hpp"
#include "RenderState.hpp"
#include "System2D.hpp"
#include "Sprite.hpp"

#include "rtanim.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rpcollis.h"
#include "rppvs.h"
#include "rplodatm.h"
#include "rprandom.h"
#include "rpmatfx.h"
#include "rpuvanim.h"
#include "rtbmp.h"
#include "rtpng.h"


/*static*/ const RwRGBA CGraphicsDevice::DEFAULT_CLEAR_COLOR = { 0 };


CGraphicsDevice::CGraphicsDevice(void)
: m_pFrameBuffer(nullptr)
, m_pZBuffer(nullptr)
, m_pCamera(nullptr)
, m_ClearColor(DEFAULT_CLEAR_COLOR)
, m_bFlipEnable(true)
, m_iFlipInterval(FLIPINTERVAL_DEFAULT)
{
    ;
};


CGraphicsDevice::~CGraphicsDevice(void)
{
    ;
};


bool CGraphicsDevice::Initialize(void)
{
    bool bResult = false;

    try
    {
        RwMemoryFunctions memfuncs =
        {
            &CMemory::rwmalloc,
            &CMemory::rwfree,
            &CMemory::rwrealloc,
            &CMemory::rwcalloc
        };

        if (!RwEngineInit(&memfuncs, 0x0, 0x400000))
            throw std::exception("Engine init");

        if (!AttachPlugin())
            throw std::exception("Attach plugins");

        RwEngineOpenParams params;
        params.displayID = Configure();
        if (!RwEngineOpen(&params))
            throw std::exception("Engine open");

        if (!RwEngineSetSubSystem(Subsystem()))
            throw std::exception("Subsystem");

        if (!RwEngineSetVideoMode(Videomode()))
            throw std::exception("Videomode");

        ASSERT(((RwGlobals*)RwEngineInstance)->dOpenDevice.fpRenderStateGet);
        ASSERT(((RwGlobals*)RwEngineInstance)->dOpenDevice.fpRenderStateSet);		

        bResult = true;
    }
    catch (std::exception& e)
    {
		REF(e);
        OUTPUT("[SYS] Failed to initialize gx device: %s\n", e.what());
        bResult = false;
    };
    
    return bResult;
};


void CGraphicsDevice::Terminate(void)
{
    RwEngineClose();
    RwEngineTerm();
};


bool CGraphicsDevice::Start(void)
{
    bool bResult = false;

    try
    {
        if (!RwEngineStart())
            throw std::exception("Engine start");

#ifdef RWDEBUG
        CDebug::StartRWDebug();
#endif        

        if (!RwImageRegisterImageFormat("bmp", RtBMPImageRead, 0))
            throw std::exception("Register image format BMP");

        if (!RwImageRegisterImageFormat("png", RtPNGImageRead, 0))
            throw std::exception("Register image format PNG");

        CRenderState::Initialize();
        CCamera::Initialize();

        if (!CreateCamera())
            throw std::exception("Create camera");
        
        ASSERT(!CCamera::CameraDefault());
        CCamera::SetCameraDefault(m_pCamera);

        if (!CreateFrameBuffer())
            throw std::exception("Create frame buffer");

        CScreen::AttachDevice(this);        
        SetFlipInterval(m_iFlipInterval);

        bResult = true;
    }
    catch (std::exception& e)
    {
		REF(e);
        OUTPUT("[SYS] Failed to start gx device: %s\n", e.what());        
        bResult = false;
    };

    return bResult;
};


void CGraphicsDevice::Stop(void)
{
    CScreen::DetachDevice();
    
    DestroyFrameBuffer();

    if (CCamera::CameraDefault() == m_pCamera)
        CCamera::SetCameraDefault(nullptr);

    DestroyCamera();

    CCamera::Terminate();
#ifdef RWDEBUG
    CDebug::StopRWDebug();
#endif
    
    RwEngineStop();
};


bool CGraphicsDevice::RenderBegin(void)
{
    ASSERT(m_pCamera);

    if (!m_bFlipEnable)
        return false;
    
    RwCameraClear(m_pCamera, &m_ClearColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

    if (RwCameraBeginUpdate(m_pCamera) != m_pCamera)
        return false;

    CCamera::SetCameraCurrent(m_pCamera);
    return true;
};


void CGraphicsDevice::RenderEnd(void)
{
    ASSERT(m_pCamera);
    ASSERT(m_pCamera == CCamera::CameraCurrent());

    if (!m_bFlipEnable)
        return;

    RwCameraEndUpdate(m_pCamera);
    CCamera::SetCameraCurrent(nullptr);
};


void CGraphicsDevice::Flip(void)
{
    ASSERT(m_pCamera);
    
	if (m_bFlipEnable)
		RwCameraShowRaster(m_pCamera, 0, rwRASTERFLIPWAITVSYNC);

    CScreen::OnFlip();
};


void CGraphicsDevice::SetFlipInterval(int32 iFlipInterval)
{
    if (iFlipInterval >= FLIPINTERVAL_DEFAULT && iFlipInterval <= FLIPINTERVAL_30)
    {
        m_iFlipInterval = iFlipInterval;
        CScreen::DeviceChanged();
    };
};


int32 CGraphicsDevice::Subsystem(void)
{
    return 0;
};


bool CGraphicsDevice::AttachPlugin(void)
{
    bool bResult = false;

    try
    {
        if (!RpWorldPluginAttach())
            throw std::exception("World");

        if (!RtAnimInitialize())
            throw std::exception("Anim");

        if (!RpHAnimPluginAttach())
            throw std::exception("HAnim");

        if (!RpSkinPluginAttach())
            throw std::exception("Skin");

        //
        // TODO Attach RpToon plugin
        //
        //if (!RpToonPluginAttach())
        //    ASSERT(false);

        if (!RpCollisionPluginAttach())
            throw std::exception("Collision");

        if (!RpPVSPluginAttach())
            throw std::exception("Pvs");

        if (!RpLODAtomicPluginAttach())
            throw std::exception("LODAtomic");

        if (!RpRandomPluginAttach())
            throw std::exception("Random");

        if (!RpMatFXPluginAttach())
            throw std::exception("MatFX");

        if (!RpUVAnimPluginAttach())
            throw std::exception("UVAnim");

        bResult = true;
    }
    catch (std::exception& e)
    {
		REF(e);
        OUTPUT("[SYS] Failed to attach plugin: %s\n", e.what());
        bResult = false;
    };

    return bResult;
};


bool CGraphicsDevice::CreateFrameBuffer(int32 iWidth, int32 iHeight, bool bFullscreen)
{
    ASSERT(!m_pFrameBuffer);
    ASSERT(!m_pZBuffer);
    
    RwVideoMode Videomode = { 0 };
	RwEngineGetVideoModeInfo(&Videomode, RwEngineGetCurrentVideoMode());

    if (iWidth == 0 || iHeight == 0)
    {
        iWidth = Videomode.width;
        iHeight = Videomode.height;

        if (ScreenWidth() < iWidth)
            iWidth = ScreenWidth();
		
        if (ScreenHeight() < iHeight)
            iHeight = ScreenHeight();
    };

    m_pFrameBuffer = RwRasterCreate(iWidth, iHeight, 0, rwRASTERTYPECAMERA);
	ASSERT(m_pFrameBuffer);
    if (!m_pFrameBuffer)
        return false;

    m_pZBuffer = RwRasterCreate(iWidth, iHeight, 0, rwRASTERTYPEZBUFFER);
    if (!m_pZBuffer)
    {
        RwRasterDestroy(m_pFrameBuffer);
        m_pFrameBuffer = nullptr;
        return false;
    };

    //ASSERT(m_pCamera);
    CCamera::FramebufferChanged(m_pFrameBuffer, m_pZBuffer);
    //RwCameraSetRaster(m_pCamera, m_pFrameBuffer);
    //RwCameraSetZRaster(m_pCamera, m_pZBuffer);

    RwV2d ViewWindow;
    ViewWindow.x = TYPEDEF::DEFAULT_VIEWWINDOW;
    ViewWindow.y = TYPEDEF::DEFAULT_VIEWWINDOW / TYPEDEF::DEFAULT_ASPECTRATIO;
    RwCameraSetViewWindow(m_pCamera, &ViewWindow);

    CScreen::DeviceChanged();

	OUTPUT(
		"[SYS] Creating framebuffer with size %d x %d"
		"(aspect ratio : %f, pixel aspect ratio: %d:%d\n",
		iWidth,
		iHeight,
		float(Videomode.width) / float(Videomode.height),
		Videomode.width / Math::Gcd(Videomode.width, Videomode.height),
		Videomode.height / Math::Gcd(Videomode.width, Videomode.height)
	);

    return true;
};


void CGraphicsDevice::DestroyFrameBuffer(void)
{
    //ASSERT(m_pFrameBuffer);
    //ASSERT(m_pZBuffer);

    if(m_pZBuffer)
        RwRasterDestroy(m_pZBuffer);
    
    if (m_pFrameBuffer)
        RwRasterDestroy(m_pFrameBuffer);

    CCamera::FramebufferChanged(nullptr, nullptr);

    m_pFrameBuffer = m_pZBuffer = nullptr;
};


bool CGraphicsDevice::CreateCamera(void)
{    
    ASSERT(!m_pCamera);

    m_pCamera = RwCameraCreate();
    if (!m_pCamera)
        return false;
    
    RwFrame* pRwFrame = RwFrameCreate();
    if (!pRwFrame)
    {
        RwCameraDestroy(m_pCamera);
        m_pCamera = nullptr;
        return false;
    };

    RwFrameSetIdentity(pRwFrame);
    RwCameraSetFrame(m_pCamera, pRwFrame);
    RwCameraSetNearClipPlane(m_pCamera, 0.1f);
    RwCameraSetFarClipPlane(m_pCamera, 100.0f);
    
    return true;
};


void CGraphicsDevice::DestroyCamera(void)
{
    ASSERT(m_pCamera);

    RwFrame* pRwFrame = RwCameraGetFrame(m_pCamera);
    RwCameraSetFrame(m_pCamera, nullptr);
    RwFrameDestroy(pRwFrame);

    RwCameraSetRaster(m_pCamera, nullptr);
    RwCameraSetZRaster(m_pCamera, nullptr);
    RwCameraDestroy(m_pCamera);
    m_pCamera = nullptr;
};


void CGraphicsDevice::CurrentVideomode(int32& iWidth, int32& iHeigh, int32& iDepth)
{
    RwVideoMode Videomode;
    RwEngineGetVideoModeInfo(&Videomode, RwEngineGetCurrentVideoMode());

    iWidth = Videomode.width;
    iHeigh = Videomode.height;
    iDepth = Videomode.depth;
};


void CGraphicsDevice::SetFlipEnable(bool bEnable)
{
    m_bFlipEnable = bEnable;
};


void CGraphicsDevice::SetClearColor(const RwRGBA* pColor)
{
    m_ClearColor = *pColor;
};


void CGraphicsDevice::ResetClearColor(void)
{
    m_ClearColor = DEFAULT_CLEAR_COLOR;
};


int32 CGraphicsDevice::GetFlipInternval(void) const
{
    return m_iFlipInterval;
};