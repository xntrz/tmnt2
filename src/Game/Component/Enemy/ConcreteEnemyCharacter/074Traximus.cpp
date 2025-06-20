#include "074Traximus.hpp"

#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/074TraximusAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


namespace MOTIONNAMES
{
    static const char* JUMP_ATTACK_READY = "JAttack1";
    static const char* JUMP_ATTACK_AERIAL = "JAttack2";
    static const char* JUMP_ATTACK_TOUCHDOWN = "JAttack3";
};


static void SetDirectionToOrderPosition(CEnemyCharacter* pEnemyChr)
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    pEnemyChr->Compositor().GetPosition(&vecMyPos);

    RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&pEnemyChr->AIThinkOrder());

    RwV3d vec = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vec, &vecOrderPos, &vecMyPos);

    pEnemyChr->Compositor().SetDirectionFromVector(&vec);
};


class C074Traximus::CThinkingObserver : public Utils6034::CBaseThinkingObserver
{
public:
    virtual ~CThinkingObserver(void) {};
    virtual int32 OnGetOriginalMoveStatus(int32 iMoveType) override;
    virtual int32 OnGetOriginalAttackStatus(int32 iAttackType) override;
};


class C074Traximus::CAttackAAStatusObserver : public CMotionManagedStatusObserver
{
public:
    enum MOTIONID
    {
        MOTIONID_A = 0,
        MOTIONID_AA,
    };

public:
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
    virtual bool OnMotionChange(int32 idOld, int32 idNew) override;
};


class C074Traximus::CAttackBStatusObserver : public Utils6034::CBaseOneShotAttack
{
public:
    CAttackBStatusObserver(void);
    virtual ~CAttackBStatusObserver(void) {};
};


class C074Traximus::CBackStepStartObserver : public CEnemyCharacter::CStatusObserver
{
public:
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
};


class C074Traximus::CAttackJumpReadyObserver : public CEnemyCharacter::CStatusObserver
{
public:
    CAttackJumpReadyObserver(Utils6034::ICharacterController* pChrController);
    virtual ~CAttackJumpReadyObserver(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    Utils6034::ICharacterController* m_pChrController;
};


class C074Traximus::CAttackJumpAerialObserver : public CEnemyCharacter::CStatusObserver
{
public:
    CAttackJumpAerialObserver(Utils6034::ICharacterController* pChrController);
    virtual ~CAttackJumpAerialObserver(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    Utils6034::ICharacterController* m_pChrController;
    bool m_bFlagFirstMsg;
    bool m_bFlagTouchdownSEDone;
    bool m_bFlagTouchdownSERequired;
};


class C074Traximus::CAttackJumpTouchdownObserver : public CEnemyCharacter::CStatusObserver
{
public:
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
};


class C074Traximus::CThrowObserver : public Utils6034::CBaseThrowObserver
{
public:
    CThrowObserver(Utils6034::ICharacterController* pCharacterController);
    virtual ~CThrowObserver(void) {};
    virtual void OnStart(void) override;
};


//
// *********************************************************************************
//


/*virtual*/ int32 C074Traximus::CThinkingObserver::OnGetOriginalMoveStatus(int32 iMoveType) /*override*/
{
    switch(iMoveType)
    {
    case C074TraximusAI::ORDERTYPE_MOVE_BACK_STEP:
        return C074Traximus::STATUS_BACK_STEP_START;

    default:
        ASSERT(false);
        break;
    };

    return Utils6034::CBaseThinkingObserver::OnGetOriginalMoveStatus(iMoveType);
};


/*virtual*/ int32 C074Traximus::CThinkingObserver::OnGetOriginalAttackStatus(int32 iAttackType) /*override*/
{
    switch (iAttackType)
    {
    case C074TraximusAI::ORDERTYPE_ATTACK_AA:
        return ENEMYTYPES::STATUS_ATTACK_AA;

    case C074TraximusAI::ORDERTYPE_ATTACK_B:
        return ENEMYTYPES::STATUS_ATTACK_B;

    case C074TraximusAI::ORDERTYPE_ATTACK_THROW:
        return ENEMYTYPES::STATUS_THROW;

    case C074TraximusAI::ORDERTYPE_ATTACK_JUMP:
        return C074Traximus::STATUS_ATTACK_JUMP_READY;

    default:
        ASSERT(false);
        break;
    };

    return Utils6034::CBaseThinkingObserver::OnGetOriginalAttackStatus(iAttackType);
};


//
// *********************************************************************************
//


/*virtual*/ void C074Traximus::CAttackAAStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    MotionRegist(MOTIONID_A, ENEMYTYPES::MOTIONNAMES::ATTACK_A, true);
    MotionRegist(MOTIONID_AA, ENEMYTYPES::MOTIONNAMES::ATTACK_AA);
    MotionPlay(MOTIONID_A, true);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4387));
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8443));
};


