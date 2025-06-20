#include "090UltimateShredder.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/090UltimateShredderAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


static const float ATTACK_JUMP_VELOCITY_Y = 8.5f;


namespace MOTIONNAMES
{
    static const char* THUNDER                  = "E";
    static const char* JUMP_ATTACK_READY        = "F1";
    static const char* JUMP_ATTACK_AERIAL       = "F2";
    static const char* JUMP_ATTACK_TOUCHDOWN    = "F3";
};


class C090UltimateShredder::CThinkingObserver : public Utils6034::CBaseThinkingObserver
{
public:
    virtual ~CThinkingObserver(void) {};
    virtual int32 OnGetOriginalAttackStatus(int32 iAttackType) override;
};


class C090UltimateShredder::CAttackJumpReadyObserver : public CEnemyCharacter::CStatusObserver
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


class C090UltimateShredder::CAttackJumpAerialObserver : public CEnemyCharacter::CStatusObserver
{
public:
    CAttackJumpAerialObserver(Utils6034::ICharacterController* pChrController);
    virtual ~CAttackJumpAerialObserver(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    Utils6034::ICharacterController* m_pChrController;
    int32 m_step;
    float m_fStartTime;
    float m_fMotionEndTime;
    float m_fMotionMidTime;
};


class C090UltimateShredder::CAttackJumpTouchdownObserver : public CEnemyCharacter::CStatusObserver
{
public:
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
};


class C090UltimateShredder::CThunderAttackObserver : public CEnemyCharacter::CStatusObserver
{
public:
    CThunderAttackObserver(const CCharacterCompositor* pChrCompositor);
    virtual ~CThunderAttackObserver(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    Utils6034::CBodyElecEffectManager   m_elecEffectMgr;
    Utils6034::CTraceEffect             m_aHandTraceEffect[2];
    Utils6034::CBoneTracer              m_aHandBoneTracer[2];
    Utils6034::CTraceEffect             m_aSightTraceEffect[GAMETYPES::PLAYERS_MAX];
    Utils6034::CPlayerFootTracer        m_aSightFootTracer[GAMETYPES::PLAYERS_MAX];
    int32                               m_step;
    float                               m_fStartTime;
};


class C090UltimateShredder::CElecAttackObserver : public Utils6034::CElecAttackObserver
{
public:
    CElecAttackObserver(C090UltimateShredder* pShredder, Utils6034::ICharacterController* pChrController);
    virtual ~CElecAttackObserver(void) {};
};


class C090UltimateShredder::CThrowObserver : public Utils6034::CBaseThrowObserver
{
public:
    CThrowObserver(Utils6034::ICharacterController* pCharacterController);
    virtual ~CThrowObserver(void) {};
    virtual void OnThrowHit(void) override;
};


//
// *********************************************************************************
//

/*virtual*/ int32
C090UltimateShredder::CThinkingObserver::OnGetOriginalAttackStatus(int32 iAttackType) /*override*/
{
    switch (iAttackType)
    {
    case C090UltimateShredderAI::ORDERTYPE_ATTACK_TENGU:
        return ENEMYTYPES::STATUS_ATTACK_AA;

    case C090UltimateShredderAI::ORDERTYPE_ATTACK_ELEC:
        return ENEMYTYPES::STATUS_ATTACK_C;

    case C090UltimateShredderAI::ORDERTYPE_ATTACK_CHARGE:
        return ENEMYTYPES::STATUS_ATTACK_B;

    case C090UltimateShredderAI::ORDERTYPE_ATTACK_THROW:
        return ENEMYTYPES::STATUS_THROW;

    case C090UltimateShredderAI::ORDERTYPE_ATTACK_THUNDER:
        return C090UltimateShredder::STATUS_ATTACK_THUNDER;

    case C090UltimateShredderAI::ORDERTYPE_ATTACK_JUMP:
        return C090UltimateShredder::STATUS_ATTACK_JUMP_READY;

    default:
        ASSERT(false);
        break;
    };

    return Utils6034::CBaseThinkingObserver::OnGetOriginalAttackStatus(iAttackType);
};


//
// *********************************************************************************
//


C090UltimateShredder::CAttackJumpReadyObserver::CAttackJumpReadyObserver(Utils6034::ICharacterController* pChrController)
: m_pChrController(pChrController)
{
    ;
};


/*virtual*/ void
C090UltimateShredder::CAttackJumpReadyObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_ATTACK_READY, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetMotionCtrlTime(1.0f);

    m_pChrController->FlagNoReaction().IncCounter();

