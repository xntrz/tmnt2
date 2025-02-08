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
    void CAttackCommon::OnAttach(void)
    {
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT);

        CGameSound::PlayAttackSE(m_pPlayerChr);
    };


    void CAttackCommon::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackA, { PLAYERTYPES::STATUS_IDLE,
                                          PLAYERTYPES::STATUS_WALK,
                                          PLAYERTYPES::STATUS_RUN, });


    void CAttackA::OnAttach(void)
    {
        Character().ResetVelocity();
        Character().ResetAcceleration();

        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_A);

        CAttackCommon::OnAttach();
    };

    
    void CAttackA::OnDetach(void)
    {
        ;
    };

    
    void CAttackA::OnRun(void)
    {
        uint32 mask = PLAYERTYPES::FLAG_REQUEST_ATTACK_A;
        uint32 result = Character().CheckAttackConnect(static_cast<PLAYERTYPES::FLAG>(mask));

        if (result == PLAYERTYPES::FLAG_REQUEST_ATTACK_A)
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AA);

        CAttackCommon::OnRun();
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackAA, { PLAYERTYPES::STATUS_ATTACK_A });


    void CAttackAA::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AA);

        CAttackCommon::OnAttach();    
    };

    
    void CAttackAA::OnDetach(void)
    {
        ;
    };

    
    void CAttackAA::OnRun(void)
    {
        uint32 mask   = PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK;
        uint32 result = Character().CheckAttackConnect(static_cast<PLAYERTYPES::FLAG>(mask));

        if (result == PLAYERTYPES::FLAG_REQUEST_ATTACK_B)
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AAB);
        }
        else if (result == PLAYERTYPES::FLAG_REQUEST_ATTACK_C)
        {
            if (CGameData::Record().Secret().GetAttackLevel() >= 1)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AAC);
        };

        CAttackCommon::OnRun();
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackAAB, { PLAYERTYPES::STATUS_ATTACK_AA });

    
    void CAttackAAB::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AAB);

        CAttackCommon::OnAttach();        
    };

    
    void CAttackAAB::OnDetach(void)
    {
        ;
    };

    
    void CAttackAAB::OnRun(void)
    {
        uint32 mask   = PLAYERTYPES::FLAG_REQUEST_ATTACK_C | PLAYERTYPES::FLAG_REQUEST_ATTACK_B;
        uint32 result = Character().CheckAttackConnect(static_cast<PLAYERTYPES::FLAG>(mask));

        if (result == PLAYERTYPES::FLAG_REQUEST_ATTACK_B)
        {
            if (CGameData::Record().Secret().GetAttackLevel() >= 2)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABB);
        }
        else if (result == PLAYERTYPES::FLAG_REQUEST_ATTACK_C)
        {
            if (CGameData::Record().Secret().GetDefenceLevel() >= 2)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABC);
        };

        CAttackCommon::OnRun();
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackAAC, { PLAYERTYPES::STATUS_ATTACK_AA });

    
    void CAttackAAC::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AAC);

        CAttackCommon::OnAttach();
    };

    
    void CAttackAAC::OnDetach(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackAABB, { PLAYERTYPES::STATUS_ATTACK_AAB });
    
    
    void CAttackAABB::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABB);

        CAttackCommon::OnAttach();    
    };

    
    void CAttackAABB::OnDetach(void)
    {
        ;
    };

    
    void CAttackAABB::OnRun(void)
    {
        uint32 mask = PLAYERTYPES::FLAG_REQUEST_ATTACK_C | PLAYERTYPES::FLAG_REQUEST_ATTACK_B;
        uint32 result = Character().CheckAttackConnect(static_cast<PLAYERTYPES::FLAG>(mask));

        if (result == PLAYERTYPES::FLAG_REQUEST_ATTACK_B)
        {
            if (CGameData::Record().Secret().GetAerialLevel() >= 3)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABBB);
        }
        else if (result == PLAYERTYPES::FLAG_REQUEST_ATTACK_C)
        {
            if (CGameData::Record().Secret().GetChargeLevel() >= 3)
                StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_AABBC);
        };

        CAttackCommon::OnRun();
    };


    //
    // *********************************************************************************
    //

    
    DEFINE_ENABLED_STATUS_FOR(CAttackAABC, { PLAYERTYPES::STATUS_ATTACK_AAB });

    
    void CAttackAABC::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABC);

        CAttackCommon::OnAttach();    
    };

    
    void CAttackAABC::OnDetach(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackAABBB, { PLAYERTYPES::STATUS_ATTACK_AABB });    

    
    void CAttackAABBB::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABBB);

        CAttackCommon::OnAttach();        
        
        RwV3d vPosBody = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosBody);

        RwV3d vPosChr = Math::VECTOR3_ZERO;
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


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackAABBC, { PLAYERTYPES::STATUS_ATTACK_AABB });
    
    
    void CAttackAABBC::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AABBC);

        CAttackCommon::OnAttach();        

        PlayerUtil::CallVoiceOfAABBC(m_pPlayerChr);
    };

    
    void CAttackAABBC::OnDetach(void)
    {
        ;
    };

    
    void CAttackAABBC::OnRun(void)
    {
        if (Character().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
            OnDischargeWave();

        CAttackCommon::OnRun();
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackB, { PLAYERTYPES::STATUS_ATTACK_B_CHARGE });
    
    
    void CAttackB::OnAttach(void)
    {
        CAttackCommon::OnAttach();        

        CAccumulateModule* pModule = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
        if (pModule)
            pModule->SetDrawOn();

        if ((Character().GetChargePhase() == PLAYERTYPES::CHARGEPHASE_1ST) ||
            (Character().GetChargePhase() == PLAYERTYPES::CHARGEPHASE_2ND) ||
            (Character().GetChargePhase() == PLAYERTYPES::CHARGEPHASE_3RD))
        {
            PlayerUtil::CallVoiceOfChargeAttack(m_pPlayerChr);
        };
    };

    
    void CAttackB::OnDetach(void)
    {
        CAccumulateModule* pModule = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
        if (pModule)
            pModule->SetDrawOff();
        
        Character().ClearCharge();
    };

    
    void CAttackB::OnRun(void)
    {
        if (Character().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
            CallDischargeWave();

            CAccumulateModule* pModule = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
            if (pModule)
                pModule->SetDrawOff();
		};

        CAttackCommon::OnRun();
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
            
        default:
            break;
        };
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackBCharge, { PLAYERTYPES::STATUS_IDLE,
                                                PLAYERTYPES::STATUS_WALK,
                                                PLAYERTYPES::STATUS_RUN, });

    
    void CAttackBCharge::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_B);
        
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
        Character().SetMotionSpeed(0.05f);

        Character().ResetVelocity();
        Character().ResetAcceleration();
        
        Character().ClearCharge();

        CAccumulateModule* pModule = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
        if (pModule)
        {
            pModule->SetStepZero();
            pModule->SetEffectOff();
            pModule->SetDrawOff();
        };

        m_bCallEffect = false;
    };

    
    void CAttackBCharge::OnDetach(void)
    {
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

        if (m_bCallEffect)
        {
            CAccumulateModule* pModule = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
            if (pModule)
            {
                pModule->SetEffectOff();
                pModule->SetDrawOff();
            };
        };
    };

    
    void CAttackBCharge::OnRun(void)
    {
        CAccumulateModule* pModule = static_cast<CAccumulateModule*>(Character().GetModule(MODULETYPE::ACCUMULATE));
        
        if (!m_bCallEffect && (StateMachine().GetStatusDuration() >= 0.3f))
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
        
        bool bResult = Character().CheckChargeTime(StateMachine().GetStatusDuration(), &phaseNow);

        if (bResult)
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_ATTACK_B); 

        if ((phaseNow != phasePrev) && pModule)
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

            default:
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


    DEFINE_ENABLED_STATUS_FOR(CAttackKnife, { PLAYERTYPES::STATUS_IDLE,
                                              PLAYERTYPES::STATUS_WALK,
                                              PLAYERTYPES::STATUS_RUN,
                                              PLAYERTYPES::STATUS_ATTACK_KNIFE });

    
    void CAttackKnife::OnAttach(void)
    {
        if (Character().GetStatusPrev() == m_status)
            Character().ReplayMotion();
        else
            Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_KNIFE);

        Character().ResetAcceleration();
        Character().ResetVelocity();

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);
    };

    
    void CAttackKnife::OnDetach(void)
    {
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);
    };

    
    void CAttackKnife::OnRun(void)
    {
        if (Character().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
			CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(0x1020));
            Character().ShootingKnife();
            CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_KNIFE);

            Character().ClearPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);
        };

        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackKnifeJump, { PLAYERTYPES::STATUS_JUMP,
                                                  PLAYERTYPES::STATUS_JUMP_2ND,
                                                  PLAYERTYPES::STATUS_AERIAL,
                                                  PLAYERTYPES::STATUS_AERIAL_MOVE,
                                                  PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP });

    
    void CAttackKnifeJump::OnAttach(void)
    {
        if (Character().GetStatusPrev() == m_status)
            Character().ReplayMotion();
        else
            Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_KNIFE_JUMP);

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
    };

    
    void CAttackKnifeJump::OnDetach(void)
    {
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
    };

    
    void CAttackKnifeJump::OnRun(void)
    {
        if (Character().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
            CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4128));
            Character().ShootingKnife();
            CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_KNIFE_JUMP);

            Character().ClearPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);
        };
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CAttackRun, { PLAYERTYPES::STATUS_RUN });    

    
    void CAttackRun::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_RUN);

        Character().ResetVelocity();
        Character().ResetAcceleration();
        
        CGameSound::PlayAttackSE(&Character());
	};

    
    void CAttackRun::OnDetach(void)
    {
        ;
    };
};