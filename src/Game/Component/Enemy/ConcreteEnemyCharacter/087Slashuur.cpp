#include "087Slashuur.hpp"

#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/087SlashuurAI.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


#define HPSTEAL_AMOUNT (30) // last phase area drain 


static const RwV3d BANDANA_OFFSET = { -0.028f, 0.237f, -0.038f };
static const RwV3d SCYTHE_OFFSET = { 1.35f, 0.0f, 0.0f };
static const RwV3d HPSTEAL_LIGHT_OFFSET = { 0.0f, -0.5f, 0.0f };
static const float ESCAPE_JUMP_VELOCITY_Y = 10.606602f; // TODO


namespace MOTIONNAMES
{
    static const char* ATTACK_A             = "A";
    static const char* ATTACK_AA            = "AA";
    static const char* ATTACK_AAB           = "AAB";
    static const char* ATTACK_AAC           = "AAC";
    static const char* ATTACK_B             = "B";
    
    static const char* DRAIN_START          = "C1";
    static const char* DRAIN_START2         = "C2";
    static const char* DRAIN_LOOP           = "C3";
    static const char* DRAIN_END            = "C4";
    
    static const char* ESCAPE_START         = "D1";
    static const char* ESCAPE_JUMP          = "D2";
    static const char* ESCAPE_ATTACK        = "DAttack";
    static const char* ESCAPE_END           = "D3";
    
    static const char* TELEPORTATION_LOOP   = "E2";
    static const char* TELEPORTATION_END    = "E3";
    
    static const char* RUN_ATTACK           = "RunAttack";
    
    static const char* THROW_CATCH          = "Nage1";
    static const char* THROW_LIFT           = "Nage2";
    static const char* THROW_RELEASE        = "Nage3";

    static const char* SHOT                 = "Shuriken";
}; /* namespace MOTIONNAMES */


//
// *********************************************************************************
//


C087Slashuur::CBodyTracer::CBodyTracer(C087Slashuur* pSlashuur)
: m_hObj(pSlashuur->Compositor().GetHandle())
{
    ;
};


/*virtual*/ float C087Slashuur::CBodyTracer::GetDirection(void) /*override*/
{
    return 0.0f;
};


/*virtual*/ void C087Slashuur::CBodyTracer::GetPosition(RwV3d* pvPosition) /*override*/
{
    CGameObject* pGameObj = CGameObjectManager::GetObject(m_hObj);
    if (pGameObj != nullptr)
        static_cast<CCharacterCompositor*>(pGameObj)->GetBodyPosition(pvPosition);        
};


//
// *********************************************************************************
//


C087Slashuur::CScytheTracer::CScytheTracer(C087Slashuur* pSlashuur)
: m_hObj(pSlashuur->Compositor().GetHandle())
{
    ;
};


/*virtual*/ float C087Slashuur::CScytheTracer::GetDirection(void) /*override*/
{
    return 0.0f;
};


/*virtual*/ void C087Slashuur::CScytheTracer::GetPosition(RwV3d* pvPosition) /*override*/
{
    CGameObject* pGameObj = CGameObjectManager::GetObject(m_hObj);
    if (pGameObj == nullptr)
    {
        *pvPosition = Math::VECTOR3_ZERO;
        return;
    };

    CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pGameObj);

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetPosition(&vecMyPos);

    RwV3d vecScytheBladePos = Math::VECTOR3_ZERO;
    pChrCompositor->GetBonePosition(&vecScytheBladePos, 7, &SCYTHE_OFFSET);
    Math::Vec3_Add(&vecScytheBladePos, &vecScytheBladePos, &vecMyPos);

    *pvPosition = vecScytheBladePos;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CHideStatusObserver::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ C087Slashuur::CHideStatusObserver::RESULT
C087Slashuur::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


C087Slashuur::CThinkingStatusObserver::CThinkingStatusObserver()
: m_nextStatus(ENEMYTYPES::STATUS_IDLE)
{
    ;
};


/*virtual*/ void C087Slashuur::CThinkingStatusObserver::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS nextStatus = m_nextStatus;

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            switch (moveType)
            {
            case C087SlashuurAI::ORDERTYPE::MOVE_WALK_NO:
                nextStatus = ENEMYTYPES::STATUS_WALK;
                break;

            case C087SlashuurAI::ORDERTYPE::MOVE_RUN_NO:
                nextStatus = ENEMYTYPES::STATUS_RUN;
                break;

            case C087SlashuurAI::ORDERTYPE::MOVE_JUMP:
                {
                    nextStatus = ENEMYTYPES::STATUS_JUMP_READY;

                    C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
                    pState->vecJumpPos = EnemyChr().AIThinkOrder().OrderMove().m_vAt;
                }
                break;

            case C087SlashuurAI::ORDERTYPE::MOVE_DASH:
                nextStatus = ENEMYTYPES::STATUS_DASH;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case C087SlashuurAI::ORDERTYPE::ATTACK_A:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_ATTACK_A);
                break;

            case C087SlashuurAI::ORDERTYPE::ATTACK_AA:
            case C087SlashuurAI::ORDERTYPE::ATTACK_AAB:
            case C087SlashuurAI::ORDERTYPE::ATTACK_AAC:
                nextStatus = ENEMYTYPES::STATUS_IDLE; /* all chain attacks handled by slashuur ATTACK_A status */
                break;

            case C087SlashuurAI::ORDERTYPE::ATTACK_B:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_ATTACK_B);
                break;

            case C087SlashuurAI::ORDERTYPE::ATTACK_ESCAPE:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_ESCAPE_READY);
                break;

            case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT:
            case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT_SCYTHE:
            case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT_DRAIN:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_TELEPORT_START);
                break;

            case C087SlashuurAI::ORDERTYPE::ATTACK_SHOT:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_SHOT);
                break;

            case C087SlashuurAI::ORDERTYPE::ATTACK_RUN:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_RUN_ATTACK);
                break;

            case C087SlashuurAI::ORDERTYPE::ATTACK_THROW:
                nextStatus = ENEMYTYPES::STATUS_THROW;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        nextStatus = ENEMYTYPES::STATUS_GUARD;
        break;

    default:
        nextStatus = ENEMYTYPES::STATUS_IDLE;
        break;
    };

    EnemyChr().SetStatus(nextStatus);

    switch (nextStatus)
    {
    case ENEMYTYPES::STATUS_IDLE:
    case ENEMYTYPES::STATUS_WALK:
    case ENEMYTYPES::STATUS_RUN:
    case ENEMYTYPES::STATUS_GUARD:
        break;

    default:
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        break;
    };
};


