#include "Camera.hpp"


class CRwCameraContainer
{
private:
    struct RWCAMERA : public CListNode<RWCAMERA>
    {
        RwCamera* m_pRwCamera;
    };
    
public:
    CRwCameraContainer(void);
    ~CRwCameraContainer(void);
    void RegistCamera(RwCamera* pRwCamera);
    void RemoveCamera(RwCamera* pRwCamera);
    void FramebufferChanged(RwRaster* pBuffer, RwRaster* pZBuffer);

private:
    RWCAMERA m_aRwCamera[64];
    CList<RWCAMERA> m_listRwCameraPool;
    CList<RWCAMERA> m_listRwCameraAlloc;
};


CRwCameraContainer::CRwCameraContainer(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aRwCamera); ++i)
    {
        m_aRwCamera[i].m_pRwCamera = nullptr;

        m_listRwCameraPool.push_back(&m_aRwCamera[i]);
    };
};


CRwCameraContainer::~CRwCameraContainer(void)
{
    ASSERT(m_listRwCameraAlloc.empty());
};


void CRwCameraContainer::RegistCamera(RwCamera* pRwCamera)
{
    ASSERT(pRwCamera);
    ASSERT(!m_listRwCameraPool.empty());

    RWCAMERA* pNode = m_listRwCameraPool.front();
    m_listRwCameraPool.erase(pNode);
    m_listRwCameraAlloc.push_back(pNode);

    pNode->m_pRwCamera = pRwCamera;
};


void CRwCameraContainer::RemoveCamera(RwCamera* pRwCamera)
{
    ASSERT(pRwCamera);
    ASSERT(!m_listRwCameraAlloc.empty());

    RWCAMERA* pNode = nullptr;

    for (RWCAMERA& it : m_listRwCameraAlloc)
    {
        if (it.m_pRwCamera == pRwCamera)
        {
            pNode = &it;
            break;
        };
    };

    ASSERT(pNode);

    if (pNode)
    {
        pNode->m_pRwCamera = nullptr;
        m_listRwCameraAlloc.erase(pNode);
        m_listRwCameraPool.push_back(pNode);
    };
};


void CRwCameraContainer::FramebufferChanged(RwRaster* pBuffer, RwRaster* pZBuffer)
{
    for (RWCAMERA& it : m_listRwCameraAlloc)
    {
        ASSERT(it.m_pRwCamera);

        RwCameraSetRasterMacro(it.m_pRwCamera, pBuffer);
        RwCameraSetZRasterMacro(it.m_pRwCamera, pZBuffer);
    };
};


static CRwCameraContainer* s_pRwCameraContainer = nullptr;


static inline CRwCameraContainer& RwCameraContainer(void)
{
    ASSERT(s_pRwCameraContainer);
    return *s_pRwCameraContainer;
};


CHECK_SIZE(CCamera, 0xC);


/*static*/ RwCamera* CCamera::m_pCameraCurrent = nullptr;
/*static*/ RwCamera* CCamera::m_pCameraDefault = nullptr;


/*static*/ void CCamera::Initialize(void)
{
    if (!s_pRwCameraContainer)
    {
        s_pRwCameraContainer = new CRwCameraContainer;
    };
};


/*static*/ void CCamera::Terminate(void)
{
    if (s_pRwCameraContainer)
    {
        delete s_pRwCameraContainer;
        s_pRwCameraContainer = nullptr;
    };
};


/*static*/ void CCamera::FramebufferChanged(RwRaster* pBuffer, RwRaster* pZBuffer)
{
    RwCameraContainer().FramebufferChanged(pBuffer, pZBuffer);
};


/*static*/ void CCamera::SetCameraCurrent(RwCamera* pRwCamera)
{
    m_pCameraCurrent = pRwCamera;
};


/*static*/ void CCamera::SetCameraDefault(RwCamera* pRwCamera)
{
    RwCamera* pRwCameraPrev = m_pCameraDefault;

    m_pCameraDefault = pRwCamera;

    if (m_pCameraDefault && !pRwCameraPrev)
    {
        RwCameraContainer().RegistCamera(m_pCameraDefault);
    }
    else if (!m_pCameraDefault && pRwCameraPrev)
    {
        RwCameraContainer().RemoveCamera(pRwCameraPrev);
    };
};


/*static*/ CCamera* CCamera::GetCamera(void)
{
    ASSERT(m_pCameraDefault);
    
    RwCamera* pRwCamera = CloneCamera(m_pCameraDefault);
    ASSERT(pRwCamera);
    
    CCamera* pCamera = new CCamera(pRwCamera);
    ASSERT(pCamera);
    return pCamera;
};


/*static*/ RwCamera* CCamera::CloneCamera(RwCamera* pRwCamera)
{
    RwCamera* pNewCamera = RwCameraClone(pRwCamera);
    ASSERT(pNewCamera);
    
    RwFrame* pFrame = RwFrameCreate();
    ASSERT(pFrame);

    RwCameraSetFrame(pNewCamera, pFrame);
    RwMatrixSetIdentityMacro(RwFrameGetMatrixMacro(pFrame));
    RwCameraSetNearClipPlane(pNewCamera, 0.1f);
    RwCameraSetFarClipPlane(pNewCamera, 100.0f);

    RwV2d ViewWindow = { 0.5f, 0.375f };
    RwCameraSetViewWindow(pNewCamera, &ViewWindow);

    return pNewCamera;
};


/*static*/ RwCamera* CCamera::CameraCurrent(void)
{
    return m_pCameraCurrent;
};


/*static*/ RwCamera* CCamera::CameraDefault(void)
{
    return m_pCameraDefault;
};


CCamera::CCamera(RwCamera* pRwCamera)
: m_pCamera(pRwCamera)
, m_pCameraOld(nullptr)
{
    RwCameraContainer().RegistCamera(m_pCamera);
};


CCamera::~CCamera(void)
{
    RwCameraContainer().RemoveCamera(m_pCamera);
    
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

    if (m_pCameraOld && RwCameraBeginUpdate(m_pCameraOld) == m_pCameraOld)
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


RwCamera* CCamera::GetRwCamera(void) const
{
    return m_pCamera;
};


RwCamera* CCamera::GetRwCameraOld(void) const
{
    return m_pCameraOld;
};