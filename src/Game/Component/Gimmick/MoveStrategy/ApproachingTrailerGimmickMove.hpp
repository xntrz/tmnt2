#pragma once

#include "PathMoveGimmickMove.hpp"


class CApproachingTrailerGimmickMove final : public CPathMoveGimmickMove
{
public:
    CApproachingTrailerGimmickMove(void);
    virtual ~CApproachingTrailerGimmickMove(void);
    virtual void UpdateRotation(float dt) override;

private:
    float m_fTimerCounter;
    RwV3d m_vPrePos;
};


class CApproachingTrailerHeadGimmickMove final : public CGimmickMove
{
public:
    CApproachingTrailerHeadGimmickMove(void);
    virtual ~CApproachingTrailerHeadGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void GetRotation(RwV3d* pvRot) const { *pvRot = m_vRotation; };
    inline void SetRotation(const RwV3d* pvRot) { m_vRotation = *pvRot; };

private:
    RwV3d m_vVelocity;
    RwV3d m_vRotation;
    float m_fTimerCounter;
    bool m_bEnd;
};