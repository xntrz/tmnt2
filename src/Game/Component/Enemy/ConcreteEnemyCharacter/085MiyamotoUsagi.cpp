#include "085MiyamotoUsagi.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Effect/ScreenEffectFlash.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/085MiyamotoUsagiAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/BarrierModule.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


namespace MOTIONNAMES
{
    static const char* ATTACK_AAA       = "AAA";
    static const char* ATTACK_AAAAA     = "AAAAA";
    static const char* ATTACK_BB        = "BB";
    static const char* ATTACK_KICK      = "Kick";
    static const char* ATTACK_RUN_1     = "DAttack1";
    static const char* ATTACK_RUN_2     = "DAttack2";
    static const char* ATTACK_RUN_3     = "DAttack3";
    static const char* BACK             = "Backaway";
    static const char* AVOID_ATTACK     = "JAttack";
    static const char* COUNTER_ATTACK   = "Counter";
    static const char* MUTEKI_1         = "GM1";
    static const char* MUTEKI_2         = "GM2";
    static const char* MUTEKI_3         = "GM3";
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ C085MiyamotoUsagi::CThinkingStatusObserver::RESULT
C085MiyamotoUsagi::CThinkingStatusObserver::Observing(void) /*override*/
{
    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_WAIT:
        m_nextStatus = ENEMYTYPES::STATUS_IDLE;
        break;

    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            switch (moveType)
            {
            case ELITEAI::ORDERTYPE_MOVE_WALK:
            case ELITEAI::ORDERTYPE_MOVE_RUN:
                m_nextStatus = ENEMYTYPES::STATUS_WALK;
                break;

            case ELITEAI::ORDERTYPE_MOVE_DASH:
            case ELITEAI::ORDERTYPE_MOVE_DASH_LEFT:
            case ELITEAI::ORDERTYPE_MOVE_DASH_RIGHT:
                m_nextStatus = ENEMYTYPES::STATUS_DASH;
                break;

            case ELITEAI::ORDERTYPE_MOVE_BACK:
                m_nextStatus = ENEMYTYPES::STATUS_WALK_BACK;
                break;

            case ELITEAI::ORDERTYPE_MOVE_JUMP:
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
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
            case ELITEAI::ORDERTYPE_ATTACK_NORMAL:
            case ELITEAI::ORDERTYPE_ATTACK_NORMALA:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case ELITEAI::ORDERTYPE_ATTACK_NORMALB:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case USAGIAI::ORDERTYPE_ATTACK_DASH:
            case USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_1:
            case USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_2_3:
            case USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_4:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_RUN;
                break;

            case USAGIAI::ORDERTYPE_ATTACK_KICK:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(C085MiyamotoUsagi::STATUS_ATTACK_KICK);
                break;
                            
            case USAGIAI::ORDERTYPE_ATTACK_AVOID:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case USAGIAI::ORDERTYPE_ATTACK_COUNTER:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(C085MiyamotoUsagi::STATUS_ATTACK_COUNTER);
                break;

            case USAGIAI::ORDERTYPE_ATTACK_MUTEKI:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(C085MiyamotoUsagi::STATUS_MUTEKI);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        m_nextStatus = ENEMYTYPES::STATUS_GUARD;
        break;

    default:
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        break;
    };

#ifdef _DEBUG    
    if (m_nextStatus == ENEMYTYPES::STATUS_THINKING)
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
#endif /* _DEBUG */

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS
C085MiyamotoUsagi::CThinkingStatusObserver::OnEnd(void) /*override*/
{
	return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CAttackAStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_AAA, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ C085MiyamotoUsagi::CAttackAStatusObserver::RESULT
C085MiyamotoUsagi::CAttackAStatusObserver::Observing(void) /*override*/
{
    bool bIsOrderChanged = (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_ATTACK);
    if (!bIsOrderChanged)
    {
        int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
        bIsOrderChanged |= (attackType != ELITEAI::ORDERTYPE_ATTACK_NORMAL) &&
                           (attackType != ELITEAI::ORDERTYPE_ATTACK_NORMALA);
    };

    if (bIsOrderChanged)
    {
        if (EnemyChr().Compositor().IsMotionEnd())
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
            return RESULT_END;
        };

        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
        return RESULT_CONTINUE;
    };

    switch (m_step)
    {
    case 0:
        {
            float dt = CGameProperty::GetElapsedTime();

            float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
            float fMotionNextT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
            if ((fMotionNowT + dt) >= fMotionNextT)
            {
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                ++m_step;
            };
        }
        break;

    case 1:
        {
            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_AAAAA, true);
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

            CGameSound::PlayAttackSE(&EnemyChr().Compositor());

            ++m_step;
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                return RESULT_END;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    CEnemyUtilsElite::RotateToPlayer(&EnemyChr().Compositor(),
                                     EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo,
                                     EnemyChr().Feature().m_fRotateRate);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C085MiyamotoUsagi::CAttackAStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CAttackBStatusObserver::OnStart(void) /*override*/
{
    m_bWindPress = false;
    m_bFlash = false;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_BB, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ C085MiyamotoUsagi::CAttackBStatusObserver::RESULT
C085MiyamotoUsagi::CAttackBStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    float fMotionNowT = EnemyChr().Compositor().GetMotionTime();

    if (!m_bFlash && (fMotionNowT >= 0.6f))
    {
        m_bFlash = true;
        PlayEffectFlash();
    };

    if (!m_bWindPress && (fMotionNowT >= 1.0f))
    {
        m_bWindPress = true;
        PlayMagicWindPress();
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C085MiyamotoUsagi::CAttackBStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


void C085MiyamotoUsagi::CAttackBStatusObserver::PlayEffectFlash(void)
{
    RwV3d vecOffset = { 0.0f, 0.7f, 0.0f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecOffset, &vecOffset);

    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

    Math::Vec3_Add(&vecBodyPos, &vecBodyPos, &vecOffset);

    CEffectManager::Play(EFFECTID::ID_BLADE_FLASh, &vecBodyPos);
};


void C085MiyamotoUsagi::CAttackBStatusObserver::PlayMagicWindPress(void)
{
    RwV3d vecOffset = { 0.0f, 0.0f, 1.5f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecOffset, &vecOffset);

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    Math::Vec3_Add(&vecFootPos, &vecFootPos, &vecOffset);

    CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_WINDPRESS), &vecFootPos);
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CKickAttackStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_KICK, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
};


/*virtual*/ C085MiyamotoUsagi::CKickAttackStatusObserver::RESULT
C085MiyamotoUsagi::CKickAttackStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C085MiyamotoUsagi::CKickAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CRunAttackStatusObserver::OnStart(void) /*override*/
{
    m_vecOrderPos = EnemyChr().AIThinkOrder().OrderAttack().m_vAt;
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_fMoveSpeed = EnemyChr().Feature().m_fDashMoveSpeed * 4.0f;
    m_fMoveTime = 0.0f;
    m_step = 0;

    if (EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType != USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_1)
        EnemyChr().Compositor().SetMotionCtrlTime(2.5f);

    GetPredictPlayerPos(m_orderTargetNo, &m_vecOrderPos);

    CEnemyUtilsElite::RotateToTarget(&EnemyChr().Compositor(), &m_vecOrderPos);

    float fDistance = CEnemyUtils::GetDistance(&EnemyChr().Compositor(), &m_vecOrderPos);
    m_fMoveTime = (fDistance / m_fMoveSpeed);

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_RUN_1, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetEnableBodyHit(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8459));
};


/*virtual*/ C085MiyamotoUsagi::CRunAttackStatusObserver::RESULT
C085MiyamotoUsagi::CRunAttackStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ClearMotionCtrlTime();
            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_RUN_2, true);

            RwV3d vecVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
            EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
            EnemyChr().Compositor().SetVelocity(&vecVelocity);

            ++m_step;
        }
        break;