/*virtual*/ C087Slashuur::CThinkingStatusObserver::RESULT
C087Slashuur::CThinkingStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);
    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
};


/*virtual*/ C087Slashuur::CDeathStatusObserver::RESULT
C087Slashuur::CDeathStatusObserver::Observing(void) /*override*/
{
    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CDeathStatusObserver::OnEnd(void) /*override*/
{
    CEnemyUtils::EntryNexusDeathEffect(&EnemyChr(), 1.5f);
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CDashStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_fDashTime = EnemyChr().Feature().m_fDashMoveTime;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DASH1, true);

    float fDashDir = EnemyChr().AIThinkOrder().OrderMove().m_vAt.y;
    EnemyChr().Compositor().SetDirection(fDashDir);

    float fDashSpeed = EnemyChr().Feature().m_fDashMoveSpeed;
    RwV3d vecVelocity = { std::sin(fDashDir) * fDashSpeed,
                          0.0f,
                          std::cos(fDashDir) * fDashSpeed };
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH1, new CEnemyTracer(&EnemyChr()), &Math::VECTOR3_ZERO);
    CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH2, new CEnemyTracer(&EnemyChr()), &Math::VECTOR3_ZERO);
};


/*virtual*/ C087Slashuur::CDashStatusObserver::RESULT
C087Slashuur::CDashStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            m_fDashTime -= CGameProperty::GetElapsedTime();
            if (m_fDashTime <= 0.0f)
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DASH2, true);
                ++m_step;
            };
        }
        break;

    case 1:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CDashStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CAerialStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CAerialStatus::OnStart();

    C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
    
    int32 status = EnemyChr().StatusSubject().StatusPrev();
    if (status == ENEMYTYPES::STATUS_JUMP_READY)
    {
        pState->bEscape = false;

        RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

        RwV3d vecJumpDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecJumpDir, &pState->vecJumpPos, &vecMyFootPos);
        Math::Vec3_Normalize(&vecJumpDir, &vecJumpDir);

        float fJumpVelXZ = EnemyChr().Feature().m_fAerialMoveSpeed;
        float fJumpVelY = EnemyChr().Feature().m_fJumpInitializeSpeed;

        RwV3d vecJumpVel = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vecJumpVel, &vecJumpDir, fJumpVelXZ);
        vecJumpVel.y = fJumpVelY;

        EnemyChr().Compositor().SetVelocity(&vecJumpVel);
    }
    else if (status == C087Slashuur::STATUS_ESCAPE_READY)
    {
        pState->bEscape = true;
        EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ESCAPE_JUMP, true);
    };

    m_bAttackFlag = false;
    m_bShotFlag = false;
};


/*virtual*/ C087Slashuur::CAerialStatusObserver::RESULT
C087Slashuur::CAerialStatusObserver::Observing(void) /*override*/
{
    C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());

    float fJumpInitSpeed = (pState->bEscape ? ESCAPE_JUMP_VELOCITY_Y : EnemyChr().Feature().m_fJumpInitializeSpeed);
    float fMotionT = CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), fJumpInitSpeed);
	float fMotionET = EnemyChr().Compositor().GetMotionEndTime();
    if (pState->bEscape)
    {
        if (m_bAttackFlag && !m_bShotFlag && (fMotionT > 0.18032786f)) // TODO
        {
            m_bShotFlag = true;

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8414));

            RwV3d vecPosition = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetPosition(&vecPosition);

            RwV3d vecOffset = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetOffsetPosition(&vecOffset, 1);

            Math::Vec3_Add(&vecPosition, &vecPosition, &vecOffset);

            RwV3d vecDirection = { 0.0f, -1.0f, 0.0f };

            CMagicManager::CParameter param;
            param.SetObject(&EnemyChr().Compositor());
            param.SetPositon(&vecPosition);
            param.SetDirection(&vecDirection);

            uint32 hMagic = CMagicManager::Play(MAGICID::ID_SLABALL, &param);
            ASSERT(hMagic != 0);
            if (hMagic)
            {
                CMagicManager::SetScale(hMagic, 1.73f);
                CMagicManager::SetSpeed(hMagic, 50.0f);
            };

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4401));
        };

		if (!m_bAttackFlag)
		{
			RwV3d vecVelocity = Math::VECTOR3_ZERO;
			EnemyChr().Compositor().GetVelocity(&vecVelocity);

			if (vecVelocity.y <= 0.0f)
			{
				EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ESCAPE_ATTACK, true);
				m_bAttackFlag = true;
			};
		};
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CAerialStatusObserver::OnEnd(void) /*override*/
{
    CCommonEnemyObserver::CAerialStatus::OnEnd();
    return ENEMYTYPES::STATUS_TOUCHDOWN;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CTouchdownStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CTouchdownStatus::OnStart();

    C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
    if (pState->bEscape)
        EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ESCAPE_END, true);
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CGuardStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CGuardStatus::OnStart();
    m_step = 0;
};


