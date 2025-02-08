#include "015CaptainOfFederation.hpp"

#include "Game/Component/Enemy/EnemyParameter.hpp"



DEFINE_NEEDED_EFFECTS_FOR(C015CaptainOfFederation, { EFFECTID::ID_KO_FLASH,
                                                     EFFECTID::ID_LASER_BEAM_FED,
                                                     EFFECTID::ID_LASER_FIRE_FED,
                                                     EFFECTID::ID_LASER_HIT_FED });



C015CaptainOfFederation::C015CaptainOfFederation(void)
: CBaseGunnerEnemyChr(ENEMYID::ID_CAPTAIN_OF_FEDERATION)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseGunnerAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.8f;

    parameter.m_feature.m_fWalkMoveSpeed        = 1.9f;
    parameter.m_feature.m_fRunMoveSpeed         = 4.9f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.8f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 35;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.4f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.3f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.2f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 20.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 15.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 0, 0,  0 },   
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
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 0, 50, 0 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 15, 0 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    { 0, 50, 0 },
        /* FREQUENCY_FIRE_REPEATABLE        21 */    { 0, 30, 0 },
        /* FREQUENCY_FIRE_REPEATCOUNT       22 */    { 0, 3,  0 },
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
    AttachStatusObserver(ENEMYTYPES::STATUS_TURN,       new CBaseGeneralEnemyChr::CTurnStatusObserver(75.0f, ENEMYTYPES::MOTIONNAMES::TURNL, ENEMYTYPES::MOTIONNAMES::TURNR));

    /* CBaseGunnerEnemyChr status */
    int32 repeatCount = static_cast<int32>(FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCOUNT));
    bool bRepeatChase = (FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCHASE) != 0);
    
    CFireStatusObserver::PARAM fireParam;
    fireParam.type          = CFireStatusObserver::PARAM::TYPE_MAGIC;
    fireParam.magicId       = MAGICID::ID_LASER_BEAM_FED;
    fireParam.repeatCount   = repeatCount;
    fireParam.bRepeatChase  = bRepeatChase;
    fireParam.boneId        = 0;

    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_A,     new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_B,     new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_C,     new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_AA,    new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_BB,    new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_CC,    new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));

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
};


/*virtual*/ C015CaptainOfFederation::~C015CaptainOfFederation(void)
{
    ;
};