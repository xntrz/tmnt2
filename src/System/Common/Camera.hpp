#pragma once


class CCamera
{
public:
    static CCamera* GetCamera(void);
    static RwCamera* CloneCamera(RwCamera* pRwCamera);
    
    CCamera(RwCamera* pRwCamera);
    virtual ~CCamera(void);
    bool BeginScene(void);
    bool EndScene(void);
    void Release(void);

    /*inline*/ static void CameraCurrent(RwCamera* pRwCamera);
    /*inline*/ static RwCamera* CameraCurrent(void);
    /*inline*/ static void CameraDefault(RwCamera* pRwCamera);
    /*inline*/ static RwCamera* CameraDefault(void);
    /*inline*/ RwCamera* GetRwCamera(void) const;

private:
    static RwCamera* m_pCameraCurrent;
    static RwCamera* m_pCameraDefault;
    RwCamera*        m_pCamera;
    RwCamera*        m_pCameraOld;
};


inline /*static*/ void CCamera::CameraCurrent(RwCamera* pRwCamera)
{
    m_pCameraCurrent = pRwCamera;
};


inline /*static*/ RwCamera* CCamera::CameraCurrent(void)
{
    return m_pCameraCurrent;
};


inline /*static*/ void CCamera::CameraDefault(RwCamera* pRwCamera)
{
    m_pCameraDefault = pRwCamera;
};


inline /*static*/ RwCamera* CCamera::CameraDefault(void)
{
    return m_pCameraDefault;
};


inline RwCamera* CCamera::GetRwCamera(void) const
{
    return m_pCamera;
};