/*virtual*/ C087Slashuur::CGuardStatusObserver::RESULT
C087Slashuur::CGuardStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_START:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_1, true);
            ++m_step;
        }
        break;

    case STEP_START_RUN:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                ++m_step;
        }        
        break;

    case STEP_HOLD:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_2, true);
            ++m_step;
        }
        break;

    case STEP_HOLD_RUN:
        {
            if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_DEFENCE)
                ++m_step;
        }
        break;

    case STEP_END:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_3, true);
            ++m_step;
        }
        break;

    case STEP_END_RUN:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CNormalAttackStatusObserver::OnStart(void) /*override*/
{
    switch (static_cast<int32>(Status()))
    {
    case C087Slashuur::STATUS_ATTACK_A:     m_pszMotionName = MOTIONNAMES::ATTACK_A;    break;
    case C087Slashuur::STATUS_ATTACK_AA:    m_pszMotionName = MOTIONNAMES::ATTACK_AA;   break;
    case C087Slashuur::STATUS_ATTACK_AAB:   m_pszMotionName = MOTIONNAMES::ATTACK_AAB;  break;
    case C087Slashuur::STATUS_ATTACK_AAC:   m_pszMotionName = MOTIONNAMES::ATTACK_AAC;  break;
    case C087Slashuur::STATUS_ATTACK_B:     m_pszMotionName = MOTIONNAMES::ATTACK_B;    break;
    default: ASSERT(false); break;
    };

    EnemyChr().Compositor().ChangeMotion(m_pszMotionName, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    if (static_cast<C087Slashuur::STATUS>(Status()) == C087Slashuur::STATUS_ATTACK_B)
    {
        int32 targetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);
        if (pPlayerChr)
        {
            RwV3d vecFootPosTgt = Math::VECTOR3_ZERO;
            pPlayerChr->GetFootPosition(&vecFootPosTgt);

            float fDirMe = EnemyChr().Compositor().GetDirection();
            float fDirToTgt = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecFootPosTgt);
            float fDirDiff = (fDirToTgt - fDirMe);
            fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
            fDirDiff = Clamp(fDirDiff, -MATH_DEG2RAD(90.0f), MATH_DEG2RAD(90.0f));

            EnemyChr().Compositor().SetDirection(fDirMe + fDirDiff);
        };        
    };

    m_bCheckChain = true;
    m_statusNext = ENEMYTYPES::STATUS_THINKING;

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ C087Slashuur::CNormalAttackStatusObserver::RESULT
C087Slashuur::CNormalAttackStatusObserver::Observing(void) /*override*/
{
    if (m_bCheckChain)
    {
        float fNowMotionT = EnemyChr().Compositor().GetMotionTime();
        float fConnectMotionT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
        if (fNowMotionT > fConnectMotionT)
        {
            m_bCheckChain = false;

            if (EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK)
            {
                int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;

                ENEMYTYPES::STATUS statusNow = EnemyChr().GetStatus();
                switch (static_cast<int32>(statusNow))
                {
                case C087Slashuur::STATUS_ATTACK_A:
                    {
                        if (attackType == C087SlashuurAI::ORDERTYPE::ATTACK_AA)
                            m_statusNext = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_ATTACK_AA);
                    }
                    break;

                case C087Slashuur::STATUS_ATTACK_AA:
                    {
                        if (attackType == C087SlashuurAI::ORDERTYPE::ATTACK_AAB)
                            m_statusNext = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_ATTACK_AAB);
                        else if (attackType == C087SlashuurAI::ORDERTYPE::ATTACK_AAC)
                            m_statusNext = static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_ATTACK_AAC);
                    }
                    break;

                default:
                    break;
                };

                if (m_statusNext != ENEMYTYPES::STATUS_THINKING)
                    return RESULT_END;
            };
        };
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CNormalAttackStatusObserver::OnEnd(void) /*override*/
{
    return m_statusNext;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CRunAttackStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::RUN_ATTACK, true);

    int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTargetNo);
    ASSERT(pPlrChr != nullptr);

    RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecFootPosPlr);
    vecFootPosPlr.y = 0.0f;

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);
    vecFootPosMe.y = 0.0f;

    float fDir = EnemyChr().Compositor().GetDirection();

    if (!Math::Vec3_IsEqual(&vecFootPosMe, &vecFootPosPlr))
    {
        fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecFootPosPlr);
        EnemyChr().Compositor().SetDirection(fDir);
    };

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);

    float fSpeed = Math::Vec3_Length(&vecVelocity);
    vecVelocity = { std::sin(fDir) * fSpeed * 0.5f,
                    0.0f,
                    std::cos(fDir) * fSpeed * 0.5f };
                    
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ C087Slashuur::CRunAttackStatusObserver::RESULT
C087Slashuur::CRunAttackStatusObserver::Observing(void) /*override*/
{
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);

    float fSpeed = Math::Vec3_Length(&vecVelocity);
    fSpeed -= CGameProperty::GetElapsedTime() * 2.6f;
    if (fSpeed < 0.0f)
        fSpeed = 0.0f;

    Math::Vec3_Normalize(&vecVelocity, &vecVelocity);
    Math::Vec3_Scale(&vecVelocity, &vecVelocity, fSpeed);

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CRunAttackStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CEscapeReadyStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ESCAPE_START, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CEscapeReadyStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);    
    vecVelocity.y = ESCAPE_JUMP_VELOCITY_Y;
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CTeleportationStartStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_fTime = 0.0f;
    m_orderType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_WARP_START, &vecMyFootPos);
    ASSERT(hEffect != 0);
    if (hEffect)
        CEffectManager::SetScale(hEffect, 1.5f);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4396));

    switch (m_orderType)
    {
    case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT:
        {
            C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
            pState->iTeleportPlayerNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        }
        break;

    case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT_DRAIN:
        {
            CMessageManager::Request(SEGROUPID::VALUE(119));
        }
        break;

    default:
        break;
    };
};


/*virtual*/ C087Slashuur::CTeleportationStartStatusObserver::RESULT
C087Slashuur::CTeleportationStartStatusObserver::Observing(void) /*override*/
{
    m_fTime += CGameProperty::GetElapsedTime();

    switch (m_step)
    {
    case 0:
        {
            if (m_fTime < 0.46f)
                break;
            
            EnemyChr().Compositor().GetModel()->SetDrawEnable(false);
            EnemyChr().Compositor().SetEnableBodyHit(false);
            EnemyChr().Compositor().SetEnableCatchHit(false);

            IModule* pMod = EnemyChr().Compositor().GetModule(MODULETYPE::CIRCLE_SHADOW);
            ASSERT(pMod != nullptr);

            CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(pMod);
            pCircleShadowMod->SetEnable(false);

            ++m_step;
            m_fTime = 0.0f;
        }
        break;

    case 1:
        {
            if (m_fTime >= 0.46f)
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CTeleportationStartStatusObserver::OnEnd(void) /*override*/
{
    switch (m_orderType)
    {
    case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT:
        return static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_TELEPORT_ATTACK);

    case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT_SCYTHE:
        return static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_TELEPORT_SCYTHE);

    case C087SlashuurAI::ORDERTYPE::ATTACK_TELEPORT_DRAIN:
        return static_cast<ENEMYTYPES::STATUS>(C087Slashuur::STATUS_TELEPORT_DRAIN);

    default:
        ASSERT(false);
        return ENEMYTYPES::STATUS_THINKING;
    };
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CTeleportationAttackStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_fTime = 0.0f;

    C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(pState->iTeleportPlayerNo);

    float fPlrDir = pPlrChr->GetDirection();

    RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecPlrFootPos);

    RwV3d vecPtStart = vecPlrFootPos;
    vecPtStart.y += 1.0f;

    RwV3d vecPtEnd = vecPtStart;
    vecPtEnd.x -= std::sin(fPlrDir);
    vecPtEnd.z -= std::cos(fPlrDir);

    RwV3d vecTeleportPos = Math::VECTOR3_ZERO;
    if (CWorldMap::CheckCollisionLine(&vecPtStart, &vecPtEnd))
    {
        const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
        ASSERT(pCollisionResult != nullptr);

        vecTeleportPos = pCollisionResult->m_vClosestPt;
    }
    else
    {
        vecTeleportPos = vecPtEnd;
    };

    vecTeleportPos.y = CWorldMap::GetMapHeight(&vecTeleportPos);

    EnemyChr().Compositor().SetDirection(fPlrDir);
    EnemyChr().Compositor().SetPosition(&vecTeleportPos);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_WARP_OUT, &vecTeleportPos);
    ASSERT(hEffect != 0);
    if (hEffect)
        CEffectManager::SetScale(hEffect, 1.5f);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4395));
};


