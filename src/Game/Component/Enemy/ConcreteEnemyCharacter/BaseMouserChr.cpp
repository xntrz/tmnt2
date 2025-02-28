#include "BaseMouserChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseMouserAI.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Sound/GameSound.hpp"


CBaseMouserChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseMouserChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseMouserChr::CThinkingStatusObserver::RESULT
CBaseMouserChr::CThinkingStatusObserver::Observing(void) /*override*/
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
            case BASEMOUSER_AI::ORDERTYPE_ATTACK_A:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case BASEMOUSER_AI::ORDERTYPE_ATTACK_B:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;
                                
            case BASEMOUSER_AI::ORDERTYPE_ATTACK_C:
                if (EnemyChr().GetID() == ENEMYID::ID_RAPTOR)
                    m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                else
                    m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
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


/*virtual*/ ENEMYTYPES::STATUS CBaseMouserChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


CBaseMouserChr::CAttackAStatusObserver::CAttackAStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttack(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseMouserChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttack(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseMouserChr::CAttackCStatusObserver::CAttackCStatusObserver(CBaseMouserChr* pBaseMouserChr)
: m_fTimer(0.0f)
, m_bMotionChangeFlag(false)
, m_hEffectElectric(0)
, m_modelColorCtrl()
{
    m_modelColorCtrl.Initialize(&pBaseMouserChr->Compositor());
};


/*virtual*/ void CBaseMouserChr::CAttackCStatusObserver::OnStart(void) /*override*/
{
    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(BASEMOUSER::MOTIONNAMES::ATTACK_C1, bForce);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    InitBodyColorChange();
    InitElecEffect();

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4337));

    m_fTimer = 0.0f;
    m_bMotionChangeFlag = false;
};


/*virtual*/ CBaseMouserChr::CAttackCStatusObserver::RESULT
CBaseMouserChr::CAttackCStatusObserver::Observing(void) /*override*/
{
    if (!m_bMotionChangeFlag)
    {
        if (EnemyChr().Compositor().IsMotionEnd())
        {
            bool bForce = true;
            EnemyChr().Compositor().ChangeMotion(BASEMOUSER::MOTIONNAMES::ATTACK_C2, bForce);

            m_bMotionChangeFlag = true;
        };
    };

    ProcBodyColorChange();
    m_fTimer += CGameProperty::GetElapsedTime();

    if (m_fTimer >= 1.5f)
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseMouserChr::CAttackCStatusObserver::OnEnd(void) /*override*/
{
    TermElecEffect();
    TermBodyColorChange();

    if (m_fTimer < 1.5f)
        return ENEMYTYPES::STATUS_IDLE;

    return ENEMYTYPES::STATUS_DEATH;
};


void CBaseMouserChr::CAttackCStatusObserver::InitElecEffect(void)
{
    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

    float fScale = 1.5f;

    m_hEffectElectric = CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_SHRD_ELEC,
                                                          &EnemyChr().Compositor(),
                                                          fScale,
                                                          CEnemyUtils6045::TRACE_BODY,
                                                          false);

    ASSERT(m_hEffectElectric);    
};


void CBaseMouserChr::CAttackCStatusObserver::TermElecEffect(void)
{
    if (m_hEffectElectric)
    {
        CEffectManager::Finish(m_hEffectElectric);
        m_hEffectElectric = 0;
    };
};


void CBaseMouserChr::CAttackCStatusObserver::InitBodyColorChange(void)
{
    m_modelColorCtrl.SetModelColorControlEnable(true);
    m_modelColorCtrl.SetModelColorChange(255.0f, 0.0f, 0.0f, 1.5f);
};


void CBaseMouserChr::CAttackCStatusObserver::TermBodyColorChange(void)
{
    m_modelColorCtrl.SetModelColorControlEnable(false);
};


void CBaseMouserChr::CAttackCStatusObserver::ProcBodyColorChange(void)
{
    m_modelColorCtrl.Period();
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseMouserChr::CMoveStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CMoveStatus::OnStart();

    if (EnemyChr().GetID() != ENEMYID::ID_RAPTOR)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4340));
};


//
// *********************************************************************************
//


CBaseMouserChr::CFlyawayStatusObserver::CFlyawayStatusObserver(void)
: CCommonEnemyObserver::CFlyawayStatus(false)
{
    ;
};


/*virtual*/ void CBaseMouserChr::CFlyawayStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CFlyawayStatus::OnStart();
    EnemyChr().Compositor().PlayMotion();
    m_bMotionChangeFlag = false;    
};


