#include "057UtromSaucer.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/057UtromSaucerAI.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseGunnerEnemyChr.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ float C057UtromSaucer::CMoveStatusObserver::AltitudeNoise(void) /*override*/
{
    return 0.0f;
};


//
// *********************************************************************************
//


C057UtromSaucer::CFireStatusObserver::CFireStatusObserver(MAGICID::VALUE magicId,
                                                          int32 repeatCount,
                                                          bool bRepeatChase)
: m_magicId(magicId)
, m_repeatCount(repeatCount)
, m_fireNo(0)
, m_targetNo(-1)
, m_offsetNo(0)
, m_vecOrderAt(Math::VECTOR3_ZERO)
, m_bRepeatChase(bRepeatChase)
, m_bContinuationFire(false)
{
    ;
};


/*virtual*/ void C057UtromSaucer::CFireStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    m_fireNo            = 0;
    m_targetNo          = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_vecOrderAt        = EnemyChr().AIThinkOrder().OrderAttack().m_vAt;
    m_bContinuationFire = (Status() == ENEMYTYPES::STATUS_FIRE_BB);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_A, bForce);
};


/*virtual*/ C057UtromSaucer::CFireStatusObserver::RESULT
C057UtromSaucer::CFireStatusObserver::Observing(void) /*override*/
{
    if (m_bRepeatChase)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_targetNo);
        if (pPlayerChr)
        {
            RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
            pPlayerChr->GetFootPosition(&vecFootPosPlayer);

            RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetFootPosition(&vecFootPosEnemy);

            float fDir = CEnemyUtils::GetDirection(&vecFootPosEnemy, &vecFootPosPlayer);
            float fRotRate = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
            EnemyChr().Compositor().RotateDirection(fDir, fRotRate);
        };
    };

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        Fire();
        CGameSound::PlayAttackSE(&EnemyChr().Compositor());
    };

    if (!EnemyChr().Compositor().IsMotionEnd())
        return RESULT_CONTINUE;

    if (!m_bContinuationFire)
        return RESULT_END;

    if (++m_fireNo < m_repeatCount)
    {
        bool bForce = true;
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_A, bForce);
        return RESULT_CONTINUE;
    };

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS C057UtromSaucer::CFireStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ void C057UtromSaucer::CFireStatusObserver::Fire(void)
{
    RwV3d vecOfsPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetOffsetPosition(&vecOfsPos, m_offsetNo);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

	Math::Vec3_Add(&vecPos, &vecPos, &vecOfsPos);

    float fConvergenceFreq = static_cast<float>(EnemyChr().AIModerator().AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE));

    float fHigh = 1.0f - (fConvergenceFreq * 0.01f);
    float fLow  = -fHigh;

    float fConvergence = Math::RandFloatRange(fLow, fHigh) * MATH_DEG2RAD(1.8f);
    float fDir = EnemyChr().Compositor().GetDirection();

    float fYaw = (fDir + fConvergence);
    float fPith = m_vecOrderAt.x;

    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    Math::Matrix_RotationYawPitchRoll(&matRot, fYaw, fPith, 0.0f);

    RwV3d vecDir = Math::VECTOR3_AXIS_Z;
    RwV3dTransformVector(&vecDir, &vecDir, &matRot);

    uint32 hShot = CShotManager::Shot(m_magicId, &vecPos, &vecDir, &EnemyChr().Compositor(), MATH_DEG2RAD(15.0f));
    ASSERT(hShot);
};


//
// *********************************************************************************
//


C057UtromSaucer::CDeathStatusObserver::CDeathStatusObserver(float fSparkPlaytime)
: m_fSparkPlaytime(fSparkPlaytime)
, m_fTimer(0.0f)
{
    ;
};


/*virtual*/ void C057UtromSaucer::CDeathStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().SetEnableCatchHit(false);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DEATH, bForce);

    RwV3d vecAttackDir = EnemyChr().Compositor().AttackParameter().m_vDirection;
    EnemyChr().Compositor().SetDirectionFromVector(&vecAttackDir);

    RwV3d vecAttackVel = EnemyChr().Compositor().AttackParameter().m_vVelocity;
    EnemyChr().Compositor().SetVelocity(&vecAttackVel);

    m_fTimer = m_fSparkPlaytime;

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4337));
};


