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
#include "rtcharse.h"


/*static*/ const RwRGBA CGraphicsDevice::DEFAULT_CLEAR_COLOR = { 0 };


CGraphicsDevice::CGraphicsDevice(void)
: m_pFrameBuffer(nullptr)
, m_pZBuffer(nullptr)
, m_pCamera(nullptr)
, m_ClearColor(DEFAULT_CLEAR_COLOR)
, m_bFlipEnable(true)
, m_iFlipInterval(0)
{
	;
};


CGraphicsDevice::~CGraphicsDevice(void)
{
    ;
};


bool CGraphicsDevice::Initialize(void)
{
    RwMemoryFunctions memfuncs =
    {
        &CMemory::rwmalloc,
        &CMemory::rwfree,
        &CMemory::rwrealloc,
        &CMemory::rwcalloc
    };

    if (!RwEngineInit(&memfuncs, 0x0, 0x400000))
    {
        OUTPUT("RwEngineInit failed");
        return false;
    };

	if (!AttachPlugin())
	{
		OUTPUT("attach plugin failed");
		return false;
	};

    RwEngineOpenParams params;
    params.displayID = Configure();
    if (!RwEngineOpen(&params))
    {
        OUTPUT("RwEngineOpen failed");
        return false;
    };

    if (!RwEngineSetSubSystem(Subsystem()))
    {
        OUTPUT("RwEngineSetSubSystem failed");
        return false;
    };

    if (!RwEngineSetVideoMode(Videomode()))
    {
        OUTPUT("RwEngineSetVideoMode failed");
        return false;
    };
    
    return true;
};


void CGraphicsDevice::Terminate(void)
{
    RwEngineClose();
    RwEngineTerm();
};


bool CGraphicsDevice::Start(void)
{
    if (!RwEngineStart())
    {
        OUTPUT("RwEngineStart failed");
        return false;
    };

#ifdef RWDEBUG
    CDebug::StartRwDebug();
    RtCharsetOpen();
#endif        

    if (!RwImageRegisterImageFormat("bmp", RtBMPImageRead, 0))
    {
        OUTPUT("RwImageRegisterImageFormat 'bmp' failed");
        return false;
    };

    if (!RwImageRegisterImageFormat("png", RtPNGImageRead, 0))
    {
        OUTPUT("RwImageRegisterImageFormat 'png' failed");
        return false;
    };

    CRenderState::Initialize();

    if (!CreateFrameBuffer())
    {
        OUTPUT("CreateFrameBuffer failed");
        return false;
    };

    if (!CreateCamera())
    {
        OUTPUT("CreateCamera failed");
        return false;
    };

    ASSERT(!CCamera::CameraDefault());
    
    CCamera::SetCameraDefault(m_pCamera);
    CScreen::AttachDevice(this);
    SetFlipInterval(m_iFlipInterval);

    return true;
};


void CGraphicsDevice::Stop(void)
{
    CScreen::DetachDevice();
    
    DestroyFrameBuffer();

    if (CCamera::CameraDefault() == m_pCamera)
        CCamera::SetCameraDefault(nullptr);

    DestroyCamera();

#ifdef RWDEBUG
    CDebug::StopRwDebug();
    RtCharsetClose();
#endif
    
    RwEngineStop();
};