/*virtual*/ C074Traximus::CAttackAAStatusObserver::RESULT
C074Traximus::CAttackAAStatusObserver::Observing(void) /*override*/
{
    if (MotionCurrent() == MOTIONID_A)
    {
        RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());

        CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(),
                                      &vecOrderPos,
                                      EnemyChr().Feature().m_fRotateRate,
                                      0.0f);
    };

    if (IsMotionEnd())
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C074Traximus::CAttackAAStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ bool C074Traximus::CAttackAAStatusObserver::OnMotionChange(int32 idOld, int32 idNew) /*override*/
{
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4387));
    return true;
};


//
// *********************************************************************************
//


C074Traximus::CAttackBStatusObserver::CAttackBStatusObserver(void)
: Utils6034::CBaseOneShotAttack(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void C074Traximus::CBackStepStartObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JUMP_READY, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    SetDirectionToOrderPosition(&EnemyChr());
};


/*virtual*/ C074Traximus::CBackStepStartObserver::RESULT
C074Traximus::CBackStepStartObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C074Traximus::CBackStepStartObserver::OnEnd(void) /*override*/
{
    RwV3d vecVelocity = { 0.0f, 0.0f, -10.0f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    vecVelocity.y = 4.5f;
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


C074Traximus::CAttackJumpReadyObserver::CAttackJumpReadyObserver(Utils6034::ICharacterController* pChrController)
: m_pChrController(pChrController)
{
    ;
};


/*virtual*/ void C074Traximus::CAttackJumpReadyObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_ATTACK_READY, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    SetDirectionToOrderPosition(&EnemyChr());

    m_pChrController->FlagNoReaction().IncCounter();

    C074Traximus::CSharedData* pSharedData = static_cast<C074Traximus::CSharedData*>(EnemyChr().SharedData());
    pSharedData->bJumpAttack = true;
};


/*virtual*/ C074Traximus::CAttackJumpReadyObserver::RESULT
C074Traximus::CAttackJumpReadyObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C074Traximus::CAttackJumpReadyObserver::OnEnd(void) /*override*/
{
    RwV3d vecVelocity = { 0.0f, 0.0f, 2.0f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    vecVelocity.y = 9.0f;
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    m_pChrController->FlagNoReaction().DecCounter();

    return static_cast<ENEMYTYPES::STATUS>(C074Traximus::STATUS_ATTACK_JUMP_AERIAL);
};


//
// *********************************************************************************
//


C074Traximus::CAttackJumpAerialObserver::CAttackJumpAerialObserver(Utils6034::ICharacterController* pChrController)
: m_pChrController(pChrController)
, m_bFlagFirstMsg(false)
, m_bFlagTouchdownSEDone(false)
, m_bFlagTouchdownSERequired(false)
{
    ;
};


/*virtual*/ void C074Traximus::CAttackJumpAerialObserver::OnStart(void) /*override*/
{
    m_bFlagFirstMsg = false;
    m_bFlagTouchdownSEDone = false;
    m_bFlagTouchdownSERequired = false;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_ATTACK_AERIAL, true);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    m_pChrController->FlagNoReaction().IncCounter();

    if (Math::RandRange(0, 100) >= 25)
    {
        if (m_bFlagFirstMsg || (Math::RandRange(0, 100) < 90))
        {
            CMessageManager::Request(SEGROUPID::VALUE(74));
        }
        else
        {
            CMessageManager::Request(SEGROUPID::VALUE(75));
            m_bFlagFirstMsg = true;
        };
    }
    else
    {
        m_bFlagTouchdownSERequired = true;
    };
};


/*virtual*/ C074Traximus::CAttackJumpAerialObserver::RESULT
C074Traximus::CAttackJumpAerialObserver::Observing(void) /*override*/
{
    CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(),
                                  EnemyChr().Feature().m_fJumpInitializeSpeed);

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);

    if (vecVelocity.y < 0.0f)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4387));

    if ((vecVelocity.y < 0.0f) && (!m_bFlagTouchdownSEDone) && (m_bFlagTouchdownSERequired))
    {
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8444));
        m_bFlagTouchdownSEDone = true;
        m_bFlagTouchdownSERequired = false;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C074Traximus::CAttackJumpAerialObserver::OnEnd(void) /*override*/
{
    m_pChrController->FlagNoReaction().DecCounter();

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    return ENEMYTYPES::STATUS_TOUCHDOWN;
};


