#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CThrowingGimmickMove : public CGimmickMove
{
public:
    CThrowingGimmickMove(void);
    virtual ~CThrowingGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SetRadius(float fRadius);
    void SetVelocity(const RwV3d* pvVelocity);
    void SetRotation(const RwV3d* pvRotation);
    void SetRotVelocity(const RwV3d* pvRotVel);
    void GetRotation(RwV3d* pvRotation) const;
    void GetRotVelocity(RwV3d* pvRotVel);

private:
    RwV3d m_vVelocity;
    RwV3d m_vRotation;
    RwV3d m_vRotVelocity;
    float m_fRadius;
};