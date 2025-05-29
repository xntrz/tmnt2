#include "BaseFootMechSplinter.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseSplinterAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Module/BarrierModule.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


namespace MOTIONNAMES
{
    static const char* SHOT      = "Shuriken";
    static const char* SHOT_JUMP = "JShuriken";
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFootMechSplinter::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ CBaseFootMechSplinter::CThinkingStatusObserver::RESULT
CBaseFootMechSplinter::CThinkingStatusObserver::Observing(void) /*override*/
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
            case ELITEAI::ORDERTYPE_ATTACK_NORMALB:
            case ELITEAI::ORDERTYPE_ATTACK_NORMALC:
            case SPLINTERAI::ORDERTYPE_ATTACK_RUN:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case SPLINTERAI::ORDERTYPE_ATTACK_B:
            case SPLINTERAI::ORDERTYPE_ATTACK_B_CHARGE:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case ELITEAI::ORDERTYPE_ATTACK_HIGH:
            case SPLINTERAI::ORDERTYPE_ATTACK_KICK:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseFootMechSplinter::STATUS_ATTACK_KICK);
                break;

            case SPLINTERAI::ORDERTYPE_ATTACK_SHOT:
            case SPLINTERAI::ORDERTYPE_ATTACK_SHOT_JUMP:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseFootMechSplinter::STATUS_ATTACK_SHOT);
                break;

            case SPLINTERAI::ORDERTYPE_ATTACK_THROW:
                m_nextStatus = ENEMYTYPES::STATUS_THROW;
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
CBaseFootMechSplinter::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFootMechSplinter::CAttackAStatusObserver::OnStart(void) /*override*/
{
    m_attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
    m_step = STEP_A;

    const char* pszMotion = (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_RUN) ? ENEMYTYPES::MOTIONNAMES::ATTACK_RUN :
                                                                                 ENEMYTYPES::MOTIONNAMES::ATTACK_A;

    EnemyChr().Compositor().ChangeMotion(pszMotion, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    if (m_attackType == ELITEAI::ORDERTYPE_ATTACK_NORMALB)
    {
        FootMechSplinter().AccumulateBegin();

        EnemyChr().Compositor().SetMotionSpeed(0.8f);
        EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
        EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);

        CMessageManager::Request(SEGROUPID::VALUE(116));
    };

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ CBaseFootMechSplinter::CAttackAStatusObserver::RESULT
CBaseFootMechSplinter::CAttackAStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    switch (m_step)
    {
    case STEP_A:
        OnStepMotionA();
        break;

    case STEP_AA:
        OnStepMotionAA();
        break;

    case STEP_AAX:
        OnStepMotionAAX();
        break;

    case STEP_END:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        ASSERT(false, "unknown step %" PRId32, m_step);
        break;
    };

    CEnemyUtilsElite::RotateToPlayer(&EnemyChr().Compositor(),
                                     EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo,
                                     EnemyChr().Feature().m_fRotateRate);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
CBaseFootMechSplinter::CAttackAStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().Compositor().SetMotionSpeed(1.0f);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

    FootMechSplinter().AccumulateEnd();

    return ENEMYTYPES::STATUS_THINKING;
};


void CBaseFootMechSplinter::CAttackAStatusObserver::OnStepMotionA(void)
{
    if (!IsMotionChainConnectHit())
        return;

    switch (m_attackType)
    {
    case ELITEAI::ORDERTYPE_ATTACK_NORMAL:
        LeoChargeAttack();
        break;

    case ELITEAI::ORDERTYPE_ATTACK_NORMALA:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_AA, true);
        break;

    case ELITEAI::ORDERTYPE_ATTACK_NORMALB:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_AA, true);
        FootMechSplinter().AccumulateSetStep(2);
        break;

    /*  TODO: AA motion frame is missed for C attack ??
              There is skipped AA motion for some reason for C order attack */
    //case ELITEAI::ORDERTYPE_ATTACK_NORMALC:
    //    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_AA, true);
    //    break;

    default:
        break;
    };

    if ((m_attackType == ELITEAI::ORDERTYPE_ATTACK_NORMAL) ||
        (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_RUN))
    {
        m_step = STEP_END;
    }
    else
    {
        m_step = STEP_AA;
    };
};


