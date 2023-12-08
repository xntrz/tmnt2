#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CCrumbleFloorGimmickMove final : public CGimmickMove
{
private:
    enum STEP
    {
        STEP_WAIT = 0,
        STEP_PREMOVE,
        STEP_ENDURE,
        STEP_SHAKE,
        STEP_FALLDOWN,
    };
    
public:
    CCrumbleFloorGimmickMove(void);
    virtual ~CCrumbleFloorGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void Wait(float dt);
    void PreMove(float dt);
    void Endure(float dt);
    void Shake(float dt);
    void Falldown(float dt);
    void GetTheta(RwV3d* pvTheta);
    void GetOmega(RwV3d* pvOmega);
    void GetTotalMoment(RwV3d* pvMoment);
    void SetCharacterPositionFromName(const char* pszName);
    void SetMatrixFromClump(RpClump* pClump);

private:
    RwMatrix m_invMat;
    int32 m_nIndex;
    RwV3d m_avPosition[GAMETYPES::PLAYERS_MAX];
    float m_fTheta;
    float m_fOmega;
    float m_fTotalMoment;
    float m_fCounter;
    RwV3d m_vVelocity;
    STEP m_eStep;
    bool m_bEffect;
};