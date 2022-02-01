#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CRideGimmickMove : public CGimmickMove
{
public:
    CRideGimmickMove(void);
    virtual ~CRideGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SetVelocity(const RwV3d* pvVelocity);
    void SetRotation(const RwV3d* pvRotation);
    void SetMoveDirection(const RwV3d* pvMoveDirection);
    void SetSpeed(float fSpeed);
    void GetRotation(RwV3d* pvRotation) const;
    
protected:
    RwV3d m_vRotation;
    RwV3d m_vMoveDirection;
    float m_fSpeed;
    RwV3d m_vVelocity;
    float m_fTimer;
};