void CBaseFootMechSplinter::CAttackAStatusObserver::OnStepMotionAA(void)
{
    if (!IsMotionChainConnectHit())
        return;

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());

    switch (m_attackType)
    {
    case ELITEAI::ORDERTYPE_ATTACK_NORMALA:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_AAA, true);
        break;

    case ELITEAI::ORDERTYPE_ATTACK_NORMALB:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_AAB, true);
        FootMechSplinter().AccumulateSetStep(3);
        break;

    case ELITEAI::ORDERTYPE_ATTACK_NORMALC:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_AAC, true);
        break;

    default:
        ASSERT(false);
        break;
    };

    m_step = STEP_AAX;
};


void CBaseFootMechSplinter::CAttackAStatusObserver::OnStepMotionAAX(void)
{
    if (m_attackType != ELITEAI::ORDERTYPE_ATTACK_NORMALB)
    {
        m_step = STEP_END;
        return;
    };

    float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
    if (fMotionNowT < 0.43f)
        return;

    FootMechSplinter().AccumulateEnd();
    MicChargeAttack();

    m_step = STEP_END;
};


bool CBaseFootMechSplinter::CAttackAStatusObserver::IsMotionChainConnectHit(void) const
{
    float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
    float fMotionNextT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
    if (fMotionNowT < fMotionNextT)
        return false;

    return true;
};


void CBaseFootMechSplinter::CAttackAStatusObserver::LeoChargeAttack(void)
{
    if (EnemyChr().GetID() == ENEMYID::ID_SPLINTER)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8342));

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4269));

    RwV3d vecDir = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().RotateVectorByDirection(&vecDir, &Math::VECTOR3_AXIS_Z);

    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);
    Math::Vec3_Add(&vecBodyPos, &vecBodyPos, &vecDir);

    CMagicManager::CParameter magicParam;
    magicParam.SetPositon(&vecBodyPos);
    magicParam.SetDirection(EnemyChr().Compositor().GetDirection());
    magicParam.SetObject(&EnemyChr().Compositor());

    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    uint32 hMagic = CMagicManager::Play(MAGICID::ID_LEO_SP, &magicParam);
    CMagicManager::SetHitAttack(hMagic, feature);
    CMagicManager::SetDamage(hMagic, 10);
};


void CBaseFootMechSplinter::CAttackAStatusObserver::MicChargeAttack(void)
{
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4285));

    RwV3d vecDir = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().RotateVectorByDirection(&vecDir, &Math::VECTOR3_AXIS_Z);

    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);
    Math::Vec3_Add(&vecBodyPos, &vecBodyPos, &vecDir);

    CMagicManager::CParameter magicParam;
    magicParam.SetPositon(&vecBodyPos);
    magicParam.SetObject(&EnemyChr().Compositor());

    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    const float fDegAngleStep = 30.0f;
    const float afMagicDirOfs[] =
    {
        0.0f,                                // center
        -MATH_DEG2RAD(fDegAngleStep * 1.0f), // right
        MATH_DEG2RAD(fDegAngleStep * 1.0f),  // left
        -MATH_DEG2RAD(fDegAngleStep * 2.0f), // right
        MATH_DEG2RAD(fDegAngleStep * 2.0f),  // left
    };

    for (int32 i = 0; i < COUNT_OF(afMagicDirOfs); ++i)
    {
        magicParam.SetDirection(EnemyChr().Compositor().GetDirection() + afMagicDirOfs[i]);

        uint32 hMagic = CMagicManager::Play(MAGICID::ID_MIC_SP, &magicParam);
        CMagicManager::SetHitAttack(hMagic, feature);
        CMagicManager::SetDamage(hMagic, 10);
    };
};


