#include "System2D.hpp"
#include "Camera.hpp"
#include "Sprite.hpp"
#include "RenderState.hpp"
#include "Screen.hpp"


static CCamera* s_pDefaultCamera = nullptr;
static CCamera* s_pCurrentCamera = nullptr;
static uint32 s_uReferenceCount = 0;


/*static*/ bool CSystem2D::Initialize(void)
{
    if (!s_uReferenceCount++)
    {
        RwBBox WorldBBox;
        WorldBBox.inf.x = -100.0f;        
        WorldBBox.inf.y = -100.0f;
        WorldBBox.inf.z = -100.0f;
        WorldBBox.sup.x = 100.0f;
        WorldBBox.sup.y = 100.0f;
        WorldBBox.sup.z = 100.0f;

        RpWorld* pRpWorld = RpWorldCreate(&WorldBBox);
        ASSERT(pRpWorld);

        s_pDefaultCamera = CCamera::GetCamera();
        ASSERT(s_pDefaultCamera);
        
        s_pCurrentCamera = s_pDefaultCamera;
        RpWorldAddCamera(pRpWorld, s_pDefaultCamera->GetRwCamera());
        Rt2dOpen(s_pDefaultCamera->GetRwCamera());
    };

	CSprite::SetDefaultVirtualScreen();
	Rt2dDeviceSetMetric(
		CSprite::m_fVirtualScreenX,
		CSprite::m_fVirtualScreenY,
		CSprite::m_fVirtualScreenW,
		CSprite::m_fVirtualScreenH
	);

	Rt2dDeviceSetLayerDepth(1.0f);

	return true;
};


/*static*/ void CSystem2D::Terminate(void)
{
    ASSERT(s_uReferenceCount > 0);
    
    if (!--s_uReferenceCount)
    {
        Rt2dClose();
        
        RpWorld* pRpWorld = RwCameraGetWorld(s_pDefaultCamera->GetRwCamera());
        RpWorldRemoveCamera(pRpWorld, s_pDefaultCamera->GetRwCamera());
        RpWorldDestroy(pRpWorld);

        s_pDefaultCamera->Release();
        
        s_pDefaultCamera = nullptr;
        s_pCurrentCamera = nullptr;
    };
};


/*static*/ bool CSystem2D::BeginScene(void)
{
    ASSERT(s_pCurrentCamera);
    return s_pCurrentCamera->BeginScene();
};


/*static*/ void CSystem2D::EndScene(void)
{
    ASSERT(s_pCurrentCamera);
    s_pCurrentCamera->EndScene();
};


/*static*/ bool CSystem2D::ScreenChanged(void)
{
    if (!s_uReferenceCount)
        return false;

    Rt2dDeviceSetMetric(
        CSprite::m_fVirtualScreenX,
        CSprite::m_fVirtualScreenY,
        CSprite::m_fVirtualScreenW,
        CSprite::m_fVirtualScreenH
    );
    
    ASSERT(s_pDefaultCamera);
    s_pCurrentCamera = s_pDefaultCamera;
    return Rt2dDeviceSetCamera(s_pDefaultCamera->GetRwCamera()) != 0;
};


/*static*/ void CSystem2D::PushRenderState(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
};


/*static*/ void CSystem2D::PopRenderState(void)
{
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGCOLOR);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
};


/*static*/ float CSystem2D::VirtualScreenWidth(void)
{
    return CSprite::m_fVirtualScreenW;
};


/*static*/ float CSystem2D::VirtualScreenHeight(void)
{
    return CSprite::m_fVirtualScreenH;
};


/*static*/ float CSystem2D::VirtualScreenX(void)
{
    return CSprite::m_fVirtualScreenX;
};


/*static*/ float CSystem2D::VirtualScreenY(void)
{
    return CSprite::m_fVirtualScreenY;
};