#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CHatchGimmickMove final : public CGimmickMove
{
public:
    CHatchGimmickMove(void);
    virtual ~CHatchGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void SetRadius(float r) { m_fRadius = r; };
    inline float GetRadius(void) const { return m_fRadius; };

    inline void SetVelocity(const RwV3d* pvVel) { m_vVelocity = *pvVel; };
    inline void GetVelocity(RwV3d* pvVel) const { *pvVel = m_vVelocity; };

    inline void SetRotation(const RwV3d* pvRot) { m_vRotation = *pvRot; };
    inline void GetRotation(RwV3d* pvRot) const { *pvRot = m_vRotation; };

    inline void SetRotVelocity(const RwV3d* pvRotVel) { m_vRotVelocity = *pvRotVel; };
    inline void GetRotVelocity(RwV3d* pvRotVel) const { *pvRotVel = m_vRotVelocity; };

private:
    RwV3d m_vVelocity;
    float m_fRadius;
    RwV3d m_vRotation;
    RwV3d m_vRotVelocity;
};