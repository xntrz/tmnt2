#include "Donatello.hpp"

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


namespace Donatello
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
        Character().ChangeMotion(Donatello::MOTIONNAMES::ATTACK_JUMP1);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);
        vVelocity.y = 0.0f;
        Character().SetVelocity(&vVelocity);
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


    void CAttackLaser::OnAttach(void)
    {
        Character().ChangeMotion(Donatello::MOTIONNAMES::ATTACK_LASER);
        Character().ResetAcceleration();
        Character().ResetVelocity();
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE, true);
    };
    

    void CAttackLaser::OnRun(void)
    {
        if (Character().IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
            CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_KNIFE);
            ShootingLaser();
        };

        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    void CAttackLaser::ShootingLaser(void)
    {
        RwV3d vChrPos = Math::VECTOR3_ZERO;
        Character().GetPosition(&vChrPos);
        
        RwV3d vPosition = *Character().GetModel()->GetBonePositionFromID(Character().Feature().m_nKnifeAttachBoneID);
        RwV3d vLocalPos = Math::VECTOR3_ZERO;

        Math::Vec3_Sub(&vLocalPos, &vPosition, &vChrPos);
        float fBuffY = vLocalPos.y;
        vLocalPos.y = 0.0f;

        float fLen = Math::Vec3_Length(&vLocalPos);
        if (fLen > 0.6f)
        {
            vLocalPos.x *= 0.5f / fLen;
            vLocalPos.z *= 0.5f / fLen;
            vLocalPos.y = fBuffY;

            Math::Vec3_Add(&vPosition, &vLocalPos, &vChrPos);
        };

        RwV3d vDirection = Math::VECTOR3_AXIS_Z;
        Character().RotateVectorByDirection(&vDirection, &vDirection);

        uint32 hMagic = CMagicManager::Play(Donatello::EFFECTNAMES::LASER_BEAM, &vPosition, &vDirection, &Character(), true);
        ASSERT(hMagic);
        
        if (hMagic)
            CMagicManager::SetStatusTime(hMagic, 4.0f);

        if (!Character().IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE))
            CGameProperty::Player(Character().GetPlayerNo())->AddShurikenNum(-1);
    };


    //
    // *********************************************************************************
    //

    
    void CTouchdown::OnAttach(void)
    {
        PlayerStatus::CTouchdown::OnAttach();

        if (Character().GetStatusPrev() == PLAYERTYPES::STATUS_ATTACK_JUMP)
            Character().ChangeMotion(Donatello::MOTIONNAMES::ATTACK_JUMP2);
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
                if (Character().GetMotionTime() >= 0.6f)
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


    //
    // *********************************************************************************
    //

    
    bool CConsole::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
            PLAYERTYPES::STATUS_ATTACK_A,
            PLAYERTYPES::STATUS_ATTACK_B,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };

    
    void CConsole::OnAttach(void)
    {
        Character().ChangeMotion(Donatello::MOTIONNAMES::CONSOLE1);

        const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);

        CGimmickManager::PostEvent(
            pWallInfo->m_gimmickinfo.m_szGimmickObjName,
            Character().GetName(),
            GIMMICKTYPES::EVENTTYPE_ACTIVATE
        );

        Character().SetEnableBodyHitSelfToOther(false);
        
        m_fStartTimer = CGameProperty::GetTotalElapsedTime();
        m_iMotionStep = 0;
    };

    
    void CConsole::OnDetach(void)
    {
        Character().SetEnableBodyHitSelfToOther(false);

        const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);

        CGimmickManager::PostEvent(
            pWallInfo->m_gimmickinfo.m_szGimmickObjName,
            Character().GetName(),
            GIMMICKTYPES::EVENTTYPE_ACTIVATE
        );
    };
    

    void CConsole::OnRun(void)
    {
        switch (m_iMotionStep)
        {
        case 0:
            {
                if (Character().IsMotionEnd())
                {
                    Character().ChangeMotion(Donatello::MOTIONNAMES::CONSOLE2);
                    m_iMotionStep = 1;
                };
            }
            break;

        case 1:
            {
                if (CGameProperty::GetTotalElapsedTime() - m_fStartTimer > 1.0f)
                {
                    Character().ChangeMotion(Donatello::MOTIONNAMES::CONSOLE3);
                    m_iMotionStep = 2;
                };
            }
            break;

        case 2:
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
};




CDonatello::CDonatello(GAMETYPES::COSTUME costume)
: CPlayerCharacter("donatello", PLAYERID::ID_DON, costume)
{
    //
	//	Model parts:
	//		0 - laser
	//		1 - weapon
	//		2 - model
    //
    
    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon            = true;
    parameter.m_chrparameter.m_pszModelName     = "donatello";
    parameter.m_chrparameter.m_pszMotionSetName = "donatello";
    parameter.m_feature.m_fWalkMoveSpeed        = 2.0f;
    parameter.m_feature.m_fLiftWalkMoveSpeed    = 3.6f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 16.0f;
    parameter.m_feature.m_fDashTime             = 0.2f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_nKnifeAttachBoneID    = CHARACTERTYPES::BONEID_RIGHT_WRIST;

    parameter.m_pStateMachine = new CPlayerStateMachine(this, PLAYERTYPES::STATUS::NORMALMAX);
    ASSERT(parameter.m_pStateMachine);

    CStatus::RegistDefaultForStateMachine(*parameter.m_pStateMachine);

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE,   new Donatello::CAttackLaser);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,   new Donatello::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,       new Donatello::CAttackB);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_TOUCHDOWN,      new Donatello::CTouchdown);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP,    new Donatello::CAttackJump);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_CONSOLE,        new Donatello::CConsole);

    Initialize(&parameter);

    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, true));

    if (costume != GAMETYPES::COSTUME_SAMURAI)
    {
        m_pModuleMan->Include(new CBandanaModule(
            this,
            m_pModel,
            CHARACTERTYPES::BONEID_HEAD,
            &Donatello::BANDANA_OFFSET,
            CBandanaModule::BANDANACOLOR_PURPLE
        ));
    };

    m_pModuleMan->Include(new CBarrierModule(new CPlayerTracer(this), this, 2.0f));
};


CDonatello::~CDonatello(void)
{
    ;
};


void CDonatello::OnChangeMotion(void)
{
    CPlayerCharacter::OnChangeMotion();

    m_pModel->SetPartsDrawEnable(0, CGameData::Record().Secret().IsDonLazerEnabled());
};