    C090UltimateShredder::CSharedData* pSharedData = static_cast<C090UltimateShredder::CSharedData*>(EnemyChr().SharedData());
    pSharedData->bJumpAttack = true;
};


/*virtual*/ C090UltimateShredder::CAttackJumpReadyObserver::RESULT
C090UltimateShredder::CAttackJumpReadyObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C090UltimateShredder::CAttackJumpReadyObserver::OnEnd(void) /*override*/
{
    m_pChrController->FlagNoReaction().DecCounter();

    RwV3d vecVelocity = { 0.0f, ATTACK_JUMP_VELOCITY_Y, 0.0f };
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
    
    return static_cast<ENEMYTYPES::STATUS>(C090UltimateShredder::STATUS_ATTACK_JUMP_AERIAL);
};


//
// *********************************************************************************
//


C090UltimateShredder::CAttackJumpAerialObserver::CAttackJumpAerialObserver(Utils6034::ICharacterController* pChrController)
: m_pChrController(pChrController)
, m_step(-1)
, m_fStartTime(0.0f)
, m_fMotionEndTime(0.0f)
, m_fMotionMidTime(0.0f)
{
    ;
};


/*virtual*/ void
C090UltimateShredder::CAttackJumpAerialObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_ATTACK_AERIAL, true);
    EnemyChr().Compositor().StopMotion();
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    m_pChrController->FlagNoReaction().IncCounter();
    
    m_step = 0;
    m_fStartTime = CGameProperty::GetTotalElapsedTime();
    m_fMotionEndTime = EnemyChr().Compositor().GetMotionEndTime();
    m_fMotionMidTime = (m_fMotionEndTime * 0.6f);
};


/*virtual*/ C090UltimateShredder::CAttackJumpAerialObserver::RESULT
C090UltimateShredder::CAttackJumpAerialObserver::Observing(void) /*override*/
{
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);

    switch (m_step)
    {
    case 0:
        {
            if (vecVelocity.y < 0.0f)
                ++m_step;
        }
        break;

    case 1:
        {
            RwV3d vecPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetPosition(&vecPos);

            float fHeight = CWorldMap::GetMapHeight(&vecPos);
            if (vecPos.y < (fHeight + 0.7f))
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_ATTACK_TOUCHDOWN, true);
                ++m_step;
            };
        }
        break;

    case 2:
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_step < 2)
    {
        float fMotionT = 0.0f;
        if (vecVelocity.y >= 0.0f)
            fMotionT = ((ATTACK_JUMP_VELOCITY_Y - vecVelocity.y) * m_fMotionMidTime) / ATTACK_JUMP_VELOCITY_Y;
        else
            fMotionT = m_fMotionMidTime + (((m_fMotionEndTime - m_fMotionMidTime) * vecVelocity.y) / -ATTACK_JUMP_VELOCITY_Y);

        EnemyChr().Compositor().SetMotionTime(fMotionT);
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C090UltimateShredder::CAttackJumpAerialObserver::OnEnd(void) /*override*/
{
    m_pChrController->FlagNoReaction().DecCounter();

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().Compositor().SetMotionCtrlTime(1.0f);

    return static_cast<ENEMYTYPES::STATUS>(C090UltimateShredder::STATUS_ATTACK_JUMP_TOUCHDOWN);
};


//
// *********************************************************************************
//


/*virtual*/ void
C090UltimateShredder::CAttackJumpTouchdownObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    C090UltimateShredder::CSharedData* pSharedData = static_cast<C090UltimateShredder::CSharedData*>(EnemyChr().SharedData());
    if (pSharedData->bJumpAttack)
    {
        RwV3d vecPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecPos);
        vecPos.y = CWorldMap::GetMapHeight(&vecPos);

        uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_WINDPRESS), &vecPos);
        CMagicManager::SetScale(hMagic, 3.0f);
    }
    else
    {
        CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::TOUCHDOWN, true);
    };
};


