#pragma once


class CModel;
class CMotion;


class CMotionController
{
public:
    enum STATUS
    {
        STATUS_NORMAL = 0,
        STATUS_BLEND,
    };

    enum PLAYMODE
    {
        PLAYMODE_ONESHOT= 0,
        PLAYMODE_REPEAT,
    };

public:
    CMotionController(CModel* pModel);
    virtual ~CMotionController(void);
    void AddMotion(const char* pszName);
    void SetCurrentMotion(const char* pszName, float fStartTime, float fEndTime, float fBlendTime, bool bForce);
    float GetCurrentMotionEndTime(void) const;
    void Update(float dt);
    void UpdateMatrices(void);
    void SetTime(float fTime);
    CMotion* FindMotion(const char* pszName);
    void SetMotion(CMotion* pMotion, float fStartTime, float fEndTime);
    void UpdateMotion(float dt);
    void SetTimeMotion(float fTime);
    void SetBlendMotion(CMotion* pMotion, float fStartTime, float fEndTime, float fBlendTime);
    void UpdateBlendMotion(float dt);
    void SetPlaymode(PLAYMODE playmode);
    void Pause(void);
    void Play(void);
    bool IsPaused(void) const;
    float GetTime(void) const;
    float GetEndTime(void) const;
    const char* GetMotionName(void) const;
    PLAYMODE GetPlaymode(void) const;
    CMotion* GetCurrentMotion(void) const;

private:
    CList<CMotion> m_listMotion;
    CMotion* m_pCurrentMotion;
    CModel* m_pModel;
    RpHAnimHierarchy* m_pInHierarchy;
    RpHAnimHierarchy* m_pOutHierarchy;
    float m_fTime;
    float m_fStartTime;
    float m_fEndTime;
    float m_fBlendTime;
    STATUS m_status;
    PLAYMODE m_playmode;
    bool m_bPause;
};