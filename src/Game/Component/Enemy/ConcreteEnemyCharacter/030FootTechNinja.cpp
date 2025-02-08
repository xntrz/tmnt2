#include "030FootTechNinja.hpp"

#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseFootNinjaChr.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/030FootTechNinjaAI.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ C030FootTechNinja::CWalkStatusObserver::RESULT
C030FootTechNinja::CWalkStatusObserver::Observing(void) /*override*/
{
    return CBaseGrapplerEnemyChr::CWalkStatusObserver::Observing();
};


//
// *********************************************************************************
//


/*virtual*/ C030FootTechNinja::CRunStatusObserver::RESULT
C030FootTechNinja::CRunStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4323));
    
    return CBaseGrapplerEnemyChr::CRunStatusObserver::Observing();
};


//
// *********************************************************************************
//


/*virtual*/ C030FootTechNinja::CMotionAppearStatusObserver::RESULT
C030FootTechNinja::CMotionAppearStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4323));

    return CCommonEnemyObserver::CMotionAppearStatus::Observing();
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C030FootTechNinja, { EFFECTID::ID_KO_FLASH,
                                               EFFECTID::ID_WINDWARP_START,
                                               EFFECTID::ID_WINDWARP_OUT });


C030FootTechNinja::C030FootTechNinja(void)
: CBaseFootNinjaChr(ENEMYID::ID_FOOT_TECH_NINJA)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C030FootTechNinjaAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.2f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.4f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.5f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.6f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 8.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 7.5f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 100;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.4f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 15.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 3.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 0, 29, 0 },   
        /* FREQUENCY_ATTACK_AA              1  */    { 0, 25, 0 },
        /* FREQUENCY_ATTACK_AAA             2  */    { 0, 21, 0 },
        /* FREQUENCY_ATTACK_B               3  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_C               4  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_RUN             5  */    { 0, 0,  0 },
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

    /* CBaseGeneralEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,      new CBaseGeneralEnemyChr::CGuardStatusObserver(1.8f));

    /* C030FootTechNinja status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new C030FootTechNinja::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,        new C030FootTechNinja::CRunStatusObserver);

    /* CBaseFootNinjaChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_SPECIAL,    new CBaseFootNinjaChr::CBackAwayStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_ATTACK,new CBaseFootNinjaChr::CJAttackStatusObserver);    

    /* CBaseGrapplerEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,   new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,  new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AAA, new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);

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
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new C030FootTechNinja::CMotionAppearStatusObserver);
        break;

    default:
        break;
    };
};


/*virtual*/ C030FootTechNinja::~C030FootTechNinja(void)
{
    ;
};


/*virtual*/ void C030FootTechNinja::OnMessageTouchdown(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statucCurr = GetStatus();
    switch (statucCurr)
    {
    case ENEMYTYPES::STATUS_AERIAL:
    case ENEMYTYPES::STATUS_JUMP:
    case ENEMYTYPES::STATUS_JUMP_2ND:
    case ENEMYTYPES::STATUS_JUMP_ATTACK:
        CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4323));
        break;

    default:
        break;
    };

    CBaseFootNinjaChr::OnMessageTouchdown(fHeight);
};


/*virtual*/ void C030FootTechNinja::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CHitCatchData::RESULT catchResult = pCatch->GetResult();

    if ((catchResult == CHitCatchData::RESULT_HIT)        ||
        (catchResult == CHitCatchData::RESULT_THROWFRONT) ||
        (catchResult == CHitCatchData::RESULT_THROWBACK))
    {
        static_cast<C030FootTechNinjaAI&>(AIModerator()).SetInvisible(false);
    };

    CBaseFootNinjaChr::OnMessageAttackResult(pCatch);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C030FootTechNinja::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    static_cast<C030FootTechNinjaAI&>(AIModerator()).SetInvisible(false);

    return CBaseFootNinjaChr::OnDamage(rCalc);
};


/*virtual*/ void C030FootTechNinja::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    CBaseFootNinjaChr::OnMessageReceivedDamage(iAmount);

    float fHPMax = static_cast<float>(CharacterParameter().m_feature.m_iHPMax);
    float fThirdOfHPMax = (fHPMax / 3.0f);

    float fHPNow = static_cast<float>(CharacterParameter().m_feature.m_iHP);

    if (fHPNow < fThirdOfHPMax)
        static_cast<C030FootTechNinjaAI&>(AIModerator()).CancelInvisible();
};