/*virtual*/ C090UltimateShredder::CAttackJumpTouchdownObserver::RESULT
C090UltimateShredder::CAttackJumpTouchdownObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C090UltimateShredder::CAttackJumpTouchdownObserver::OnEnd(void) /*override*/
{
    C090UltimateShredder::CSharedData* pSharedData = static_cast<C090UltimateShredder::CSharedData*>(EnemyChr().SharedData());
    pSharedData->bJumpAttack = false;

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


C090UltimateShredder::CThunderAttackObserver::CThunderAttackObserver(const CCharacterCompositor* pChrCompositor)
: m_elecEffectMgr()
, m_aHandTraceEffect()
, m_aHandBoneTracer()
, m_aSightTraceEffect()
, m_aSightFootTracer()
, m_step(-1)
, m_fStartTime(0.0f)
{
    m_elecEffectMgr.SetCharacter(pChrCompositor);

    m_aHandBoneTracer[0].Initialize(pChrCompositor, 4);
    m_aHandTraceEffect[0].Initialize(EFFECTID::ID_SHRD_ELEC, &m_aHandBoneTracer[0]);

    m_aHandBoneTracer[1].Initialize(pChrCompositor, 3);
    m_aHandTraceEffect[1].Initialize(EFFECTID::ID_SHRD_ELEC, &m_aHandBoneTracer[1]);

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        m_aSightFootTracer[i].Initialize(i);
        m_aSightTraceEffect[i].Initialize(EFFECTID::ID_SHRD_ELEND, &m_aSightFootTracer[i]);
        m_aSightTraceEffect[i].SetResetTime(0.05f);
    };
};


/*virtual*/ void
C090UltimateShredder::CThunderAttackObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_fStartTime = CGameProperty::GetTotalElapsedTime();

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::THUNDER, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    m_elecEffectMgr.SetUpdateStepTime(0.02f);
    m_elecEffectMgr.SetDispType(true, true);
    m_elecEffectMgr.StartUpdate();

    bool bIsNexusMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                       CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X);

    if ((Math::RandRange(0, 100) < 14) || bIsNexusMap)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8469));
    else
        CMessageManager::Request(SEGROUPID::VALUE(138));
};


/*virtual*/ C090UltimateShredder::CThunderAttackObserver::RESULT
C090UltimateShredder::CThunderAttackObserver::Observing(void) /*override*/
{
    float fElapsedTime = CGameProperty::GetTotalElapsedTime() - m_fStartTime;

    switch (m_step)
    {
    case 0:
        {
            if (fElapsedTime < 0.233f)
                break;

            m_elecEffectMgr.SetUpdateStepTime(0.0066f);
            m_elecEffectMgr.SetDispType(false, true);
            m_elecEffectMgr.StartUpdate();

            for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
                m_aHandTraceEffect[i].Start();

            ++m_step;
        }
        break;

    case 1:
        {
            if (fElapsedTime < 0.5f)
                break;

            int32 numPlayers = CAIUtils::GetPlayerNum();
            for (int32 i = 0; i < numPlayers; ++i)
            {
                m_aSightTraceEffect[i].Start();
                m_aSightFootTracer[i].SetEnableManualUpdate(false);
            };

            ++m_step;
        }
        break;

    case 2:
        {
            if (fElapsedTime < 1.1f)
            {
                int32 numPlayers = CAIUtils::GetPlayerNum();
                for (int32 i = 0; i < numPlayers; ++i)
                {
                    if (!m_aSightFootTracer[i].IsEnabledManualUpdate())
                        m_aSightFootTracer[i].OnUpdate();                
                };
            }
            else
            {
                int32 numPlayers = CAIUtils::GetPlayerNum();
                for (int32 i = 0; i < numPlayers; ++i)
                    m_aSightFootTracer[i].SetEnableManualUpdate(true);

                ++m_step;
            };
        }
        break;

    case 3:
        {
            if (fElapsedTime < 0.8f)
                break;
            
            for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
                m_aHandTraceEffect[i].Stop();

            ++m_step;
        }
        break;

    case 4:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                int32 numPlayers = CAIUtils::GetPlayerNum();
                for (int32 i = 0; i < numPlayers; ++i)
                {
                    m_aSightTraceEffect[i].Stop();

                    RwV3d vecPlrFootPos = m_aSightFootTracer[i].GetPosition();
                    CMagicManager::Play("thunder", &vecPlrFootPos);
                };

                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

                return RESULT_END;
            };            
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    m_elecEffectMgr.Update();

    for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
        m_aHandTraceEffect[i].Update();

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
        m_aSightTraceEffect[i].Update();
    
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C090UltimateShredder::CThunderAttackObserver::OnEnd(void) /*override*/
{
    m_elecEffectMgr.SwitchAllEffectOn(false);

    for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
        m_aHandTraceEffect[i].Stop();

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
        m_aSightTraceEffect[i].Stop();

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


C090UltimateShredder::CElecAttackObserver::CElecAttackObserver(C090UltimateShredder* pShredder,
                                                               Utils6034::ICharacterController* pChrController)
: Utils6034::CElecAttackObserver(&pShredder->Compositor(), ENEMYTYPES::MOTIONNAMES::ATTACK_C, pChrController)
{
    ;
};


//
// *********************************************************************************
//


C090UltimateShredder::CThrowObserver::CThrowObserver(Utils6034::ICharacterController* pCharacterController)
: Utils6034::CBaseThrowObserver(SDCODE_SE(4362), pCharacterController)
{
    ;
};


/*virtual*/ void C090UltimateShredder::CThrowObserver::OnThrowHit(void) /*override*/
{
    Utils6034::CBaseThrowObserver::OnThrowHit();
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8464));
};


