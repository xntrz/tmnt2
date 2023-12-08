#pragma once

#include "PathMoveGimmickMove.hpp"


class CGSMachineGimmickMove final : public CPathMoveGimmickMove
{
public:
    CGSMachineGimmickMove(void);
    virtual ~CGSMachineGimmickMove(void);
    virtual void UpdatePathTime(float dt) override;
    virtual void UpdateRotation(float dt) override;
};