CBaseFootMechSplinter& CBaseFootMechSplinter::CAttackAStatusObserver::FootMechSplinter(void)
{
    return static_cast<CBaseFootMechSplinter&>(EnemyChr());
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFootMechSplinter::CAttackBStatusObserver::OnStart(void) /*override*/
{
    m_attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
    m_step = 0;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_B, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetMotionSpeed(1.4f);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

    if (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_B_CHARGE)
    {
        EnemyChr().Compositor().SetMotionSpeed(1.2f);
        EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

        FootMechSplinter().AccumulateBegin();

        EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
    };

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ CBaseFootMechSplinter::CAttackBStatusObserver::RESULT
CBaseFootMechSplinter::CAttackBStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    RESULT result = RESULT_CONTINUE;

    if (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_B)
        result = ObservingAttackNormal();

    if (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_B_CHARGE)
        result = ObservingAttackCharge();

    return result;
};


/*virtual*/ ENEMYTYPES::STATUS
CBaseFootMechSplinter::CAttackBStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().Compositor().SetMotionSpeed(1.0f);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

    FootMechSplinter().AccumulateEnd();

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ CBaseFootMechSplinter::CAttackBStatusObserver::RESULT 
CBaseFootMechSplinter::CAttackBStatusObserver::ObservingAttackNormal(void)
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ CBaseFootMechSplinter::CAttackBStatusObserver::RESULT 
CBaseFootMechSplinter::CAttackBStatusObserver::ObservingAttackCharge(void)
{
    switch (m_step)
    {
    case 0:
        {
            float fMotionNowT = EnemyChr().Compositor().GetMotionTime();

            if (fMotionNowT > 0.3f)
                FootMechSplinter().AccumulateSetEffectOff();

            if (fMotionNowT > 0.85f)
            {
                FootMechSplinter().BarrierInvoke();
                EnemyChr().Compositor().StopMotion();
                ++m_step;
            };
        }
        break;

    case 1:
        {
            if (!FootMechSplinter().BarrierCheckEnd())
                break;

            EnemyChr().Compositor().PlayMotion();
            EnemyChr().Compositor().SetMotionSpeed(1.0f);
            EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

            FootMechSplinter().AccumulateEnd();

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4295));

            ++m_step;
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().StopMotion();
                return RESULT_END;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_CONTINUE;
};


CBaseFootMechSplinter& CBaseFootMechSplinter::CAttackBStatusObserver::FootMechSplinter(void)
{
    return static_cast<CBaseFootMechSplinter&>(EnemyChr());
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFootMechSplinter::CAttackKickStatusObserver::OnStart(void) /*override*/
{
    m_attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
    m_fJumpInitSpeed = 0.0f;
    m_bCharge = false;

    switch (m_attackType)
    {
    case ELITEAI::ORDERTYPE_ATTACK_HIGH:
        m_fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
        break;

    case SPLINTERAI::ORDERTYPE_ATTACK_KICK:
        m_fJumpInitSpeed = (EnemyChr().Feature().m_fJumpInitializeSpeed / 1.5f);
        break;

    default:
        ASSERT(false);
        break;
    };

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);
    vecVelocity.y += m_fJumpInitSpeed;
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
	
	EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JATTACK, true);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);
    EnemyChr().Compositor().SetMaxVelocity(EnemyChr().Feature().m_fAerialMoveSpeed);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ CBaseFootMechSplinter::CAttackKickStatusObserver::RESULT
CBaseFootMechSplinter::CAttackKickStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), m_fJumpInitSpeed);

    if (!m_bCharge)
    {
        float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
        if (fMotionNowT >= 0.03f)
        {
            RapChargeAttack();
            m_bCharge = true;
        };
    };

    /* it will ends by touchdown status change */
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
CBaseFootMechSplinter::CAttackKickStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);

    return ENEMYTYPES::STATUS_THINKING;
};


