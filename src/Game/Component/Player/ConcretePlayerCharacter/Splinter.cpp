#include "Splinter.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Module/BarrierModule.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace Splinter
{
    bool CAttackJump::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_JUMP,
            PLAYERTYPES::STATUS_JUMP_2ND,
            PLAYERTYPES::STATUS_JUMP_WALL,
            PLAYERTYPES::STATUS_AERIAL,
            PLAYERTYPES::STATUS_AERIAL_MOVE,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CAttackJump::OnAttach(void)
    {
        Character().ChangeMotion("JAttack");
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };


    void CAttackJump::OnDetach(void)
    {
        ;
    };


    void CAttackJump::OnRun(void)
    {
        ;
    };


    void CAttackAABBC::OnAttach(void)
    {
        PlayerStatus::CAttackAABBC::OnAttach();
        m_phase = PHASE_WAIT_TIMING;
    };


    void CAttackAABBC::OnDetach(void)
    {
        PlayerStatus::CAttackAABBC::OnDetach();

        CBarrierModule* pBarrierMod = (CBarrierModule*)Character().GetModule(MODULETYPE::BARRIER);
        ASSERT(pBarrierMod);

        pBarrierMod->Disappear();
    };


    void CAttackAABBC::OnRun(void)
    {
        switch (m_phase)
        {
        case PHASE_WAIT_TIMING:
            {
                if (Character().GetMotionTime() >= 0.6f)
                {
                    OnDischargeWave();
                    Character().StopMotion();
                    m_phase = PHASE_INVOKE_BARRIER;
                };
            }
            break;

        case PHASE_INVOKE_BARRIER:
            {
                CBarrierModule* pBarrierMod = (CBarrierModule*)Character().GetModule(MODULETYPE::BARRIER);
                ASSERT(pBarrierMod);

                switch (pBarrierMod->GetState())
                {
                case CBarrierModule::STATE_DISAPPEAR:
                    {
                        ;
                    }
                    break;

                case CBarrierModule::STATE_SLEEP:
                    {
                        m_phase = PHASE_FINISH;
                        Character().PlayMotion();

                        CAccumulateModule* pAccumMod = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
                        ASSERT(pAccumMod);
                        pAccumMod->SetDrawOff();
                    }
                    break;
                };
            }
            break;

        case PHASE_FINISH:
            {
                if (Character().IsMotionEnd())
                    StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };


    void CAttackAABBC::OnDischargeWave(void)
    {
        CBarrierModule* pBarrierMod = (CBarrierModule*)Character().GetModule(MODULETYPE::BARRIER);
        ASSERT(pBarrierMod);

        pBarrierMod->Appear();
        pBarrierMod->SetStepThree();
    };


    void CAttackB::OnAttach(void)
    {
        PlayerStatus::CAttackB::OnAttach();
        m_phase = PHASE_WAIT_TIMING;
    };


    void CAttackB::OnDetach(void)
    {
        PlayerStatus::CAttackB::OnDetach();

        CBarrierModule* pBarrierMod = (CBarrierModule*)Character().GetModule(MODULETYPE::BARRIER);
        ASSERT(pBarrierMod);

        pBarrierMod->Disappear();
    };


    void CAttackB::OnRun(void)
    {
        switch (m_phase)
        {
        case PHASE_WAIT_TIMING:
            {
                if (Character().GetMotionTime() >= 0.9f)
                {
                    CallDischargeWave();
                    Character().StopMotion();
                    m_phase = PHASE_INVOKE_BARRIER;
                };
            }
            break;

        case PHASE_INVOKE_BARRIER:
            {
                CBarrierModule* pBarrierMod = (CBarrierModule*)Character().GetModule(MODULETYPE::BARRIER);
                ASSERT(pBarrierMod);

                switch (pBarrierMod->GetState())
                {
                case CBarrierModule::STATE_DISAPPEAR:
                    {
                        ;
                    }
                    break;

                case CBarrierModule::STATE_SLEEP:
                    {
                        m_phase = PHASE_FINISH;
                        Character().PlayMotion();

                        CAccumulateModule* pAccumMod = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
                        ASSERT(pAccumMod);
                        pAccumMod->SetDrawOff();
                    }
                    break;
                };
            }
            break;

        case PHASE_FINISH:
            {
                if (Character().IsMotionEnd())
                    StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };


    void CAttackB::OnDischargeWave(MAGIC_GENERIC::STEP step)
    {
        CBarrierModule* pBarrierMod = (CBarrierModule*)Character().GetModule(MODULETYPE::BARRIER);
        ASSERT(pBarrierMod);

        pBarrierMod->Appear();
        switch (step)
        {
        case MAGIC_GENERIC::STEP_ONE:
            pBarrierMod->SetStepOne();
            break;

        case MAGIC_GENERIC::STEP_TWO:
            pBarrierMod->SetStepTwo();
            break;

        case MAGIC_GENERIC::STEP_THREE:
            pBarrierMod->SetStepThree();
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};




CSplinter::CSplinter(GAMETYPES::COSTUME costume)
: CPlayerCharacter("Splinter", PLAYERID::ID_SPL, costume)
{
    //
	//	Model parts:
	//		0 - weapon
	//		1 - model legs
	//		2 - model top
    //
    
    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon = true;
    parameter.m_chrparameter.m_pszModelName = "splinter";
    parameter.m_chrparameter.m_pszMotionSetName = "splinter";
    parameter.m_feature.m_fWalkMoveSpeed = 2.0f;
    parameter.m_feature.m_fLiftWalkMoveSpeed = 3.6f;
    parameter.m_feature.m_fRunMoveSpeed = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed = 16.0f;
    parameter.m_feature.m_fDashTime = 0.2f;
    parameter.m_feature.m_fJumpInitializeSpeed = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed = 5.2f;
    parameter.m_feature.m_fAerialAcceleration = 12.0f;
    parameter.m_feature.m_nKnifeAttachBoneID = CHARACTERTYPES::BONEID_RIGHT_WRIST;
    parameter.m_pStateMachine = new CPlayerStateMachine(this, PLAYERTYPES::NORMALMAX);
    ASSERT(parameter.m_pStateMachine);

    CStatus::RegistDefaultForStateMachine(*parameter.m_pStateMachine);

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC, new Splinter::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B, new Splinter::CAttackB);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP, new Splinter::CAttackJump);

    Initialize(&parameter);

    m_pModuleMan->Include(
        CCircleShadowModule::New(this, 1.5f, 1.5f, true)
    );

    m_pModuleMan->Include(new CBarrierModule(new CPlayerTracer(this), this, 2.0f));
};


CSplinter::~CSplinter(void)
{
    ;
};


void CSplinter::Run(void)
{
    CPlayerCharacter::Run();
};