#pragma once


class CCamera
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void FramebufferChanged(RwRaster* pBuffer, RwRaster* pZBuffer);
    static void SetCameraCurrent(RwCamera* pRwCamera);
    static void SetCameraDefault(RwCamera* pRwCamera);
    static CCamera* GetCamera(void);
    static RwCamera* CloneCamera(RwCamera* pRwCamera);
    static RwCamera* CameraCurrent(void);
    static RwCamera* CameraDefault(void);

    CCamera(RwCamera* pRwCamera);
    virtual ~CCamera(void);
    bool BeginScene(void);
    bool EndScene(void);
    void Release(void);
    RwCamera* GetRwCamera(void) const;
    RwCamera* GetRwCameraOld(void) const;

private:
    static RwCamera* m_pCameraCurrent;
    static RwCamera* m_pCameraDefault;
    RwCamera* m_pCamera;
    RwCamera* m_pCameraOld;
};