    case 1:
        {
            m_fMoveTime -= CGameProperty::GetElapsedTime();
            if (m_fMoveTime >= 0.0f)
                break;

            if (EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType != USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_4)
                EnemyChr().Compositor().SetMotionCtrlTime(2.5f);

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_RUN_3, true);
            EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
            EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

            ++m_step;
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (CEnemyUtilsElite::IsCrashWallPossible(&EnemyChr().Compositor()))
    {
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C085MiyamotoUsagi::CRunAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);

    EnemyChr().Compositor().SetEnableBodyHit(true);
    EnemyChr().Compositor().ClearMotionCtrlTime();

    return ENEMYTYPES::STATUS_THINKING;
};


void C085MiyamotoUsagi::CRunAttackStatusObserver::GetPredictPlayerPos(int32 playerNo,
                                                                      RwV3d* pvecPredictPos) const
{
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(playerNo);
    if (!pPlrChr)
        return;

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecFootPos);

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    pPlrChr->GetVelocity(&vecVelocity);

    RwV3d vecVelocityPerFrame = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecVelocityPerFrame, &vecVelocity, CGameProperty::GetElapsedTime());
    
    Math::Vec3_Add(&vecFootPos, &vecFootPos, &vecVelocityPerFrame);
    *pvecPredictPos = vecFootPos;
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CBackRollStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::BACK, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    RwV3d vecVelocity = { 0.0f, 0.0f, (EnemyChr().Feature().m_fRunMoveSpeed * 1.2f) };
    float fDirection = (EnemyChr().Compositor().GetDirection() + MATH_PI);

    CEnemyUtils::RotateVectorByDirection(&vecVelocity, fDirection);

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ C085MiyamotoUsagi::CBackRollStatusObserver::RESULT
C085MiyamotoUsagi::CBackRollStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_MOVE);

    float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
    if (fMotionNowT >= 0.3f)
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C085MiyamotoUsagi::CBackRollStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CAvoidAttackStatusObserver::OnStart(void) /*override*/
{
    m_bEffectEntry = false;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::AVOID_ATTACK, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ C085MiyamotoUsagi::CAvoidAttackStatusObserver::RESULT
C085MiyamotoUsagi::CAvoidAttackStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    if (!m_bEffectEntry)
    {
        float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
        if (fMotionNowT > 0.7f)
        {
            CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
            m_bEffectEntry = true;
        };
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C085MiyamotoUsagi::CAvoidAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};



//
// *********************************************************************************
//


C085MiyamotoUsagi::CCounterAttackStatusObserver::CCounterAttackStatusObserver(void)
: CBaseEliteChr::CWarpBaseStatusObserver(WARPTYPE_RIGHT)
{
    ;
};


/*virtual*/ void C085MiyamotoUsagi::CCounterAttackStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::COUNTER_ATTACK, true);
    EnemyChr().Compositor().StopMotion();

    SetPlayerNo(EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo);
    CBaseEliteChr::CWarpBaseStatusObserver::OnStart();

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);

    CScreenEffectFlash::SetColor({ 255, 255, 255, 192 });
    CScreenEffectFlash::Flash();
};


