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


/*static*/ CManipulator* CManipulator::New(CPlayerCharacter* pPlayerChr, int32 nControllerNo)
{
    switch (pPlayerChr->GetID())
    {
    case PLAYERID::ID_LEO:
        return new CLeonardoManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_RAP:
        return new CRaphaelManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_MIC:
        return new CMichelangeroManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_DON:
        return new CDonatelloManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_SLA:
        return new CSlashuurManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_CAS:
        return new CCaseyManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_KAR:
        return new CKaraiManipulator(pPlayerChr, nControllerNo);

    case PLAYERID::ID_SPL:
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
, m_input()
, m_nControllerNo(nControllerNo)
{
	ClearInput();

    if (CPadStreamSwitch::m_mode)
    {
        STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();

        bool bResult = m_padstream.Open(CPadStreamSwitch::m_mode, stageId, m_nControllerNo);
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

    default:
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
	bool bChangeStatus = true;

    if (m_input.m_attack == ATTACK_A)
    {
        status = PLAYERTYPES::STATUS_ATTACK_A;
    }
    else if (m_input.m_attack == ATTACK_B)
    {
        status = PLAYERTYPES::STATUS_ATTACK_B;
    }
    else if ((m_input.m_knife == KNIFE_ON) &&
              m_pPlayerChr->IsEnableAttackKnife())
    {
        status = PLAYERTYPES::STATUS_ATTACK_KNIFE;
    }
    else if (m_input.m_guard == GUARD_ON)
    {
        status = PLAYERTYPES::STATUS_GUARD_READY;
    }
    else if (m_input.m_dash == DASH_ON)
    {
        status = PLAYERTYPES::STATUS_DASH;
    }
    else if (m_input.m_jump == JUMP_ON)
    {
        status = PLAYERTYPES::STATUS_JUMP;
    }
    else if (m_input.m_move == MOVE_WALK)
    {
		if (m_pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_WALK)
			bChangeStatus = false;

		status = PLAYERTYPES::STATUS_WALK;
    }
    else if (m_input.m_move == MOVE_RUN)
    {
		if (m_pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_RUN)
			bChangeStatus = false;

        status = PLAYERTYPES::STATUS_RUN;
    }
	else
	{
        if (m_pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_IDLE)
            bChangeStatus = false;

        status = PLAYERTYPES::STATUS_IDLE;
    };

    if (bChangeStatus)
		m_pPlayerChr->ChangeStatus(status);

    if (m_input.m_move)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);
};


void CManipulator::RunJump(void)
{
    if (!RunAerialCommon() && m_input.m_move)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);
};


void CManipulator::RunJump2nd(void)
{
    if (!RunAerialCommon() && m_input.m_move)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);
};


void CManipulator::RunJumpWall(void)
{
    RunAerialCommon();
};


void CManipulator::RunAerial(void)
{
    if (!RunAerialCommon() && m_input.m_move)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);
};


void CManipulator::RunAerialMove(void)
{
    if (!RunAerialCommon())
    {
        if (m_input.m_move)
            m_pPlayerChr->SetDirection(m_input.m_fDirection);
        else
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL);
    };
};


bool CManipulator::RunAerialCommon(void)
{
    bool bResult = true;

    if (m_input.m_attack &&
        m_pPlayerChr->IsEnableAttackJump())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_JUMP);
    }
    else if ((m_input.m_knife == KNIFE_ON) &&
              m_pPlayerChr->IsEnableAttackKnife())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP);
    }
    else if ((m_input.m_jump == JUMP_ON) &&
              m_pPlayerChr->IsEnableJumpWall())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_JUMP_WALL);
    }
    else if ((m_input.m_jump == JUMP_ON) &&
              m_pPlayerChr->IsEnableJump2nd())
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_JUMP_2ND);
    }
    else
    {
        bResult = false;
    };
    
    return bResult;
};


void CManipulator::RunGuardReady(void)
{
    RunGuardCommon();
};


void CManipulator::RunGuard(void)
{
    if (m_input.m_guard)
        RunGuardCommon();
    else
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_IDLE);
};


void CManipulator::RunGuardCommon(void)
{
    if (m_input.m_lift == LIFT_ON)
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
    if (m_input.m_dash == DASH_ON)
    {
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_DASH_CANCEL);        
    }
    else
    {
        switch (m_input.m_attack)
        {
        case ATTACK_A:
            m_pPlayerChr->RequestAttackA();
            break;

        case ATTACK_B:
            m_pPlayerChr->RequestAttackB();
            break;

        case ATTACK_C:
            m_pPlayerChr->RequestAttackC();
            break;

        default:
            break;
        };

        if (m_input.m_move)
            m_pPlayerChr->RotateDirection(m_input.m_fDirection);
    };
};


