#include "BaseEliteFootChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseEliteFootAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Module/BarrierModule.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace MOTIONNAMES
{
    static const char* ATTACK_SWING_1 = "B1";
    static const char* ATTACK_SWING_2 = "B2";
    static const char* ATTACK_SWING_3 = "B3";
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CAppearStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::TOJO, true);
};


/*virtual*/ CBaseEliteFootChr::CAppearStatusObserver::RESULT
CBaseEliteFootChr::CAppearStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CEliteDeathStatusObserver::OnStart(void) /*override*/
{
    CDeathStatusObserver::OnStart();

    BUNSHIN_INFO* pBunshinInfo = static_cast<BUNSHIN_INFO*>(EnemyChr().SharedData());
    ASSERT(pBunshinInfo != nullptr);

    if (pBunshinInfo)
    {
        for (int32 i = 0; i < BUNSHIN_INFO::MAX_COUNT; ++i)
        {
            CGameObject* pGameObj = CGameObjectManager::GetObject(pBunshinInfo->ahOwnerObj[i]);
            if(!pGameObj)
                continue;

            ASSERT(pGameObj->GetType() == GAMEOBJECTTYPE::ENEMY);
            CEnemy* pEnemy = static_cast<CEnemy*>(pGameObj);

            CEnemyCharacter* pEnemyChr = &pEnemy->Character();
            if (pEnemyChr->GetStatus() != ENEMYTYPES::STATUS_DEATH)
            {
                pEnemyChr->SetStatus(ENEMYTYPES::STATUS_FLYAWAY_BACK);
                pEnemyChr->SetFlag(ENEMYTYPES::FLAG_DEATH_STATUS);
            };
        };
    };
};


//
// *********************************************************************************
//


/*virtual*/ CBaseEliteFootChr::CEliteDummyDeathStatusObserver::RESULT
CBaseEliteFootChr::CEliteDummyDeathStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CEliteDummyDeathStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_NORMAL, &vecPos);
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ CBaseEliteFootChr::CThinkingStatusObserver::RESULT
CBaseEliteFootChr::CThinkingStatusObserver::Observing(void) /*override*/
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
                m_nextStatus = ENEMYTYPES::STATUS_DASH;
                break;

            case ELITEAI::ORDERTYPE_MOVE_JUMP:
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
                break;

            case ELITEFOOTAI::ORDERTYPE_MOVE_WARP_POS:
            case ELITEFOOTAI::ORDERTYPE_MOVE_WARP_NO:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_RUN;
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
            case ELITEAI::ORDERTYPE_ATTACK_HIGH:
            case ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN:
            case ELITEFOOTAI::ORDERTYPE_ATTACK_SPIN:
            case ELITEFOOTAI::ORDERTYPE_ATTACK_DROP:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case ELITEFOOTAI::ORDERTYPE_ATTACK_SWING:                
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_AA;
                break;

            case ELITEFOOTAI::ORDERTYPE_ATTACK_SPEAR:
                m_nextStatus = ENEMYTYPES::STATUS_FIRE_A;
                break;

            case ELITEFOOTAI::ORDERTYPE_ATTACK_BUNSHIN:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseEliteFootChr::STATUS_BUNSHIN);
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

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CDashStatusObserver::OnStart(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().OrderMove().m_iMoveType == ELITEAI::ORDERTYPE_MOVE_DASH);

    m_fMoveSpeed = EnemyChr().Feature().m_fDashMoveSpeed;
    m_fMoveTime = EnemyChr().Feature().m_fDashMoveTime;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::RUN, true);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().Compositor().SetMotionSpeed(2.0f);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH2,
                              new CEnemyTracer(&EnemyChr()),
                              &Math::VECTOR3_ZERO,
                              EnemyChr().Compositor().GetDirection());
};