/*virtual*/ C085MiyamotoUsagi::CCounterAttackStatusObserver::RESULT
C085MiyamotoUsagi::CCounterAttackStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    switch (m_step)
    {
    case 0:
        {
            RESULT result = CBaseEliteChr::CWarpBaseStatusObserver::Observing();
            if (result == RESULT_END)
            {
                CBaseEliteChr::CWarpBaseStatusObserver::OnEnd();
                ++m_step;
            };

            float fElapsedRatio = (m_fTime / m_fEndTime);
            if (fElapsedRatio >= 1.0f)
                EnemyChr().Compositor().PlayMotion();
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


/*virtual*/ ENEMYTYPES::STATUS
C085MiyamotoUsagi::CCounterAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    CMessageManager::Request(SEGROUPID::VALUE(152));

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagi::CMutekiGuardStatusObserver::OnStart(void) /*override*/
{
    m_apszMutekiMotion[0] = MOTIONNAMES::MUTEKI_2;
    m_apszMutekiMotion[1] = MOTIONNAMES::MUTEKI_3;
    m_step = 0;
    m_fMutekiTime = 2.0f;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::MUTEKI_1, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
};


/*virtual*/ C085MiyamotoUsagi::CMutekiGuardStatusObserver::RESULT
C085MiyamotoUsagi::CMutekiGuardStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_ATTACK)
        return RESULT_END;

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ChangeMotion(m_apszMutekiMotion[m_step], true);
            ++m_step;
        }
        break;

    case 1:
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            
            m_fMutekiTime -= CGameProperty::GetElapsedTime();
            if (m_fMutekiTime > 0.0f)
            {
                if ((EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_ATTACK) ||
                    (EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType != USAGIAI::ORDERTYPE_ATTACK_MUTEKI))
                {
                    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
                    return RESULT_END;
                };
            }
            else
            {
                EnemyChr().Compositor().ChangeMotion(m_apszMutekiMotion[m_step], true);
                ++m_step;
            };
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
    
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C085MiyamotoUsagi::CMutekiGuardStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ C085MiyamotoUsagi::CDeathStatusObserver::RESULT
C085MiyamotoUsagi::CDeathStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