void CBaseFootMechSplinter::CAttackKickStatusObserver::RapChargeAttack(void)
{
    if (EnemyChr().GetID() == ENEMYID::ID_SPLINTER)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8342));

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4277));

    RwV3d vecMyBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecMyBodyPos);

    CMagicManager::CParameter magicParam;
    magicParam.SetPositon(&vecMyBodyPos);
    magicParam.SetDirection(EnemyChr().Compositor().GetDirection());
    magicParam.SetObject(&EnemyChr().Compositor());

    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    uint32 hMagic = CMagicManager::Play(MAGICID::ID_RAP_SP, &magicParam);
    CMagicManager::SetHitAttack(hMagic, feature);
    CMagicManager::SetDamage(hMagic, 30);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFootMechSplinter::CAttackShotStatusObserver::OnStart(void) /*override*/
{
    m_attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_fJumpInitSpeed = 0.0f;
    m_step = 0;

    switch (m_attackType)
    {
    case SPLINTERAI::ORDERTYPE_ATTACK_SHOT:
        {
            EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::SHOT, true);
        }
        break;

    case SPLINTERAI::ORDERTYPE_ATTACK_SHOT_JUMP:
        {
            m_fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::SHOT_JUMP, true);

            RwV3d vecVelocity = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetVelocity(&vecVelocity);
            vecVelocity.y += m_fJumpInitSpeed;
            EnemyChr().Compositor().SetVelocity(&vecVelocity);

            EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);
            EnemyChr().Compositor().SetMaxVelocity(EnemyChr().Feature().m_fAerialMoveSpeed);

            EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    CEnemyUtilsElite::RotateToPlayer(&EnemyChr().Compositor(), m_orderTargetNo);
};


/*virtual*/ CBaseFootMechSplinter::CAttackShotStatusObserver::RESULT
CBaseFootMechSplinter::CAttackShotStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK);

    if (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_SHOT_JUMP)
        CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), m_fJumpInitSpeed);

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
                break;

            if (EnemyChr().GetID() == ENEMYID::ID_FOOT_MECH_SPLINTER)
                ShotLazer();
            else
                ShotKnife();

            ++m_step;
        }
        break;

    case 1:
        {
            /* normal shot ends by motion playtime ends
               jump shot ends by touchdown status change */
            if (m_attackType != SPLINTERAI::ORDERTYPE_ATTACK_SHOT)
                break;
            
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    CEnemyUtilsElite::RotateToPlayer(&EnemyChr().Compositor(),
                                     m_orderTargetNo,
                                     EnemyChr().Feature().m_fRotateRate);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
CBaseFootMechSplinter::CAttackShotStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);

    return ENEMYTYPES::STATUS_THINKING;
};


void CBaseFootMechSplinter::CAttackShotStatusObserver::ShotKnife(void)
{
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4128));

    RwV3d vecPosHand = Math::VECTOR3_ZERO;
    GetOffsetWorldPosition(&vecPosHand, 0);

    RwV3d vecDirection = Math::VECTOR3_AXIS_Z;
    /* TODO:  */
    //if (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_SHOT_JUMP)
    //    CalcDirectionPitch(&vecDirection);
    EnemyChr().Compositor().RotateVectorByDirection(&vecDirection, &vecDirection);

    CShotManager::Shot(SHOTID::ID_KNIFE_ENEMY,
                       &vecPosHand,
                       &vecDirection,
                       &EnemyChr().Compositor());
};


void CBaseFootMechSplinter::CAttackShotStatusObserver::ShotLazer(void)
{
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4404));

    RwV3d vecPosEyeLeft = Math::VECTOR3_ZERO;
    GetOffsetWorldPosition(&vecPosEyeLeft, 0);

    RwV3d vecPosEyeRight = Math::VECTOR3_ZERO;
    GetOffsetWorldPosition(&vecPosEyeRight, 1);

    RwV3d vecDirection = Math::VECTOR3_AXIS_Z;
    if (m_attackType == SPLINTERAI::ORDERTYPE_ATTACK_SHOT_JUMP)
        CalcDirectionPitch(&vecDirection);
    EnemyChr().Compositor().RotateVectorByDirection(&vecDirection, &vecDirection);

    SetLazer(&vecPosEyeLeft, &vecDirection);
    SetLazer(&vecPosEyeRight, &vecDirection);
};


void CBaseFootMechSplinter::CAttackShotStatusObserver::SetLazer(const RwV3d* pvecPos,
                                                                const RwV3d* pvecDir)
{
    CMagicManager::CParameter magicParam;
    magicParam.SetPositon(pvecPos);
    magicParam.SetDirection(pvecDir);
    magicParam.SetObject(&EnemyChr().Compositor());

    uint32 hMagic = CMagicManager::Play(MAGICID::ID_LASER_BEAM_ALL, &magicParam);
    CMagicManager::SetDamage(hMagic, 5);
    CMagicManager::SetScale(hMagic, 0.2f);
};