void CManipulator::RunAttackBCharge(void)
{
    if (m_input.m_attackCharge == ATTACK_CHARGE_OFF)
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_B);
};


void CManipulator::RunAttackKnife(void)
{
    if ((m_input.m_knife == KNIFE_ON) && m_pPlayerChr->IsEnableAttackKnife())
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE);
};


void CManipulator::RunAttackKnifeJump(void)
{
    if ((m_input.m_knife == KNIFE_ON) && m_pPlayerChr->IsEnableAttackKnifeJump())
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP);
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
    
    if (m_input.m_attack == ATTACK_B)
    {
        m_pPlayerChr->LiftInfo().m_bMissThrow = false;
        m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_THROW);
    }
    else
    {
		if (m_input.m_move)
		{
            if (((m_input.m_move > MOVE_IDLE) && (m_input.m_move <= MOVE_RUN)) &&
                (m_pPlayerChr->GetStatus() != PLAYERTYPES::STATUS_LIFT_WALK))
            {
                m_pPlayerChr->LiftInfo().m_bMissThrow = false;
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_LIFT_WALK);
            };
		}
        else if (m_pPlayerChr->GetStatus() != PLAYERTYPES::STATUS_LIFT)
        {
            m_pPlayerChr->LiftInfo().m_bMissThrow = false;
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_LIFT);
        };
    };

    if (m_input.m_move)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);
};


void CManipulator::RunThrownCombination(void)
{
    if (m_input.m_move)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);
};


void CManipulator::RunFlyawayBound(void)
{
    if (m_input.m_passive == PASSIVE_ON)
        m_pPlayerChr->SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_PASSIVE);
};


void CManipulator::RunStatusDamage(void)
{
    if (m_input.m_recover == RECOVER_ON)
        m_pPlayerChr->SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_RECOVER);
};


void CManipulator::RunPush(void)
{
    ;
};


void CManipulator::AnalyzeInputDevice(void)
{
    ClearInput();

    m_padstream.GetPadData(m_nControllerNo);

    RwV3d vInputVector = Math::VECTOR3_ZERO;
    float fInputVectorLen = 0.0f;

    AnalyzeInputVector(vInputVector, fInputVectorLen);

    if (fInputVectorLen <= 0.4f)
    {
        m_input.m_move = MOVE_IDLE;
        m_input.m_fDirection = 0.0f;
    }
    else
    {
        m_input.m_move = (fInputVectorLen >= 0.9f ? MOVE_RUN : MOVE_WALK);

        RwMatrix modeling;
        CGameProperty::GetCameraFrameMatrix(&modeling);

        RwV3d vecRight = Math::VECTOR3_ZERO;
        Math::Vec3_Normalize(&vecRight, &modeling.right);
        Math::Vec3_Scale(&vecRight, &vecRight, vInputVector.x);

        RwV3d vecAt = Math::VECTOR3_ZERO;
        Math::Vec3_Normalize(&vecAt, &modeling.at);
        Math::Vec3_Scale(&vecAt, &vecAt, vInputVector.z);

        RwV3d vecTarget = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecTarget, &vecAt, &vecRight);
        vecTarget.y = 0.0f;
        
        Math::Vec3_Normalize(&vecTarget, &vecTarget);

        m_input.m_fDirection = Math::ATan2(-vecTarget.x, -vecTarget.z);
    };

    uint32 uDigital = m_padstream.GetDigital(m_nControllerNo);
    uint32 uDigitalTrigger = m_padstream.GetDigitalTrigger(m_nControllerNo);

    if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_DASH))
        m_input.m_dash = DASH_ON;

    if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_JUMP))
        m_input.m_jump = JUMP_ON;
    
    if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_ATTACK_A) &&
        IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_ATTACK_B))
    {
        m_input.m_attack = ATTACK_C;
    }
    else if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_ATTACK_B))
    {
        m_input.m_attack = ATTACK_B;
    }
    else if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_ATTACK_A))
    {
        m_input.m_attack = ATTACK_A;
    };

    if (IGamepad::CheckFunction(uDigital, IGamepad::FUNCTION_ATTACK_B))
        m_input.m_attackCharge = ATTACK_CHARGE_ON;
    
    if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_SHOT))
        m_input.m_knife = KNIFE_ON;

    if (IGamepad::CheckFunction(uDigital, IGamepad::FUNCTION_GUARD))
        m_input.m_guard = GUARD_ON;

    if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_ATTACK_B) &&
        IGamepad::CheckFunction(uDigital, IGamepad::FUNCTION_GUARD))
        m_input.m_lift = LIFT_ON;

    uint32 uRecorverMask = CController::DIGITAL_RUP
                         | CController::DIGITAL_RDOWN
                         | CController::DIGITAL_RLEFT
                         | CController::DIGITAL_RRIGHT;
    if (uDigitalTrigger & uRecorverMask)
        m_input.m_recover = RECOVER_ON;

    if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_ATTACK_A) ||
        IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_ATTACK_B) ||
        IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_JUMP))
        m_input.m_passive = PASSIVE_ON;

    m_padstream.GetInput(&m_input, sizeof(m_input));
};


