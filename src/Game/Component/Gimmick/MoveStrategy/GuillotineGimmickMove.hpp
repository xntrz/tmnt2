#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"

#include "Game/System/Map/WorldMap.hpp"


class CGuillotineGimmickMove final : public CGimmickMove
{
public:
    struct INITPARAM
    {
        float m_fTheta;
        float m_fOnceTheta;
        float m_fMaxMove;
        float m_fHitRadius;
        RwV3d m_vFulcrumVector;
        RwV3d m_vHitSpherePos;
    };

    enum PENDULUMSTATE
    {
		PENDULUMSTATE_RIGHT = 0,
		PENDULUMSTATE_CENTER_TO_LEFT,
		PENDULUMSTATE_LEFT,
		PENDULUMSTATE_CENTER_TO_RIGHT,
    };
    
public:
    CGuillotineGimmickMove(const INITPARAM* pInitParam);
    virtual ~CGuillotineGimmickMove(void);    
    virtual RESULT OnMove(float dt) override;
    void InitFulcrumMatrix(void);

    inline void SetRotation(const RwV3d* pvRot) { m_vRotation = *pvRot; };
    inline void GetRotation(RwV3d* pvRot) { *pvRot = m_vRotation; };

    inline const CWorldMap::COLLISIONRESULT& GetCollisionResult(void) const { return m_collisionResult; };
    
    inline PENDULUMSTATE GetPendulumState(void) const { return m_ePendulumState; };

private:
    void windSE(void);
    void pendulumMove(float dt);
    void reviceTheta(float dt);

private:
    RwV3d m_vRotation;
    float m_fTheta;
    float m_fOnceTheta;
    float m_fMaxMove;
    float m_fHitRadius;
    float m_fSwingTimer;
    float m_fSwingTime;
    RwMatrix m_matFulcrumPoint;
    RwV3d m_vFulcrumVector;
    RwV3d m_vHitSpherePos;
    CWorldMap::COLLISIONRESULT m_collisionResult;
    PENDULUMSTATE m_ePendulumState;
};