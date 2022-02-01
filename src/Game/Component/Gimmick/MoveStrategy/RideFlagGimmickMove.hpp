#pragma once

#include "RideGimmickMove.hpp"


class CRideFlagGimmickMove : public CRideGimmickMove
{
public:
    CRideFlagGimmickMove(int32 iType);
    virtual ~CRideFlagGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

protected:
    float m_fA;
    float m_fT;
};