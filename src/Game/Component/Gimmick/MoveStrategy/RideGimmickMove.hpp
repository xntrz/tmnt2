#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CRideGimmickMove : public CGimmickMove
{
public:
    CRideGimmickMove(void);
    virtual ~CRideGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void SetVelocity(const RwV3d* pvVelocity) { m_vVelocity = *pvVelocity; };
    
    inline void SetRotation(const RwV3d* pvRotation) { m_vRotation = *pvRotation; };
    inline void GetRotation(RwV3d* pvRotation) const { *pvRotation = m_vRotation; };
    
    inline void SetMoveDirection(const RwV3d* pvMoveDirection) { m_vMoveDirection = *pvMoveDirection; };
    
    inline void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; };

protected:
    RwV3d m_vRotation;
    RwV3d m_vMoveDirection;
    float m_fSpeed;
    RwV3d m_vVelocity;
    float m_fTimer;
};