void CManipulator::AnalyzeInputVector(RwV3d& rvInputVector, float& rfInputVectorLength)
{
    float x =  static_cast<float>(m_padstream.GetAnalogX(m_nControllerNo));
    float y = -static_cast<float>(m_padstream.GetAnalogY(m_nControllerNo));

    x = (x >= 0.0f ?  (x / static_cast<float>(TYPEDEF::SINT16_MAX)) : -(x / static_cast<float>(TYPEDEF::SINT16_MIN)));
    y = (y >= 0.0f ? -(y / static_cast<float>(TYPEDEF::SINT16_MAX)) :  (y / static_cast<float>(TYPEDEF::SINT16_MIN)));

    rvInputVector.x = x;
    rvInputVector.y = 0.0f;
    rvInputVector.z = y;
    
    rfInputVectorLength = Math::Vec3_Length(&rvInputVector);

#ifdef TARGET_PC
    uint32 digitalMovementMask = CController::DIGITAL_LUP
                               | CController::DIGITAL_LDOWN
                               | CController::DIGITAL_LLEFT
                               | CController::DIGITAL_LRIGHT;

    uint32 digital = m_padstream.GetDigital(m_nControllerNo);
    if (!(digital & digitalMovementMask))
        return;
    
    float xDigital, yDigital;

    //
    //  Check X axis
    //
    if (digital & CController::DIGITAL_LLEFT)
    {
        xDigital = -1.0f;
    }
    else if (digital & CController::DIGITAL_LRIGHT)
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
    if (digital & CController::DIGITAL_LUP)
    {
        yDigital = -1.0f;
    }
    else if (digital & CController::DIGITAL_LDOWN)
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
#endif /* TARGET_PC */
};


void CManipulator::SetSpecificAbilityFlag(void)
{
    /* push flag ctrl */
    if (m_input.m_move == MOVE_RUN)
        m_pPlayerChr->SetPlayerFlag(PLAYERTYPES::FLAG_PUSH);
    else
        m_pPlayerChr->ClearPlayerFlag(PLAYERTYPES::FLAG_PUSH);

    /* console flag ctrl */
    if (m_input.m_attack)
        m_pPlayerChr->SetPlayerFlag(PLAYERTYPES::FLAG_CONSOLE);
    else
        m_pPlayerChr->ClearPlayerFlag(PLAYERTYPES::FLAG_CONSOLE);
};


void CManipulator::SetConfusionInput(void)
{
	if (m_pPlayerChr->TestAttribute(PLAYERTYPES::ATTRIBUTE_CONFUSION))
	{
		m_input.m_fDirection += MATH_PI;
		if (m_input.m_fDirection > MATH_PI)
			m_input.m_fDirection -= MATH_PI2;
	};
};


void CManipulator::ClearInput(void)
{
    m_input.m_move          = MOVE_IDLE;
    m_input.m_dash          = DASH_OFF;
    m_input.m_jump          = JUMP_OFF;
    m_input.m_attack        = ATTACK_OFF;
    m_input.m_attackCharge  = ATTACK_CHARGE_OFF;
    m_input.m_knife         = KNIFE_OFF;
    m_input.m_guard         = GUARD_OFF;
    m_input.m_lift          = LIFT_OFF;
    m_input.m_recover       = RECOVER_OFF;
    m_input.m_passive       = PASSIVE_OFF;
    m_input.m_fDirection    = 0.0f;
};