#include "089FeudalJapanShredder.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/089FeudalJapanShredderAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"


class C089FeudalJapanShredder::CThinkingObserver : public Utils6034::CBaseThinkingObserver
{
public:
    virtual ~CThinkingObserver(void) {};
    virtual int32 OnGetOriginalAttackStatus(int32 iAttackType) override;
};


class C089FeudalJapanShredder::CThrowObserver : public Utils6034::CBaseThrowObserver
{
public:
    CThrowObserver(Utils6034::ICharacterController* pCharacterController);
    virtual ~CThrowObserver(void) {};
    virtual void OnThrowHit(void) override;
};


class C089FeudalJapanShredder::CElecAttackObserver : public Utils6034::CElecAttackObserver
{
public:
    CElecAttackObserver(C089FeudalJapanShredder* pShredder, Utils6034::ICharacterController* pChrController);
    virtual ~CElecAttackObserver(void) {};
};



//
// *********************************************************************************
//


/*virtual*/ int32
C089FeudalJapanShredder::CThinkingObserver::OnGetOriginalAttackStatus(int32 iAttackType) /*override*/
{
    switch (iAttackType)
    {
    case C089FeudalJapanShredderAI::ORDERTYPE_ATTACK_TENGU:
        return ENEMYTYPES::STATUS_ATTACK_AA;
        
    case C089FeudalJapanShredderAI::ORDERTYPE_ATTACK_ELEC:
        return ENEMYTYPES::STATUS_ATTACK_C;
        
    case C089FeudalJapanShredderAI::ORDERTYPE_ATTACK_CHARGE:
        return ENEMYTYPES::STATUS_ATTACK_B;
        
    case C089FeudalJapanShredderAI::ORDERTYPE_ATTACK_THROW:
        return ENEMYTYPES::STATUS_THROW;

    default:
        ASSERT(false);
        break;
    };

    return Utils6034::CBaseThinkingObserver::OnGetOriginalAttackStatus(iAttackType);
};


//
// *********************************************************************************
//


C089FeudalJapanShredder::CThrowObserver::CThrowObserver(Utils6034::ICharacterController* pCharacterController)
: Utils6034::CBaseThrowObserver(SDCODE_SE(4362), pCharacterController)
{
    ;
};


/*virtual*/ void C089FeudalJapanShredder::CThrowObserver::OnThrowHit(void) /*override*/
{
    Utils6034::CBaseThrowObserver::OnThrowHit();
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8464));
};


//
// *********************************************************************************
//


C089FeudalJapanShredder::CElecAttackObserver::CElecAttackObserver(C089FeudalJapanShredder* pShredder,
                                                                  Utils6034::ICharacterController* pChrController)
: Utils6034::CElecAttackObserver(&pShredder->Compositor(), ENEMYTYPES::MOTIONNAMES::ATTACK_C, pChrController)
{
    ;
};


//
// *********************************************************************************
//


C089FeudalJapanShredder::C089FeudalJapanShredder(void)
: CEnemyCharacter(ENEMYID::ID_FEUDAL_JAPAN_SHREDDER)
, m_chrController()
, m_pThrow(nullptr)
, m_swordBoneTracer()
, m_swordTraceEffect()
{
    /* init sword effect */
    SetupTenguSwordEffect();

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);
    
    /* Utils6034 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new Utils6034::CBaseHideObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new Utils6034::CBaseDeathObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new Utils6034::CGuardObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,              new Utils6034::CTenguAttackObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new Utils6034::CChargeAttackObserver(&m_chrController));
    AttachStatusObserver(ENEMYTYPES::STATUS_DASH,                   new Utils6034::CDashStatus);

    /* C089FeudalJapanShredder status */
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new C089FeudalJapanShredder::CThinkingObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new C089FeudalJapanShredder::CElecAttackObserver(this, &m_chrController));
    AttachThrowStatusObserver();

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C089FeudalJapanShredderAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.16f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 12.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.06f;
    parameter.m_feature.m_fRotateRate           = 2.3f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 600;
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


/*virtual*/ C089FeudalJapanShredder::~C089FeudalJapanShredder(void)
{
    ;
};


/*virtual*/ void C089FeudalJapanShredder::Run(void) /*override*/
{
    CEnemyCharacter::Run();
    m_swordTraceEffect.Update();
    Compositor().SetEnableBodyHitSelfToOther(false);
};


/*virtual*/ void C089FeudalJapanShredder::OnStart(void) /*override*/
{
    CEnemyCharacter::OnStart();
    m_swordTraceEffect.Start();
};


/*virtual*/ void C089FeudalJapanShredder::OnStop(void) /*override*/
{
    m_swordTraceEffect.Stop();
    CEnemyCharacter::OnStop();
};


/*virtual*/ bool C089FeudalJapanShredder::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    if (GetStatus() == ENEMYTYPES::STATUS_ATTACK_B)
        return false;

    return CEnemyCharacter::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void C089FeudalJapanShredder::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);
    m_pThrow->SetAttackParam(pCatch);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C089FeudalJapanShredder::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    m_chrController.StoreEnemyFlag();
    m_chrController.SetEnemyFlag();

    CHARACTERTYPES::ATTACKRESULTTYPE attackResultType = CEnemyCharacter::OnDamage(rCalc);

    m_chrController.RestoreEnemyFlag();
    m_chrController.SendAIMessage(attackResultType, rCalc.GetAttack().GetPower());

    if (m_chrController.IsResultKnockDamaged(attackResultType) &&
        m_chrController.DamageCounter().IsOverflowDamage())
    {
        m_chrController.FlagNoReaction().AddTime(1.0f);
    };    

    return attackResultType;
};


void C089FeudalJapanShredder::AttachThrowStatusObserver(void)
{
    m_pThrow = new C089FeudalJapanShredder::CThrowObserver(&m_chrController);

    RwV3d vecBone = { 0.0f, 0.3f, 0.0f };
    m_pThrow->SetTraceBoneData(4, &vecBone);

    RwV3d vecThrow = { 0.0f, 0.0f, -15.0f };
    m_pThrow->SetThrowVector(&vecThrow);

    AttachStatusObserver(ENEMYTYPES::STATUS_THROW, m_pThrow);
};


int32 C089FeudalJapanShredder::GetNoReactionDamageAmount(void) const
{
    int32 aiOverflowQuantity[] = { 100, 80, 60 };
    static_assert(COUNT_OF(aiOverflowQuantity) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return aiOverflowQuantity[difficulty];
};


void C089FeudalJapanShredder::SetupTenguSwordEffect(void)
{
    m_swordTraceEffect.Initialize(EFFECTID::ID_TENGSWORD, &m_swordBoneTracer);

    RwV3d vecOffset = { -0.3f, 0.1f, 0.0f };
    RwV3d vecDirection = { -0.6f, -0.1f, 0.0f };

    m_swordBoneTracer.Initialize(&Compositor(), 7);
    m_swordBoneTracer.SetOffset(&vecOffset);
    m_swordBoneTracer.SetDirection(&vecDirection);
    m_swordBoneTracer.SetEnableDirection(true);
};