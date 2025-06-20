#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CPathMoveGimmickMove : public CGimmickMove
{
public:
    CPathMoveGimmickMove(void);
    virtual ~CPathMoveGimmickMove(void) {};
    virtual RESULT OnMove(float dt) override;
    virtual void SetPathTimeRate(float fTimeRate);
    virtual void UpdatePathTime(float dt);
    virtual void UpdateRotation(float dt);
    void InitPath(const char* pszPathName, float fPathT);
    float GetPathLength(const char* pszPathName);
    float GetPathTByMeterPerSecond(float fMps);
    float GetNearestPathT(RwV3d* pvPosition);
    bool IsAccelerationToAimPathT(float fAimPathT, float fAimPathTimerate, float fAimPathTimerateIntervalStep, float fAimPathTimerateInterval);
    void GetSplinePos(RwV3d* pvPos, float fPathT) const;
    void GetSplinePos(RwV3d* pvPos, const char* pszPathName, float fPathT) const;
    void GetSplinePos(RwV3d* pvPos, int32 iPathID, float fPathT) const;
    void SetPathLength(float fPathLen);
    void SetRotation(const RwV3d* pvRot);
    void GetRotation(RwV3d* pvRot) const;
    bool IsPathEnd(void) const;
    float GetPathT(void) const;

protected:
    const char* m_pszPathName;
    float m_fPathLength;
    float m_fPathT;
    float m_fPathTimeRate;
    RwV3d m_vRotation;
};