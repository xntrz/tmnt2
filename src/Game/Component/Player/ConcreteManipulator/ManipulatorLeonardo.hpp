#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CLeonardoManipulator : public CManipulator
{
public:
    CLeonardoManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CLeonardoManipulator(void);
};