C085MiyamotoUsagi::C085MiyamotoUsagi(void)
: CBaseEliteChr(ENEMYID::ID_MIYAMOTO_USAGI)
{
    /* CBaseEliteChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                 new CBaseEliteChr::CAppearStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseEliteChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CBaseEliteChr::CIdleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CBaseEliteChr::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new CBaseEliteChr::CGuardStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseEliteChr::CJumpReadyStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CBaseEliteChr::CKnockStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CBaseEliteChr::CKnockStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DASH,                   new CBaseEliteChr::CDashStatusObserver);

    /* C085MiyamotoUsagi status */
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new C085MiyamotoUsagi::CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new C085MiyamotoUsagi::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK_BACK,              new C085MiyamotoUsagi::CBackRollStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new C085MiyamotoUsagi::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new C085MiyamotoUsagi::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new C085MiyamotoUsagi::CAvoidAttackStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN,             new C085MiyamotoUsagi::CRunAttackStatusObserver);
    AttachStatusObserver(C085MiyamotoUsagi::STATUS_ATTACK_KICK,     new C085MiyamotoUsagi::CKickAttackStatusObserver);
    AttachStatusObserver(C085MiyamotoUsagi::STATUS_ATTACK_COUNTER,  new C085MiyamotoUsagi::CCounterAttackStatusObserver);
    AttachStatusObserver(C085MiyamotoUsagi::STATUS_MUTEKI,          new C085MiyamotoUsagi::CMutekiGuardStatusObserver);

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C085MiyamotoUsagiAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.16f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 16.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.2f;
    parameter.m_feature.m_fRotateRate           = 2.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 5.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 640;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.7f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 15.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 40.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 3.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /****************************************/
        /*          CBaseEliteAI freq           */
        /****************************************/
        /*  FREQUENCY_GUARD                 0   */  {  10,  40,  60 },
        /*  FREQUENCY_ATTACK_HIGH           1   */  {  20,  60,  80 },
        /*  FREQUENCY_ATTACK_AVOID          2   */  {  30,  50,  80 },

        /****************************************/  
        /*          CBaseSplinterAI freq        */  
        /****************************************/  
        /*  FREQUENCY_ATTACK_NORMAL         3   */  {  10,  30,  50 },
        /*  FREQUENCY_ATTACK_NORMALA        4   */  {  30,  40,  50 },
        /*  FREQUENCY_ATTACK_NORMALB        5   */  {   5,  10,  20 },
        /*  FREQUENCY_ATTACK_KICK           6   */  {  40,  50,  60 },
        /*  FREQUENCY_ATTACK_RUN_SIDE       7   */  {  30,  40,  50 },
        /*  FREQUENCY_MOVE_CONTINUE         8   */  {  40,  50,  60 },
        /*  FREQUENCY_ATTACK_ALL_PLR        9   */  { 100, 100, 100 },
        /*  FREQUENCY_ATTACK_STAB          10   */  {  30,  30,  30 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C085MiyamotoUsagi::~C085MiyamotoUsagi(void)
{
    ;
};


/*virtual*/ bool C085MiyamotoUsagi::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();

    if ((statusNow == C085MiyamotoUsagi::STATUS_ATTACK_COUNTER) ||
        (statusNow == ENEMYTYPES::STATUS_ATTACK_RUN))
    {
        return false;
    };

    return CBaseEliteChr::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void C085MiyamotoUsagi::OnMessageTouchdown(float fHeight) /*override*/
{
    if (TestFlag(ENEMYTYPES::FLAG_AERIAL_STATUS) && (GetStatus() == ENEMYTYPES::STATUS_ATTACK_C))
        return;
    
    CBaseEliteChr::OnMessageTouchdown(fHeight);
};


/*virtual*/ void C085MiyamotoUsagi::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CHitCatchData::RESULT catchResult = pCatch->GetResult();
    if (catchResult != CHitCatchData::RESULT_INVALID)
    {
        bool bIsAttackGuarded = (catchResult == CHitCatchData::RESULT_GUARD) ||
                                (catchResult == CHitCatchData::RESULT_GUARDIMPACT);
        if (bIsAttackGuarded)
            AIModerator().AIParam().SetAttackResult(ELITEAI::ATTACK_RESULT_GUARD);    
		
        CBaseEliteChr::OnMessageAttackResult(pCatch);
        return;
    };

    if (GetStatus() == C085MiyamotoUsagi::STATUS_MUTEKI)
    {
        uint32 hObj = CEnemyUtils::GetHandleFromHitObj(pCatch->GetObject());
        AIModerator().CatchAttack(hObj, pCatch->GetObject()->GetType());

        if (pCatch->GetObject()->GetType() == GAMEOBJECTTYPE::SHOT)
        {
            AIModerator().AttackCountInc();

            CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4414));
            CMessageManager::Request(SEGROUPID::VALUE(151));
            CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(8463));
        };

        CBaseEliteChr::OnMessageAttackResult(pCatch);
        return;
    };

    AIModerator().AIParam().SetAttackResult(ELITEAI::ATTACK_RESULT_MISS);
    CBaseEliteChr::OnMessageAttackResult(pCatch);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C085MiyamotoUsagi::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceStatusFlag = CheckDefenceStatusFlag();
    CHARACTERTYPES::ATTACKRESULTTYPE attackResult = rCalc.CalcAtackParameter(Compositor().AttackParameter(),
                                                                             defenceStatusFlag);

    CHitAttackData* pAttack = &rCalc.GetAttack();

    uint32 hObj = CEnemyUtils::GetHandleFromHitObj(pAttack->GetObject());
    AIModerator().CatchAttack(hObj, pAttack->GetObject()->GetType());

    if ((GetStatus() != ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK) &&
        (GetStatus() != ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT))
    {
        if ((pAttack->GetObject()->GetType() == GAMEOBJECTTYPE::EFFECT) ||
            (pAttack->GetObject()->GetType() == GAMEOBJECTTYPE::SHOT))
            return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
    };

    if ((attackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK) ||
        (attackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY))
    {
        CheckAttackDamage(pAttack);
        CheckAttackBarrier(pAttack);        
    };

    return CBaseEliteChr::OnDamage(rCalc);
};


