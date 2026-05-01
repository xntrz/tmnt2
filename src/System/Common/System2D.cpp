#include "System2D.hpp"
#include "Camera.hpp"
#include "Sprite.hpp"
#include "RenderState.hpp"
#include "Screen.hpp"


#if defined(TMNT2_RWDRV_OPENGL)
static const RwRGBAReal LIGHT_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };
static RpLight* s_pLightAmbient = nullptr;
#endif /* defined(TMNT2_RWDRV_OPENGL) */


/*static*/ CCamera* CSystem2D::m_pDefaultCamera = nullptr;
/*static*/ CCamera* CSystem2D::m_pCurrentCamera = nullptr;
/*static*/ uint32   CSystem2D::m_uReferenceCount = 0;


/*static*/ bool CSystem2D::Initialize(void)
{
    if (!m_uReferenceCount++)
    {
        RwBBox WorldBBox;
        WorldBBox.inf.x = -100.0f;        
        WorldBBox.inf.y = -100.0f;
        WorldBBox.inf.z = -100.0f;
        WorldBBox.sup.x = 100.0f;
        WorldBBox.sup.y = 100.0f;
        WorldBBox.sup.z = 100.0f;

        RpWorld* pWorld = RpWorldCreate(&WorldBBox);
        ASSERT(pWorld);

        m_pDefaultCamera = CCamera::GetCamera();
        ASSERT(m_pDefaultCamera);
        
        m_pCurrentCamera = m_pDefaultCamera;
        RpWorldAddCamera(pWorld, m_pDefaultCamera->GetRwCamera());

#if defined(TMNT2_RWDRV_OPENGL)
        RpLight* pLight = nullptr;

        pLight = RpLightCreate(rpLIGHTAMBIENT);
        if (pLight)
        {
            RpLightSetColor(pLight, &LIGHT_COLOR);

            RpWorldAddLight(pWorld, pLight);
            s_pLightAmbient = pLight;
        };
#endif /* defined(TMNT2_RWDRV_OPENGL) */

        Rt2dOpen(m_pDefaultCamera->GetRwCamera());
    };

    CSprite::SetDefaultVirtualScreen();
    
    Rt2dDeviceSetMetric(CSprite::m_fVirtualScreenX,
                        CSprite::m_fVirtualScreenY,
                        CSprite::m_fVirtualScreenW,
                        CSprite::m_fVirtualScreenH);

    Rt2dDeviceSetLayerDepth(1.0f);

    return true;
};


/*static*/ void CSystem2D::Terminate(void)
{
    ASSERT(m_uReferenceCount > 0);
    
    if (!--m_uReferenceCount)
    {
        Rt2dClose();
        
        RpWorld* pWorld = RwCameraGetWorld(m_pDefaultCamera->GetRwCamera());

#if defined(TMNT2_RWDRV_OPENGL)
        RpLight** ppLight = nullptr;

        ppLight = &s_pLightAmbient;
        if (*ppLight)
        {
            RpWorldRemoveLight(pWorld, *ppLight);
            RpLightDestroy(*ppLight);
            *ppLight = nullptr;
        };
#endif /* defined(TMNT2_RWDRV_OPENGL) */
        
        RpWorldRemoveCamera(pWorld, m_pDefaultCamera->GetRwCamera());
        RpWorldDestroy(pWorld);

        m_pDefaultCamera->Release();
        
        m_pDefaultCamera = nullptr;
        m_pCurrentCamera = nullptr;
    };
};


/*static*/ bool CSystem2D::BeginScene(void)
{
    ASSERT(m_pCurrentCamera);
    return m_pCurrentCamera->BeginScene();
};


/*static*/ void CSystem2D::EndScene(void)
{
    ASSERT(m_pCurrentCamera);
    m_pCurrentCamera->EndScene();
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


/*static*/ bool CSystem2D::Reset(void)
{
    if (!m_uReferenceCount)
        return false;

    Rt2dDeviceSetMetric(CSprite::m_fVirtualScreenX,
                        CSprite::m_fVirtualScreenY,
                        CSprite::m_fVirtualScreenW,
                        CSprite::m_fVirtualScreenH);

    ASSERT(m_pDefaultCamera);
    m_pCurrentCamera = m_pDefaultCamera;
    
    return Rt2dDeviceSetCamera(m_pDefaultCamera->GetRwCamera()) != 0;
};


/*static*/ void CSystem2D::SetLayerDepth(float fDepth)
{
    Rt2dDeviceSetLayerDepth(RwCameraGetNearClipPlane(m_pCurrentCamera->GetRwCamera()) * fDepth);
};