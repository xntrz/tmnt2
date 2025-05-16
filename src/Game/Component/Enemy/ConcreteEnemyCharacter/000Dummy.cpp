#include "000Dummy.hpp"

#include "Game/Component/Enemy/EnemyParameter.hpp"


C000Dummy::C000Dummy(void)
: CBaseGrapplerEnemyChr(ENEMYID::ID_DUMMY)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = nullptr;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 3.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.15f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 0.8f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 1000;
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

    parameter.m_iFrequencyMax = 0;
    parameter.m_puFrequencyParam = nullptr;

    bool bReplace = false;
    Initialize(&parameter, bReplace);

    /* CBaseGrapplerEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,   new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,  new CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,   new CBaseGrapplerEnemyChr::CAttackStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,   new CBaseGrapplerEnemyChr::CAttackStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN, new CBaseGrapplerEnemyChr::CAttackStatusObserver);

    /* init appear status */
    uint8 appearType = FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case ENEMYTYPES::APPEARTYPE_WALK_SLOW:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(4.5f));
        break;

    case ENEMYTYPES::APPEARTYPE_WALK_FAST:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(1.5f));
        break;

    case ENEMYTYPES::APPEARTYPE_NONE:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_FALL:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_FLY_UP:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_FLY_DOWN:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_MOTION:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CMotionAppearStatus);
        break;

    default:
        break;
    };
};


/*virtual*/ C000Dummy::~C000Dummy(void)
{
    ;
};