//
// *********************************************************************************
//


C090UltimateShredder::C090UltimateShredder(void)
: CEnemyCharacter(ENEMYID::ID_ULTIMATE_SHREDDER)
, m_chrController()
, m_pThrow(nullptr)
, m_swordBoneTracer()
, m_swordTraceEffect()
, m_sharedData()
, m_elecEffectMgr()
, m_elecEffectTimer(0.0f)
{
    /* init common */
    SetupTenguSwordEffect();
    SetupBodyElecEffect();
    SetupSharedData();

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                      new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                          new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                       new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                            new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                            new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                     new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                      new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                          new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,                   new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,                    new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,             new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,              new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                           new CCommonEnemyObserver::CGetupStatus);
    
    /* Utils6034 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                            new Utils6034::CBaseHideObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                           new Utils6034::CBaseDeathObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                           new Utils6034::CGuardObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,                       new Utils6034::CTenguAttackObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,                        new Utils6034::CChargeAttackObserver(&m_chrController));
    AttachStatusObserver(ENEMYTYPES::STATUS_DASH,                            new Utils6034::CDashStatus);

    /* C089FeudalJapanShredder status */
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                        new C090UltimateShredder::CThinkingObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,                        new C090UltimateShredder::CElecAttackObserver(this, &m_chrController));
    AttachStatusObserver(C090UltimateShredder::STATUS_ATTACK_THUNDER,        new C090UltimateShredder::CThunderAttackObserver(&Compositor()));
    AttachStatusObserver(C090UltimateShredder::STATUS_ATTACK_JUMP_READY,     new C090UltimateShredder::CAttackJumpReadyObserver(&m_chrController));
    AttachStatusObserver(C090UltimateShredder::STATUS_ATTACK_JUMP_AERIAL,    new C090UltimateShredder::CAttackJumpAerialObserver(&m_chrController));
    AttachStatusObserver(C090UltimateShredder::STATUS_ATTACK_JUMP_TOUCHDOWN, new C090UltimateShredder::CAttackJumpTouchdownObserver);
    AttachThrowStatusObserver();

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C090UltimateShredderAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.16f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 14.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.08f;
    parameter.m_feature.m_fRotateRate           = 2.3f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 10.2f;
    parameter.m_feature.m_fAerialAcceleration   = 20.0f;
    parameter.m_feature.m_iHPMax                = 650;
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
    SetFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE);

    Utils6034::IBaseAIModerator& rAIModerator = static_cast<Utils6034::IBaseAIModerator&>(AIModerator());

    /* init chr controller */
    m_chrController.Initialize(this, &AIThinkOrder(), &rAIModerator);
    m_chrController.SetOverflowData(100, GetNoReactionDamageAmount(), 2.0f);

    /* init AI moderator */
    rAIModerator.Initialize(&m_chrController);
};


/*virtual*/ C090UltimateShredder::~C090UltimateShredder(void)
{
    ;
};


/*virtual*/ void C090UltimateShredder::Run(void) /*override*/
{
    CEnemyCharacter::Run();

    m_swordTraceEffect.Update();

    m_elecEffectTimer.Update();

    if (m_elecEffectTimer.IsAlarm())
        m_elecEffectMgr.SwitchAllEffectOn(false);
    else
        m_elecEffectMgr.Update();
};


/*virtual*/ void C090UltimateShredder::OnStart(void) /*override*/
{
    CEnemyCharacter::OnStart();
    m_swordTraceEffect.Start();
};


/*virtual*/ void C090UltimateShredder::OnStop(void) /*override*/
{
    m_swordTraceEffect.Stop();
    CEnemyCharacter::OnStop();
};


/*virtual*/ bool C090UltimateShredder::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    if ((GetStatus() == C090UltimateShredder::STATUS_ATTACK_JUMP_AERIAL) ||
        (GetStatus() == ENEMYTYPES::STATUS_ATTACK_B))
        return false;

    return CEnemyCharacter::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void C090UltimateShredder::OnMessageDoChangingAerial(void) /*override*/
{
    if (m_sharedData.bJumpAttack)
    {
        SetStatus(C090UltimateShredder::STATUS_ATTACK_JUMP_AERIAL);
        return;
    };

    CEnemyCharacter::OnMessageDoChangingAerial();
};


