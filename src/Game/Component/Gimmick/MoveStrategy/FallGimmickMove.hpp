#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CFallGimmickMove : public CGimmickMove
{
public:
    CFallGimmickMove(int32 movetype, float fReflection, float fCollisionRadius);
    virtual ~CFallGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void SetRotation(const RwV3d* pvRotation) { m_vRotation = *pvRotation; };
    inline void GetRotation(RwV3d* pvRotation) { *pvRotation = m_vRotation; };

    inline void SetVelocity(const RwV3d* pvVelocity) { m_vVelocity = *pvVelocity; };
    inline void GetVelocity(RwV3d* pvVelocity) { *pvVelocity = m_vVelocity; };
    
    inline void SetRotationVelocity(const RwV3d* pvRotationVelocity) { m_vRotationVelocity = *pvRotationVelocity; };

    inline void SetCollisionRadiusAutoChangeEnable(bool bEnable) { m_bColRadiusAutoChangeEnable = bEnable; };

private:
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