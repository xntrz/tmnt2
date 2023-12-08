#pragma once

#include "RideCharacter.hpp"

#include "Game/Component/Player/Manipulator.hpp"


class CRideManipulator : public CManipulator
{
public:
    static CRideManipulator* New(CRideCharacter* pRideCharacter, int32 nControllerNo);
    static void Delete(CRideManipulator* pManip);

    inline CRideManipulator(const char* pszName, CRideCharacter* pRideCharacter, int32 nControllerNo) : CManipulator(pszName, pRideCharacter, nControllerNo) {};
    virtual ~CRideManipulator(void) {};
	virtual void Run(void) override {}; // prevent model rotation xd
};