/*virtual*/ C087Slashuur::CTeleportationAttackStatusObserver::RESULT
C087Slashuur::CTeleportationAttackStatusObserver::Observing(void) /*override*/
{
    m_fTime += CGameProperty::GetElapsedTime();

    switch (m_step)
    {
    case 0:
        {
            if (m_fTime < 0.46f)
                break;

            EnemyChr().Compositor().GetModel()->SetDrawEnable(true);
            EnemyChr().Compositor().SetEnableBodyHit(true);
            EnemyChr().Compositor().SetEnableCatchHit(true);

            IModule* pMod = EnemyChr().Compositor().GetModule(MODULETYPE::CIRCLE_SHADOW);
            ASSERT(pMod != nullptr);

            CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(pMod);
            pCircleShadowMod->SetEnable(true);

            ++m_step;
            m_fTime = 0.0f;
        }
        break;

    case 1:
        {
            if (m_fTime >= 0.2f)
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CTeleportationAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CTeleportationScytheStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_fTime = 0.0f;
    m_fFadeTiming = 0.6f;
    m_bFadeScythe = false;

    for (int32 i = 0; i < COUNT_OF(m_ahScythe); ++i)
        m_ahScythe[i] = 0;

    RwV3d avecPlayerPos[GAMETYPES::PLAYERS_MAX];
    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (pPlrChr)
            pPlrChr->GetBodyPosition(&avecPlayerPos[i]);
    };

    /*
     *  NOTE: all calcs for away teleport position did from the center of the map
     */
    float afPlayerAngle[GAMETYPES::PLAYERS_MAX + 1] = { 0.0f };
    for (int32 i = 0; i < numPlayers; ++i)
    {
        RwV3d vecPlayerPos = avecPlayerPos[i];
        vecPlayerPos.y = 0.0f;

        if (Math::Vec3_IsEqual(&vecPlayerPos, &Math::VECTOR3_ZERO))
            afPlayerAngle[i + 1] = 0.0f;
        else
            afPlayerAngle[i + 1] = std::atan2(vecPlayerPos.x, vecPlayerPos.z);
    };

    float* itBegin  = std::begin(afPlayerAngle) + 1;
    float* itEnd    = std::begin(afPlayerAngle) + 1 + numPlayers;
    std::sort(itBegin, itEnd, [](float a, float b) { return (a < b); });

    float fMaxDiff = (MATH_PI2 - (afPlayerAngle[numPlayers] - afPlayerAngle[1]));
    float fMidAngle = (((afPlayerAngle[1] + afPlayerAngle[numPlayers]) / 2.0f) + MATH_PI);
    for (int32 i = 0; i < numPlayers; ++i)
    {
        float fAngle = afPlayerAngle[i + 1];
        float fAngleNext = afPlayerAngle[(i + 1) % numPlayers + 1];
        float fAngleDiff = (fAngleNext - fAngle);
        if (fMaxDiff < fAngleDiff)
        {
            fMidAngle = (fAngle + fAngleNext) / 2.0f;
            fMaxDiff = fAngleDiff;
        };
    };

    float fDistSum = 0.0f;
    for (int32 i = 0; i < numPlayers; ++i)
    {
        fDistSum += std::sqrt(avecPlayerPos[i].x * avecPlayerPos[i].x +
                              avecPlayerPos[i].z * avecPlayerPos[i].z);
    };

    C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());

    float fDistAway = pState->fMapMaxZ - (fDistSum / static_cast<float>(numPlayers));
    RwV3d vecAwayPos = { std::sin(fMidAngle) * fDistAway,
                         10.0f,
                         std::cos(fMidAngle) * fDistAway };

    vecAwayPos.y = CWorldMap::GetMapHeight(&vecAwayPos);

    EnemyChr().Compositor().SetDirection(fMidAngle + MATH_PI);
    EnemyChr().Compositor().SetPosition(&vecAwayPos);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_WARP_OUT, &vecAwayPos);
    CEffectManager::SetScale(hEffect, 1.5f);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4395));
};


