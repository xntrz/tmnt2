#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CPathGimmickMove : public CGimmickMove
{
public:
    CPathGimmickMove(void);
    virtual ~CPathGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    virtual void SetPathTimeRate(float fTimeRate);
    virtual void UpdatePathTime(float dt);
    virtual void UpdateRotation(float dt);
    void InitPath(const char* pszPathName, float fPathT);
    float GetPathLength(const char* pszPathName);
    float GetPathTByMeterPerSecond(float fMps);
    float GetNearestPathT(const RwV3d* pvPosition);
    bool IsAccelerationToAimPathT(float, float, float, float);
    void SetRotation(const RwV3d* pvRot);
    void SetPathLength(float fPathLen);
    void GetRotation(RwV3d* pvRot);
    bool IsPathEnd(void) const;
    
protected:
    const char* m_pszPathName;
    float m_fPathLength;
    float m_fPathT;
    float m_fPathTimeRate;
    RwV3d m_vRotation;
};