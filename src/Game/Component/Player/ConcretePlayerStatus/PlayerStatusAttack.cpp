#include "PlayerStatusAttack.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Player/PlayerUtil.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace PlayerStatus
{
    bool CAttackA::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };

    
    void CAttackA::OnAttach(void)
    {
        Character().ResetVelocity();
        Character().ResetAcceleration();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_A);
        
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };

    
    void CAttackA::OnDetach(void)
    {
        ;
    };

    
    void CAttackA::OnRun(void)
    {
        if (Character().CheckAttackConnect(PLAYERTYPES::FLAG_REQUEST_ATTACK_A))
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AA);
        }
        else if (Character().IsMotionEnd())
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };


    //
    // *********************************************************************************
    //


    bool CAttackAA::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_A);
    };

    
    void CAttackAA::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AA);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };

    
    void CAttackAA::OnDetach(void)
    {
        ;
    };

    
    void CAttackAA::OnRun(void)
    {
        if (Character().CheckAttackConnect(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
        {
            if (CGameData::Record().Secret().GetAttackLevel() >= 1)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AAC);
        }
        else if (Character().CheckAttackConnect(PLAYERTYPES::FLAG_REQUEST_ATTACK_B))
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AAB);
        }
        else if (Character().IsMotionEnd())
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };


    //
    // *********************************************************************************
    //


    bool CAttackAAB::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_AA);
    };

    
    void CAttackAAB::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AAB);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };

    
    void CAttackAAB::OnDetach(void)
    {
        ;
    };

    
    void CAttackAAB::OnRun(void)
    {
        if (Character().CheckAttackConnect(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
        {
            if (CGameData::Record().Secret().GetDefenceLevel() >= 2)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABC);
        }
        else if (Character().CheckAttackConnect(PLAYERTYPES::FLAG_REQUEST_ATTACK_B))
        {
            if (CGameData::Record().Secret().GetAttackLevel() >= 2)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABB);
        }
        else if (Character().IsMotionEnd())
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };


    //
    // *********************************************************************************
    //


    bool CAttackAAC::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_AA);
    };

    
    void CAttackAAC::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AAC);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };

    
    void CAttackAAC::OnDetach(void)
    {
        ;
    };

    
    void CAttackAAC::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    bool CAttackAABB::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_AAB);
    };

    
    void CAttackAABB::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABB);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };

    
    void CAttackAABB::OnDetach(void)
    {
        ;
    };

    
    void CAttackAABB::OnRun(void)
    {
        if (Character().CheckAttackConnect(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
        {
            if (CGameData::Record().Secret().GetChargeLevel() >= 2)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABBC);
        }
        else if (Character().CheckAttackConnect(PLAYERTYPES::FLAG_REQUEST_ATTACK_B))
        {
            if (CGameData::Record().Secret().GetAerialLevel() >= 2)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABBB);
        }
        else if (Character().IsMotionEnd())
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };


    //
    // *********************************************************************************
    //


    bool CAttackAABC::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_AAB);
    };

    
    void CAttackAABC::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABC);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };

    
    void CAttackAABC::OnDetach(void)
    {
        ;
    };

    
    void CAttackAABC::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    bool CAttackAABBB::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_AABB);
    };

    
    void CAttackAABBB::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABBB);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);

        RwV3d vPosBody = Math::VECTOR3_ZERO;
        RwV3d vPosChr = Math::VECTOR3_ZERO;
        
        Character().GetBodyPosition(&vPosBody);
        Character().GetPosition(&vPosChr);

        RwV3d vOffset = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vOffset, &vPosBody, &vPosChr);
        
        CEffectManager::PlayTrace(EFFECTID::ID_ALL_TAME_FLASH, new CPlayerTracer(m_pPlayerChr), &vOffset);

        PlayerUtil::CallVoiceOfAABBB(m_pPlayerChr);
    };

    
    void CAttackAABBB::OnDetach(void)
    {
        ;
    };

    
    void CAttackAABBB::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    bool CAttackAABBC::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_AABB);
    };

    
    void CAttackAABBC::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABBC);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);

        PlayerUtil::CallVoiceOfAABBC(m_pPlayerChr);
    };

    
    void CAttackAABBC::OnDetach(void)
    {
        ;
    };

    
    void CAttackAABBC::OnRun(void)
    {
        if (Character().IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
            OnDischargeWave();
        }
        else if (Character().IsMotionEnd())
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };        
    };


    //
    // *********************************************************************************
    //


    bool CAttackB::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_B_CHARGE);
    };

    
    void CAttackB::OnAttach(void)
    {
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
        CGameSound::PlayAttackSE(m_pPlayerChr);

        CAccumulateModule* pModule = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
        if (pModule)
            pModule->SetDrawOn();

        if (Character().GetChargePhase() == PLAYERTYPES::CHARGEPHASE_1ST ||
            Character().GetChargePhase() == PLAYERTYPES::CHARGEPHASE_2ND ||
            Character().GetChargePhase() == PLAYERTYPES::CHARGEPHASE_3RD)
        {
            PlayerUtil::CallVoiceOfChargeAttack(m_pPlayerChr);
        };
    };

    
    void CAttackB::OnDetach(void)
    {
        CAccumulateModule* pModule = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
        if (pModule)
            pModule->SetDrawOff();
        
        Character().ClearCharge();
    };

    
    void CAttackB::OnRun(void)
    {
        if (Character().IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
            CallDischargeWave();

            CAccumulateModule* pModule = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
            if (pModule)
                pModule->SetDrawOff();
		};

        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };

    
    void CAttackB::CallDischargeWave(void)
    {
        switch (Character().GetChargePhase())
        {
        case PLAYERTYPES::CHARGEPHASE_1ST:
            OnDischargeWave(MAGIC_GENERIC::STEP_ONE);
            break;

        case PLAYERTYPES::CHARGEPHASE_2ND:
            OnDischargeWave(MAGIC_GENERIC::STEP_TWO);
            break;

        case PLAYERTYPES::CHARGEPHASE_3RD:
            OnDischargeWave(MAGIC_GENERIC::STEP_THREE);
            break;
        };
    };


    //
    // *********************************************************************************
    //


    bool CAttackBCharge::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };

    
    void CAttackBCharge::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_B);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL, true);
        Character().SetMotionSpeed(0.05f);
        Character().ResetVelocity();
        Character().ResetAcceleration();
		Character().ClearCharge();

        m_bCallEffect = false;

        CAccumulateModule* pModule = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
        if (pModule)
        {
            pModule->SetStepZero();
            pModule->SetEffectOff();
            pModule->SetDrawOff();
        };
    };

    
    void CAttackBCharge::OnDetach(void)
    {
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL, false);
        if (m_bCallEffect)
        {
            CAccumulateModule* pModule = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
            if (pModule)
            {
                pModule->SetEffectOff();
                pModule->SetDrawOff();
            };
        };
    };

    
    void CAttackBCharge::OnRun(void)
    {
        CAccumulateModule* pModule = (CAccumulateModule*)Character().GetModule(MODULETYPE::ACCUMULATE);
        
        if (!m_bCallEffect && StateMachine().GetStatusDuration() >= 0.3f)
        {
            if (pModule)
            {
                pModule->SetStepZero();
                pModule->SetDrawOn();
            };
            
            m_bCallEffect = true;
        };

		PLAYERTYPES::CHARGEPHASE phasePrev = Character().GetChargePhase();
		PLAYERTYPES::CHARGEPHASE phaseNow = PLAYERTYPES::CHARGEPHASE_ZERO;
        
        bool bResult = Character().CheckChargeTime(
            StateMachine().GetStatusDuration(),
            phaseNow
        );

        if (bResult)
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_B); 

        if (phaseNow != phasePrev && pModule)
        {
            switch (phaseNow)
            {
            case PLAYERTYPES::CHARGEPHASE_1ST:
                pModule->SetStepOne();
                break;

            case PLAYERTYPES::CHARGEPHASE_2ND:
                pModule->SetStepTwo();
                break;

            case PLAYERTYPES::CHARGEPHASE_3RD:
                pModule->SetStepThree();
                break;
            };

			if (bResult)
			{
				if (pModule)
					pModule->SetEffectOff();
			};
		};
    };


    //
    // *********************************************************************************
    //


    bool CAttackKnife::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
            PLAYERTYPES::STATUS_ATTACK_KNIFE,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };

    
    void CAttackKnife::OnAttach(void)
    {
        if (Character().GetStatusPrev() == m_status)
            Character().ReplayMotion();
        else
            Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_KNIFE);

        Character().ResetAcceleration();
        Character().ResetVelocity();
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE, true);
    };

    
    void CAttackKnife::OnDetach(void)
    {
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE, false);
    };

    
    void CAttackKnife::OnRun(void)
    {
        if (Character().IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
			CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(0x1020));
            Character().ShootingKnife();
            CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_KNIFE);

            Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE, false);
        };

        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    bool CAttackKnifeJump::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_JUMP,
            PLAYERTYPES::STATUS_JUMP_2ND,
            PLAYERTYPES::STATUS_AERIAL,
            PLAYERTYPES::STATUS_AERIAL_MOVE,
            PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };

    
    void CAttackKnifeJump::OnAttach(void)
    {
        if (Character().GetStatusPrev() == m_status)
            Character().ReplayMotion();
        else
            Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_KNIFE_JUMP);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE, true);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, true);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, false);
    };

    
    void CAttackKnifeJump::OnDetach(void)
    {
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE, false);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, false);
    };

    
    void CAttackKnifeJump::OnRun(void)
    {
        if (Character().IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
            CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4128));
            Character().ShootingKnife();
            CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_KNIFE_JUMP);

            Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE, false);
        };
    };


    //
    // *********************************************************************************
    //


    bool CAttackRun::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_RUN);
    };

    
    void CAttackRun::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_RUN);
        Character().ResetVelocity();
        Character().ResetAcceleration();
    };

    
    void CAttackRun::OnDetach(void)
    {
        ;
    };

    
    void CAttackRun::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };
};