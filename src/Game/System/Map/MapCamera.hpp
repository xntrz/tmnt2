#pragma once


class CCamera;


class CMapCamera
{
public:
    class CIntroduction;
    class CVibration;
    
    enum MODE
    {
        MODE_MANUAL = 0,
        MODE_AUTOCHANGE,
        MODE_INTRODUCTION,

        MODEMAX,
    };
    
    enum PATHMODE
    {
        PATHMODE_SINGLEPLAYER = 0,
        PATHMODE_MULTIPLAYER,

        PATHMODEMAX,
    };

public:
    static float m_fDefaultViewSize;
    
    CMapCamera(void);
    virtual ~CMapCamera(void);
    bool BeginScene(void);
    void EndScene(void);
    void WorldAddCamera(RpWorld* pWorld);
    void WorldRemoveCamera(RpWorld* pWorld);
    void Update(const RwV3d* pvAt);
    void Update(const RwV3d* pvAt, float fZoom);
    void PostUpdate(void);
    void UpdateMoment(const RwV3d* pvAt);
    void UpdateAutoChangeCamera(const RwV3d* pvAt);
    void ResetCameraForCameraAreaChange(const RwV3d* pvAt);
    void MakePathAtCameraName(char* pszPathAtName, const char* pszCameraAreaName);
    void MakePathEyeCameraName(char* pszPathEyeName, const char* pszCameraAreaName);
    void MakeSetCameraName(char* pszSetCameraName, const char* pszCameraAreaName);
    void UpdateManualCamera(const RwV3d* pvAt);
    void UpdatePathCamera(const RwV3d* pvAt, bool bExPath);
    void UpdateSetCamera(const RwV3d* pvAt);
    void UpdateFixedCamera(const RwV3d* pvAt);
    void UpdateLookat(void);
    void LookAt(const RwV3d* pvEye, const RwV3d* pvAt, const RwV3d* pvUp);
    void Skip(void);
    void SetLookat(const RwV3d* pvEye, const RwV3d* pvAt);
    void SetCameraVibration(float fPower, float fTime, uint32 uFreq);
    void SetFarClipPlane(float fFarClipPlane);
    void SetNearClipPlane(float fNearClipPlane);
    void SetFogDistance(float fFogDist);
    void SetViewWindow(float fViewSize);
    void SetViewWindow(float fViewSize, float fAspect);
    void SetCameraMode(MODE mode);
    MODE GetCameraMode(void) const;
    void SetPathMode(PATHMODE pathmode);
    bool IsPosVisible(const RwV3d* pvPos);
    float CalcNiceZoon(RwV3d* avPos, int32 nNumPos);
    RwCamera* GetRwCamera(void) const;
    bool IsIntroductionEnd(void) const;
    float GetViewRatio(void) const;
    void GetEye(RwV3d* pvEye) const;
    void GetLookat(RwV3d* pvLookat) const;
    float GetViewAreaRadius(void) const;

private:
    void IntroductionCreate(void);
    void IntroductionDestroy(void);
    void IntroductionUpdate(void);
    void VibrationCreate(float fPower, float fTime, uint32 uFreq);
    void VibrationDestroy(void);
    void VibrationUpdate(void);

private:
    static float m_fDefaultClipFar;
    static float m_fDefaultClipNear;
    static float m_fDefaultFogDist;
    static RwV3d m_vDefaultLookat;
    static RwV3d m_vDefaultEye;
    CCamera* m_pCamera;
    RwV3d m_vEye;
    RwV3d m_vAt;
    float m_fDist;
    float m_fViewSize;
    float m_fRotY;
    float m_fHeight;
    float m_fLookatOffsetY;
    float m_fPathTime;
    float m_fPrePathTime;
    float m_fLookatViewAreaRadius;
    float m_fChangeTimer;
    float m_fRequestZoom;
    MODE m_mode;
    PATHMODE m_pathmode;
    char m_szCameraAreaName[32];
    bool m_bChangeMoment;
    CIntroduction* m_pIntroduction;
    CVibration* m_pVibration;
    float m_fPitch;
};