//
// *********************************************************************************
//


/*virtual*/ void C074Traximus::CAttackJumpTouchdownObserver::OnStart(void) /*override*/
{
    C074Traximus::CSharedData* pSharedData = static_cast<C074Traximus::CSharedData*>(EnemyChr().SharedData());
    if (pSharedData->bJumpAttack)
    {
        EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_ATTACK_TOUCHDOWN, true);

        RwV3d vecBoneOffset = { -2.0f, 0.0f, 0.0f }; // axe blade offset
        RwV3d vecBonePos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetBonePosition(&vecBonePos, 7, &vecBoneOffset);

        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecMyPos);

        RwV3d vecMagicPos = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecMagicPos, &vecMyPos, &vecBonePos);

        float fHeight = CWorldMap::GetMapHeight(&vecMagicPos);
        if (fHeight > CEnemyUtils::INVALID_WORLD_MAP_HEIGHT)
            vecMagicPos.y = fHeight;

        uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_WINDPRESS), &vecMagicPos);
        CMagicManager::SetScale(hMagic, 2.5f);
    }
    else
    {
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::TOUCHDOWN, true);
        CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
};


/*virtual*/ C074Traximus::CAttackJumpTouchdownObserver::RESULT
C074Traximus::CAttackJumpTouchdownObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C074Traximus::CAttackJumpTouchdownObserver::OnEnd(void) /*override*/
{
    C074Traximus::CSharedData* pSharedData = static_cast<C074Traximus::CSharedData*>(EnemyChr().SharedData());
    pSharedData->bJumpAttack = false;

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


C074Traximus::CThrowObserver::CThrowObserver(Utils6034::ICharacterController* pCharacterController)
: Utils6034::CBaseThrowObserver(pCharacterController)
{
    ;
};


/*virtual*/ void C074Traximus::CThrowObserver::OnStart(void) /*override*/
{
    Utils6034::CBaseThrowObserver::OnStart();
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8438));
};


//
// *********************************************************************************
//


C074Traximus::C074Traximus(void)
: CEnemyCharacter(ENEMYID::ID_TRAXIMUS)
, m_chrController()
, m_sharedData()
, m_pThrow(nullptr)
{
    /* init shared data */
    m_sharedData.bJumpAttack = false;
    SetSharedData(&m_sharedData);

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,             new CCommonEnemyObserver::CCounteractStatusObserver);

    /* Utils6034 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new Utils6034::CBaseHideObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new Utils6034::CBaseDeathObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new Utils6034::CGuardObserver);
    
    /* C074Traximus status */
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new C074Traximus::CThinkingObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,              new C074Traximus::CAttackAAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new C074Traximus::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new C074Traximus::CAttackJumpTouchdownObserver);
    AttachStatusObserver(C074Traximus::STATUS_ATTACK_JUMP_READY,    new C074Traximus::CAttackJumpReadyObserver(&m_chrController));
    AttachStatusObserver(C074Traximus::STATUS_ATTACK_JUMP_AERIAL,   new C074Traximus::CAttackJumpAerialObserver(&m_chrController));
    AttachStatusObserver(C074Traximus::STATUS_BACK_STEP_START,      new C074Traximus::CBackStepStartObserver);
    AttachThrowStatusObserver();

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C074TraximusAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 3.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.55f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.5f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.3f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 400;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.3f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 15.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 2.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    Initialize(&parameter, false);

    /* init AI moderator */
    Utils6034::IBaseAIModerator& rAIModerator = static_cast<Utils6034::IBaseAIModerator&>(AIModerator());
    rAIModerator.Initialize(&m_chrController);

    /* init chr controller */
    m_chrController.Initialize(this, &AIThinkOrder(), &rAIModerator);
    m_chrController.SetOverflowData(100, GetNoReactionDamageAmount(), 2.0f);
};


