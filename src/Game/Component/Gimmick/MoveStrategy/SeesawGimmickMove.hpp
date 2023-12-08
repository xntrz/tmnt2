#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CSeesawGimmickMove final : public CGimmickMove
{
public:
    CSeesawGimmickMove(void);
    virtual ~CSeesawGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void GetTheta(RwV3d* pvTheta);
    void GetOmega(RwV3d* pvOmega);
    void GetTotalMoment(RwV3d* pvMoment);
    void SetCharacterPositionFromName(const char* pszName);
    void SetMatrixFromClump(RpClump* pClump);

    inline void SetRotateY(float x) { m_fRotateY = x; };

private:
    RwMatrix m_invMat;
    float m_fTheta;
    float m_fThetaLimit;
    float m_fOmega;
    float m_fOmegaLimit;
    float m_fMoment;
    float m_fRotateY;
    int32 m_nIndex;
    RwV3d m_avPosition[20];
    float m_fHorizonizeTimer;
};