/*virtual*/ CBaseEliteFootChr::CDashStatusObserver::RESULT
CBaseEliteFootChr::CDashStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_MOVE)
        return RESULT_END;

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    m_fMoveTime -= CGameProperty::GetElapsedTime();
    if (m_fMoveTime < 0.0f)
        return RESULT_END;
    
    CEnemyUtilsElite::MoveTo(&EnemyChr().Compositor(),
                             &EnemyChr().AIThinkOrder().OrderMove().m_vAt,
                             m_fMoveSpeed,
                             EnemyChr().Feature().m_fRotateRate * 2.0f);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CDashStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().Compositor().SetMotionSpeed(1.0f);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseEliteFootChr::CEliteWarpStatusObserver::CEliteWarpStatusObserver(void)
: CWarpBaseStatusObserver(WARPTYPE_LEFT)
{
    ;
};


/*virtual*/ void CBaseEliteFootChr::CEliteWarpStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::RUN, true);

    SetPlayerNo(EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo);
    if (EnemyChr().AIThinkOrder().OrderMove().m_iMoveType == ELITEFOOTAI::ORDERTYPE_MOVE_WARP_POS)
        SetTargetPos(&EnemyChr().AIThinkOrder().OrderMove().m_vAt);

    EnemyChr().Compositor().SetEnableBodyHit(false);

    CWarpBaseStatusObserver::OnStart();

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4373));

    CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH2,
                              new CEnemyTracer(&EnemyChr()),
                              &Math::VECTOR3_ZERO,
                              EnemyChr().Compositor().GetDirection());
};


/*virtual*/ CBaseEliteFootChr::CEliteWarpStatusObserver::RESULT
CBaseEliteFootChr::CEliteWarpStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_MOVE);

    return CWarpBaseStatusObserver::Observing();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CEliteWarpStatusObserver::OnEnd(void) /*override*/
{
    CWarpBaseStatusObserver::OnEnd();

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().Compositor().SetEnableBodyHit(true);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CAttackStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_THINKING;
    m_attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;

    switch (m_attackType)
    {
    case ELITEAI::ORDERTYPE_ATTACK_NORMAL:
    case ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN:
    case ELITEFOOTAI::ORDERTYPE_ATTACK_SPIN:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_A, true);
        }        
        break;

    case ELITEAI::ORDERTYPE_ATTACK_HIGH:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_B, true);
            EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
        }
        break;

    case ELITEFOOTAI::ORDERTYPE_ATTACK_DROP:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_C, true);
            EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
            if (EnemyChr().IsAttachedStatusObserver(static_cast<ENEMYTYPES::STATUS>(CBaseEliteFootChr::STATUS_BUNSHIN)))
                EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ CBaseEliteFootChr::CAttackStatusObserver::RESULT
