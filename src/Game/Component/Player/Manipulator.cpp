#include "Manipulator.hpp"
#include "PlayerCharacter.hpp"

#include "ConcreteManipulator/ManipulatorLeonardo.hpp"
#include "ConcreteManipulator/ManipulatorRaphael.hpp"
#include "ConcreteManipulator/ManipulatorMichelangero.hpp"
#include "ConcreteManipulator/ManipulatorDonatello.hpp"
#include "ConcreteManipulator/ManipulatorSlashuur.hpp"
#include "ConcreteManipulator/ManipulatorCasey.hpp"
#include "ConcreteManipulator/ManipulatorKarai.hpp"
#include "ConcreteManipulator/ManipulatorSplinter.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Controller.hpp"


/*static*/ CManipulator* CManipulator::New(CPlayerCharacter* pPlayerChr, int32 nControllerNo)
{
    switch (pPlayerChr->GetID())
    {
    case PLAYERID::ID_LEO:
        return new CLeonardoManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_RAPH:
        return new CRaphaelManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_MIC:
        return new CMichelangeroManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_DON:
        return new CDonatelloManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_SLASHUUR:
        return new CSlashuurManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_CASEY:
        return new CCaseyManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_KARAI:
        return new CKaraiManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_SPLINTER:
        return new CSplinterManipulator(pPlayerChr, nControllerNo);

    default:
        ASSERT(false);
        return nullptr;
    };
};


/*static*/ void CManipulator::Delete(CManipulator* pManip)
{
    ASSERT(pManip);    
    delete pManip;
};


CManipulator::CManipulator(const char* pszName, CPlayerCharacter* pPlayerChr, int32 nControllerNo)
: CGameObject(pszName, GAMEOBJECTTYPE::MANIPULATOR)
, m_pPlayerChr(pPlayerChr)
, m_input({ 0 })
, m_nControllerNo(nControllerNo)
{
    if (CPadStreamSwitch::m_mode)
    {
        bool bResult = m_padstream.Open(
            CPadStreamSwitch::m_mode,
            CGameData::PlayParam().GetStage(),
            m_nControllerNo
        );

        ASSERT(bResult);
    };
};


CManipulator::~CManipulator(void)
{
    m_padstream.Close();
};


void CManipulator::Run(void)
{
    ASSERT(m_pPlayerChr);

    AnalyzeInputDevice();
    SetConfusionInput();
    SetSpecificAbilityFlag();
    BranchForStatus(m_pPlayerChr->GetStatus());
};


void CManipulator::BranchForStatus(PLAYERTYPES::STATUS status)
{
    switch (status)
    {
    case PLAYERTYPES::STATUS_IDLE:
        RunIdle();
        break;

    case PLAYERTYPES::STATUS_WALK:
        RunWalk();
        break;

    case PLAYERTYPES::STATUS_RUN:
        RunRun();
        break;

    case PLAYERTYPES::STATUS_JUMP:
        RunJump();
        break;

    case PLAYERTYPES::STATUS_JUMP_2ND:
        RunJump2nd();
        break;

    case PLAYERTYPES::STATUS_JUMP_WALL:
        RunJumpWall();
        break;

    case PLAYERTYPES::STATUS_AERIAL:
        RunAerial();
        break;

    case PLAYERTYPES::STATUS_AERIAL_MOVE:
        RunAerialMove();
        break;

    case PLAYERTYPES::STATUS_GUARD_READY:
        RunGuardReady();
        break;

    case PLAYERTYPES::STATUS_GUARD:
        RunGuard();
        break;

    case PLAYERTYPES::STATUS_ATTACK_A:
        RunAttackA();
        break;

    case PLAYERTYPES::STATUS_ATTACK_AA:
        RunAttackAA();
        break;

    case PLAYERTYPES::STATUS_ATTACK_AAB:
        RunAttackAAB();
        break;

    case PLAYERTYPES::STATUS_ATTACK_AAC:
        RunAttackAAC();
        break;

    case PLAYERTYPES::STATUS_ATTACK_AABB:
        RunAttackAABB();
        break;
        
    case PLAYERTYPES::STATUS_ATTACK_AABC:
        RunAttackAABC();
        break;

    case PLAYERTYPES::STATUS_ATTACK_AABBB:
        RunAttackAABBB();
        break;

    case PLAYERTYPES::STATUS_ATTACK_AABBC:
        RunAttackAABBC();        
        break;

    case PLAYERTYPES::STATUS_ATTACK_B_CHARGE:
        RunAttackBCharge();
        break;

    case PLAYERTYPES::STATUS_ATTACK_KNIFE:
        RunAttackKnife();
        break;

    case PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP:
        RunAttackKnifeJump();
        break;

    case PLAYERTYPES::STATUS_LIFT:
        RunLift();
        break;

    case PLAYERTYPES::STATUS_LIFT_WALK:
        RunLiftWalk();
        break;

    case PLAYERTYPES::STATUS_THROWN_COMBINATION:
        RunThrownCombination();
        break;

    case PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT:
    case PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK:
        RunFlyawayBound();
        break;
            
    case PLAYERTYPES::STATUS_DINDLE:
    case PLAYERTYPES::STATUS_STUN:
    case PLAYERTYPES::STATUS_BIND:
        RunStatusDamage();
        break;

    case PLAYERTYPES::STATUS_PUSH:
        RunPush();
        break;
    };
};


