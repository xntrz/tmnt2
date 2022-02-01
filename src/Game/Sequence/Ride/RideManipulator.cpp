#include "RideManipulator.hpp"
#include "RideCharacter.hpp"
#include "RideStage.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"


static const char* s_apszRideManipulatorClassName[] =
{
    "ride_mp_1",
    "ride_mp_2",
    "ride_mp_3",
    "ride_mp_4",
};


/*static*/ CRideManipulator* CRideManipulator::New(CRideCharacter* pRideCharacter, int32 nControllerNo)
{
    int32 nIndex = nControllerNo;
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(s_apszRideManipulatorClassName));
    
    CRideManipulator* pRideManip = new CRideManipulator(
        s_apszRideManipulatorClassName[nIndex],
        pRideCharacter,
        nControllerNo
    );
    ASSERT(pRideManip);
    
    return pRideManip;
};


/*static*/ void CRideManipulator::Delete(CRideManipulator* pManip)
{
    ASSERT(pManip);
    delete pManip;
};


CRideManipulator::CRideManipulator(const char* pszName, CRideCharacter* pRideCharacter, int32 nControllerNo)
: CManipulator(pszName, pRideCharacter, nControllerNo)
, m_pRideCharacter(pRideCharacter)
{
    ;
};


CRideManipulator::~CRideManipulator(void)
{
    ;
};


void CRideManipulator::Run(void)
{
    makeRidePadInput();
    
    if (m_pRideCharacter->IsEnableMove())
    {
        if (CRideStage::m_bSpace)
        {
            rollShip();

            if (m_ridepadinput.m_bShot)
                m_pRideCharacter->ShootingKnife();

            moveShip();
        }
        else
        {
			turnBoard();            

            if (!m_pRideCharacter->IsPlayerFlagSet(PLAYERTYPES::FLAG_AERIAL_STATUS) && m_ridepadinput.m_bJump)
                m_pRideCharacter->ChangeStatus(PLAYERTYPES::RIDESTATUS_JUMP);

            moveBoard();
        };        
    };

    BranchForStatus(m_pRideCharacter->GetStatus());
};


void CRideManipulator::BranchForStatus(PLAYERTYPES::STATUS status)
{
    switch (status)
    {
    case PLAYERTYPES::RIDESTATUS_RUN:
        {
            if (m_ridepadinput.m_fStickX > 0.4f)
            {
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_RIGHT);
            }
            else if (m_ridepadinput.m_fStickX < -0.4f)
            {
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_LEFT);
            }
            else if (m_ridepadinput.m_fStickY > 0.4f)
            {
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_SPEED_UP);
            }
            else if (m_ridepadinput.m_fStickY < -0.4f)
            {
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_SPEED_DOWN);
            };
        }
        break;
        
    case PLAYERTYPES::RIDESTATUS_RIGHT:
        if (m_ridepadinput.m_fStickX < 0.4f)
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);        
        break;
        
    case PLAYERTYPES::RIDESTATUS_LEFT:
        if (m_ridepadinput.m_fStickX > -0.4f)
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
        break;
        
    case PLAYERTYPES::RIDESTATUS_SPEED_UP:
        if (m_ridepadinput.m_fStickY < 0.4f)
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
        break;
        
    case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:
        if (m_ridepadinput.m_fStickY > -0.4f)
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
        break;
        
    case PLAYERTYPES::RIDESTATUS_CRASH_WALL:
        break;
        
    case PLAYERTYPES::RIDESTATUS_REVIVE:
        break;
        
    case PLAYERTYPES::RIDESTATUS_STAGGER:
        break;
        
    case PLAYERTYPES::RIDESTATUS_JUMP:
        break;
        
    case PLAYERTYPES::RIDESTATUS_TOUCHDOWN:
        break;
        
    case PLAYERTYPES::RIDESTATUS_AERIAL:
        break;
        
    case PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER:
        break;
        
    case PLAYERTYPES::RIDESTATUS_SHIP_CRASH:
        break;
        
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT:
        break;
        
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT:
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CRideManipulator::makeRidePadInput(void)
{
    std::memset(&m_ridepadinput, 0x00, sizeof(m_ridepadinput));
    
    int16 nAnalogRX = m_padstream.GetAnalogX(m_nControllerNo);
    int16 nAnalogRY = m_padstream.GetAnalogY(m_nControllerNo);

    float rx = 0.0f;
    float ry = 0.0f;

    if (nAnalogRX >= 0)
        rx = float(nAnalogRX / TYPEDEF::SINT16_MAX);
    else
        rx = -float(nAnalogRX / TYPEDEF::SINT16_MIN);

    if (rx < 0.0f)
        rx = -rx;

    if (rx < 0.4f)
        rx = 0.0f;
    
    if (nAnalogRY >= 0)
        ry = -float(nAnalogRY / TYPEDEF::SINT16_MAX);
    else
        ry = float(nAnalogRY / TYPEDEF::SINT16_MIN);

    if (ry < 0.0f)
        ry = -ry;

    if (ry < 0.4f)
        ry = 0.0f;

    m_ridepadinput.m_fStickX = rx;
    m_ridepadinput.m_fStickY = ry;

#ifdef _TARGET_PC
    uint32 uDigitalMovementMask = ( CController::DIGITAL_UP     |
                                    CController::DIGITAL_DOWN   |
                                    CController::DIGITAL_LEFT   |
                                    CController::DIGITAL_RIGHT);

    uint32 uDigital = m_padstream.GetDigital(m_nControllerNo);
    if (IS_FLAG_SET_ANY(uDigital, uDigitalMovementMask))
    {
        float xDigital = 0.0f;
        float yDigital = 0.0f;

        if (IS_FLAG_SET(uDigital, CController::DIGITAL_LEFT))
        {
            xDigital = -1.0f;
        }
        else if (IS_FLAG_SET(uDigital, CController::DIGITAL_RIGHT))
        {
            xDigital = 1.0f;
        }
        else
        {
            xDigital = 0.0f;
        };

        if (IS_FLAG_SET(uDigital, CController::DIGITAL_UP))
        {
            yDigital = 1.0f;
        }
        else if (IS_FLAG_SET(uDigital, CController::DIGITAL_DOWN))
        {
            yDigital = -1.0f;
        }
        else
        {
            yDigital = 0.0f;
        };

        m_ridepadinput.m_fStickX = xDigital;
        m_ridepadinput.m_fStickY = yDigital;

        if (CRideStage::m_bSpace)
            m_ridepadinput.m_fStickY = -yDigital;
    };
#endif    

    uint32 uDigitalTrigger = m_padstream.GetDigitalTrigger(m_nControllerNo);

    m_ridepadinput.m_bShot = CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_SHOT);
    m_ridepadinput.m_bJump = CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_JUMP);
    m_ridepadinput.m_bLeftTurn = CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_DASH);
    m_ridepadinput.m_bRightTurn = CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_GUARD);
};


