#include "PlayerStatusThrow.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Player/PlayerUtil.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/Gamepad.hpp"


namespace PlayerStatus
{
    DEFINE_ENABLED_STATUS_FOR(CLiftChallenge, { PLAYERTYPES::STATUS_GUARD_READY,
                                                PLAYERTYPES::STATUS_GUARD });


    void CLiftChallenge::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::LIFT_CHALLENGE);
    };


    void CLiftChallenge::OnDetach(void)
    {
        Character().LiftInfo().m_bLiftSuccess = false;
    };


    void CLiftChallenge::OnRun(void)
    {
        if (Character().LiftInfo().m_bLiftSuccess)
        {
            CGameObject* pLiftObj = CGameObjectManager::GetObject(Character().LiftInfo().m_szLiftObjectName);
            if (pLiftObj)
            {
                switch (pLiftObj->GetType())
                {
                case GAMEOBJECTTYPE::CHARACTER:
                    {
                        CCharacter* pCharacter = static_cast<CCharacter*>(pLiftObj);
                        if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
                        {
                            CPlayerCharacter* pPlayerCharacter = static_cast<CPlayerCharacter*>(pCharacter);
                            if (IsCombinationPossible(m_pPlayerChr, pPlayerCharacter) && (pPlayerCharacter->GetID() != -1))
                                ChangeCombinationStep(pLiftObj);
                        }
                        else
                        {
                            ASSERT(pCharacter->GetCharacterType() == CCharacter::TYPE_ENEMY);

                            if (Character().LiftInfo().m_iStatus == CHitCatchData::RESULT_THROWBACK)
                                ChangeBackThrowStep(pLiftObj);
                            else
                                ChangeCatchStep(pLiftObj);
                        };
                    }
                    break;

                case GAMEOBJECTTYPE::GIMMICK:
                    {
                        ChangeCatchStep(pLiftObj);
                    }
                    break;

                default:
                    break;
                };
            };
        };

        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    void CLiftChallenge::ChangeCatchStep(CGameObject* pObject)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_LIFT_SUCCESS);

        CGameObjectManager::SendMessage(pObject, CHARACTERTYPES::MESSAGEID_CATCH, &Character().LiftInfo().m_iStatus);
    };


    void CLiftChallenge::ChangeCombinationStep(CGameObject* pObject)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_THROW_COMBINATION);

        CGameObjectManager::SendMessage(pObject, CHARACTERTYPES::MESSAGEID_COMBINATION);
    };


    void CLiftChallenge::ChangeBackThrowStep(CGameObject* pObject)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_THROW_BACK);

        CGameObjectManager::SendMessage(pObject, CHARACTERTYPES::MESSAGEID_CATCH, &Character().LiftInfo().m_iStatus);
    };


    bool CLiftChallenge::IsCombinationPossible(CPlayerCharacter* pPlayerChrLifting, CPlayerCharacter* pPlayerChrLifted)
    {
        PLAYERID::VALUE playerIdA = pPlayerChrLifting->GetID();
        PLAYERID::VALUE playerIdB = pPlayerChrLifted->GetID();

        bool bIsTurtleA = (playerIdA >= PLAYERID::ID_LEO) && (playerIdA <= PLAYERID::ID_DON);
        bool bIsTurtleB = (playerIdB >= PLAYERID::ID_LEO) && (playerIdB <= PLAYERID::ID_DON);

        if (bIsTurtleA && bIsTurtleB)
            return true;

        return false;
    };
    

    //
    // *********************************************************************************
    //

    
    DEFINE_ENABLED_STATUS_FOR(CLiftSuccess, { PLAYERTYPES::STATUS_LIFT_CHALLENGE,
                                              PLAYERTYPES::STATUS_ATTACK_AAC });


    void CLiftSuccess::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::LIFT_SUCCESS);
        Character().LiftInfo().m_bMissThrow = true;
        Character().SetEnableBodyHit(false);
        PlayerUtil::CallVoiceOfLiftUp(m_pPlayerChr);
    };


    void CLiftSuccess::OnDetach(void)
    {
        Character().SetEnableBodyHit(true);

        if (Character().LiftInfo().m_bMissThrow)
            Throw::SendMissThrowCommon(Character());
    };


    void CLiftSuccess::OnRun(void)
    {
        if (Throw::SendPositionCommon(Character()))
        {
            if (Character().IsMotionEnd())
            {
                Character().LiftInfo().m_bMissThrow = false;
                
                if (Character().IsMotionEnd())
                    StateMachine().ChangeStatus(PLAYERTYPES::STATUS_LIFT);
            };
        };
    };


    //
    // *********************************************************************************
    //

    
    DEFINE_ENABLED_STATUS_FOR(CLift, { PLAYERTYPES::STATUS_LIFT_SUCCESS,
                                       PLAYERTYPES::STATUS_LIFT_WALK });


    void CLift::OnAttach(void)
    {
        Character().ResetVelocity();
        Character().ResetAcceleration();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::LIFT);
        Character().LiftInfo().m_bMissThrow = true;
    };


    void CLift::OnDetach(void)
    {
        if (Character().LiftInfo().m_bMissThrow)
            Throw::SendMissThrowCommon(Character());
    };


    void CLift::OnRun(void)
    {
        Throw::SendPositionCommon(Character());
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CLiftWalk, { PLAYERTYPES::STATUS_LIFT });


    void CLiftWalk::OnAttach(void)
    {
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::LIFT_WALK);
        Character().LiftInfo().m_bMissThrow = true;
    };


    void CLiftWalk::OnDetach(void)
    {
        if (Character().LiftInfo().m_bMissThrow)
            Throw::SendMissThrowCommon(Character());
    };


    void CLiftWalk::OnRun(void)
    {
        RwV3d vVelocity = { 0.0f, 0.0f, Character().Feature().m_fLiftWalkMoveSpeed * 0.6f };
        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);
        
        Throw::SendPositionCommon(Character());
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CThrow, { PLAYERTYPES::STATUS_LIFT,
                                        PLAYERTYPES::STATUS_LIFT_WALK });


    void CThrow::OnAttach(void)
    {
        Character().ResetVelocity();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::THROW);

        CGameObject* pLiftObj = CGameObjectManager::GetObject(Character().LiftInfo().m_szLiftObjectName);
        ASSERT(pLiftObj);

        RwV3d vArc = { 0.0f, 1.0f, 1.0f };
        Character().RotateVectorByDirection(&vArc, &vArc);
		Math::Vec3_Normalize(&vArc, &vArc);

        CGameObjectManager::SendMessage(pLiftObj, CHARACTERTYPES::MESSAGEID_THROW, &vArc);

        CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_THROW);
    };


    void CThrow::OnDetach(void)
    {
        ;
    };


    void CThrow::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CThrowBack, { PLAYERTYPES::STATUS_LIFT_CHALLENGE,
                                            PLAYERTYPES::STATUS_ATTACK_AAC });


    void CThrowBack::OnAttach(void)
    {
        Character().ResetVelocity();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::THROW_BACK);
        Character().SetEnableBodyHit(false);

        PlayerUtil::CallVoiceOfBackThrow(m_pPlayerChr);
    };


    void CThrowBack::OnDetach(void)
    {
        Character().SetEnableBodyHit(true);
    };


    void CThrowBack::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_EMPTY_ENABLED_STATUS_FOR(CThrowCombination, true);


    void CThrowCombination::OnAttach(void)
    {
        Character().ResetVelocity();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::THROW_COMBINATION);
        Character().SetEnableCatchHit(false);
        Character().SetEnableBodyHit(false);
        
        m_iMotionStep = 0;
    };


    void CThrowCombination::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
        Character().SetEnableBodyHit(true);
    };


    void CThrowCombination::OnRun(void)
    {
        switch (m_iMotionStep)
        {
        case 0:
            {
                if (Character().IsMotionEnd())
                {
                    Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::THROW_COMBINATION);
                    m_iMotionStep = 1;
                };
            }
            break;

        case 1:
            {
                if (Character().IsMotionEnd())
                    StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
            }
            break;

        default:
            break;
        };
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_EMPTY_ENABLED_STATUS_FOR(CCaught, true);


    void CCaught::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::CAUGHT);
        Character().SetEnableCatchHit(false);
        Character().SetEnableBodyHit(false);
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        Character().ResetVelocity();
        Character().ResetAcceleration();

        IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_NORMAL, 0.2f);
    };


    void CCaught::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
        Character().SetEnableBodyHit(true);
    };


    void CCaught::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CLifted, { PLAYERTYPES::STATUS_CAUGHT,
                                         PLAYERTYPES::STATUS_LIFTED_WALK });


    void CLifted::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::LIFTED);
    };


    void CLifted::OnDetach(void)
    {
        ;
    };


    void CLifted::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CLiftedWalk, { PLAYERTYPES::STATUS_LIFTED });
    

    void CLiftedWalk::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::LIFTED_WALK);
    };
    

    void CLiftedWalk::OnDetach(void)
    {
        ;
    };
    

    void CLiftedWalk::OnRun(void)
    {
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
    };
    
    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CThrown, { PLAYERTYPES::STATUS_LIFTED,
                                         PLAYERTYPES::STATUS_LIFTED_WALK,
                                         PLAYERTYPES::STATUS_CAUGHT });


    void CThrown::OnAttach(void)
    {
        Character().SetEnableBodyHit(false);
        Character().SetEnableCatchHit(false);

        /* init direction */
        RwV3d vDirection = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vDirection);
        vDirection.y = 0.0f;

        Math::Vec3_Normalize(&vDirection, &vDirection);
        Math::Vec3_Negate(&vDirection, &vDirection);

        Character().AttackParameter().m_vDirection = vDirection;
        Character().AttackParameter().m_direction = CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT;

        /* init velocity */
        Character().GetVelocity(&Character().AttackParameter().m_vVelocity);

        /* init other */
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_FLYAWAY_FRONT);

        CGameEvent::SetPlayerDamaged(Character().GetPlayerNo());
    };


    void CThrown::OnDetach(void)
    {
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        Character().SetEnableBodyHit(true);
        Character().SetEnableCatchHit(true);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
    };


    void CThrown::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_EMPTY_ENABLED_STATUS_FOR(CThrownBack, true);


    void CThrownBack::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::THROWN_BACK);
    };


    void CThrownBack::OnDetach(void)
    {
        ;
    };


    void CThrownBack::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_EMPTY_ENABLED_STATUS_FOR(CThrownCombination, true);


    void CThrownCombination::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::THROWN_COMBINATION1);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        Character().SetEnableCatchHit(false);

        CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_COMBINATION);

        IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_NORMAL, 0.2f);

        m_fStartTimer = 0.0f;
        m_hEffect = 0;
        m_iMotionStep = 0;
    };


    void CThrownCombination::OnDetach(void)
    {
        if (m_hEffect)
        {
            CEffectManager::Finish(m_hEffect);
            m_hEffect = 0;
        };

        Character().SetEnableCatchHit(true);
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    };


    void CThrownCombination::OnRun(void)
    {
        switch (m_iMotionStep)
        {
        case 0:
            {
                SetVelocity();
                if (Character().IsMotionEnd())
                {
                    Character().ResetVelocity();
                    Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
                    Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::THROWN_COMBINATION2);

                    RwV3d vPosition = Math::VECTOR3_ZERO;
                    Character().GetPosition(&vPosition);

                    m_hEffect = CEffectManager::Play(PLAYERTYPES::EFFECTNAMES::THROW_COMBINATION, &vPosition);
                    ASSERT(m_hEffect);

                    m_iMotionStep = 1;
                    m_fStartTimer = CGameProperty::GetTotalElapsedTime();
                }
            };
            break;

        case 1:
            {
                ASSERT(m_hEffect);

                float fMoveSpeed = Character().Feature().m_fRunMoveSpeed;
                RwV3d vVelocity = { 0.0f, 0.0f, fMoveSpeed };

                Character().RotateVectorByDirection(&vVelocity, &vVelocity);
                Character().SetVelocity(&vVelocity);

                RwV3d vPosition = Math::VECTOR3_ZERO;
                Character().GetBodyPosition(&vPosition);

                CEffectManager::SetPosition(m_hEffect, &vPosition);
                CEffectManager::SetDirection(m_hEffect, Character().GetDirection());

                if ((CGameProperty::GetTotalElapsedTime() - m_fStartTimer) > 2.0f)
                {
                    CEffectManager::Finish(m_hEffect);
                    Character().ChangeStatus(PLAYERTYPES::STATUS_THROWN_COMBINATION_END);
                };
            }
            break;

        default:
            break;
        };
    };


    void CThrownCombination::SetVelocity(void)
    {
        CGameObject* pLiftObj = CGameObjectManager::GetObject(Character().LiftInfo().m_szLiftObjectName);
        if (pLiftObj && pLiftObj->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CPlayerCharacter* pCharacter = static_cast<CPlayerCharacter*>(pLiftObj);
            ASSERT(pCharacter->GetCharacterType() == CPlayerCharacter::TYPE_PLAYER);

            RwV3d vVelocity = Math::VECTOR3_AXIS_Z;
            pCharacter->RotateVectorByDirection(&vVelocity, &vVelocity);

            RwV3d vPositionMe = Math::VECTOR3_ZERO;
            Character().GetPosition(&vPositionMe);

            RwV3d vPositionLift = Math::VECTOR3_ZERO;
            pCharacter->GetPosition(&vPositionLift);

            Math::Vec3_Add(&vVelocity, &vVelocity, &vPositionLift);
            Math::Vec3_Sub(&vVelocity, &vVelocity, &vPositionMe);
            Math::Vec3_Scale(&vVelocity, &vVelocity, (1.0f / CGameProperty::GetElapsedTime()));

            Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
            Character().SetDirection(pCharacter->GetDirection());
            Character().SetVelocity(&vVelocity);
            Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
        };
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CThrownCombinationEnd, { PLAYERTYPES::STATUS_THROWN_COMBINATION });


    void CThrownCombinationEnd::OnAttach(void)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_AERIAL);
    };


    void CThrownCombinationEnd::OnDetach(void)
    {
        ;
    };


    void CThrownCombinationEnd::OnRun(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CThrownMiss, { PLAYERTYPES::STATUS_CAUGHT,
                                             PLAYERTYPES::STATUS_LIFTED,
                                             PLAYERTYPES::STATUS_LIFTED_WALK });


    void CThrownMiss::OnAttach(void)
    {
        Character().SetEnableBodyHit(true);
        Character().SetEnableCatchHit(true);
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    void CThrownMiss::OnDetach(void)
    {
        ;
    };


    void CThrownMiss::OnRun(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //


    namespace Throw
    {
        void SendMissThrowCommon(CPlayerCharacter& rPlayerChr)
        {
            CGameObject* pLiftObj = CGameObjectManager::GetObject(rPlayerChr.LiftInfo().m_szLiftObjectName);
            if (pLiftObj)
            {
                RwMatrix matrix;
                RwMatrixSetIdentity(&matrix);
                Math::Matrix_RotateY(&matrix, rPlayerChr.GetDirection());

                RwV3d vVelocity = { 0.0f, -1.0f, 1.0f };
                RwV3dTransformVector(&vVelocity, &vVelocity, &matrix);

                Math::Vec3_Normalize(&vVelocity, &vVelocity);
                Math::Vec3_Scale(&vVelocity, &vVelocity, -0.2f);
                
                CGameObjectManager::SendMessage(pLiftObj, CHARACTERTYPES::MESSAGEID_MISSTHROW, &vVelocity);
            };
        };


        bool SendPositionCommon(CPlayerCharacter& rPlayerChr)
        {
            CGameObject* pLiftObj = CGameObjectManager::GetObject(rPlayerChr.LiftInfo().m_szLiftObjectName);
			if (!pLiftObj)
			{
				rPlayerChr.ChangeStatus(PLAYERTYPES::STATUS_IDLE);
                return false;
			};

            CModel* pModel = rPlayerChr.GetModel();
            ASSERT(pModel);

            CCharacter::MSG_LIFT_INFO liftinfo = { 0 };
            liftinfo.m_vPosition    = *pModel->GetBonePositionFromID(3);
            liftinfo.m_fDirection   = rPlayerChr.GetDirection();
            liftinfo.m_iStatus      = rPlayerChr.GetStatus();
            
            if (rPlayerChr.GetStatus() == PLAYERTYPES::STATUS_LIFT_SUCCESS)
            {
                CGameObjectManager::SendMessage(pLiftObj, CHARACTERTYPES::MESSAGEID_LIFT, &liftinfo);
                return true;
            };

            if (pLiftObj->GetType() != GAMEOBJECTTYPE::CHARACTER)
            {
                CGameObjectManager::SendMessage(pLiftObj, CHARACTERTYPES::MESSAGEID_LIFT, &liftinfo);
                return true;
            };
            
			RwV3d vTarPos = Math::VECTOR3_ZERO;
			static_cast<CCharacter*>(pLiftObj)->GetPosition(&vTarPos);

			RwV3d vMePos = Math::VECTOR3_ZERO;
			rPlayerChr.GetPosition(&vMePos);

			RwV3d vDltPos = Math::VECTOR3_ZERO;
			Math::Vec3_Sub(&vDltPos, &vTarPos, &vMePos);
			vDltPos.y = 0.0f;

			float fDist = Math::Vec3_Length(&vDltPos);
            if (fDist <= 0.8f)
            {
                CGameObjectManager::SendMessage(pLiftObj, CHARACTERTYPES::MESSAGEID_LIFT, &liftinfo);
                return true;
            };

            rPlayerChr.ChangeStatus(PLAYERTYPES::STATUS_IDLE);
            return false;
        }; 
    };
};