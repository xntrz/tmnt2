#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CCrystalGimmickMove final : public CGimmickMove
{
public:
    enum MOVETYPE
    {
        MOVETYPE_NORMAL = 0,
        MOVETYPE_SHAKE,        
    };

public:
    static int32 m_nCrystal;

public:
    CCrystalGimmickMove(const RwV3d* pvInitialPos);
    virtual ~CCrystalGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SpasmMove(float dt);
    void UpDownMove(float dt);

    inline void SetRotation(const RwV3d* pvRotation) { m_vRotation = *pvRotation; };
    inline void GetRotation(RwV3d* pvRotation) const { *pvRotation = m_vRotation; };

    inline void SetRotationVelocity(const RwV3d* pvRotationV) { m_vRotationVelocity = *pvRotationV; };
    inline void GetRotationVelocity(RwV3d* pvRotationV) const { *pvRotationV = m_vRotationVelocity; };

    inline void SetMoveType(MOVETYPE movetype) { m_movetype = movetype; };

private:
    RwV3d m_vRotation;
    RwV3d m_vRotationVelocity;
    RwV3d m_vInitPos;
    float m_fTheta;
    float m_fThetaAdd;
    MOVETYPE m_movetype;
};