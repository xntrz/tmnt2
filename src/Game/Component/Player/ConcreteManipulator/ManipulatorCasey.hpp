#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CCaseyManipulator : public CManipulator
{
public:
    CCaseyManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CCaseyManipulator(void);
};