#include "Camera.hpp"
#include "Configure.hpp"
#include "rweval.hpp"


/*static*/ RwCamera* CCamera::m_pCameraCurrent = nullptr;
/*static*/ RwCamera* CCamera::m_pCameraDefault = nullptr;
/*static*/ RwCamera* CCamera::m_aCamera[32];


/*static*/ CCamera* CCamera::GetCamera(void)
{
    ASSERT(m_pCameraDefault);
    
    RwCamera* pRwCamera = CloneCamera(m_pCameraDefault);
    ASSERT(pRwCamera);
    
    return new CCamera(pRwCamera);
};


/*static*/ RwCamera* CCamera::CloneCamera(RwCamera* pRwCamera)
{
    RwCamera* pNewCamera = RwCameraClone(pRwCamera);
    ASSERT(pNewCamera);
    
    pNewCamera->endUpdate = rwevalGetCameraEndUpdateFunc();

    RwFrame* pFrame = RwFrameCreate();
    ASSERT(pFrame);

    RwCameraSetFrame(pNewCamera, pFrame);
    RwMatrixSetIdentity(RwFrameGetMatrix(pFrame));
    RwCameraSetNearClipPlane(pNewCamera, TYPEDEF::DEFAULT_CLIP_NEAR);
    RwCameraSetFarClipPlane(pNewCamera, TYPEDEF::DEFAULT_CLIP_FAR);

    RwV2d ViewWindow =
    {
        TYPEDEF::DEFAULT_VIEWWINDOW,
        TYPEDEF::DEFAULT_VIEWWINDOW / TYPEDEF::DEFAULT_ASPECTRATIO
    };
    RwCameraSetViewWindow(pNewCamera, &ViewWindow);

    return pNewCamera;
};


/*static*/ void CCamera::FramebufferChanged(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aCamera); ++i)
    {
        if ((m_aCamera[i] != nullptr) &&
            (m_aCamera[i] != m_pCameraDefault))
        {
            RwRaster* fb = RwCameraGetRaster(m_pCameraDefault);
            RwRaster* zb = RwCameraGetZRaster(m_pCameraDefault);

            RwCameraSetRasterMacro(m_aCamera[i], fb);
            RwCameraSetZRasterMacro(m_aCamera[i], zb);
        };
    };
};


CCamera::CCamera(RwCamera* pRwCamera)
: m_pCamera(pRwCamera)
, m_pCameraOld(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_aCamera); ++i)
    {
        if (m_aCamera[i] == nullptr)
        {
            m_aCamera[i] = pRwCamera;
            break;
        };
    };
};


CCamera::~CCamera(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aCamera); ++i)
    {
        if (m_aCamera[i] == m_pCamera)
        {
            m_aCamera[i] = nullptr;
            break;
        };
    };

    RwFrame* pFrame = RwCameraGetFrame(m_pCamera);
    ASSERT(pFrame);
    
    RwCameraSetFrame(m_pCamera, nullptr);
    RwFrameDestroy(pFrame);
    RwCameraDestroy(m_pCamera);
};


bool CCamera::BeginScene(void)
{
    m_pCameraOld = m_pCameraCurrent;
    
    if (m_pCameraCurrent)
        RwCameraEndUpdate(m_pCameraCurrent);

    if (RwCameraBeginUpdate(m_pCamera) != m_pCamera)
        return false;

    ASSERT(RwCameraGetCurrentCamera() != nullptr);
    
    m_pCameraCurrent = m_pCamera;
    
    return true;
};


bool CCamera::EndScene(void)
{
    ASSERT(m_pCamera == CameraCurrent());

    RwCameraEndUpdate(m_pCamera);

    if (!m_pCameraOld || (RwCameraBeginUpdate(m_pCameraOld) == m_pCameraOld))
    {
        m_pCameraCurrent = m_pCameraOld;
        m_pCameraOld = nullptr;
        return true;
    }
    else
    {
        m_pCameraCurrent = nullptr;
        m_pCameraOld = nullptr;
        return false;
    };
};


void CCamera::Release(void)
{
    delete this;
};