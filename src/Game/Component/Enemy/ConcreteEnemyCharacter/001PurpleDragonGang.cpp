#include "001PurpleDragonGang.hpp"

#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Sound/GameSound.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/001PurpleDragonGangAI.hpp"


/*virtual*/ void C001PurpleDragonGang::CAttackGrenadeStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_C, true);
};


/*virtual*/ C001PurpleDragonGang::CAttackGrenadeStatusObserver::RESULT
C001PurpleDragonGang::CAttackGrenadeStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        CGameSound::PlayAttackSE(&EnemyChr().Compositor());

        /* init pos */
        int32 posParameterNo = 0;
        RwV3d vOfsPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetOffsetPosition(&vOfsPos, posParameterNo);

        RwV3d vPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vPos);

        Math::Vec3_Add(&vPos, &vPos, &vOfsPos);

        /* init dir */
        uint8 freqValue = EnemyChr().AIModerator().AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE);

        float fHigh = (1.0f - (static_cast<float>(freqValue) * 0.01f));
        float fLow  = -fHigh;

        float fConvergence = Math::RandFloatRange(fLow, fHigh) * MATH_DEG2RAD(9.0f);
        float fDir = EnemyChr().Compositor().GetDirection();

        float fShotDir = (fDir + fConvergence);

        /* make shot */
        CShotManager::Shot(SHOTID::ID_GRENADE, &vPos, fShotDir, &EnemyChr().Compositor(), MATH_DEG2RAD(15.0f), 5.0f);
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C001PurpleDragonGang::CAttackGrenadeStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C001PurpleDragonGang, { EFFECTID::ID_KO_FLASH });


C001PurpleDragonGang::C001PurpleDragonGang(void)
: CBaseGrapplerEnemyChr(ENEMYID::ID_PURPLE_DRAGON_GANG)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C001PurpleDragonGangAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 1.9f;
    parameter.m_feature.m_fRunMoveSpeed         = 4.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 2.4f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 60;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.4f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.8f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.2f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 20.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 2.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 0, 70, 0 },   
        /* FREQUENCY_ATTACK_AA              1  */    { 0, 50, 0 },
        /* FREQUENCY_ATTACK_AAA             2  */    { 0, 50, 0 },
        /* FREQUENCY_ATTACK_B               3  */    { 0, 40, 0 },
        /* FREQUENCY_ATTACK_C               4  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_RUN             5  */    { 0, 30, 0 },
        /* FREQUENCY_GUARD_A                6  */    { 0, 50, 0 },
        /* FREQUENCY_GUARD_B                7  */    { 0, 10, 0 },
        /* FREQUENCY_GUARD_SHOT             8  */    { 0, 0,  0 },
        /* FREQUENCY_GUARD_JUMP_ATTACK      9  */    { 0, 10, 0 },
        /* FREQUENCY_GUARD_RUN_ATTACK       10 */    { 0, 10, 0 },
        /* FREQUENCY_GUARD_THROW            11 */    { 0, 0,  0 },
        /* FREQUENCY_MOVE_APPROACH          12 */    { 0, 0,  0 },
        /* FREQUENCY_MOVE_FORMATION         13 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_ATTACK         14 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_MOVE           15 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_ESCAPE         16 */    { 0, 0,  0 },
        /* FREQUENCY_TRIGGER_REDUCTION_HP   17 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 0, 50, 0 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 10, 0 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    { 0, 20, 0 },
        /* FREQUENCY_FIRE_REPEATABLE        21 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_REPEATCOUNT       22 */    { 0, 1,  0 },
        /* FREQUENCY_FIRE_REPEATCHASE       23 */    { 0, 0,  0 },
        /* FREQUENCY_COMMON_ATTACKINTERVAL  24 */    { 0, 2,  0 },
        /* FREQUENCY_COMMON_1               25 */    { 0, 0,  0 },
        /* FREQUENCY_COMMON_2               26 */    { 0, 0,  0 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = true;
    Initialize(&parameter, bReplace);

    /* CBaseGeneralEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK_LEFT,  new CBaseGeneralEnemyChr::CSideWalkStatusObserver(0.78f, ENEMYTYPES::MOTIONNAMES::WALKL));
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK_RIGHT, new CBaseGeneralEnemyChr::CSideWalkStatusObserver(0.78f, ENEMYTYPES::MOTIONNAMES::WALKR));
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,      new CBaseGeneralEnemyChr::CGuardStatusObserver(1.0f));

    /* CBaseGrapplerEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,   new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,  new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AAA, new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,   new CBaseGrapplerEnemyChr::CAttackStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN, new CBaseGrapplerEnemyChr::CAttackStatusObserver);

    /* C001PurpleDragonGang status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,   new C001PurpleDragonGang::CAttackGrenadeStatusObserver);

    /* init appear status */
    uint8 appearType = FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case 0:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(5.5f));
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
};


/*virtual*/ C001PurpleDragonGang::~C001PurpleDragonGang(void)
{
    ;
};