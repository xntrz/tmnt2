#include "088Shredder.hpp"

#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/088ShredderAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


static const RwV3d BANDANA_OFFSET = { -0.116f, -0.08f, 0.128f };


namespace MOTIONNANMES
{
    static const char* DOWN_1 = "Yarare1";
    static const char* DOWN_2 = "Yarare2";
    static const char* DOWN_3 = "Yarare3";
};


class C088Shredder::CThinkingObserver : public Utils6034::CBaseThinkingObserver
{
public:
    virtual int32 OnGetOriginalAttackStatus(int32 iAttackType) override;
};

class C088Shredder::CSpecialDamagedObserver : public CEnemyCharacter::CStatusObserver
{
public:
    CSpecialDamagedObserver(Utils6034::ICharacterController* pChrController);
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
    float GetDownTime(void) const;

protected:
    int32 m_step;
    Utils6034::ICharacterController* m_pChrController;
    Utils6034::CFixTimer m_downTimer;
};


class C088Shredder::CElecAttackObserver : public Utils6034::CElecAttackObserver
{
public:
    CElecAttackObserver(C088Shredder* pShredder, Utils6034::ICharacterController* pChrController);
    virtual ~CElecAttackObserver(void) {};
};


//
// *********************************************************************************
//


/*virtual*/ int32 C088Shredder::CThinkingObserver::OnGetOriginalAttackStatus(int32 iAttackType) /*override*/
{
    return ENEMYTYPES::STATUS_ATTACK_C;
};


//
// *********************************************************************************
//


C088Shredder::CSpecialDamagedObserver::CSpecialDamagedObserver(Utils6034::ICharacterController* pChrController)
: m_pChrController(pChrController)
{
    ;
};


/*virtual*/ void C088Shredder::CSpecialDamagedObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_pChrController->FlagNoReaction().IncCounter();
    m_downTimer.SetAlarmTime(GetDownTime());

    EnemyChr().Compositor().ChangeMotion(MOTIONNANMES::DOWN_1, true);
};


/*virtual*/ C088Shredder::CSpecialDamagedObserver::RESULT
C088Shredder::CSpecialDamagedObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNANMES::DOWN_2, true);

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4356));
            if (Math::Rand() & 1)
                CMessageManager::Request(SEGROUPID::VALUE(95));
            else
                CMessageManager::Request(SEGROUPID::VALUE(96));

            m_downTimer.Start();
            ++m_step;
        }
        break;

    case 1:
        {
            m_downTimer.Update();

            if (!m_downTimer.IsAlarm())
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNANMES::DOWN_3, true);

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

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C088Shredder::CSpecialDamagedObserver::OnEnd(void) /*override*/
{
    m_pChrController->FlagNoReaction().DecCounter();
    m_pChrController->DamageCounter().Reset();

    return ENEMYTYPES::STATUS_THINKING;
};


float C088Shredder::CSpecialDamagedObserver::GetDownTime(void) const
{
    float afDownTime[] = { 4.0f, 3.5f, 2.8f };
    static_assert(COUNT_OF(afDownTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afDownTime[difficulty];
};


//
// *********************************************************************************
//


C088Shredder::CElecAttackObserver::CElecAttackObserver(C088Shredder* pShredder,
                                                       Utils6034::ICharacterController* pChrController)
: Utils6034::CElecAttackObserver(&pShredder->Compositor(), ENEMYTYPES::MOTIONNAMES::ATTACK_A, pChrController)
{
    ;
};


//
// *********************************************************************************
//


C088Shredder::C088Shredder(void)
: CEnemyCharacter(ENEMYID::ID_SHREDDER)
, m_chrController()
{
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,           new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,           new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,    new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,     new CCommonEnemyObserver::CKnockStatus);

    /* Utils6034 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,           new Utils6034::CBaseHideObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,          new Utils6034::CBaseDeathObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,          new Utils6034::CGuardObserver);
    
    /* C088Shredder status */
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,       new C088Shredder::CThinkingObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,          new C088Shredder::CSpecialDamagedObserver(&m_chrController));
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,       new C088Shredder::CElecAttackObserver(this, &m_chrController));

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C088ShredderAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 1.2f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.5f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.6f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 10000;
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
    m_chrController.SetOverflowData(3, 200, 8.0f);

    /* init chr modules */
    CBandanaModule* pBandanaModule = new CBandanaModule(&Compositor(), 2, &BANDANA_OFFSET, CBandanaModule::BANDANACOLOR_SHREDDERGREY);
    Compositor().IncludeModule(pBandanaModule);
};


/*virtual*/ C088Shredder::~C088Shredder(void)
{
    ;
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C088Shredder::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    m_chrController.StoreEnemyFlag();
    m_chrController.SetEnemyFlag();

    CHARACTERTYPES::ATTACKRESULTTYPE attackResultType = CEnemyCharacter::OnDamage(rCalc);

    m_chrController.RestoreEnemyFlag();
    m_chrController.SendAIMessage(attackResultType, rCalc.GetAttack().GetPower());

    bool bIsKnocked = m_chrController.IsResultKnockDamaged(attackResultType);
    bool bIsDamageOverflow = m_chrController.DamageCounter().IsOverflowDamage();
    bool bIsGetupNow = (GetStatus() == ENEMYTYPES::STATUS_GETUP);

    if (bIsKnocked && bIsDamageOverflow && !bIsGetupNow)
        SetStatus(ENEMYTYPES::STATUS_GETUP);

    return attackResultType;
};