#include "031FootSumoNinja.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/System/Sound/GameSound.hpp"


static const RwV3d BANDANA_OFFSET = { 0.0f, 0.174f, -0.016f };


/*virtual*/ C031FootSumoNinja::CWalkStatusObserver::RESULT
C031FootSumoNinja::CWalkStatusObserver::Observing(void) /*override*/
{
    return CBaseGrapplerEnemyChr::CWalkStatusObserver::Observing();
};


//
// *********************************************************************************
//


/*virtual*/ C031FootSumoNinja::CRunStatusObserver::RESULT
C031FootSumoNinja::CRunStatusObserver::Observing(void) /*override*/
{
    return CBaseGrapplerEnemyChr::CRunStatusObserver::Observing();
};


//
// *********************************************************************************
//


/*virtual*/ void C031FootSumoNinja::CRunAttackStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::RUN, bForce);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().Compositor().SetMotionSpeed(1.5f);

    EnemyChr().Compositor().GetPosition(&m_vStartPos);
    m_vStartPos.y = 0.0f;

    m_fTimer = Math::RandFloatRange(1.5f, 3.5f);

    RwV3d vOffset = Math::VECTOR3_ZERO;
    float fDir = EnemyChr().Compositor().GetDirection();

    m_hEffect = CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH2, new CEnemyTracer(&EnemyChr()), &vOffset, fDir);
    ASSERT(m_hEffect);

    if (m_hEffect)
        CEffectManager::SetScale(m_hEffect, 2.0f);
};


/*virtual*/ C031FootSumoNinja::CRunAttackStatusObserver::RESULT
C031FootSumoNinja::CRunAttackStatusObserver::Observing(void) /*override*/
{
    m_fTimer -= CGameProperty::GetElapsedTime();
    if (m_fTimer < 0.0f)
        return RESULT_END;

    float fRadiusOfAction = EnemyChr().AICharacteristic().m_fRadiusOfAction;
    float fDist = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &m_vStartPos);

    if (fDist > fRadiusOfAction)
        return RESULT_END;

    const CCharacter::COLLISIONWALLINFO* pWallInfo = EnemyChr().Compositor().GetCollisionWall();
    ASSERT(pWallInfo != nullptr);

    if (pWallInfo->m_bHit)
        return RESULT_END;

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4334));

    float fRunSpeed = (EnemyChr().CharacterParameter().m_feature.m_fRunMoveSpeed * 1.5f);
    RwV3d vecVel = { 0.0f, 0.0f, fRunSpeed };

    EnemyChr().Compositor().RotateVectorByDirection(&vecVel, &vecVel);
    EnemyChr().Compositor().SetVelocity(&vecVel);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C031FootSumoNinja::CRunAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().Compositor().SetMotionSpeed(1.0f);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    if (m_hEffect)
    {
        if (!CEffectManager::IsEnd(m_hEffect))
        {
            CEffectManager::End(m_hEffect);
            m_hEffect = 0;
        };
    };

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C031FootSumoNinja, { EFFECTID::ID_KO_FLASH,
                                               EFFECTID::ID_ALL_DASH2 });


C031FootSumoNinja::C031FootSumoNinja(void)
: CBaseGrapplerEnemyChr(ENEMYID::ID_FOOT_SUMO_NINJA)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseGrapplerAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 3.8f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.5f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 0.8f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 250;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.2f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.4f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 10.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 3.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 0, 60, 0 },   
        /* FREQUENCY_ATTACK_AA              1  */    { 0, 50, 0 },
        /* FREQUENCY_ATTACK_AAA             2  */    { 0, 40, 0 },
        /* FREQUENCY_ATTACK_B               3  */    { 0, 50, 0 },
        /* FREQUENCY_ATTACK_C               4  */    { 0, 30, 0 },
        /* FREQUENCY_ATTACK_RUN             5  */    { 0, 30, 0 },
        /* FREQUENCY_GUARD_A                6  */    { 0, 10, 0 },
        /* FREQUENCY_GUARD_B                7  */    { 0, 20, 0 },
        /* FREQUENCY_GUARD_SHOT             8  */    { 0, 40, 0 },
        /* FREQUENCY_GUARD_JUMP_ATTACK      9  */    { 0, 0,  0 },
        /* FREQUENCY_GUARD_RUN_ATTACK       10 */    { 0, 10, 0 },
        /* FREQUENCY_GUARD_THROW            11 */    { 0, 0,  0 },
        /* FREQUENCY_MOVE_APPROACH          12 */    { 0, 0,  0 },
        /* FREQUENCY_MOVE_FORMATION         13 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_ATTACK         14 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_MOVE           15 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_ESCAPE         16 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_REDUCTION_HP   17 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_REPEATABLE        21 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_REPEATCOUNT       22 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_REPEATCHASE       23 */    { 0, 0,  0 },
        /* FREQUENCY_COMMON_ATTACKINTERVAL  24 */    { 0, 0,  0 },
        /* FREQUENCY_COMMON_1               25 */    { 0, 0,  0 },
        /* FREQUENCY_COMMON_2               26 */    { 0, 0,  0 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = true;
    Initialize(&parameter, bReplace);

    /* C031FootSumoNinja status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new C031FootSumoNinja::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,        new C031FootSumoNinja::CRunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN, new C031FootSumoNinja::CRunAttackStatusObserver);
    
    /* CBaseGrapplerEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,   new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,  new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AAA, new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,   new CBaseGrapplerEnemyChr::CAttackStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,   new CBaseGrapplerEnemyChr::CAttackStatusObserver);

    /* init appear status */
    uint8 appearType = FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case 0:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(4.5f));
        break;

    case 1:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(3.0f));
        break;

    case 2:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case 3:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CFallAppearStatus);
        break;

    case 4:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case 5:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case 6:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    default:
        break;
    };

        /* init modules */
    CCharacterCompositor* pChrCompositor = &Compositor();

    CBandanaModule* pBandanaModule = new CBandanaModule(pChrCompositor,
                                                        pChrCompositor->GetModel(),
                                                        8,
                                                        &BANDANA_OFFSET,
                                                        CBandanaModule::BANDANACOLOR_BLACK);

    pChrCompositor->IncludeModule(pBandanaModule);
};


/*virtual*/ C031FootSumoNinja::~C031FootSumoNinja(void)
{
    ;
};


/*virtual*/ void C031FootSumoNinja::OnMessageTouchdown(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statucCurr = GetStatus();
    switch (statucCurr)
    {
    case ENEMYTYPES::STATUS_AERIAL:
    case ENEMYTYPES::STATUS_JUMP:
    case ENEMYTYPES::STATUS_JUMP_2ND:
    case ENEMYTYPES::STATUS_JUMP_ATTACK:
        CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4334));
        break;

    default:
        break;
    };

    CBaseGrapplerEnemyChr::OnMessageTouchdown(fHeight);
};


/*virtual*/ CHARACTERTYPES::DEFENCERSTATUSFLAG C031FootSumoNinja::CheckDefenceStatusFlag(void) /*override*/
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceFlag = CBaseGeneralEnemyChr::CheckDefenceStatusFlag();
    defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW;
    return defenceFlag;
};
