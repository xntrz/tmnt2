#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CSplinterManipulator : public CManipulator
{
public:
    CSplinterManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CSplinterManipulator(void);
};