CBaseEliteFootChr::CAttackStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4375));

    float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
    if (fMotionNowT > 0.3f)
        EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);

    if ((m_attackType == ELITEAI::ORDERTYPE_ATTACK_NORMAL) ||
        (m_attackType == ELITEAI::ORDERTYPE_ATTACK_HIGH)   ||
        (m_attackType == ELITEFOOTAI::ORDERTYPE_ATTACK_DROP))
    {
        if (EnemyChr().Compositor().IsMotionEnd())
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            return RESULT_END;
        };
    }
    else
    {
        ASSERT((m_attackType == ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN) ||
               (m_attackType == ELITEFOOTAI::ORDERTYPE_ATTACK_SPIN));

        float fMotionConnectT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
        if (fMotionConnectT <= fMotionNowT)
        {
            m_nextStatus = ENEMYTYPES::STATUS_ATTACK_AA;
            return RESULT_END;
        };
    };

    if ((m_attackType != ELITEAI::ORDERTYPE_ATTACK_HIGH) &&
        (m_attackType != ELITEFOOTAI::ORDERTYPE_ATTACK_DROP))
    {
        int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTargetNo);
        if (pPlrChr)
        {
            RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
            pPlrChr->GetFootPosition(&vecPlrFootPos);

            CEnemyUtilsElite::RotateToTarget(&EnemyChr().Compositor(),
                                             &vecPlrFootPos,
                                             EnemyChr().Feature().m_fRotateRate);
        };
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CAttackStatusObserver::OnEnd(void) /*override*/
{
    if (m_attackType == ELITEAI::ORDERTYPE_ATTACK_HIGH)
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        EnemyChr().Compositor().SetDirection(fDir + MATH_PI);
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
    };

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CSpinAttackStatusObserver::OnStart(void) /*override*/
{
    m_attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_nextStatus = ENEMYTYPES::STATUS_THINKING;
    m_fSpinTimer = 0.0f;
    m_step = 0;
    m_fMoveSpeed = EnemyChr().Feature().m_fRunMoveSpeed * 1.2f;

    m_apszSpinMotion[0] = ENEMYTYPES::MOTIONNAMES::ATTACK_AA;
    m_apszSpinMotion[1] = ENEMYTYPES::MOTIONNAMES::ATTACK_AAA;

    switch (m_attackType)
    {
    case ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN:
        m_apszSpinMotion[2] = ENEMYTYPES::MOTIONNAMES::ATTACK_AAB;
		m_fSpinTimer = 2.0f;
        break;

    case ELITEFOOTAI::ORDERTYPE_ATTACK_SPIN:
        m_apszSpinMotion[2] = ENEMYTYPES::MOTIONNAMES::ATTACK_AAC;
		m_fSpinTimer = 1.0f;
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().ChangeMotion(m_apszSpinMotion[m_step], true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseEliteFootChr::CSpinAttackStatusObserver::RESULT
CBaseEliteFootChr::CSpinAttackStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                break;
            };

            ++m_step;
            EnemyChr().Compositor().ChangeMotion(m_apszSpinMotion[m_step], true);
        }
        break;

    case 1:
        {
            m_fSpinTimer -= CGameProperty::GetElapsedTime();
            CEnemyUtilsElite::MovePursuit(&EnemyChr().Compositor(), m_orderTargetNo, m_fMoveSpeed);

            if (m_fSpinTimer > 0.0f)
                break;

            ++m_step;
            EnemyChr().Compositor().ChangeMotion(m_apszSpinMotion[m_step], true);
        }
        break;

    case 2:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                break;
            };

            if (m_attackType == ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN)
            {
                m_nextStatus = ENEMYTYPES::STATUS_GETUP;
            }
            else
            {
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            };

            float fDir = EnemyChr().Compositor().GetDirection();
            EnemyChr().Compositor().SetDirection(fDir + MATH_PI);

            return RESULT_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CSpinAttackStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CSwingAttackStatusObserver::OnStart(void) /*override*/
{
    m_apszSwingMotion[0] = MOTIONNAMES::ATTACK_SWING_1;
    m_apszSwingMotion[1] = MOTIONNAMES::ATTACK_SWING_2;
    m_apszSwingMotion[2] = MOTIONNAMES::ATTACK_SWING_3;
    m_step = 0;
    m_fSwingTimer = 1.0f;
    m_fMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed * 0.8f;
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;

    EnemyChr().Compositor().ChangeMotion(m_apszSwingMotion[m_step], true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseEliteFootChr::CSwingAttackStatusObserver::RESULT
CBaseEliteFootChr::CSwingAttackStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                break;
            };

            ++m_step;
            EnemyChr().Compositor().ChangeMotion(m_apszSwingMotion[m_step], true);
        }
        break;

    case 1:
        {
            m_fSwingTimer -= CGameProperty::GetElapsedTime();
            CEnemyUtilsElite::MovePursuit(&EnemyChr().Compositor(), m_orderTargetNo, m_fMoveSpeed);

            if (m_fSwingTimer > 0.0f)
                break;

            ++m_step;
            EnemyChr().Compositor().ChangeMotion(m_apszSwingMotion[m_step], true);
        }
        break;

    case 2:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                break;
            };

            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

            return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CSwingAttackStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseEliteFootChr::CBunshinStatusObserver::CBunshinStatusObserver(void)
