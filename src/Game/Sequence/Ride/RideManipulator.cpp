#include "RideManipulator.hpp"


/*static*/ CRideManipulator* CRideManipulator::New(CRideCharacter* pRideCharacter, int32 nControllerNo)
{
    char szName[GAMEOBJECTTYPES::NAME_MAX];
    szName[0] = '\0';

    std::sprintf(szName, "ride_mp_%d", nControllerNo);

    return new CRideManipulator(szName, pRideCharacter, nControllerNo);
};


/*static*/ void CRideManipulator::Delete(CRideManipulator* pManip)
{
    ASSERT(pManip);
    delete pManip;
};