void C085MiyamotoUsagi::CheckAttackBarrier(const CHitAttackData* pAttack)
{
    bool bIsRunAttackNow = (GetStatus() == ENEMYTYPES::STATUS_ATTACK_RUN);

    bool bIsBarrierAttack = (!std::strcmp(pAttack->GetMotion(), CBarrierModule::BARRIER_MOTIONNAME));

    if (bIsRunAttackNow && bIsBarrierAttack)
        ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


void C085MiyamotoUsagi::CheckAttackDamage(const CHitAttackData* pAttack)
{
    const char* pszAttackMotion = pAttack->GetMotion();
    bool bIsMoveAttack = !std::strcmp(pszAttackMotion, ENEMYTYPES::MOTIONNAMES::FLYAWAY_FRONT) ||
                         !std::strcmp(pszAttackMotion, ENEMYTYPES::MOTIONNAMES::FLYAWAY_BACK) ||
                         !std::strcmp(pszAttackMotion, ENEMYTYPES::MOTIONNAMES::DASH1) ||
                         !std::strcmp(pszAttackMotion, ENEMYTYPES::MOTIONNAMES::DASH2);

    CGameObject* pAttacker = pAttack->GetObject();
    bool bIsChrAttacker = (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER);

    bool bIsCounterAttack = (GetStatus() == C085MiyamotoUsagi::STATUS_ATTACK_COUNTER);

    if (bIsChrAttacker && !bIsMoveAttack && !bIsCounterAttack)
        AIModerator().AttackCountDamage(pAttack->GetPower());
};