: m_bunshinInfo()
, m_bunshinType(-1)
, m_fBunshinTime(0.0f)
, m_execNum(0)
, m_vecDummyPos(Math::VECTOR3_ZERO)
, m_fPosBias(0.0f)
, m_fDirSign(0.0f)
, m_shakeNum(0)
, m_dummyId(ENEMYID::ID_DUMMY)
{
	;
};


/*virtual*/ void CBaseEliteFootChr::CBunshinStatusObserver::OnStart(void) /*override*/
{
    if (m_execNum == 0)
    {
        m_bunshinType = 0;
        std::memset(&m_bunshinInfo, 0, sizeof(m_bunshinInfo));
        EnemyChr().SetSharedData(&m_bunshinInfo);
    };

    if (m_execNum < EXEC_MAX)
    {
        ++m_bunshinType;
        m_fBunshinTime = 3.0f;
        m_fPosBias = 0.0f;
        m_shakeNum = 0;
        m_fDirSign = 1.0f;

        EnemyChr().Compositor().GetFootPosition(&m_vecDummyPos);
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
        EnemyChr().Compositor().SetEnableBodyHit(false);

        EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4371));
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8449));
    };
};


/*virtual*/ CBaseEliteFootChr::CBunshinStatusObserver::RESULT
CBaseEliteFootChr::CBunshinStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    if (m_execNum >= EXEC_MAX)
        return RESULT_END;
    
    if (m_shakeNum++ % m_bunshinType)
        EnemyChr().Compositor().SetPosition(&m_vecDummyPos);
    else
        m_fDirSign = -m_fDirSign;
    
    m_fPosBias += 0.5f;
    RwV3d vecVelocity = { m_fPosBias * m_fDirSign, 0.0f, 0.0f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    m_fBunshinTime -= CGameProperty::GetElapsedTime();
    if (m_fBunshinTime < 0.0f)
        return RESULT_END;
    
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CBunshinStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    if (m_execNum < EXEC_MAX)
    {
        EnemyChr().Compositor().SetPosition(&m_vecDummyPos);
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
        EnemyChr().Compositor().SetEnableBodyHit(true);

        int32 numDummyCreated = 0;
        for (int32 i = 0; i < BUNSHIN_INFO::MAX_COUNT; ++i)
        {
            if (CGameObjectManager::GetObject(m_bunshinInfo.ahOwnerObj[i]))
                continue;

            /* create elite dummy */
            CBaseEliteFootChr* pBaseEliteFootDummyChr = CreateEliteDummy();
            ASSERT(pBaseEliteFootDummyChr != nullptr);

            /* update elite dummy AI info */
            if (pBaseEliteFootDummyChr->IsAIModeratorExist())
            {
                int32 eliteDummyNo = m_execNum + numDummyCreated + 1;
                pBaseEliteFootDummyChr->AIModerator().AIParam().SetEliteDummyNo(eliteDummyNo);

                uint32 eliteDummyHandle = pBaseEliteFootDummyChr->Compositor().GetHandle();
                pBaseEliteFootDummyChr->AIModerator().AIParam().SetEliteDummyHandle(eliteDummyHandle);
            };

            /* update elite dummy velocity */
            RwV3d vecVelocity = { (i == 2 ? -3.0f : 3.0f), 0.0f, 0.0f };
            EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
            pBaseEliteFootDummyChr->Compositor().SetVelocity(&vecVelocity);

            /* update elite dummy position */
            RwV3d vecOffset = { (i == 2 ? -0.1f : 0.1f), 0.0f, 0.0f };
            EnemyChr().Compositor().RotateVectorByDirection(&vecOffset, &vecOffset);

            RwV3d vecPos = Math::VECTOR3_ZERO;
            Math::Vec3_Add(&vecPos, &m_vecDummyPos, &vecOffset);
            pBaseEliteFootDummyChr->Compositor().SetPosition(&vecPos);

            /* update bunshin info (CEnemy handle) */
            m_bunshinInfo.ahOwnerObj[i] = pBaseEliteFootDummyChr->GetOwner();

            if (++numDummyCreated == m_bunshinType)
                break;
        };

        ++m_execNum;
    };
    
    return ENEMYTYPES::STATUS_THINKING;
};


