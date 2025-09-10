#include "098Fugitoid.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


/*virtual*/ void C098Fugitoid::CCrashWallStatusObserver::OnStart(void) /*override*/
{
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4420));
    CCommonEnemyObserver::CCrashWallStatus::OnStart();
};


//
// *********************************************************************************
//


/*virtual*/ void C098Fugitoid::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DEATH, true);

    m_bMessageRequest = false;
};


/*virtual*/ C098Fugitoid::CDeathStatusObserver::RESULT
C098Fugitoid::CDeathStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd() && !m_bMessageRequest)
    {
        m_bMessageRequest = true;
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8216));
        CMessageManager::Request(SEGROUPID::VALUE(57));
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C098Fugitoid::CDeathStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


C098Fugitoid::C098Fugitoid(void)
: CBaseGeneralEnemyChr(ENEMYID::ID_FUGITOID)   
{
    /* CBaseGeneralEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new CBaseGeneralEnemyChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new CBaseGeneralEnemyChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CBaseGeneralEnemyChr::CIdleStatusObserver);

    /* C098Fugitoid status */
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new C098Fugitoid::CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FRONT,            new C098Fugitoid::CCrashWallStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_BACK,             new C098Fugitoid::CCrashWallStatusObserver);
    
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                     new CCommonEnemyObserver::CNoneAppearStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CCommonEnemyObserver::CKnockFrontStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CCommonEnemyObserver::CKnockBackStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,              new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,               new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,        new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,         new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT,       new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK,        new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT,  new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK,   new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CDindleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                       new CCommonEnemyObserver::CStunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_SLEEP,                      new CCommonEnemyObserver::CSleepStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CFreezeStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_BIND,                       new CCommonEnemyObserver::CBindStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_CAUGHT,                     new CCommonEnemyObserver::CCaughtStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED,                     new CCommonEnemyObserver::CLiftedStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED_WALK,                new CCommonEnemyObserver::CLiftedWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_FRONT,               new CCommonEnemyObserver::CThrownFrontStatusObserver(true, 1.0f));
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_BACK,                new CCommonEnemyObserver::CThrownBackStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = nullptr;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.8f;

    parameter.m_feature.m_fWalkMoveSpeed        = 0.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 0.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 0.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 80;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.0f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 0.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 0.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    parameter.m_iFrequencyMax    = 0;
    parameter.m_puFrequencyParam = nullptr;

    if (CGameProperty::GetDifficulty() == GAMETYPES::DIFFICULTY_EASY)
        parameter.m_feature.m_iHPMax = static_cast<int32>(static_cast<float>(parameter.m_feature.m_iHPMax) * 1.2f);

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C098Fugitoid::~C098Fugitoid(void)
{
    ;
};


/*virtual*/ void C098Fugitoid::Run(void) /*override*/
{
    CBaseGeneralEnemyChr::Run();
};


/*virtual*/ void C098Fugitoid::OnMessageTouchdown(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS currentStatus = GetStatus();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_FLYAWAY_FRONT:
    case ENEMYTYPES::STATUS_FLYAWAY_BACK:
    case ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT:
    case ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK:
        CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4420));
        break;

    case ENEMYTYPES::STATUS_THROWN_FRONT:
        Compositor().RequestDamage(20);
        CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4420));
        break;

    default:
        break;
    };

    if ((currentStatus == ENEMYTYPES::STATUS_FLYAWAY_FRONT) ||
        (currentStatus == ENEMYTYPES::STATUS_FLYAWAY_BACK))
    {
        int32 damageRequested = Compositor().GetRequestedDamage();
        if (damageRequested > 0)
        {
            CGameObjectManager::SendMessage(&Compositor(),
                                            CHARACTERTYPES::MESSAGEID_RECVDMG,
                                            reinterpret_cast<void*>(damageRequested));

            Compositor().RequestDamage(0);
        };
    };

    CBaseGeneralEnemyChr::OnMessageTouchdown(fHeight);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE C098Fugitoid::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    CHARACTERTYPES::ATTACKTYPE attackType = rCalc.GetAttackType();

    /* always handle if its throw attack from anyone */
    if (attackType == CHARACTERTYPES::ATTACKTYPE_THROW)
        return CBaseGeneralEnemyChr::OnDamage(rCalc);

    /* ignore all PLAYERS attack except throw */
    CGameObject* pAttacker = rCalc.GetAttacker();
    if (pAttacker && (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER))
    {
        CCharacter* pChr = static_cast<CCharacter*>(pAttacker);
        if (pChr->GetCharacterType() == CCharacter::TYPE_PLAYER)
            return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
    };

    return CBaseGeneralEnemyChr::OnDamage(rCalc);
};


/*virtual*/ C098Fugitoid::CHRTYPE C098Fugitoid::GetAttackCharacterType(void) /*override*/
{
    return CHRTYPE_PLAYER;
};