/*virtual*/ C057UtromSaucer::CDeathStatusObserver::RESULT
C057UtromSaucer::CDeathStatusObserver::Observing(void) /*override*/
{
    const CCharacterCompositor::COLLISIONGROUNDINFO* pGroundInfo = EnemyChr().Compositor().GetCollisionGround();
    ASSERT(pGroundInfo != nullptr);

    if (pGroundInfo->m_bHit)
        return RESULT_END;

    m_fTimer -= CGameProperty::GetElapsedTime();

    if (m_fTimer <= 0.0f)
        return RESULT_END;

    float fElapsedRate = (m_fTimer / m_fSparkPlaytime);
    float fRand = Math::RandFloat();

    if (fElapsedRate < fRand)
    {
        RwV3d vecPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecPos);

        float fRadius = (EnemyChr().Compositor().GetCollisionParameter().m_fRadius * 1.5f);

        float fLow = -fRadius;
        float fHigh = fRadius;

        vecPos.x += Math::RandFloatRange(fLow, fHigh);
        vecPos.z += Math::RandFloatRange(fLow, fHigh);
        vecPos.y += Math::RandFloatRange(0.0f, fHigh * 0.5f);

        uint32 hEffect = CEffectManager::Play(EFFECTID::ID_SPARKS, &vecPos);
        ASSERT(hEffect);

        if (hEffect)
        {
            RwMatrix matRot;
            RwMatrixSetIdentityMacro(&matRot);
            Math::Matrix_RotationYawPitchRoll(&matRot, 0.0f, -MATH_DEG2RAD(90.0f), 0.0f);

            CEffectManager::SetDirection(hEffect, &matRot);
            CEffectManager::SetScale(hEffect, 0.4f);
        };
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C057UtromSaucer::CDeathStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(true);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_MACHINE, &vecPos, 1.5f);

    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C057UtromSaucer, { EFFECTID::ID_KO_EXPL,
                                             EFFECTID::ID_SPARKS,
                                             EFFECTID::ID_LASER_BEAM_FED,
                                             EFFECTID::ID_LASER_FIRE_FED,
                                             EFFECTID::ID_LASER_HIT_FED });


C057UtromSaucer::C057UtromSaucer(void)
: CBaseFlyingEnemyChr(ENEMYID::ID_UTROM_SAUCER)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C057UtromSaucerAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.2f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.8f;
    parameter.m_feature.m_fRunMoveSpeed         = 0.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.2f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 0.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 0.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.0f;
    parameter.m_feature.m_iHPMax                = 5;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.4f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.4f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 15.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 5.0f;
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
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 5,  0 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    { 0, 25, 0 },
        /* FREQUENCY_FIRE_REPEATABLE        21 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_REPEATCOUNT       22 */    { 0, 1,  0 },
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
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,       new CBaseFlyingEnemyChr::CStunStatusObserver);

    /* C057UtromSaucer status */
    int32 repeatCount = static_cast<int32>(FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCOUNT));
    bool  bRepeatChase = (FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCHASE) != 0);
    MAGICID::VALUE magicId = MAGICID::ID_LASER_BEAM_FED;

    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,      new CDeathStatusObserver(1.5f));
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new CMoveStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_B,     new CFireStatusObserver(magicId, repeatCount, bRepeatChase));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_BB,    new CFireStatusObserver(magicId, repeatCount, bRepeatChase));

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
    SetAttackAltitude(0.8f);
};


/*virtual*/ C057UtromSaucer::~C057UtromSaucer(void)
{
    ;
};


/*virtual*/ float C057UtromSaucer::GetAltitude(void) const /*override*/
{
    float fAltitude = CBaseFlyingEnemyChr::GetAltitude();

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    Compositor().GetFootPosition(&vecFootPos);

    float fHeight = CWorldMap::GetMapHeight(&vecFootPos);

    if (fHeight <= CWorldMap::INVALID_MAP_HEIGHT)
        return fAltitude;

    float fAttackAltitude = (GetAttackAltitude() + fHeight + 2.0f);

    if (fAttackAltitude >= fAltitude)
        return fAltitude;

    return fAttackAltitude;
};