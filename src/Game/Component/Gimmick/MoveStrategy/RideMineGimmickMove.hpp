#pragma once

#include "RideGimmickMove.hpp"


class CRideMineGimmickMove final : public CRideGimmickMove
{
public:
    CRideMineGimmickMove(int32 iNum);
    virtual ~CRideMineGimmickMove(void);
    virtual RESULT OnMove(float dt) override;

private:
    static float m_fT;
    static float m_fA;
};