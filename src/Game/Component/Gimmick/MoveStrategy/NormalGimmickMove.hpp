#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CNormalGimmickMove final : public CGimmickMove
{
public:
    CNormalGimmickMove(void);
    virtual ~CNormalGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void SetVelocity(const RwV3d* pvVel) { m_vVelocity = *pvVel; };
    inline void GetVelocity(RwV3d* pvVel) const { *pvVel = m_vVelocity; };

    inline void SetRadius(float r) { m_fRadius = r; };

private:
    RwV3d m_vVelocity;
    float m_fRadius;
};