#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CRaphaelManipulator : public CManipulator
{
public:
    CRaphaelManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CRaphaelManipulator(void);
};