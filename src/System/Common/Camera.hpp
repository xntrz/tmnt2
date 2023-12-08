#pragma once


class CCamera
{
public:
    static CCamera* GetCamera(void);
    static RwCamera* CloneCamera(RwCamera* pRwCamera);
    
    inline static void SetCameraCurrent(RwCamera* pRwCamera) { m_pCameraCurrent = pRwCamera; };
    inline static void SetCameraDefault(RwCamera* pRwCamera) { m_pCameraDefault = pRwCamera; };
    inline static RwCamera* CameraCurrent(void) { return m_pCameraCurrent; };
    inline static RwCamera* CameraDefault(void) { return m_pCameraDefault; };

    CCamera(RwCamera* pRwCamera);
    virtual ~CCamera(void);
    bool BeginScene(void);
    bool EndScene(void);
    void Release(void);
    
    inline RwCamera* GetRwCamera(void) const { return m_pCamera; };
    
private:
    static RwCamera* m_pCameraCurrent;
    static RwCamera* m_pCameraDefault;
    RwCamera* m_pCamera;
    RwCamera* m_pCameraOld;
};