void CManipulator::RunIdle(void)
{
    RunGrounding();
};


void CManipulator::RunWalk(void)
{
    RunGrounding();
};


void CManipulator::RunRun(void)
{
    RunGrounding();
};


void CManipulator::RunGrounding(void)
{
    PLAYERTYPES::STATUS status = PLAYERTYPES::STATUS_IDLE;
    
    if (m_input.m_uAttack == 1)
    {
        status = PLAYERTYPES::STATUS_ATTACK_A;
    }
    else if (m_input.m_uAttack == 2)
    {
        status = PLAYERTYPES::STATUS_ATTACK_B;
    }
    else if (m_input.m_uKnife == 1 && m_pPlayerChr->IsEnableAttackKnife())
    {
        status = PLAYERTYPES::STATUS_ATTACK_KNIFE;
    }
    else if (m_input.m_uGuard == 1)
    {
        status = PLAYERTYPES::STATUS_GUARD_READY;
    }
    else if (m_input.m_uDash == 1)
    {
        status = PLAYERTYPES::STATUS_DASH;
    }
    else if (m_input.m_uJump == 1)
    {
        status = PLAYERTYPES::STATUS_JUMP;
    }
    else if (m_input.m_uMove == 1)
    {
        status = PLAYERTYPES::STATUS_WALK;
    }
    else if (m_input.m_uMove == 2)
    {
        status = PLAYERTYPES::STATUS_RUN;
    };

    m_pPlayerChr->ChangeStatus(status);
    if (m_input.m_uMove)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);
};


void CManipulator::RunJump(void)
{
    if (!RunAerialCommon() && m_input.m_uMove)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);
};


void CManipulator::RunJump2nd(void)
{
    if (!RunAerialCommon() && m_input.m_uMove)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);
};


void CManipulator::RunJumpWall(void)
{
    RunAerialCommon();
};


void CManipulator::RunAerial(void)
{
    if (!RunAerialCommon() && m_input.m_uMove)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);
};


void CManipulator::RunAerialMove(void)
{
    if (!RunAerialCommon())
    {
        if (m_input.m_uMove)
            m_pPlayerChr->SetDirection(m_input.m_fDirection);
        else
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL);
    };
};


bool CManipulator::RunAerialCommon(void)
{
    bool bResult = false;

    if (m_input.m_uAttack && m_pPlayerChr->IsEnableAttackJump())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_JUMP);
        bResult = true;
    }
    else if ((m_input.m_uKnife == 1) && m_pPlayerChr->IsEnableAttackKnife())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP);
        bResult = true;
    }
    else if ((m_input.m_uJump == 1) && m_pPlayerChr->IsEnableJumpWall())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_JUMP_WALL);
        bResult = true;
    }
    else if ((m_input.m_uJump == 1) && m_pPlayerChr->IsEnableJump2nd())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_JUMP_2ND);
        bResult = true;
    };

    return bResult;
};


void CManipulator::RunGuardReady(void)
{
    RunGuardCommon();
};


void CManipulator::RunGuard(void)
{
    if (m_input.m_uGuard)
        RunGuardCommon();
    else
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_IDLE);
};


void CManipulator::RunGuardCommon(void)
{
    if (m_input.m_uLift == 1)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_LIFT);
};


void CManipulator::RunAttackA(void)
{
    RunAttackCommon();
};


void CManipulator::RunAttackAA(void)
{
    RunAttackCommon();
};


void CManipulator::RunAttackAAB(void)
{
    RunAttackCommon();
};


