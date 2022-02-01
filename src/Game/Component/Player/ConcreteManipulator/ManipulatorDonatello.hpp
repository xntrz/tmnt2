#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CDonatelloManipulator : public CManipulator
{
public:
    CDonatelloManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CDonatelloManipulator(void);
};