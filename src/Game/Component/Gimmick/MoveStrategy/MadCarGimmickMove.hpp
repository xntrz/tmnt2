#pragma once

#include "PathMoveGimmickMove.hpp"


class CMadCarGimmickMove final : public CPathMoveGimmickMove
{
public:
    CMadCarGimmickMove(void);
    virtual ~CMadCarGimmickMove(void);
    virtual void UpdatePathTime(float dt) override;
    virtual void UpdateRotation(float dt) override;
    void SetCarDirection(bool State);
    bool IsCarDirection(void) const;

private:
    bool m_bCarDirection;
};