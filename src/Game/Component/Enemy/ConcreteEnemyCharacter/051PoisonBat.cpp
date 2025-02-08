#include "051PoisonBat.hpp"

#include "Game/Component/Enemy/EnemyParameter.hpp"


DEFINE_NEEDED_EFFECTS_FOR(C051PoisonBat, { EFFECTID::ID_KO_SMOKE });


C051PoisonBat::C051PoisonBat(void)
: CBaseBatChr(ENEMYID::ID_POISON_BAT)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseBatAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 0.8f;

    parameter.m_feature.m_fWalkMoveSpeed        = 6.5f;
    parameter.m_feature.m_fRunMoveSpeed         = 0.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 2.4f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 0.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 0.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.0f;
    parameter.m_feature.m_iHPMax                = 20;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 40.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 0, 25, 0 },   
        /* FREQUENCY_ATTACK_AA              1  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_AAA             2  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_B               3  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_C               4  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_RUN             5  */    { 0, 0,  0 },
        /* FREQUENCY_GUARD_A                6  */    { 0, 0,  0 },
        /* FREQUENCY_GUARD_B                7  */    { 0, 0,  0 },
        /* FREQUENCY_GUARD_SHOT             8  */    { 0, 0,  0 },
        /* FREQUENCY_GUARD_JUMP_ATTACK      9  */    { 0, 0,  0 },
        /* FREQUENCY_GUARD_RUN_ATTACK       10 */    { 0, 0,  0 },
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

    /* CBaseFlyingEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,   new CBaseFlyingEnemyChr::CStunStatusObserver);

    /* init appear status */
    uint8 appearType = FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case 0:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case 1:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case 2:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case 3:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case 4:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CBaseFlyingEnemyChr::CAppearFlyStatusObserver(10.0f));
        break;

    case 5:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CBaseFlyingEnemyChr::CAppearFlyStatusObserver(-10.0f));
        break;

    case 6:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    default:
        break;
    };

    /* init altitude */
    SetAttackAltitude(1.0f);
};


/*virtual*/ C051PoisonBat::~C051PoisonBat(void)
{
    ;
};