/*virtual*/ C087Slashuur::CTeleportationScytheStatusObserver::RESULT
C087Slashuur::CTeleportationScytheStatusObserver::Observing(void) /*override*/
{
    m_fTime += CGameProperty::GetElapsedTime();

    switch (m_step)
    {
    case 0:
        {
            if (m_fTime < 0.41f)
                break;

            int32 numPlayers = CAIUtils::GetPlayerNum();
            for (int32 i = 0; i < numPlayers; ++i)
            {
                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
                if (pPlrChr)
                    pPlrChr->GetBodyPosition(&m_avecPlayerBodyPos[i]);
            };

            ++m_step;
        }
        break;

    case 1:
        {            
            if (m_fTime < 0.46f)
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::TELEPORTATION_LOOP, true);
            EnemyChr().Compositor().GetModel()->SetDrawEnable(true);
            EnemyChr().Compositor().SetEnableBodyHit(true);
            EnemyChr().Compositor().SetEnableCatchHit(true);

            IModule* pMod = EnemyChr().Compositor().GetModule(MODULETYPE::CIRCLE_SHADOW);
            ASSERT(pMod != nullptr);

            CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(pMod);
            pCircleShadowMod->SetEnable(true);

            ++m_step;
            m_fTime = 0.0f;
        }
        break;

    case 2:
        {
            if (m_fTime < 0.2f)
                break;

            RwV3d vecMyBodyPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetBodyPosition(&vecMyBodyPos);

            int32 numPlayers = CAIUtils::GetPlayerNum();
            for (int32 i = 0; i < numPlayers; ++i)
            {
                m_ahScythe[i] = CShotManager::Shot(SHOTID::ID_SCYTHE,
                                                   &m_avecPlayerBodyPos[i],
                                                   &vecMyBodyPos,
                                                   &EnemyChr().Compositor(),
                                                   0.0f,
                                                   2.0f);

                CGameSound::PlayPositionSE(&m_avecPlayerBodyPos[i], SDCODE_SE(4403));
            };

            EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
            EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);

            ++m_step;
            m_fTime = 0.0f;
        }
        break;

    case 3:
        {
            if (m_fTime < 2.0f)
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::TELEPORTATION_END, true);
            CGameSound::FadeOutSE(SDCODE_SE(4403), CGameSound::FADESPEED_NORMAL);

            FadeScythe();
            m_bFadeScythe = true;

            ++m_step;
        }
        break;

    case 4:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CTeleportationScytheStatusObserver::OnEnd(void) /*override*/
{
    if (!m_bFadeScythe)
    {
        /* boss was damaged during scythe teleport loop */
        FadeScythe();
    };

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
        CShotManager::Finish(m_ahScythe[i]);

    CGameSound::FadeOutSE(SDCODE_SE(4403), CGameSound::FADESPEED_NORMAL);

    return ENEMYTYPES::STATUS_THINKING;
};


void C087Slashuur::CTeleportationScytheStatusObserver::FadeScythe(void)
{
    IModule* pMod = EnemyChr().Compositor().GetModule(MODULETYPE::ACCUMULATE);
    ASSERT(pMod != nullptr);

    CEnemySlashuurAccumulateModule* pAccumulateMod = static_cast<CEnemySlashuurAccumulateModule*>(pMod);
    pAccumulateMod->Fade(m_fFadeTiming);
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CTeleportationDrainStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_fTime = 0.0f;
    m_fEffectScale = 0.0f;
    m_bDrain = false;
    m_bDrainProc = true;
    m_hEffect = 0;

    RwV3d vecWarpOutPos = { 0.0f, 10.0f, 0.0f };
    vecWarpOutPos.y = CWorldMap::GetMapHeight(&vecWarpOutPos);

    EnemyChr().Compositor().SetPosition(&vecWarpOutPos);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_WARP_OUT, &vecWarpOutPos);
    CEffectManager::SetScale(hEffect, 1.5f);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4395));
};


