#include "BaseSpiderMonsterChr.hpp"

#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyWatcher.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseSpiderMonsterAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ void CBaseSpiderMonsterChr::CHideStatusObserver::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ CBaseSpiderMonsterChr::CHideStatusObserver::RESULT
CBaseSpiderMonsterChr::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseSpiderMonsterChr::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseSpiderMonsterChr::CAppearStatusObserver::OnStart(void) /*override*/
{
    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::TOJO, bForce);    

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);
    vecPos.y = 100.0f;    
    vecPos.y = CWorldMap::GetMapHeight(&vecPos);

    EnemyChr().Compositor().SetPosition(&vecPos);
};


/*virtual*/ CBaseSpiderMonsterChr::CAppearStatusObserver::RESULT
CBaseSpiderMonsterChr::CAppearStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
    {
        CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseSpiderMonsterChr::CAppearStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseSpiderMonsterChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseSpiderMonsterChr::CThinkingStatusObserver::RESULT
CBaseSpiderMonsterChr::CThinkingStatusObserver::Observing(void) /*override*/
{
    RESULT result = RESULT_END;

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_WAIT:
        {
            m_nextStatus = ENEMYTYPES::STATUS_IDLE;
        }
        break;

    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;

            if ((moveType >= BASEAI6045::ORDERTYPE_MOVE_WALK_POS) &&
                (moveType <= BASEAI6045::ORDERTYPE_MOVE_WALK_NO))
            {
                m_nextStatus = ENEMYTYPES::STATUS_WALK;
            }
            else if ((moveType >= BASEAI6045::ORDERTYPE_MOVE_RUN_POS) &&
                (moveType <= BASEAI6045::ORDERTYPE_MOVE_RUN_NO))
            {
                m_nextStatus = ENEMYTYPES::STATUS_RUN;
            }
            else if (moveType == BASEAI6045::ORDERTYPE_MOVE_JUMP)
            {
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case BASEAI6045::ORDERTYPE_ATTACK_COUNTER:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASESPIDERMONSTER::STATUS_ATTACK_COUNTER);
                break;

            case BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_WEB_SHOT:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_B:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_C:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_D:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASESPIDERMONSTER::STATUS_ATTACK_D);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        {
            m_nextStatus = ENEMYTYPES::STATUS_GUARD;
        }
        break;

    default:
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            result = RESULT_CONTINUE;
        }
        break;
    };

    return result;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseSpiderMonsterChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CSpiderWebShotObserver::CSpiderWebShotObserver(void)
: CAttackSalivaShotStatusObserver(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
{
    ;
};


/*virtual*/ void CBaseSpiderMonsterChr::CSpiderWebShotObserver::OnStart(void) /*override*/
{
    m_eMagicID = MAGICID::ID_SPIDERNET;
    m_iSEID = SDCODE_SE(4315);
    m_iBoneID = 5;
    m_vecOffset = Math::VECTOR3_ZERO;
    m_fAimArea = MATH_DEG2RAD(36.0f);
    m_fScale = 1.0f;
    m_iAimRateFreqTableNo = CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE;
    m_bPlayerAimFlag = false;

    CAttackSalivaShotStatusObserver::OnStart();
};


/*virtual*/ CBaseSpiderMonsterChr::CSpiderWebShotObserver::RESULT
CBaseSpiderMonsterChr::CSpiderWebShotObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        RwV3d vecPos = Math::VECTOR3_ZERO;
        CEnemyUtils::GetWorldBonePosition(&vecPos, &EnemyChr().Compositor(), 5);

        uint32 hEffect = CEffectManager::Play(EFFECTID::ID_SPIDERSMOKE, &vecPos, 1.0f);
        ASSERT(hEffect);
    };

    return CAttackSalivaShotStatusObserver::Observing();
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CAttackCStatusObserver::CAttackCStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_C)
{
    ;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CAttackDStatusObserver::CAttackDStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(BASESPIDERMONSTER::MOTIONNAMES::ATTACK_D)
{
    ;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CAttackCounterStatusObserver::CAttackCounterStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(BASESPIDERMONSTER::MOTIONNAMES::ATTACK_D)
{
    ;
};


/*virtual*/ void CBaseSpiderMonsterChr::CAttackCounterStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseSpiderMonsterChr::CAttackCounterStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    return CBaseChr6045::COneShotMotionForAttackPlayerAim::OnEnd();
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CGuardStatusObserver::CGuardStatusObserver(void)
{
    m_fGuardTime = 0.5f;
    m_fGuardTotalTime = 1.5f;
};


//
// *********************************************************************************
//


/*virtual*/ ENEMYTYPES::STATUS CBaseSpiderMonsterChr::CJumpReadyStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);
    vecVelocity.y = 0.0f;

    float fMoveSpeed = Math::Vec3_Length(&vecVelocity);
    if (fMoveSpeed < 0.1f)
    {
        float fWalkMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed;

        vecVelocity = { 0.0f, 0.0f, fWalkMoveSpeed };
        EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    };

    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
    vecVelocity.y = fJumpInitSpeed;

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CDeathStatusObserver::CDeathStatusObserver(CBaseSpiderMonsterChr* pBaseSpiderMonsterChr)
{
    ASSERT(pBaseSpiderMonsterChr != nullptr);

    if (pBaseSpiderMonsterChr->GetID() == ENEMYID::ID_SPIDER_MONSTER)
    {
        if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
            CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
            SetParameter(DEATHKIND_NEXUS, nullptr, 1.0f);
        else if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M04NB))
            SetParameter(DEATHKIND_MOTION, nullptr, 1.0f);
		else 
			SetParameter(DEATHKIND_MONSTER, nullptr, 2.0f);
    }
    else
    {
        SetParameter(DEATHKIND_MONSTER, nullptr, 2.0f);
    };
};


//
// *********************************************************************************
//


CBaseSpiderMonsterChr::CBaseSpiderMonsterChr(ENEMYID::VALUE enemyId)
: CBaseChr6045(enemyId)
{
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,             new CCommonEnemyObserver::CCounteractStatusObserver);    
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                   new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* CBaseSpiderMonsterChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseSpiderMonsterChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseSpiderMonsterChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseSpiderMonsterChr::CSpiderWebShotObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseSpiderMonsterChr::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new CBaseSpiderMonsterChr::CAttackCStatusObserver);
    AttachStatusObserver(BASESPIDERMONSTER::STATUS_ATTACK_D,        new CBaseSpiderMonsterChr::CAttackDStatusObserver);
    AttachStatusObserver(BASESPIDERMONSTER::STATUS_ATTACK_COUNTER,  new CBaseSpiderMonsterChr::CAttackCounterStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseSpiderMonsterChr::CDeathStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseSpiderMonsterChr::CJumpReadyStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new CBaseSpiderMonsterChr::CGuardStatusObserver);
};


/*virtual*/ CBaseSpiderMonsterChr::~CBaseSpiderMonsterChr(void)
{
    ;
};


void CBaseSpiderMonsterChr::AttachAppearStatusObserver(void)
{
    uint8 appearType = FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case ENEMYTYPES::APPEARTYPE_WALK_SLOW:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(4.5f));
        break;

    case ENEMYTYPES::APPEARTYPE_WALK_FAST:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(3.0f));
        break;

    case ENEMYTYPES::APPEARTYPE_NONE:
    case ENEMYTYPES::APPEARTYPE_FLY_UP:
    case ENEMYTYPES::APPEARTYPE_FLY_DOWN:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_FALL:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CFallAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_MOTION:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CBaseSpiderMonsterChr::CAppearStatusObserver);
        break;

    default:
        break;
    };
};