void CRideManipulator::moveBoard(void)
{
    RwV3d vBasisVector = Math::VECTOR3_ZERO;
    CRideStage::GetBasisVector(&vBasisVector);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    m_pRideCharacter->GetVelocity(&vVelocity);

    float fControlRate = m_pRideCharacter->GetControlRate();
    
    float dt = CGameProperty::GetElapsedTime();
    float fDamping = vVelocity.x * 0.97f;
    
    float dx = fDamping - (dt * (m_ridepadinput.m_fStickX * fControlRate) * 18.0f);
    float dy = (m_ridepadinput.m_fStickY * fControlRate) * 1.9444f;
    
    vVelocity.x = dx;
    vVelocity.z = (dy + vBasisVector.z);

    m_pRideCharacter->SetVelocity(&vVelocity);    
};


void CRideManipulator::turnBoard(void)
{
    float fTurnDirection = m_pRideCharacter->GetTurnDirection();

    if (fTurnDirection > 0.0f)
    {
        fTurnDirection += CGameProperty::GetElapsedTime() * 9.424778f;
        if (fTurnDirection > Math::PI2)
            fTurnDirection = 0.0f;

        m_pRideCharacter->SetTurnDirection(fTurnDirection);
        m_pRideCharacter->SetDirection(fTurnDirection);
		return;
    }
    else if (fTurnDirection < 0.0f)
    {
        fTurnDirection -= CGameProperty::GetElapsedTime() * 9.424778f;
        if (fTurnDirection < -Math::PI2)
            fTurnDirection = 0.0f;

        m_pRideCharacter->SetTurnDirection(fTurnDirection);
        m_pRideCharacter->SetDirection(fTurnDirection);
		return;
	};

	if (m_pRideCharacter->GetStatus() == PLAYERTYPES::RIDESTATUS_JUMP)
    {
        if (m_ridepadinput.m_bLeftTurn)
        {
            fTurnDirection += CGameProperty::GetElapsedTime() * 9.424778f;
            CGameEvent::SetPlayerRideAction(m_pRideCharacter->GetPlayerNo(), GAMETYPES::RIDEACT_TRICK);
        }
        else if (m_ridepadinput.m_bRightTurn)
        {
            fTurnDirection -= CGameProperty::GetElapsedTime() * 9.424778f;
            CGameEvent::SetPlayerRideAction(m_pRideCharacter->GetPlayerNo(), GAMETYPES::RIDEACT_TRICK);
        };

        m_pRideCharacter->SetTurnDirection(fTurnDirection);
        m_pRideCharacter->SetDirection(fTurnDirection);
    };
};


void CRideManipulator::moveShip(void)
{
    RwV3d vBasisVector = Math::VECTOR3_ZERO;
    CRideStage::GetBasisVector(&vBasisVector);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    m_pRideCharacter->GetVelocity(&vVelocity);

    float fControlRate = m_pRideCharacter->GetControlRate();

    float dt = CGameProperty::GetElapsedTime();
    float fDampingX = vVelocity.x * 0.97f;
    float fDampingY = vVelocity.y * 0.97f;

    float dx = fDampingX - (dt * (m_ridepadinput.m_fStickX * fControlRate) * 18.0f);
    float dy = fDampingY - (dt * (m_ridepadinput.m_fStickY * fControlRate) * 18.0f);

    vVelocity.x = dx;
    vVelocity.y = dy;
    vVelocity.z = vBasisVector.z;

    m_pRideCharacter->SetVelocity(&vVelocity);
};


void CRideManipulator::rollShip(void)
{
    if (m_ridepadinput.m_bLeftTurn)
    {
        m_pRideCharacter->ChangeStatus(PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT);
    }
    else if(m_ridepadinput.m_bRightTurn)
    {
        m_pRideCharacter->ChangeStatus(PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT);
    };
};