/*virtual*/ C087Slashuur::CTeleportationDrainStatusObserver::RESULT
C087Slashuur::CTeleportationDrainStatusObserver::Observing(void) /*override*/
{
    float fShadowScale = 1.0f;
    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
        fShadowScale = 2.0f;

    m_fTime += CGameProperty::GetElapsedTime();

    switch (m_step)
    {
    case 0:
        {
            if (m_fTime < 0.46f)
                break;

            EnemyChr().Compositor().GetModel()->SetDrawEnable(true);
            EnemyChr().Compositor().SetEnableBodyHit(true);
            EnemyChr().Compositor().SetEnableCatchHit(true);

            IModule* pMod = EnemyChr().Compositor().GetModule(MODULETYPE::CIRCLE_SHADOW);
            ASSERT(pMod != nullptr);

            CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(pMod);
            pCircleShadowMod->SetEnable(true);

            ++m_step;
            m_fTime = 0.0f;
        }
        break;

    case 1:
        {
            if (m_fTime >= 0.0f)
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::DRAIN_START);
                ++m_step;
            };
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
            {
                RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);
                vecMyFootPos.y += 0.1f;

                m_hEffect = CEffectManager::Play(EFFECTID::ID_SLA_SHADOW, &vecMyFootPos);
                m_fEffectScale = fShadowScale;
                m_bDrain = true;

                CEffectManager::SetScale(m_hEffect, m_fEffectScale);

                CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4402));

                EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
                EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);

                ++m_step;
            };
        }
        break;

    case 3:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::DRAIN_START2, true);
                CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8421));
                ++m_step;
            };
        }
        break;

    case 4:
        {
            C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
            
            if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
                pState->bDrain = true;

            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::DRAIN_LOOP, true);
                m_fTime = 0.0f;
                ++m_step;
            };
        }
        break;

    case 5:
        {
            if (m_fTime < 0.1f)
                break;

            /* hold step until there is hpsteal magic or drain active */
            bool bHoldDrain = false;
            
            C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
            for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
            {
				if (pState->ahMagic[i] || (pState->afDrainTime[i] > 0.0f))
					bHoldDrain = true;
            };

			if (bHoldDrain)
				break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::DRAIN_END, true);
            m_bDrain = false;
            m_fTime = 0.0f;
            m_fEffectScale = fShadowScale;
            ++m_step;
        }
        break;

    case 6:
        {
            if (m_fTime >= 0.4f)
            {
                m_fTime = 0.4f;
                ++m_step;
                CGameSound::FadeOutSE(SDCODE_SE(4402), CGameSound::FADESPEED_NORMAL);
            };

            float fScale = (0.4f - m_fTime) * (fShadowScale * 2.5f);
            float fScaleNow = (fScale / m_fEffectScale);
            m_fEffectScale = fScale;
            CEffectManager::SetScale(m_hEffect, fScaleNow);
        }
        break;

    case 7:
        {
            float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
            if (fMotionNowT > 0.466f)
            {
                int32 numPlayers = CAIUtils::GetPlayerNum();
                for (int32 i = 0; i < numPlayers; ++i)
                {
                    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
                    if (!pPlrChr)
                        continue;

                    if (pPlrChr->GetStatus() == PLAYERTYPES::STATUS_SLEEP)
                    {
                        RwSphere hitSphere;
                        hitSphere.radius = 0.01f;
                        pPlrChr->GetBodyPosition(&hitSphere.center);

                        CHitAttackData attackAwake;
                        attackAwake.SetObject(EnemyChr().Compositor().GetHandle());
                        attackAwake.SetTarget(CHitAttackData::TARGET_PLAYER);                        
                        attackAwake.SetShape(CHitAttackData::SHAPE_SPHERE);
                        attackAwake.SetStatus(CHitAttackData::STATUS_KNOCK);
                        attackAwake.SetSphere(&hitSphere);

                        CHitAttackManager::RegistAttack(&attackAwake);

                        CMessageManager::Request(SEGROUPID::VALUE(120));
                    };
                };

                ++m_step;
            };
        }
        break;

    case 8:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                CEffectManager::Finish(m_hEffect);
                m_hEffect = 0;

                return RESULT_END;
            };
        }
        break;

    default:
        break;
    };

    if (m_bDrain)
    {
        if (m_bDrainProc)
        {
            m_bDrainProc = false;

            RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

            float fDrainRadiusSQ = (5.0f * m_fEffectScale) * (5.0f * m_fEffectScale);

            int32 numPlayers = CAIUtils::GetPlayerNum();
            for (int32 i = 0; i < numPlayers; ++i)
            {
                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
                if (!pPlrChr)
                    continue;

                RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
                pPlrChr->GetFootPosition(&vecFootPosPlr);

                RwV3d vecDist = Math::VECTOR3_ZERO;
                Math::Vec3_Sub(&vecDist, &vecFootPosMe, &vecFootPosPlr);

                float fDistSQ = Math::Vec3_Dot(&vecDist, &vecDist);
                if ((fDistSQ < fDrainRadiusSQ) && (pPlrChr->GetStatus() != PLAYERTYPES::STATUS_SLEEP))
                {
                    RwSphere hitSphere;
                    hitSphere.radius = 0.01f;
                    pPlrChr->GetBodyPosition(&hitSphere.center);

                    CHitAttackData attackSleep;
                    attackSleep.SetObject(EnemyChr().Compositor().GetHandle());
                    attackSleep.SetTarget(CHitAttackData::TARGET_PLAYER);
                    attackSleep.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
                    attackSleep.SetStatus(CHitAttackData::STATUS_SLEEP);
                    attackSleep.SetTroubleParameter(60.0f);
                    attackSleep.SetShape(CHitAttackData::SHAPE_SPHERE);
                    attackSleep.SetSphere(&hitSphere);

                    CHitAttackManager::RegistAttack(&attackSleep);
                };
            };
        }
        else
        {
            m_bDrainProc = true;
        };
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CTeleportationDrainStatusObserver::OnEnd(void) /*override*/
{
    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (!pPlrChr)
            continue;

        if (pPlrChr->GetStatus() == PLAYERTYPES::STATUS_SLEEP)
        {
            RwSphere hitSphere;
            hitSphere.radius = 0.01f;
            pPlrChr->GetBodyPosition(&hitSphere.center);

            CHitAttackData attackAwake;
            attackAwake.SetObject(EnemyChr().Compositor().GetHandle());
            attackAwake.SetTarget(CHitAttackData::TARGET_PLAYER);
            attackAwake.SetShape(CHitAttackData::SHAPE_SPHERE);
            attackAwake.SetSphere(&hitSphere);

            CHitAttackManager::RegistAttack(&attackAwake);

            CMessageManager::Request(SEGROUPID::VALUE(120));
        };
    };

    if (m_hEffect)
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };

    C087Slashuur::STATE* pState = static_cast<C087Slashuur::STATE*>(EnemyChr().SharedData());
    pState->bDrain = false;

    for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
    {
        pState->abPlayerDrained[i] = false;
        pState->abPlayerSleeping[i] = false;

        if (pState->ahMagic[i])
        {
            CMagicManager::SetLive(pState->ahMagic[i], 0.0f);
            pState->ahMagic[i] = 0;
        };
    };

    CGameSound::FadeOutSE(SDCODE_SE(4402), CGameSound::FADESPEED_NORMAL);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CShotStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::SHOT, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
};


/*virtual*/ C087Slashuur::CShotStatusObserver::RESULT
C087Slashuur::CShotStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8414));

        /* get magic start position */
        RwV3d vecPosition = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecPosition);

        RwV3d vecOffset = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetOffsetPosition(&vecOffset, 1);

        Math::Vec3_Add(&vecPosition, &vecPosition, &vecOffset);

        /* get magic direction */        
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
        ASSERT(pPlayerChr != nullptr);

        RwV3d vecPositionTarget = Math::VECTOR3_ZERO;
        pPlayerChr->GetBodyPosition(&vecPositionTarget);

        RwV3d vecDirection = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDirection, &vecPositionTarget, &vecPosition);

        /* start magic */
        CMagicManager::CParameter param;
        param.SetObject(&EnemyChr().Compositor());
        param.SetPositon(&vecPosition);
        param.SetDirection(&vecDirection);

        uint32 hMagic = CMagicManager::Play(MAGICID::ID_SLABALL, &param);
        ASSERT(hMagic != 0);

        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4401));
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CShotStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C087Slashuur::CThrowStatusObserver::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ C087Slashuur::CThrowStatusObserver::RESULT
C087Slashuur::CThrowStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS C087Slashuur::CThrowStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


