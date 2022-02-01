#pragma once

#include "Game/Component/Player/Manipulator.hpp"


class CRideCharacter;


class CRideManipulator : public CManipulator
{
private:
    struct RIDEPADINPUT
    {
        float m_fStickX;
        float m_fStickY;
        bool m_bJump;
        bool m_bShot;
        bool m_bRightTurn;
        bool m_bLeftTurn;
    };
    
public:
    static CRideManipulator* New(CRideCharacter* pRideCharacter, int32 nControllerNo);
    static void Delete(CRideManipulator* pManip);

    CRideManipulator(const char* pszName, CRideCharacter* pRideCharacter, int32 nControllerNo);
    virtual ~CRideManipulator(void);
    virtual void Run(void) override;
    virtual void BranchForStatus(PLAYERTYPES::STATUS status) override;
    
private:
    void makeRidePadInput(void);
    void moveBoard(void);
    void turnBoard(void);
    void moveShip(void);
    void rollShip(void);

private:
    RIDEPADINPUT m_ridepadinput;
    CRideCharacter* m_pRideCharacter;
};