/*virtual*/ C074Traximus::~C074Traximus(void)
{
    ;
};


/*virtual*/ bool C074Traximus::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    if (GetStatus() == C074Traximus::STATUS_ATTACK_JUMP_AERIAL)
        return false;

    return CEnemyCharacter::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void C074Traximus::OnMessageDoChangingAerial(void) /*override*/
{
    if (m_sharedData.bJumpAttack)
    {
        SetStatus(C074Traximus::STATUS_ATTACK_JUMP_AERIAL);
        return;
    };

    CEnemyCharacter::OnMessageDoChangingAerial();
};


/*virtual*/ void C074Traximus::OnMessageTouchdown(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();

    if ((statusNow == C074Traximus::STATUS_ATTACK_JUMP_READY) ||
        (statusNow == ENEMYTYPES::STATUS_TOUCHDOWN))
    {
        return;
    };

    bool bIsDead = TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS);
    bool bIsInAerial = (m_sharedData.bJumpAttack) ||
                       (statusNow == C074Traximus::STATUS_ATTACK_JUMP_AERIAL);

    if (!bIsDead && bIsInAerial)
    {
        SetStatus(ENEMYTYPES::STATUS_TOUCHDOWN);
        return;
    };

    CEnemyCharacter::OnMessageTouchdown(fHeight);
};


/*virtual*/ void C074Traximus::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);

    if (pCatch->GetResult() == CHitCatchData::RESULT_GUARDIMPACT)
        m_sharedData.bJumpAttack = false;

    m_pThrow->SetAttackParam(pCatch);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C074Traximus::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    m_chrController.StoreEnemyFlag();
    m_chrController.SetEnemyFlag();

    CHitAttackData* pAttack = &rCalc.GetAttack();
    CGameObject* pAttacker = pAttack->GetObject();
    switch (pAttacker->GetType())
    {
    case GAMEOBJECTTYPE::CHARACTER:
        {
            float fPower = static_cast<float>(pAttack->GetPower());
            int32 iPower = static_cast<int32>(fPower * -0.5f);
            pAttack->SetPower(1 - iPower);
        }
        break;

    case GAMEOBJECTTYPE::SHOT:
        {
            SetFlag(ENEMYTYPES::FLAG_NOREACTION);
            pAttack->SetPower(1);
        };
        break;

    default:
        break;
    };

    CHARACTERTYPES::ATTACKRESULTTYPE attackResultType = CEnemyCharacter::OnDamage(rCalc);

    m_chrController.RestoreEnemyFlag();
    m_chrController.SendAIMessage(attackResultType, pAttack->GetPower());

    if (m_chrController.IsResultKnockDamaged(attackResultType))
    {
        m_sharedData.bJumpAttack = false;
        if (m_chrController.DamageCounter().IsOverflowDamage())
            m_chrController.FlagNoReaction().AddTime(1.0f);
    };

    return attackResultType;
};


void C074Traximus::AttachThrowStatusObserver(void)
{
    m_pThrow = new C074Traximus::CThrowObserver(&m_chrController);

    RwV3d vecBone = { -2.01f, 0.0f, 0.0f };
    m_pThrow->SetTraceBoneData(7, &vecBone);

    RwV3d vecThrow = { 0.0f, 0.0f, -10.0f };
    m_pThrow->SetThrowVector(&vecThrow);

    AttachStatusObserver(ENEMYTYPES::STATUS_THROW, m_pThrow);
};


int32 C074Traximus::GetNoReactionDamageAmount(void) const
{
    int32 aiOverflowQuantity[] = { 60, 50, 40 };
    static_assert(COUNT_OF(aiOverflowQuantity) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return aiOverflowQuantity[difficulty];
};