#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CFallGimmickMove : public CGimmickMove
{
public:
    CFallGimmickMove(int32 movetype, float fReflection, float fCollisionRadius);
    virtual ~CFallGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SetRotation(const RwV3d* pvRotation);
    void SetVelocity(const RwV3d* pvVelocity);
    void SetRotationVelocity(const RwV3d* pvRotationVelocity);
    void GetRotation(RwV3d* pvRotation);
    void GetVelocity(RwV3d* pvVelocity);
    void SetCollisionRadiusAutoChangeEnable(bool bEnable);

private:
    void clear(void);
    void collisionRadiusAutoChange(void);

private:
    int32 m_movetype;
    RwV3d m_vRotation;
    RwV3d m_vVelocity;
    RwV3d m_vRotationVelocity;
    float m_fReflection;
    float m_fCollisionRadius;
    bool m_bColRadiusAutoChangeEnable;
};