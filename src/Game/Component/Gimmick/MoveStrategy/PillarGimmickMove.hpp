#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CPillarGimmickMove : public CGimmickMove
{
public:
    CPillarGimmickMove(void);
    virtual ~CPillarGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

    inline void SetVelocity(const RwV3d* pvVelocity) { m_vVelocity = *pvVelocity; };
    inline void GetVelocity(RwV3d* pvVelocity) const { *pvVelocity = m_vVelocity; };

    inline void SetRotation(const RwV3d* pvRotation) { m_vRotation = *pvRotation; };
    inline void GetRotation(RwV3d* pvRotation) const { *pvRotation = m_vRotation; };

private:
    RwV3d m_vVelocity;
    RwV3d m_vRotation;
};