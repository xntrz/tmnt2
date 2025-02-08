#include "056Kabuto.hpp"

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/056KabutoAI.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseGunnerEnemyChr.hpp"


/*virtual*/ float C056Kabuto::CMoveStatusObserver::AltitudeNoise(void) /*override*/
{
    return 0.0f;
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C056Kabuto, { EFFECTID::ID_KO_EXPL,
                                        EFFECTID::ID_BLACKSMOKE,
                                        EFFECTID::ID_LASER_BEAM_ALL,
                                        EFFECTID::ID_LASER_FIRE_ALL,
                                        EFFECTID::ID_LASER_HIT_ALL });


C056Kabuto::C056Kabuto(void)
: CBaseFlyingEnemyChr(ENEMYID::ID_KABUTO)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C056KabutoAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.8f;

    parameter.m_feature.m_fWalkMoveSpeed        = 6.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 0.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.2f;
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
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 20.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 8.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 0, 35, 0 },   
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
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 0, 80, 0 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 8,  0 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    { 0, 30, 0 },
        /* FREQUENCY_FIRE_REPEATABLE        21 */    { 0, 15, 0 },
        /* FREQUENCY_FIRE_REPEATCOUNT       22 */    { 0, 2,  0 },
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
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,      new CBaseFlyingEnemyChr::CDeathStatusObserver(EFFECT_GENERIC::DEATHTYPE_MONSTER, 1.0f));

    /* CBaseGunnerEnemyChr status */
    int32 repeatCount  = static_cast<int32>(FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCOUNT));
    bool  bRepeatChase = (FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCHASE) != 0);

    CBaseGunnerEnemyChr::CFireStatusObserver::PARAM fireParam;
    fireParam.type          = CBaseGunnerEnemyChr::CFireStatusObserver::PARAM::TYPE_MAGIC;
    fireParam.magicId       = MAGICID::ID_LASER_BEAM_ALL;
    fireParam.repeatCount   = repeatCount;
    fireParam.bRepeatChase  = bRepeatChase;
    fireParam.boneId        = 0;

    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_B,     new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_BB,    new CBaseGunnerEnemyChr::CFireStatusObserver(&fireParam));

    /* C056Kabuto status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new CMoveStatusObserver);

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
    SetAttackAltitude(2.0f);
};


/*virtual*/ C056Kabuto::~C056Kabuto(void)
{
    ;
};