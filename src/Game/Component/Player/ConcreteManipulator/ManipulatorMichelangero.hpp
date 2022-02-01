#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CMichelangeroManipulator : public CManipulator
{
public:
    CMichelangeroManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CMichelangeroManipulator(void);
    virtual void BranchForStatus(PLAYERTYPES::STATUS status) override;
    void RunAttackJump(void);
};