void CBaseFootMechSplinter::CAttackShotStatusObserver::GetOffsetWorldPosition(RwV3d* pvecPos, int32 no) const
{
    RwV3d vecPosOffset = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetOffsetPosition(&vecPosOffset, no);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    Math::Vec3_Add(&vecPos, &vecPos, &vecPosOffset);

    *pvecPos = vecPos;
};


void CBaseFootMechSplinter::CAttackShotStatusObserver::CalcDirectionPitch(RwV3d* pvecDirection) const
{
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (!pPlrChr)
        return;
    
    RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecFootPosPlr);

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    RwV3d vecDir = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDir, &vecFootPosMe, &vecFootPosPlr);
    Math::Vec3_Normalize(&vecDir, &vecDir);

    float y = vecDir.y;

    vecDir.y = 0.0f;
    float z = Math::Vec3_Length(&vecDir);

    float fPitch = std::atan2(y, z);

    RwMatrix matRotX;
    RwMatrixSetIdentityMacro(&matRotX);
    Math::Matrix_RotateX(&matRotX, fPitch);

    RwV3dTransformVector(pvecDirection, pvecDirection, &matRotX);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFootMechSplinter::CThrowStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CBaseThrow::OnStart();
    m_fMessageTimer = 0.0f;
};


/*virtual*/ void CBaseFootMechSplinter::CThrowStatusObserver::OnLift(void) /*override*/
{
    if (EnemyChr().GetID() != ENEMYID::ID_SPLINTER)
        return;

    m_fMessageTimer += CGameProperty::GetElapsedTime();
    if ((m_fMessageTimer > 0.6f) && (m_fMessageTimer < 10.0f))
    {
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8341));
        m_fMessageTimer = 10.0f;
    };
};


/*virtual*/ void CBaseFootMechSplinter::CThrowStatusObserver::OnThrowHit(void) /*override*/
{
    if (EnemyChr().GetID() == ENEMYID::ID_SPLINTER)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8318));
};


//
// *********************************************************************************
//


CBaseFootMechSplinter::CBaseFootMechSplinter(ENEMYID::VALUE enemyId)
: CBaseEliteChr(enemyId)
, m_pThrow(nullptr)
, m_pModuleAccumulate(nullptr)
, m_pModuleBarrier(nullptr)
{
    /* CBaseEliteChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                     new CBaseEliteChr::CAppearStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new CBaseEliteChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CBaseEliteChr::CIdleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                       new CBaseEliteChr::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new CBaseEliteChr::CGuardStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CBaseEliteChr::CJumpReadyStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CBaseEliteChr::CKnockStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CBaseEliteChr::CKnockStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new CBaseEliteChr::CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DASH,                       new CBaseEliteChr::CDashStatusObserver);

    /* CBaseFootMechSplinter status */
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new CBaseFootMechSplinter::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,                   new CBaseFootMechSplinter::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,                   new CBaseFootMechSplinter::CAttackBStatusObserver);
    AttachStatusObserver(CBaseFootMechSplinter::STATUS_ATTACK_KICK,     new CBaseFootMechSplinter::CAttackKickStatusObserver);
    AttachStatusObserver(CBaseFootMechSplinter::STATUS_ATTACK_SHOT,     new CBaseFootMechSplinter::CAttackShotStatusObserver);
    AttachThrowStatusObserver();

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CCommonEnemyObserver::CGetupStatus);
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
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                       new CCommonEnemyObserver::CCharacterEffectStatusObserver);
};


/*virtual*/ CBaseFootMechSplinter::~CBaseFootMechSplinter(void)
{
    ;
};


/*virtual*/ bool CBaseFootMechSplinter::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*overrid*/
{
    bool bResult = CBaseEliteChr::Initialize(pParameter, bReplaceParameter);
    if (bResult)
    {
        m_pModuleBarrier = new CBarrierModule(new CEnemyTracer(this), &Compositor(), 2.0f);
        Compositor().IncludeModule(m_pModuleBarrier);

        m_pModuleAccumulate = CAccumulateModule::New(&Compositor());
        Compositor().IncludeModule(m_pModuleAccumulate);
    };

    return bResult;
};


