#pragma once

#include "RideGimmickMove.hpp"


class CRideRockGimmickMove : public CRideGimmickMove
{
public:
    CRideRockGimmickMove(void);
    virtual ~CRideRockGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

protected:
    static int32 m_iRandomSeed;
    RwV3d m_vRotDir;
};