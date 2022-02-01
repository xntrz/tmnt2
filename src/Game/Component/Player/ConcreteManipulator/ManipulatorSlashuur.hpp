#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CSlashuurManipulator : public CManipulator
{
public:
    CSlashuurManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CSlashuurManipulator(void);
};