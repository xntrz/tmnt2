#pragma once


class CHitAttackData;
class CHitCatchData;
class CMotionParameter;
class CCharacterParameter;


class CMotionParameterController
{
public:
    struct DRAW_ATOMIC
    {
        int32 m_nAtomic;
        bool m_bDraw;
    };

    struct DRAW_LOCUS
    {
        int32 m_nLocus;
        bool m_bDraw;
    };

    struct COLLISIONPARAMETER
    {
        float m_fRadius;
        float m_fHeight;
    };

    struct LOCUSPARAMETER
    {
        int32 m_nBoneID;
        RwV3d m_vPositionHigh;
        RwV3d m_vPositionLow;
        RwRGBA m_Color;
    };

    enum PLAYMODE
    {
        PLAYMODE_ONESHOT = 0,
        PLAYMODE_REPEAT,
        PLAYMODE_STOP,

        PLAYMODEMAX,
    };

public:
    CMotionParameterController(const char* pszModelName, int32 nPartsNum);
    virtual ~CMotionParameterController(void);
    int32 GetMotionNum(void) const;
    const char* GetMotionName(int32 no);
    void SetCurrentMotion(const char* pszMotionName);
    float GetBlendTime(void) const;
    PLAYMODE GetPlaymode(void) const;
    float GetNextChainMotionConnectTime(const char* pszNextMotion);
    const COLLISIONPARAMETER* GetCollisionParameter(void) const;
    int32 GetNumHitBodyData(void) const;
    int32 GetHitBodyBoneID(int32 no);
    const RwSphere* GetHitBodySphere(int32 no);
    int32 GetNumHitCatchData(void) const;
    int32 GetHitCatchBoneID(int32 no);
    const RwSphere* GetHitCatchSphere(int32 no);
    void GetHitCatchData(int32 no, CHitCatchData* pHitCatchData);
    int32 GetNumHitAttackData(void) const;
    bool IsHitAttackEnableTime(int32 no, float fNow);
    int32 GetHitAttackBoneID(int32 no);
    const RwSphere* GetHitAttackSphere(int32 no);
    void GetHitAttackData(int32 no, CHitAttackData* pHitAttackData);
    int32 GetNumLocus(void) const;
    void GetLocusParameter(int32 no, LOCUSPARAMETER* pParam);
    int32 GetNumDrawAtomicData(float fPrevTime, float fNowTime);
    void GetDrawAtomicData(int32 no, DRAW_ATOMIC* pData);
    int32 GetNumDrawLocusData(float fPrevTime, float fNowTime);
    void GetDrawLocusData(int32 no, DRAW_LOCUS* pData);
    bool IsAttackTiming(float fPrevTime, float fNowTime);
    bool IsInvincibleTime(float fNowTime);
    bool IsDrawAtomic(int32 nAtomicNo);
    void GetOffsetPosition(int32 no, int32& rnBoneNo, RwV3d& rvOffset);
    CMotionParameter* GetCurrentMotionParameter(void) const;

private:
    CCharacterParameter& CharacterParam(void);
    CMotionParameter& CurrentMotionParam(void);
    CCharacterParameter& CharacterParam(void) const;
    CMotionParameter& CurrentMotionParam(void) const;

private:
    CCharacterParameter* m_pCharacterParameter;
    CMotionParameter* m_pCurrentMotionParameter;
};