void CManipulator::RunAttackAAC(void)
{
    RunAttackCommon();
};


void CManipulator::RunAttackAABB(void)
{
    RunAttackCommon();
};


void CManipulator::RunAttackAABC(void)
{
    RunAttackCommon();
};


void CManipulator::RunAttackAABBB(void)
{
    RunAttackCommon();
};


void CManipulator::RunAttackAABBC(void)
{
    ;
};


void CManipulator::RunAttackCommon(void)
{
    if (m_input.m_uDash == 1)
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_DASH_CANCEL);        
    }
    else
    {
        switch (m_input.m_uAttack)
        {
        case 1:
            m_pPlayerChr->RequestAttackA();
            break;

        case 2:
            m_pPlayerChr->RequestAttackB();
            break;

        case 3:
            m_pPlayerChr->RequestAttackC();
            break;
        };

        if (m_input.m_uMove)
            m_pPlayerChr->RotateDirection(m_input.m_fDirection);
    };
};


void CManipulator::RunAttackBCharge(void)
{
    if (!m_input.m_uAttackCharge)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_B);
};


void CManipulator::RunAttackKnife(void)
{
    if (m_input.m_uKnife == 1 && m_pPlayerChr->IsEnableAttackKnife())
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE);
};


void CManipulator::RunAttackKnifeJump(void)
{
    if (m_input.m_uKnife == 1 && m_pPlayerChr->IsEnableAttackKnifeJump())
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE);
    else
        m_pPlayerChr->SetDirection(m_input.m_fDirection);
};


void CManipulator::RunLift(void)
{
    RunLiftCommon();
};


void CManipulator::RunLiftWalk(void)
{
    RunLiftCommon();
};


void CManipulator::RunLiftCommon(void)
{
    m_pPlayerChr->LiftInfo().m_bMissThrow = true;
    
    if (m_input.m_uAttack == 2)
    {
        m_pPlayerChr->LiftInfo().m_bMissThrow = false;
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_THROW);
    }
    else
    {
		if (m_input.m_uMove)
		{
			if ((m_input.m_uMove > 0 && m_input.m_uMove <= 2) && (m_pPlayerChr->GetStatus() != PLAYERTYPES::STATUS_LIFT_WALK))
			{
				m_pPlayerChr->LiftInfo().m_bMissThrow = false;
				m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_LIFT_WALK);
			}
		}
        else if (m_pPlayerChr->GetStatus() != PLAYERTYPES::STATUS_LIFT)
        {
            m_pPlayerChr->LiftInfo().m_bMissThrow = false;
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_LIFT);
        };
    };

    if (m_input.m_uMove)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);
};


void CManipulator::RunThrownCombination(void)
{
    if (m_input.m_uMove)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);
};


void CManipulator::RunFlyawayBound(void)
{
    if (m_input.m_uPassive == 1)
        m_pPlayerChr->SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_PASSIVE, true);
};


void CManipulator::RunStatusDamage(void)
{
    if (m_input.m_uRecover == 1)
        m_pPlayerChr->SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_RECOVER, true);
};


void CManipulator::RunPush(void)
{

};