bool CGraphicsDevice::RenderBegin(void)
{
    ASSERT(m_pCamera);

    if (m_bFlipEnable)
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


void CGraphicsDevice::SetFlipInterval(int32 nVsyncCount)
{
    if (nVsyncCount >= 0 && nVsyncCount <= 2)
    {
        m_iFlipInterval = nVsyncCount;
        CScreen::DeviceChanged();
    };
};


int32 CGraphicsDevice::Subsystem(void)
{
    return 0;
};


bool CGraphicsDevice::AttachPlugin(void)
{
    if (!RpWorldPluginAttach())
        return false;
    
    if (!RtAnimInitialize())
        return false;

    if (!RpHAnimPluginAttach())
        return false;

    if (!RpSkinPluginAttach())
        return false;

    //
    // TODO Attach RpToon plugin
    //
    //if (!RpToonPluginAttach())
    //    ASSERT(false);

    if (!RpCollisionPluginAttach())
        return false;

    if (!RpPVSPluginAttach())
        return false;

    if (!RpLODAtomicPluginAttach())
        return false;

    if (!RpRandomPluginAttach())
        return false;

    if (!RpMatFXPluginAttach())
        return false;

    if (!RpUVAnimPluginAttach())
        return false;
	
    return true;
};


bool CGraphicsDevice::CreateFrameBuffer(void)
{
    ASSERT(!m_pFrameBuffer);
    ASSERT(!m_pZBuffer);

    RwVideoMode Videomode = { 0 };
	RwEngineGetVideoModeInfo(&Videomode, RwEngineGetCurrentVideoMode());

    int32 vmw = Videomode.width;
    int32 vmh = Videomode.height;

    if (ScreenWidth() < vmw)
        vmw = ScreenWidth();

    if (ScreenHeight() < vmh)
        vmh = ScreenHeight();

    m_pFrameBuffer = RwRasterCreate(vmw, vmh, 0, rwRASTERTYPECAMERA);
	ASSERT(m_pFrameBuffer);
    if (!m_pFrameBuffer)
        return false;

    m_pZBuffer = RwRasterCreate(vmw, vmh, 0, rwRASTERTYPEZBUFFER);
    ASSERT(m_pZBuffer);
    if (!m_pZBuffer)
    {
        RwRasterDestroy(m_pFrameBuffer);
        m_pFrameBuffer = nullptr;
        return false;
    };

	if (m_pCamera)
	{
		RwCameraSetRasterMacro(m_pCamera, m_pFrameBuffer);
        RwCameraSetZRasterMacro(m_pCamera, m_pZBuffer);
    };

    return true;
};


void CGraphicsDevice::DestroyFrameBuffer(void)
{
	if (m_pZBuffer)
	{
		RwCameraSetZRasterMacro(m_pCamera, nullptr);
		RwRasterDestroy(m_pZBuffer);
		m_pZBuffer = nullptr;
	};

	if (m_pFrameBuffer)
	{
		RwCameraSetRasterMacro(m_pCamera, nullptr);
		RwRasterDestroy(m_pFrameBuffer);
		m_pFrameBuffer = nullptr;
	};
};


bool CGraphicsDevice::CreateCamera(void)
{
    ASSERT(!m_pCamera);

    m_pCamera = RwCameraCreate();
    if (!m_pCamera)
        return false;

    RwCameraSetRasterMacro(m_pCamera, m_pFrameBuffer);
    RwCameraSetZRasterMacro(m_pCamera, m_pZBuffer);

    RwFrame* pRwFrame = RwFrameCreate();
    if (!pRwFrame)
    {
        RwCameraDestroy(m_pCamera);
        m_pCamera = nullptr;
        return false;
    };

    RwCameraSetFrameMacro(m_pCamera, pRwFrame);
    RwFrameSetIdentity(pRwFrame);
    RwCameraSetNearClipPlane(m_pCamera, TYPEDEF::DEFAULT_CLIP_NEAR);
    RwCameraSetFarClipPlane(m_pCamera, TYPEDEF::DEFAULT_CLIP_FAR);

    RwV2d ViewWindow;
    ViewWindow.x = TYPEDEF::DEFAULT_VIEWWINDOW;
    ViewWindow.y = TYPEDEF::DEFAULT_VIEWWINDOW / TYPEDEF::DEFAULT_ASPECTRATIO;
    RwCameraSetViewWindow(m_pCamera, &ViewWindow);
    
    RwCameraClear(m_pCamera, &m_ClearColor, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
    RwCameraShowRaster(m_pCamera, nullptr, rwRASTERFLIPWAITVSYNC);

    return true;
};


void CGraphicsDevice::DestroyCamera(void)
{
	if (!m_pCamera)
		return;

    RwFrame* pRwFrame = RwCameraGetFrame(m_pCamera);
    RwCameraSetFrame(m_pCamera, nullptr);
    RwFrameDestroy(pRwFrame);

    RwCameraSetRaster(m_pCamera, nullptr);
    RwCameraSetZRaster(m_pCamera, nullptr);
    RwCameraDestroy(m_pCamera);
    m_pCamera = nullptr;
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