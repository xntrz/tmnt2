#include "040StoneBiter.hpp"

#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseStoneBiterAI.hpp"


C040StoneBiter::C040StoneBiter(void)
: CBaseStoneBiterChr(ENEMYID::ID_STONE_BITER)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseStoneBiterAI::Intsance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 4.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 3.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 6.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 0.7f;
    parameter.m_feature.m_fRotateRate           = 0.8f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 8.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 3.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.2f;
    parameter.m_feature.m_iHPMax                = 150;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 20.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 4.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 50, 50, 50 },   
        /* FREQUENCY_ATTACK_AA              1  */    { 50, 50, 50 },
        /* FREQUENCY_ATTACK_AAA             2  */    {  0,  0,  0 },
        /* FREQUENCY_ATTACK_B               3  */    { 30, 30, 30 },
        /* FREQUENCY_ATTACK_C               4  */    { 50, 50, 50 },
        /* FREQUENCY_ATTACK_RUN             5  */    { 10, 10, 10 },
        /* FREQUENCY_GUARD_A                6  */    { 10, 10, 10 },
        /* FREQUENCY_GUARD_B                7  */    { 10, 10, 10 },
        /* FREQUENCY_GUARD_SHOT             8  */    {  0,  0,  0 },
        /* FREQUENCY_GUARD_JUMP_ATTACK      9  */    {  0,  0,  0 },
        /* FREQUENCY_GUARD_RUN_ATTACK       10 */    {  0,  0,  0 },
        /* FREQUENCY_GUARD_THROW            11 */    {  0,  0,  0 },
        /* FREQUENCY_MOVE_APPROACH          12 */    {  0,  0,  0 },
        /* FREQUENCY_MOVE_FORMATION         13 */    {  0,  0,  0 },
        /* FREQUENCY_TRIGGER_ATTACK         14 */    {  0,  0,  0 },
        /* FREQUENCY_TRIGGER_MOVE           15 */    {  0,  0,  0 },
        /* FREQUENCY_TRIGGER_ESCAPE         16 */    {  0,  0,  0 },
        /* FREQUENCY_TRIGGER_REDUCTION_HP   17 */    {  0,  0,  0 },
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 60, 60, 60 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 10, 10, 10 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    {  0,  0,  0 },
        /* FREQUENCY_FIRE_REPEATABLE        21 */    {  0,  0,  0 },
        /* FREQUENCY_FIRE_REPEATCOUNT       22 */    {  0,  0,  0 },
        /* FREQUENCY_FIRE_REPEATCHASE       23 */    {  0,  0,  0 },
        /* FREQUENCY_COMMON_ATTACKINTERVAL  24 */    { 50, 50, 50 },
        /* FREQUENCY_COMMON_1               25 */    {  0,  0,  0 },
        /* FREQUENCY_COMMON_2               26 */    {  0,  0,  0 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = true;
    Initialize(&parameter, bReplace);

    /* init appear status */
    uint8 appearType = FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case ENEMYTYPES::APPEARTYPE_WALK_SLOW:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(8.5f));
        break;

    case ENEMYTYPES::APPEARTYPE_WALK_FAST:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(3.0f));
        break;

    case ENEMYTYPES::APPEARTYPE_NONE:
    case ENEMYTYPES::APPEARTYPE_FLY_UP:
    case ENEMYTYPES::APPEARTYPE_FLY_DOWN:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_FALL:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CFallAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_MOTION:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CBaseStoneBiterChr::CAppearStatusObserver);
        break;

    default:
        break;
    };
};


/*virtual*/ C040StoneBiter::~C040StoneBiter(void)
{
    ;
};