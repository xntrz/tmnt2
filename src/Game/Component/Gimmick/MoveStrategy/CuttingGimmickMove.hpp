#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CCuttingGimmickMove : public CGimmickMove
{
public:
    CCuttingGimmickMove(void);
    virtual ~CCuttingGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    
    inline void SetVelocity(const RwV3d* pvVelocity) { m_vVelocity = *pvVelocity; };
    inline void GetVelocity(RwV3d* pvVelocity) const { *pvVelocity = m_vVelocity; };
    
    inline void SetRotation(const RwV3d* pvRotation) { m_vRotation = *pvRotation; };
    inline void GetRotation(RwV3d* pvRotation) const { *pvRotation = m_vRotation; };
    
    inline void SetRotVelocity(const RwV3d* pvRotVel){ m_vRotVelocity = *pvRotVel; };
    inline void GetRotVelocity(RwV3d* pvRotVel) const{ *pvRotVel = m_vRotVelocity; };

private:
    RwV3d m_vVelocity;
    RwV3d m_vRotation;
    RwV3d m_vRotVelocity;
};