/*virtual*/ bool CBaseFootMechSplinter::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();
    
    if ((statusNow == CBaseFootMechSplinter::STATUS_ATTACK_KICK) ||
        (statusNow == CBaseFootMechSplinter::STATUS_ATTACK_SHOT) ||
        (statusNow == ENEMYTYPES::STATUS_TOUCHDOWN))
    {
        return false;
    };

    return CBaseEliteChr::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void CBaseFootMechSplinter::OnMessageTouchdown(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();

    bool bIsInAerial = TestFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    bool bIsAerialStatus = (statusNow == CBaseFootMechSplinter::STATUS_ATTACK_KICK) ||
                           (statusNow == CBaseFootMechSplinter::STATUS_ATTACK_SHOT);

    if (bIsInAerial && bIsAerialStatus)
    {
        bool bResult = SetStatus(ENEMYTYPES::STATUS_TOUCHDOWN);
        if (!bResult)
            bResult = SetStatus(ENEMYTYPES::STATUS_IDLE);

        ASSERT(bResult == true);
    };

    CBaseEliteChr::OnMessageTouchdown(fHeight);
};


/*virtual*/ void CBaseFootMechSplinter::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CBaseEliteChr::OnMessageAttackResult(pCatch);
    m_pThrow->SetAttackParam(pCatch);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
CBaseFootMechSplinter::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceStatusFlag = CheckDefenceStatusFlag();
    CHARACTERTYPES::ATTACKRESULTTYPE attackResult = rCalc.CalcAtackParameter(Compositor().AttackParameter(),
                                                                             defenceStatusFlag);

    CHitAttackData* pAttack = &rCalc.GetAttack();

    CGameObject* pAttacker = pAttack->GetObject();
    uint32 hAttacker = CEnemyUtils::GetHandleFromHitObj(pAttacker);
    AIModerator().CatchAttack(hAttacker);

    bool bIsAttackHit = (attackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK) ||
                        (attackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY);
    bool bIsAttackerChr = (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER);
    if (bIsAttackHit && bIsAttackerChr)
        AIModerator().AttackCountInc();

    return CBaseEliteChr::OnDamage(rCalc);
};


void CBaseFootMechSplinter::AttachThrowStatusObserver(void)
{
    m_pThrow = new CBaseFootMechSplinter::CThrowStatusObserver;

    RwV3d vecBone = { 0.0f, 0.0f, 0.0f };
    m_pThrow->SetTraceBoneData(7, &vecBone);

    RwV3d vecThrow = { 0.0f, 10.0f, 10.0f };
    m_pThrow->SetThrowVector(&vecThrow);

    AttachStatusObserver(ENEMYTYPES::STATUS_THROW, m_pThrow);
};


void CBaseFootMechSplinter::AccumulateBegin(void)
{
    m_pModuleAccumulate->SetStepZero();
    m_pModuleAccumulate->SetDrawOn();
    m_pModuleAccumulate->SetStepOne();
};


void CBaseFootMechSplinter::AccumulateEnd(void)
{
    m_pModuleAccumulate->SetEffectOff();
    m_pModuleAccumulate->SetDrawOff();
};


void CBaseFootMechSplinter::AccumulateSetStep(int32 step)
{
    switch (step)
    {
    case 0: m_pModuleAccumulate->SetStepZero();  break;
    case 1: m_pModuleAccumulate->SetStepOne();   break;
    case 2: m_pModuleAccumulate->SetStepTwo();   break;
    case 3: m_pModuleAccumulate->SetStepThree(); break;
    default: ASSERT(false); break;
    };
};


void CBaseFootMechSplinter::AccumulateSetEffectOff(void)
{
    m_pModuleAccumulate->SetEffectOff();
};


void CBaseFootMechSplinter::BarrierInvoke(void)
{
    if (GetID() == ENEMYID::ID_SPLINTER)
        CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(8342));

    CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4294));

    m_pModuleBarrier->Appear();
    m_pModuleBarrier->SetStepThree();
    m_pModuleBarrier->SetHitTarget(CHitAttackData::TARGET_PLAYER);
    m_pModuleBarrier->SetLive(0.5f);
};


bool CBaseFootMechSplinter::BarrierCheckEnd(void)
{
    if (m_pModuleBarrier->GetState() != CBarrierModule::STATE_DISAPPEAR)
        return false;

    return true;
};