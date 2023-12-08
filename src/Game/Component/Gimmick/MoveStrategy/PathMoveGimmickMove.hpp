#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CPathMoveGimmickMove : public CGimmickMove
{
public:
    CPathMoveGimmickMove(void);
    virtual ~CPathMoveGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    virtual void SetPathTimeRate(float fTimeRate);
    virtual void UpdatePathTime(float dt);
    virtual void UpdateRotation(float dt);
    void InitPath(const char* pszPathName, float fPathT);
    float GetPathLength(const char* pszPathName);
    float GetPathTByMeterPerSecond(float fMps);
    float GetNearestPathT(RwV3d* pvPosition);
    bool IsAccelerationToAimPathT(float fAimPathT, float fAimPathTimerate, float fIntervalStep, float fInterval);

    inline void SetPathLength(float fPathLen) { m_fPathLength = fPathLen; };

    inline void SetRotation(const RwV3d* pvRot) { m_vRotation = *pvRot; };
    inline void GetRotation(RwV3d* pvRot) const { *pvRot = m_vRotation; };

    inline bool IsPathEnd(void) const { return (m_fPathT >= 1.0f); };
    inline float GetPathT(void) const { return m_fPathT; };

protected:
    const char* m_pszPathName;
    float m_fPathLength;
    float m_fPathT;
    float m_fPathTimeRate;
    RwV3d m_vRotation;
};