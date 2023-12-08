#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CRollingRockGimmickMove final : public CGimmickMove
{
public:
    CRollingRockGimmickMove(void);
    virtual ~CRollingRockGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void Reset(void);
    
    inline void SetInitPosition(const RwV3d* pvPos) { m_vInitPosition = *pvPos; };
    inline void GetInitPosition(RwV3d* pvPos) const { *pvPos = m_vInitPosition; };

    inline void SetVelocity(const RwV3d* pvVel) { m_vVelocity = *pvVel; };
    inline void GetVelocity(RwV3d* pvVel) const { *pvVel = m_vVelocity; };
    
    inline void GetRotation(RwV3d* pvRot) const { *pvRot = m_vTheta; };
    
    inline float GetHitRadius(void) const { return m_fHitRadius; };
    inline void SetHitRadius(float fHitRadius) { m_fHitRadius = fHitRadius; };

    inline bool IsReset(void) const { return m_bReset; };
    
private:
    RwV3d m_vVelocity;
    RwV3d m_vTheta;
    float m_fOmega;
    float m_fHitRadius;
    RwV3d m_vInitPosition;
    bool m_bReset;
};