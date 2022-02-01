#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CCuttingGimmickMove : public CGimmickMove
{
public:
    CCuttingGimmickMove(void);
    virtual ~CCuttingGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SetVelocity(const RwV3d* pvVelocity);
    void SetRotation(const RwV3d* pvRotation);
    void SetRotVelocity(const RwV3d* pvRotVel);
    void GetRotation(RwV3d* pvRotation);
    void GetRotVelocity(RwV3d* pvRotVel);

private:
    RwV3d m_vVelocity;
    RwV3d m_vRotation;
    RwV3d m_vRotVelocity;
};