/*virtual*/ CBaseMouserChr::CFlyawayStatusObserver::RESULT
CBaseMouserChr::CFlyawayStatusObserver::Observing(void) /*override*/
{
    if (!m_bMotionChangeFlag)
    {
        if (EnemyChr().Compositor().IsMotionEnd())
        {
            const char* pszMotion = nullptr;

            if (Status() == ENEMYTYPES::STATUS_FLYAWAY_BACK)
                pszMotion = BASEMOUSER::MOTIONNAMES::FLYAWAY_BACK2;
            else
                pszMotion = BASEMOUSER::MOTIONNAMES::FLYAWAY_FRONT2;

            bool bForce = true;
            EnemyChr().Compositor().ChangeMotion(pszMotion, bForce);

            m_bMotionChangeFlag = true;
        };
    };

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseMouserChr::CFlyawayBoundStatusObserver::OnStart(void) /*override*/
{
    const char* pszMotion = nullptr;

    if (Status() == ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK)
        pszMotion = BASEMOUSER::MOTIONNAMES::FLYAWAY_BOUND_BACK;
    else
        pszMotion = BASEMOUSER::MOTIONNAMES::FLYAWAY_BOUND_FRONT;

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(pszMotion, bForce);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vVelocity);

    float endT = EnemyChr().Compositor().GetMotionEndTime();
    RwV3d vAccel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vAccel, &vVelocity, -(1.0f / endT));

    EnemyChr().Compositor().SetAcceleration(&vAccel);

    m_bCheckVelocityFlag = false;
    m_bHitFlag = false;
    m_bDirCorrection = false;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseMouserChr::CGetupStatusObserver::OnStart(void) /*override*/
{
    m_bBackFlag = false;

    const char* pszMotion = nullptr;

    if (EnemyChr().StatusSubject().StatusPrev() == ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK)
    {
        m_bBackFlag = true;
        pszMotion = BASEMOUSER::MOTIONNAMES::GETUP_BACK;
    }
    else
    {
        pszMotion = BASEMOUSER::MOTIONNAMES::GETUP_FRONT;
    };

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(pszMotion, bForce);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ CBaseMouserChr::CGetupStatusObserver::RESULT
CBaseMouserChr::CGetupStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseMouserChr::CGetupStatusObserver::OnEnd(void) /*override*/
{
    if (m_bBackFlag)
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        fDir += MATH_PI;
        EnemyChr().Compositor().SetDirection(fDir);
    };

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


CBaseMouserChr::CDeathStatusObserver::CDeathStatusObserver(CBaseMouserChr* pBaseMouserChr)
{
    ENEMYID::VALUE enemyId = pBaseMouserChr->GetID();
    if (enemyId == ENEMYID::ID_RAPTOR)
    {
        SetParameter(DEATHKIND_MONSTER, nullptr, 0.85f);
    }
    else
    {
        float fScale = (enemyId == ENEMYID::ID_KROKODIL_MOUSER) ? 0.8f : 0.65f;
        SetParameter(DEATHKIND_MACHINE, nullptr, fScale);
    };
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(CBaseMouserChr, { EFFECTID::ID_SHRD_ELEC });


CBaseMouserChr::CBaseMouserChr(ENEMYID::VALUE enemyId)
: CEnemyCharacter(enemyId)
{
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CCommonEnemyObserver::CKnockFrontStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CCommonEnemyObserver::CKnockBackStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_CAUGHT,                 new CCommonEnemyObserver::CCaughtStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED,                 new CCommonEnemyObserver::CLiftedStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED_WALK,            new CCommonEnemyObserver::CLiftedWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_FRONT,           new CCommonEnemyObserver::CThrownFrontStatusObserver(false, 1.0f));
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_BACK,            new CCommonEnemyObserver::CThrownBackStatusObserver);

    /* CBaseChr6045 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseChr6045::CJumpReadyStatusObserver);

    /* CBaseMouserChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseMouserChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseMouserChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseMouserChr::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseMouserChr::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new CBaseMouserChr::CAttackCStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CBaseMouserChr::CMoveStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new CBaseMouserChr::CMoveStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CBaseMouserChr::CFlyawayStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CBaseMouserChr::CFlyawayStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CBaseMouserChr::CFlyawayBoundStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CBaseMouserChr::CFlyawayBoundStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CBaseMouserChr::CGetupStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseMouserChr::CDeathStatusObserver(this));
};


/*virtual*/ CBaseMouserChr::~CBaseMouserChr(void)
{
    ;
};


/*virtual*/ bool CBaseMouserChr::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*override*/
{
    bool bResult = CEnemyCharacter::Initialize(pParameter, bReplaceParameter);

    DetachStatusObserver(ENEMYTYPES::STATUS_APPEAR);

    uint8 appearType = AIModerator().AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case ENEMYTYPES::APPEARTYPE_WALK_SLOW:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(4.5f));
        break;
        
    case ENEMYTYPES::APPEARTYPE_WALK_FAST:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(3.0f));
        break;
        
    case ENEMYTYPES::APPEARTYPE_FALL:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CFallAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_NONE:
    case ENEMYTYPES::APPEARTYPE_FLY_UP:
    case ENEMYTYPES::APPEARTYPE_FLY_DOWN:
    case ENEMYTYPES::APPEARTYPE_MOTION:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    default:
        break;
    };

    return bResult;
};


/*virtual*/ void CBaseMouserChr::Run(void) /*override*/
{
    CEnemyCharacter::Run();
};