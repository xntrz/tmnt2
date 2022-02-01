#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CKaraiManipulator : public CManipulator
{
public:
    CKaraiManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CKaraiManipulator(void);
};