C087Slashuur::C087Slashuur(void)
: CEnemyCharacter(ENEMYID::ID_SLASSHUR)
, m_state()
{
    SetFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE);

    /* init shared state */
    InitState();

    /* C087Slashuur status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new C087Slashuur::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new C087Slashuur::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new C087Slashuur::CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DASH,                       new C087Slashuur::CDashStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new C087Slashuur::CAerialStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new C087Slashuur::CTouchdownStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new C087Slashuur::CGuardStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_ATTACK_A,                 new C087Slashuur::CNormalAttackStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_ATTACK_AA,                new C087Slashuur::CNormalAttackStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_ATTACK_AAB,               new C087Slashuur::CNormalAttackStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_ATTACK_AAC,               new C087Slashuur::CNormalAttackStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_ATTACK_B,                 new C087Slashuur::CNormalAttackStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_ESCAPE_READY,             new C087Slashuur::CEscapeReadyStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_TELEPORT_START,           new C087Slashuur::CTeleportationStartStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_TELEPORT_ATTACK,          new C087Slashuur::CTeleportationAttackStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_TELEPORT_SCYTHE,          new C087Slashuur::CTeleportationScytheStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_TELEPORT_DRAIN,           new C087Slashuur::CTeleportationDrainStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_SHOT,                     new C087Slashuur::CShotStatusObserver);
    AttachStatusObserver(C087Slashuur::STATUS_RUN_ATTACK,               new C087Slashuur::CRunAttackStatusObserver);

    // not implemented here for now due unused in retail game but implemented there
    //AttachStatusObserver(ENEMYTYPES::STATUS_THROW, new C087Slashuur::CThrowStatusObserver);

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                       new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                        new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,              new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,               new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,        new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,         new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FRONT,            new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_BACK,             new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT,       new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK,        new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT,  new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK,   new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                       new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C087SlashuurAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 16.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.2f;
    parameter.m_feature.m_fRotateRate           = 1.5f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 300;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 10.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 20.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_THINKING   0 */  {  10,   5,   0 },
        /* FREQUENCY_HPSTEAL    1 */  { 100, 100, 100 },
    };

    if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST20FB)
    {
        parameter.m_feature.m_iHPMax /= 2;

        /* make thinking slower */
        aFreqTable[FREQUENCY_THINKING][GAMETYPES::DIFFICULTY_EASY]   *= 2;
        aFreqTable[FREQUENCY_THINKING][GAMETYPES::DIFFICULTY_NORMAL] *= 2;
        aFreqTable[FREQUENCY_THINKING][GAMETYPES::DIFFICULTY_HARD]   *= 2;
    };
    
    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

	bool bReplace = false;
	Initialize(&parameter, bReplace);

    /* init bandana mod */
    CCharacterCompositor* pChrCompositor = &Compositor();

    CBandanaModule* pBandanaModule = new CBandanaModule(pChrCompositor,
                                                        pChrCompositor->GetModel(),
                                                        10,
                                                        &BANDANA_OFFSET,
                                                        CBandanaModule::BANDANACOLOR_SLASHUURGREY);

    pChrCompositor->IncludeModule(pBandanaModule);

    /* init accumuluate mod */
    pChrCompositor->IncludeModule(CAccumulateModule::New(pChrCompositor));
};


/*virtual*/ C087Slashuur::~C087Slashuur(void)
{
    ;
};


/*virtual*/ void C087Slashuur::Run(void) /*override*/
{
    CEnemyCharacter::Run();
    RegistBarrierCatch();

    if (GetStatus() == C087Slashuur::STATUS_TELEPORT_DRAIN)
        TeleportDrainProc();
};


/*virtual*/ void C087Slashuur::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);

    ENEMYTYPES::STATUS statusNow = GetStatus();
    switch (static_cast<int32>(statusNow))
    {
    case ENEMYTYPES::STATUS_THROW:
        {
            if ((pCatch->GetResult() == CHitCatchData::RESULT_THROWFRONT) ||
                (pCatch->GetResult() == CHitCatchData::RESULT_THROWBACK))
            {
                ASSERT(pCatch->GetObject()->GetType() == GAMEOBJECTTYPE::CHARACTER);
                ASSERT(static_cast<CCharacter*>(pCatch->GetObject())->GetCharacterType() == CCharacter::TYPE_PLAYER);

                CPlayerCharacter* pPlrChr = static_cast<CPlayerCharacter*>(pCatch->GetObject());
                if (m_state.iThrowPlayerNo == -1)
                {
                    m_state.iTeleportPlayerNo = pPlrChr->GetPlayerNo();
                    CGameObjectManager::SendMessage(pPlrChr, CHARACTERTYPES::MESSAGEID_CATCH);
                    pPlrChr->RequestDamage(pCatch->GetAttack()->GetPower());
                };
            };
        }
        break;

    case C087Slashuur::STATUS_ATTACK_A:
    case C087Slashuur::STATUS_ATTACK_AA:
    case C087Slashuur::STATUS_ATTACK_AAC:
    case C087Slashuur::STATUS_ATTACK_B:
    case C087Slashuur::STATUS_ESCAPE_READY:
    case C087Slashuur::STATUS_TELEPORT_SCYTHE:
    case C087Slashuur::STATUS_RUN_ATTACK:
        {
            int32 iAttackDamage = pCatch->GetAttack()->GetPower();
            CGameObjectManager::SendMessage(&Compositor(),
                                            CHARACTERTYPES::MESSAGEID_RECVDMG,
                                            reinterpret_cast<void*>(-iAttackDamage));

            if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST20FB)
                CMessageManager::Request(SEGROUPID::VALUE(69));
        }
        break;

    case C087Slashuur::STATUS_TELEPORT_DRAIN:
        {
            ASSERT(pCatch->GetObject());
            if (pCatch->GetObject()->GetType() == GAMEOBJECTTYPE::CHARACTER)
            {
                ASSERT(static_cast<CCharacter*>(pCatch->GetObject())->GetCharacterType() == CCharacter::TYPE_PLAYER);

                CPlayerCharacter* pPlrChr = static_cast<CPlayerCharacter*>(pCatch->GetObject());
                if (pPlrChr->GetStatus() == PLAYERTYPES::STATUS_SLEEP)
                {
                    int32 playerNo = pPlrChr->GetPlayerNo();
                    ASSERT(playerNo >= 0);
                    ASSERT(playerNo < GAMETYPES::PLAYERS_MAX);

                    m_state.abPlayerSleeping[playerNo] = true;
                };
            };
        }
        break;

    default:
        break;
    };
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C087Slashuur::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    CHitAttackData* pAttack = &rCalc.GetAttack();
    CHitCatchData* pCatch = pAttack->GetCatch();
    
    CGameObject* pAttacker = pCatch->GetAttack()->GetObject();

    /* check for don laser shot */
    if (pAttacker->GetType() == GAMEOBJECTTYPE::EFFECT)
    {
        CEffect* pEffect = static_cast<CEffect*>(pAttacker);
        ASSERT(pEffect->GetEffectType() == CEffect::TYPE_WITHHIT);

        CMagic* pMagic = static_cast<CMagic*>(pEffect);
        if (pMagic->GetID() == MAGICID::ID_DON_LASER_BEAM)
        {
            InvokeBarrier(pCatch);
            return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
        };
    };

    /* check for simple shot */
    if (pAttacker->GetType() == GAMEOBJECTTYPE::SHOT)
    {
        InvokeBarrier(pCatch);
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
    };

    /* barrier shot catch */
    if (pCatch->GetCatchNo() == 1)
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

    return CEnemyCharacter::OnDamage(rCalc);
};