/*virtual*/ void C090UltimateShredder::OnMessageTouchdown(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();

    if ((statusNow == C090UltimateShredder::STATUS_ATTACK_JUMP_READY) ||
        (statusNow == C090UltimateShredder::STATUS_ATTACK_JUMP_TOUCHDOWN))
    {
        return;
    };

    bool bIsDead = TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS);
    bool bIsInAerial = (m_sharedData.bJumpAttack) ||
                       (statusNow == C090UltimateShredder::STATUS_ATTACK_JUMP_AERIAL);

    if (!bIsDead && bIsInAerial)
    {
        SetStatus(C090UltimateShredder::STATUS_ATTACK_JUMP_TOUCHDOWN);
        return;
    };

    CEnemyCharacter::OnMessageTouchdown(fHeight);
};


/*virtual*/ void C090UltimateShredder::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);
    m_pThrow->SetAttackParam(pCatch);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C090UltimateShredder::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    m_chrController.StoreEnemyFlag();
    m_chrController.SetEnemyFlag();

    if (!m_chrController.IsFlyawayState())
    {
        CHitAttackData* pAttack = &rCalc.GetAttack();
        CGameObject* pAttacker = pAttack->GetObject();

        switch (pAttacker->GetType())
        {
        case GAMEOBJECTTYPE::GIMMICK:
            {
                SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

                if (!m_chrController.IsDownState())
                {
                    RwV3d vecMyBodyPos = Math::VECTOR3_ZERO;
                    Compositor().GetBodyPosition(&vecMyBodyPos);

                    CEffectManager::Play(EFFECTID::ID_ALL_GUARD2, &vecMyBodyPos);

                    CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4264));
                };
            }
            break;

        case GAMEOBJECTTYPE::EFFECT:
            {
                if (!std::strcmp(pAttacker->GetName(), "flame_map"))
                    SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
            }
            break;

        default:
            break;
        };
    };

    CHARACTERTYPES::ATTACKRESULTTYPE attackResultType = CEnemyCharacter::OnDamage(rCalc);

    m_chrController.RestoreEnemyFlag();
    m_chrController.SendAIMessage(attackResultType, rCalc.GetAttack().GetPower());

    if (m_chrController.IsResultKnockDamaged(attackResultType))
    {
        m_sharedData.bJumpAttack = false;
        if (m_chrController.DamageCounter().IsOverflowDamage())
            m_chrController.FlagNoReaction().AddTime(1.0f);
    };

    return attackResultType;
};


void C090UltimateShredder::AttachThrowStatusObserver(void)
{
    m_pThrow = new C090UltimateShredder::CThrowObserver(&m_chrController);

    RwV3d vecBone = { 0.0f, 0.3f, 0.0f };
    m_pThrow->SetTraceBoneData(4, &vecBone);

    RwV3d vecThrow = { 0.0f, 0.0f, -15.0f };
    m_pThrow->SetThrowVector(&vecThrow);

    AttachStatusObserver(ENEMYTYPES::STATUS_THROW, m_pThrow);
};


int32 C090UltimateShredder::GetNoReactionDamageAmount(void) const
{
    int32 aiOverflowQuantity[] = { 90, 70, 50 };
    static_assert(COUNT_OF(aiOverflowQuantity) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return aiOverflowQuantity[difficulty];
};


void C090UltimateShredder::SetupTenguSwordEffect(void)
{
    m_swordTraceEffect.Initialize(EFFECTID::ID_TENGSWORD, &m_swordBoneTracer);

    RwV3d vecOffset = { -0.3f, 0.1f, 0.0f };
    RwV3d vecDirection = { -0.6f, -0.1f, 0.0f };

    m_swordBoneTracer.Initialize(&Compositor(), 7);
    m_swordBoneTracer.SetOffset(&vecOffset);
    m_swordBoneTracer.SetDirection(&vecDirection);
    m_swordBoneTracer.SetEnableDirection(true);
};


void C090UltimateShredder::SetupBodyElecEffect(void)
{
    m_elecEffectMgr.SetCharacter(&Compositor());
    m_elecEffectMgr.SetDispType(true, false);
    m_elecEffectMgr.SetUpdateStepTime(0.05f);

    m_elecEffectTimer.SetAlarmTime(0.7f);
    m_elecEffectTimer.SetElapseTime(1.0f);
};


void C090UltimateShredder::SetupSharedData(void)
{
    m_sharedData.bJumpAttack = false;

    SetSharedData(&m_sharedData);
};