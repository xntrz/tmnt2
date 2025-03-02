#include "BaseFixedEnemyChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseCarnivorousPlantAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Sound/GameSound.hpp"


CBaseFixedEnemyChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFixedEnemyChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseFixedEnemyChr::CThinkingStatusObserver::RESULT
CBaseFixedEnemyChr::CThinkingStatusObserver::Observing(void) /*override*/
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

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case BASECARNIVOROUSPLANT_AI::ORDERTYPE_ATTACK_A:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case BASECARNIVOROUSPLANT_AI::ORDERTYPE_ATTACK_B:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            default:
                ASSERT(false);
                break;
            };
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


/*virtual*/ ENEMYTYPES::STATUS CBaseFixedEnemyChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


CBaseFixedEnemyChr::CAttackAStatusObserver::CAttackAStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttack(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseFixedEnemyChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::CAttackSalivaShotStatusObserver(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
{
    m_eMagicID = MAGICID::ID_SPSM_TBSHOT;
    m_iSEID = SDCODE_SE(4315);
    m_iBoneID = 3;
    m_vecOffset = { 0.0f, 0.0f, 1.0f };
    m_fAimArea = MATH_DEG2RAD(36.0f);
    m_fScale = 0.5f;
    m_iAimRateFreqTableNo = CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE;
    m_bPlayerAimFlag = false;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFixedEnemyChr::CKnockStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CKnockStatus::OnStart();

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFixedEnemyChr::CDindleStatusObserver::OnStart(void) /*override*/
{
    if (EnemyChr().GetID() != ENEMYID::ID_MURAL_CARNIVOROUS_PLANT)
    {
        CCommonEnemyObserver::CCharacterEffectStatusObserver::OnStart();
        return;
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    m_bBodyHitFlag = EnemyChr().Compositor().IsEnableBodyHitSelfToOther();
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    m_fTargetTime = EnemyChr().Compositor().AttackParameter().m_fTroubleTime;
    m_fTime = 0.0f;
    
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DINDLE, true);

    float fScale = 1.0f;
    bool bTraceDirection = false;
    RwV3d vecOffset = { 0.0f, -0.5f, 0.0f };

    m_hEffectBiribiri = CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_BIRIBIRI,
                                                          &EnemyChr().Compositor(),
                                                          fScale,
                                                          CEnemyUtils6045::TRACE_BODY,
                                                          bTraceDirection,
                                                          &vecOffset);

    ASSERT(m_hEffectBiribiri);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFixedEnemyChr::CDindleStatusObserver::OnEnd(void) /*override*/
{
    if ((EnemyChr().GetID() == ENEMYID::ID_MURAL_CARNIVOROUS_PLANT) && m_hEffectBiribiri)
    {
        CEffectManager::Finish(m_hEffectBiribiri);
        m_hEffectBiribiri = 0;
    };

    return CCommonEnemyObserver::CCharacterEffectStatusObserver::OnEnd();
};


//
// *********************************************************************************
//


CBaseFixedEnemyChr::CDeathStatusObserver::CDeathStatusObserver(void)
{
    SetParameter(DEATHKIND_MONSTER, ENEMYTYPES::MOTIONNAMES::DEATH, 1.0f);
};


//
// *********************************************************************************
//


CBaseFixedEnemyChr::CBaseFixedEnemyChr(ENEMYID::VALUE enemyId)
: CEnemyCharacter(enemyId)
{
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    
    /* CBaseFixedEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseFixedEnemyChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseFixedEnemyChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseFixedEnemyChr::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseFixedEnemyChr::CAttackBStatusObserver);    
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseFixedEnemyChr::CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                 new CBaseFixedEnemyChr::CDindleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK,                  new CBaseFixedEnemyChr::CKnockStatusObserver);
};


/*virtual*/ CBaseFixedEnemyChr::~CBaseFixedEnemyChr(void)
{
    ;
};


/*virtual*/ bool CBaseFixedEnemyChr::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*override*/
{
    bool bResult = CEnemyCharacter::Initialize(pParameter, bReplaceParameter);

    Compositor().SetEnableBodyHitSelfToOther(false);

    DetachStatusObserver(ENEMYTYPES::STATUS_APPEAR);
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);

    return bResult;
};