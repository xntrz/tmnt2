#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CFallRubbleGimmickMove : public CGimmickMove
{
public:
    CFallRubbleGimmickMove(float fRefCoe, float fBodyHitRadius);
    virtual ~CFallRubbleGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void SetRotation(const RwV3d* pvRot)     { m_vRotation = *pvRot; };
    inline void SetVelocity(const RwV3d* pvVel)     { m_vVelocity = *pvVel; };
    inline void SetRotVelocity(const RwV3d* pvVel)  { m_vRotVelocity = *pvVel; };
    inline void GetVelocity(RwV3d* pvVel) const     { *pvVel = m_vVelocity; };
    inline void GetRotation(RwV3d* pvRot) const     { *pvRot = m_vRotation; };

private:
    RwV3d m_vVelocity;
    RwV3d m_vRotVelocity;
    RwV3d m_vRotation;
    float m_fE;
    float m_fBodyHitRadius;
};