/*virtual*/ void C087Slashuur::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    if (iAmount < 0)
    {
        iAmount = (iAmount * static_cast<int32>(FrequencyParameter(FREQUENCY_HPSTEAL))) / 100;

        CEffectManager::PlayTrace(EFFECTID::ID_HPSTEAL_LIGHT,
                                  new C087Slashuur::CBodyTracer(this),
                                  &HPSTEAL_LIGHT_OFFSET);
    };

    if (Feature().m_iHPMax < (Feature().m_iHP - iAmount))
        iAmount = (Feature().m_iHP - Feature().m_iHPMax);

    CEnemyCharacter::OnMessageReceivedDamage(iAmount);
};


void C087Slashuur::InvokeBarrier(CHitCatchData* pCatch) const
{
    RwV3d vecHitPos = *pCatch->GetHitPos();
    
    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    Compositor().GetBodyPosition(&vecBodyPos);

    RwV3d vecDirection = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDirection, &vecHitPos, &vecBodyPos);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_SLA_BARRIER, &vecHitPos);
    if (hEffect)
        CEffectManager::SetDirection(hEffect, &vecDirection);
    
    CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4399));

    if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST20FB)
        CMessageManager::Request(SEGROUPID::VALUE(68));
};


void C087Slashuur::RegistBarrierCatch(void)
{
    RwV3d vecMyBodyPos = Math::VECTOR3_ZERO;
    Compositor().GetBodyPosition(&vecMyBodyPos);

    RwSphere hitSphere;
    hitSphere.center = vecMyBodyPos;
    hitSphere.radius = 2.0f;

    CHitCatchData catchBarrier;
    catchBarrier.SetObject(Compositor().GetHandle());
    catchBarrier.SetObjectType(Compositor().GetType());
    catchBarrier.SetCatchNo(1);
    catchBarrier.SetShape(CHitCatchData::SHAPE_SPHERE);
    catchBarrier.SetSphere(&hitSphere);

    CHitAttackManager::RegistCatch(&catchBarrier);
};


void C087Slashuur::TeleportDrainProc(void)
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    Compositor().GetPosition(&vecMyPos);

    RwV3d vecScytheBladePos = Math::VECTOR3_ZERO;
    Compositor().GetBonePosition(&vecScytheBladePos, 7, &SCYTHE_OFFSET);
    Math::Vec3_Add(&vecScytheBladePos, &vecScytheBladePos, &vecMyPos);

    for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
    {
        /* if player is entered to drain area */
        if (m_state.bDrain && !m_state.abPlayerDrained[i] && m_state.abPlayerSleeping[i])
        {
            m_state.abPlayerDrained[i] = true;

            CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);

            RwV3d vecPlrBodyPos = Math::VECTOR3_ZERO;
            pPlrChr->GetBodyPosition(&vecPlrBodyPos);

            CMagicManager::CParameter param;
            param.SetPositon(&vecPlrBodyPos);
            param.SetTarget(new C087Slashuur::CScytheTracer(this));

            m_state.ahMagic[i] = CMagicManager::Play(MAGICID::ID_HPSTEAL, &param);
            ASSERT(m_state.ahMagic[i] != 0);
            if (m_state.ahMagic[i])
            {
                float fSpeed = 1.0f;
                if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                    CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
                    fSpeed = 2.0f;

                CMagicManager::SetSpeed(m_state.ahMagic[i], fSpeed);
            };

            CGameObjectManager::SendMessage(pPlrChr,
                                            CHARACTERTYPES::MESSAGEID_RECVDMG,
                                            reinterpret_cast<void*>(HPSTEAL_AMOUNT));
        };

        /* if hpsteal active - check if reached scythe blade */
        if (m_state.ahMagic[i])
        {
            RwV3d vecMagicPos = Math::VECTOR3_ZERO;
            CMagicManager::GetPosition(m_state.ahMagic[i], &vecMagicPos);

            RwV3d vecDist = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecDist, &vecMagicPos, &vecScytheBladePos);

            float fDistSQ = Math::Vec3_Dot(&vecDist, &vecDist);
            if (fDistSQ < 0.01f)
            {
                CMagicManager::SetLive(m_state.ahMagic[i], 0.0f);

                m_state.ahMagic[i] = 0;
                m_state.afDrainTime[i] = 0.7f;

                CGameObjectManager::SendMessage(&Compositor(),
                                                CHARACTERTYPES::MESSAGEID_RECVDMG,
                                                reinterpret_cast<void*>(-HPSTEAL_AMOUNT));
            };
        };

        /* if hpsteal is reached scythe blade - play drain anim */
        if (m_state.afDrainTime[i] > 0.0f)
        {
            float fDrainTimePrev = m_state.afDrainTime[i];

            m_state.afDrainTime[i] -= CGameProperty::GetElapsedTime();
            if (m_state.afDrainTime[i] < 0.0f)
                m_state.afDrainTime[i] = 0.0f;

            if ((fDrainTimePrev > 0.5) && (m_state.afDrainTime[i] <= 0.5f))
            {
                RwV3d vecOfs = HPSTEAL_LIGHT_OFFSET;
                uint32 hEffect = CEffectManager::PlayTrace(EFFECTID::ID_HPSTEAL_LIGHT,
                                                           new C087Slashuur::CBodyTracer(this),
                                                           &vecOfs);
                CEffectManager::SetScale(hEffect, 1.5f);
            };
        };
    };
};


void C087Slashuur::InitState(void)
{
    std::memset(&m_state, 0, sizeof(m_state));

    RwV3d vecPos = Math::VECTOR3_ZERO;
    vecPos.y = CWorldMap::GetMapHeight(&vecPos) + 1.0f;

    RwV3d vecPtStart = vecPos;
    RwV3d vecPtEnd = vecPtStart;
    vecPtEnd.z += 20.0f;

    bool bResult = CWorldMap::CheckCollisionLine(&vecPtStart, &vecPtEnd);
    ASSERT(bResult);
    if (bResult)
    {
        const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
        ASSERT(pCollisionResult != nullptr);

        m_state.fMapMaxZ = pCollisionResult->m_vClosestPt.z;
        if (m_state.fMapMaxZ > 12.0f)
            m_state.fMapMaxZ = 12.0f;
    };

    SetSharedData(&m_state);
};