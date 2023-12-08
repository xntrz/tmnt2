#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CUtromGimmickMove final : public CGimmickMove
{
public:
    CUtromGimmickMove(void);
    virtual ~CUtromGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void SetRadius(float r)      { m_fRadius = r; };
    inline float GetRadius(void) const  { return m_fRadius; };
    
    inline void SetVelocity(const RwV3d* pvVel) { m_vVelocity = *pvVel; };
    inline void GetVelocity(RwV3d* pvVel) const { *pvVel = m_vVelocity; };
    
    inline void SetRotation(const RwV3d* pvRot) { m_vRotation = *pvRot; };
    inline void GetRotation(RwV3d* pvRot) const { *pvRot = m_vRotation; };
    
private:
    RwV3d m_vVelocity;
    RwV3d m_vRotation;
    float m_fRadius;
};