CBaseEliteFootChr* CBaseEliteFootChr::CBunshinStatusObserver::CreateEliteDummy(void)
{
    ENEMYID::VALUE dummyId = ENEMYID::ID_DUMMY;

    /* create and start elite dummy enemy */
    ENEMYID::VALUE enemyId = EnemyChr().GetID();
    switch (enemyId)
    {
    case ENEMYID::ID_ELITE_FOOT_A: dummyId = ENEMYID::ID_ELITE_FOOT_DUMMY_A; break;
    case ENEMYID::ID_ELITE_FOOT_B: dummyId = ENEMYID::ID_ELITE_FOOT_DUMMY_B; break;
    case ENEMYID::ID_ELITE_FOOT_C: dummyId = ENEMYID::ID_ELITE_FOOT_DUMMY_C; break;
    case ENEMYID::ID_ELITE_FOOT_D: dummyId = ENEMYID::ID_ELITE_FOOT_DUMMY_D; break;
    default: ASSERT(false); break;
    };

    ENEMYTYPES::CREATEINFO createInfo;
    std::memset(&createInfo, 0, sizeof(createInfo));
    createInfo.m_idEnemy    = dummyId;
    createInfo.m_vPosition  = m_vecDummyPos;
    createInfo.m_fDirection = 0.0f;
    createInfo.m_status     = ENEMYTYPES::STATUS_HIDE;
    createInfo.m_iPattern   = 0;    
    createInfo.m_fRadius    = 100.0f;
#ifdef _DEBUG
    createInfo.m_iHPMax     = 0;
#endif /* _DEBUG */   

    CEnemy* pEnemy = CEnemy::New(&createInfo);
    ASSERT(pEnemy != nullptr);

    pEnemy->Start();

    /* set elite dummy model color & pos & dir */
    CEnemyCharacter& dummyEnemyChr = pEnemy->Character();

    RwRGBA materialColor = dummyEnemyChr.Compositor().GetModel()->GetColor();
    materialColor.red   = static_cast<RwUInt8>(static_cast<float>(materialColor.red)   * 0.3f);
    materialColor.green = static_cast<RwUInt8>(static_cast<float>(materialColor.green) * 0.3f);
    materialColor.blue  = static_cast<RwUInt8>(static_cast<float>(materialColor.blue)  * 0.3f);
    dummyEnemyChr.Compositor().GetModel()->SetColor(materialColor);

    CEnemyUtils::SetAmbientLightEnable(&dummyEnemyChr.Compositor(), false);

    dummyEnemyChr.Compositor().SetDirection(EnemyChr().Compositor().GetDirection());
    dummyEnemyChr.Compositor().SetPosition(&m_vecDummyPos);
    dummyEnemyChr.Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);

    return static_cast<CBaseEliteFootChr*>(&dummyEnemyChr);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootChr::CGetUpStatusObserver::OnStart(void) /*override*/
{
    m_fLieDowntime = 1.0f;
    m_step = 0;
    
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ CBaseEliteFootChr::CGetUpStatusObserver::RESULT
CBaseEliteFootChr::CGetUpStatusObserver::Observing(void) /*override*/
{
    m_fLieDowntime -= CGameProperty::GetElapsedTime();

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_GETUP, true);
            ++m_step;
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


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteFootChr::CGetUpStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    if (EnemyChr().StatusSubject().StatusPrev() == ENEMYTYPES::STATUS_ATTACK_AA)
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    else
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseEliteFootChr::CBaseEliteFootChr(ENEMYID::VALUE enemyId)
: CBaseEliteChr(enemyId)
{
    /* CBaseEliteChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new CBaseEliteChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CBaseEliteChr::CIdleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                       new CBaseEliteChr::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new CBaseEliteChr::CGuardStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CBaseEliteChr::CJumpReadyStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CBaseEliteChr::CKnockStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CBaseEliteChr::CKnockStatusObserver);

    /* CBaseEliteFootChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new CBaseEliteFootChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                     new CBaseEliteFootChr::CAppearStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,                   new CBaseEliteFootChr::CAttackStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CBaseEliteFootChr::CGetUpStatusObserver);

    if (!IsEliteDummy(enemyId))
    {
        AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseEliteFootChr::CEliteDeathStatusObserver);
        AttachStatusObserver(ENEMYTYPES::STATUS_DASH,                   new CBaseEliteFootChr::CDashStatusObserver);
        AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN,             new CBaseEliteFootChr::CEliteWarpStatusObserver);
        AttachStatusObserver(CBaseEliteFootChr::STATUS_BUNSHIN,         new CBaseEliteFootChr::CBunshinStatusObserver);
    }
    else
    {
        AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseEliteFootChr::CEliteDummyDeathStatusObserver);
    };

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new CCommonEnemyObserver::CTouchdownStatus);
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
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,                 new CCommonEnemyObserver::CCounteractStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* "disable" knockback control for elite guard dummies */
    if (IsEliteDummy(enemyId))
        m_knockBackCtrl.Initialize(this, 2000, 0.0f);
};


/*virtual*/ CBaseEliteFootChr::~CBaseEliteFootChr(void)
{
    ;
};


/*virtual*/ bool CBaseEliteFootChr::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();
    if ((statusNow == ENEMYTYPES::STATUS_ATTACK_RUN) ||
        (statusNow == STATUS_BUNSHIN))
        return false;

    return CBaseEliteChr::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
CBaseEliteFootChr::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    if (IsEliteDummy(GetID()))
        return CBaseEliteChr::OnDamage(rCalc);

    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceStatusFlag = CheckDefenceStatusFlag();
    CHARACTERTYPES::ATTACKRESULTTYPE attackResult = rCalc.CalcAtackParameter(Compositor().AttackParameter(),
                                                                             defenceStatusFlag);

    CHitAttackData* pAttack = &rCalc.GetAttack();
    AIModerator().AttackCountDamage(pAttack->GetPower());

    if ((attackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK) ||
        (attackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY))
    {
        const char* pszAttackMotion = pAttack->GetMotion();
        if ((GetStatus() == ENEMYTYPES::STATUS_ATTACK_RUN) &&
            (!std::strcmp(pszAttackMotion, CBarrierModule::BARRIER_MOTIONNAME)))
        {
            ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
        };
    };

    uint32 hObj = CEnemyUtils::GetHandleFromHitObj(pAttack->GetObject());
    AIModerator().CatchAttack(hObj, pAttack->GetObject()->GetType());

	return CBaseEliteChr::OnDamage(rCalc);
};


CBaseEliteFootChr::ELITETYPE CBaseEliteFootChr::GetEliteType(void) const
{
    ENEMYID::VALUE enemyId = GetID();
    switch (enemyId)
    {
    case ENEMYID::ID_ELITE_FOOT_A:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_A:
        return ELITETYPE_A;
        
    case ENEMYID::ID_ELITE_FOOT_B:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_B:
        return ELITETYPE_B;

    case ENEMYID::ID_ELITE_FOOT_C:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_C:
        return ELITETYPE_C;
            
    case ENEMYID::ID_ELITE_FOOT_D:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_D:        
        return ELITETYPE_D;

    default:
        ASSERT(false);
        break;
    };

    return ELITETYPE_UNKNOWN;
};


bool CBaseEliteFootChr::IsEliteDummy(ENEMYID::VALUE enemyId) const
{
    return (enemyId == ENEMYID::ID_ELITE_FOOT_DUMMY_A)
        || (enemyId == ENEMYID::ID_ELITE_FOOT_DUMMY_B)
        || (enemyId == ENEMYID::ID_ELITE_FOOT_DUMMY_C)
        || (enemyId == ENEMYID::ID_ELITE_FOOT_DUMMY_D);
};