void CManipulator::AnalyzeInputDevice(void)
{
    m_padstream.GetPad(m_nControllerNo);

    RwV3d vInputVector = Math::VECTOR3_ZERO;
    float fInputVectorLen = 0.0f;
    
    AnalyzeInputVector(vInputVector, fInputVectorLen);

    if (fInputVectorLen <= 0.4f)
    {
        m_input.m_uMove = 0;
        m_input.m_fDirection = 0.0f;
    }
    else
    {
        if (fInputVectorLen >= 0.9f)
            m_input.m_uMove = 2;
        else
            m_input.m_uMove = 1;

        RwMatrix modeling;
        RwV3d right = Math::VECTOR3_ZERO;
        RwV3d at = Math::VECTOR3_ZERO;
        RwV3d target = Math::VECTOR3_ZERO;

        CGameProperty::GetCameraFrameMatrix(&modeling);

        Math::Vec3_Normalize(&right, &modeling.right);
        Math::Vec3_Multiply(&right, &right, vInputVector.x);

        Math::Vec3_Normalize(&at, &modeling.at);
        Math::Vec3_Multiply(&at, &at, vInputVector.z);

        Math::Vec3_Add(&target, &at, &right);
        target.y = 0.0f;
        Math::Vec3_Normalize(&target, &target);

        m_input.m_fDirection = atan2(-target.x, -target.z);
    };

    uint32 uDigital = m_padstream.GetDigital(m_nControllerNo);
    uint32 uDigitalTrigger = m_padstream.GetDigitalTrigger(m_nControllerNo);

    m_input.m_uDash = CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_DASH);
    m_input.m_uJump = CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_JUMP);

    if (CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_ATTACK_A) &&
        CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_ATTACK_B))
    {
        m_input.m_uAttack = 3;
    }
    else if (CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_ATTACK_B))
    {
        m_input.m_uAttack = 2;
    }
    else if (CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_ATTACK_A))
    {
        m_input.m_uAttack = 1;
    }
	else
	{
		m_input.m_uAttack = 0;
	};

    m_input.m_uAttackCharge = CGamepad::CheckFunction(uDigital, CGamepad::FUNCTION_ATTACK_B);
    m_input.m_uKnife = CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_SHOT);
    m_input.m_uGuard = CGamepad::CheckFunction(uDigital, CGamepad::FUNCTION_GUARD);
    
	m_input.m_uLift =
		CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_ATTACK_B) &&
		CGamepad::CheckFunction(uDigital, CGamepad::FUNCTION_GUARD);

    m_input.m_uRecover = IS_FLAG_SET_ANY(
        uDigitalTrigger,
        CController::DIGITAL_A |
        CController::DIGITAL_B |
        CController::DIGITAL_X |
        CController::DIGITAL_Y
    );

    m_input.m_uPassive =
        (CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_ATTACK_A) ||
        CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_ATTACK_B) ||
        CGamepad::CheckFunction(uDigitalTrigger, CGamepad::FUNCTION_JUMP));

	m_padstream.GetInput(&m_input, sizeof(m_input));
};


void CManipulator::AnalyzeInputVector(RwV3d& rvInputVector, float& rfInputVectorLength)
{
    int16 nAnalogRX = m_padstream.GetAnalogX(m_nControllerNo);
    int16 nAnalogRY = m_padstream.GetAnalogY(m_nControllerNo);

    float rx = 0.0f;
    float ry = 0.0f;

    if (nAnalogRX >= 0)
        rx = float(nAnalogRX / TYPEDEF::SINT16_MAX);
    else
        rx = -float(nAnalogRX / TYPEDEF::SINT16_MIN);

    if (nAnalogRY >= 0)
        ry = -float(nAnalogRY / TYPEDEF::SINT16_MAX);
    else
        ry = float(nAnalogRY / TYPEDEF::SINT16_MIN);

    rvInputVector.x = rx;
    rvInputVector.y = 0.0f;
    rvInputVector.z = ry;
    rfInputVectorLength = Math::Vec3_Length(&rvInputVector);

#ifdef _TARGET_PC
    uint32 uDigitalMovementMask =   (CController::DIGITAL_UP |
                                    CController::DIGITAL_DOWN |
                                    CController::DIGITAL_LEFT |
                                    CController::DIGITAL_RIGHT);

    uint32 uDigital = m_padstream.GetDigital(m_nControllerNo);
    if (!IS_FLAG_SET_ANY(uDigital, uDigitalMovementMask))
        return;
    
    float xDigital, yDigital;

    //
    //  Check X axis
    //
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

    //
    //  Check Y axis
    //
    if (IS_FLAG_SET(uDigital, CController::DIGITAL_UP))
    {
        yDigital = -1.0f;
    }
    else if (IS_FLAG_SET(uDigital, CController::DIGITAL_DOWN))
    {
        yDigital = 1.0f;
    }
    else
    {
        yDigital = 0.0f;
    };
    
    rvInputVector.x = xDigital;
    rvInputVector.y = 0.0f;
    rvInputVector.z = yDigital;
    rfInputVectorLength = 1.0f;
#endif    
};


void CManipulator::SetSpecificAbilityFlag(void)
{
    m_pPlayerChr->SetPlayerFlag(
        PLAYERTYPES::FLAG_PUSH,
        bool(m_input.m_uMove != 0 )
    );

    m_pPlayerChr->SetPlayerFlag(
        PLAYERTYPES::FLAG_CONSOLE,
        bool(m_input.m_uAttack != 0)
    );
};


void CManipulator::SetConfusionInput(void)
{
    if (m_pPlayerChr->IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_CONFUSION))
    {
        m_pPlayerChr->SetDirection(
            m_pPlayerChr->GetDirection() + Math::PI
        );
    };
};

