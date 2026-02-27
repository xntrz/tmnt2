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
    DEFINE_ENABLED_STATUS_FOR(CAttackJump, { PLAYERTYPES::STATUS_JUMP,
                                             PLAYERTYPES::STATUS_JUMP_2ND,
                                             PLAYERTYPES::STATUS_JUMP_WALL,
                                             PLAYERTYPES::STATUS_AERIAL,
                                             PLAYERTYPES::STATUS_AERIAL_MOVE });
        

    void CAttackJump::OnAttach(void)
    {
        Character().ChangeMotion(Splinter::MOTIONNAMES::ATTACK_JUMP);
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

    
    //
    // *********************************************************************************
    //


    void CAttackAABBC::OnAttach(void)
    {
        PlayerStatus::CAttackAABBC::OnAttach();
        m_phase = PHASE_WAIT_TIMING;
    };


    void CAttackAABBC::OnDetach(void)
    {
        PlayerStatus::CAttackAABBC::OnDetach();

        CBarrierModule* pBarrierMod = static_cast<CBarrierModule*>(Character().GetModule(MODULETYPE::BARRIER));
        ASSERT(pBarrierMod);

        pBarrierMod->Disappear();
    };


    void CAttackAABBC::OnRun(void)
    {
        if (m_phase == PHASE_FINISH)
        {
            if (Character().IsMotionEnd())
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);

            return;
        };

        if (m_phase != PHASE_INVOKE_BARRIER)
        {
            if (Character().GetMotionTime() < 0.6f)
                return;

            OnDischargeWave();
            Character().StopMotion();
            m_phase = PHASE_INVOKE_BARRIER;
        };

        CBarrierModule* pBarrierMod = static_cast<CBarrierModule*>(Character().GetModule(MODULETYPE::BARRIER));
        ASSERT(pBarrierMod);

        if (pBarrierMod->GetState() == CBarrierModule::STATE_DISAPPEAR)
        {
            CAccumulateModule* pAccumMod = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
            ASSERT(pAccumMod);

            pAccumMod->SetDrawOff();
        };

        if (pBarrierMod->GetState() == CBarrierModule::STATE_SLEEP)
        {
            m_phase = PHASE_FINISH;
            Character().PlayMotion();

            if (Character().IsMotionEnd())
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };


    void CAttackAABBC::OnDischargeWave(void)
    {
        CBarrierModule* pBarrierMod = static_cast<CBarrierModule*>(Character().GetModule(MODULETYPE::BARRIER));
        ASSERT(pBarrierMod);

        pBarrierMod->Appear();
        pBarrierMod->SetStepThree();
    };

    
    //
    // *********************************************************************************
    //


    void CAttackB::OnAttach(void)
    {
        PlayerStatus::CAttackB::OnAttach();
        m_phase = PHASE_WAIT_TIMING;
    };


    void CAttackB::OnDetach(void)
    {
        PlayerStatus::CAttackB::OnDetach();

        CBarrierModule* pBarrierMod = static_cast<CBarrierModule*>(Character().GetModule(MODULETYPE::BARRIER));
        ASSERT(pBarrierMod);

        pBarrierMod->Disappear();
    };


    void CAttackB::OnRun(void)
    {
        if (m_phase == PHASE_FINISH)
        {
            if (Character().IsMotionEnd())
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);

            return;
        };

        if (m_phase != PHASE_INVOKE_BARRIER)
        {
            if (Character().GetMotionTime() < 0.9f)
                return;

            CallDischargeWave();
            Character().StopMotion();
            m_phase = PHASE_INVOKE_BARRIER;
        };

        CBarrierModule* pBarrierMod = static_cast<CBarrierModule*>(Character().GetModule(MODULETYPE::BARRIER));
        ASSERT(pBarrierMod);

        if (pBarrierMod->GetState() == CBarrierModule::STATE_DISAPPEAR)
        {
            CAccumulateModule* pAccumMod = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
            ASSERT(pAccumMod);

            pAccumMod->SetDrawOff();
        };

        if (pBarrierMod->GetState() == CBarrierModule::STATE_SLEEP)
        {
            m_phase = PHASE_FINISH;
            Character().PlayMotion();

            if (Character().IsMotionEnd())
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };


    void CAttackB::OnDischargeWave(MAGIC_GENERIC::STEP step)
    {
        CBarrierModule* pBarrierMod = static_cast<CBarrierModule*>(Character().GetModule(MODULETYPE::BARRIER));
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
}; /* namespace Splinter */


CSplinter::CSplinter(GAMETYPES::COSTUME costume)
: CPlayerCharacter("splinter", PLAYERID::ID_SPL, costume)
{
    //
	//	Model parts:
	//		0 - weapon
	//		1 - model legs
	//		2 - model top
    //
    
    CPlayerCharacter::PARAMETER parameter = {};
    parameter.m_chrparameter.m_bToon            = true;
    parameter.m_chrparameter.m_pszModelName     = "splinter";
    parameter.m_chrparameter.m_pszMotionSetName = "splinter";
    parameter.m_feature.m_fWalkMoveSpeed        = 2.0f;
    parameter.m_feature.m_fLiftWalkMoveSpeed    = 3.6f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 16.0f;
    parameter.m_feature.m_fDashTime             = 0.2f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_nKnifeAttachBoneID    = 3;

    parameter.m_pStateMachine = new CPlayerStateMachine(this, PLAYERTYPES::STATUS::NORMALMAX);

    CStatus::RegistDefaultForStateMachine(*parameter.m_pStateMachine);

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,   new Splinter::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,       new Splinter::CAttackB);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP,    new Splinter::CAttackJump);

    Initialize(&parameter);

    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, false));

    m_pModuleMan->Include(new CBarrierModule(new CPlayerTracer(this), this, 2.0f));
};


CSplinter::~CSplinter(void)
{
    ;
};