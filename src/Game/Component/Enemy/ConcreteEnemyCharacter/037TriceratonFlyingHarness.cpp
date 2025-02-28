#include "037TriceratonFlyingHarness.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"


namespace
{
    class CBurnerTracer : public CEnemyTracer
    {
    public:
        inline CBurnerTracer(CEnemyCharacter* pEnemyChr) : CEnemyTracer(pEnemyChr) {};
        inline virtual ~CBurnerTracer(void) {};

        virtual void GetPosition(RwV3d* pvPosition) override;
    };
};


//
// *********************************************************************************
//


/*virtual*/ void CBurnerTracer::GetPosition(RwV3d* pvPosition) /*override*/
{
    CGameObject* pObj = CGameObjectManager::GetObject(m_hObj);
    if (!pObj)
    {
        *pvPosition = Math::VECTOR3_ZERO;
        return;
    };

    CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pObj);

    RwV3d vecOfsPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetOffsetPosition(&vecOfsPos, 2); // TODO offset no naming

    RwV3d vecPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetPosition(&vecPos);

    Math::Vec3_Add(pvPosition, &vecPos, &vecOfsPos);
};


//
// *********************************************************************************
//


/*virtual*/ void C037TriceratonFlyingHarness::CMoveStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    m_fMoveSpeed          = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed;
    m_fRotRate            = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
    m_fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_order               = EnemyChr().AIThinkOrder().GetOrder();
    m_fDirection          = static_cast<C037TriceratonFlyingHarnessAI&>(EnemyChr().AIModerator()).OnGetDirection();
};


/*virtual*/ C037TriceratonFlyingHarness::CMoveStatusObserver::RESULT
C037TriceratonFlyingHarness::CMoveStatusObserver::Observing(void) /*override*/
{
    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order != m_order)
        return RESULT_END;

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    RwV3d vecAt = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vecAt);

    Direction(&vecPos, &vecAt);

    int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);
    float fAltitude = 0.0f;

    if (moveTarget)
    {
        fAltitude = GetAttackAltitude();
        fAltitude += vecAt.y;
    }
    else
    {
        fAltitude = GetAltitude();
    };

    m_fAltitude = fAltitude;
    vecAt.y = m_fAltitude;

    Velocity(&vecPos, &vecAt);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C037TriceratonFlyingHarness::CMoveStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ float C037TriceratonFlyingHarness::CMoveStatusObserver::AltitudeNoise(void)
{
    return 0.0f;
};


void C037TriceratonFlyingHarness::CMoveStatusObserver::Direction(const RwV3d* vecPos, const RwV3d* vecAt)
{
    float fDir = 0.0f;

    if (Math::FEqual(vecPos->x, vecAt->x) &&
        Math::FEqual(vecPos->z, vecAt->z))
    {
        fDir = EnemyChr().Compositor().GetDirection();
    }
    else
    {
        float fPrevDir = EnemyChr().Compositor().GetDirection();

        fDir = CEnemyUtils::GetDirection(vecPos, vecAt);
        EnemyChr().Compositor().SetDirection(m_fDirection);
        EnemyChr().Compositor().RotateDirection(fDir, m_fRotRate);

        fDir = EnemyChr().Compositor().GetDirection();

        EnemyChr().Compositor().SetDirection(fPrevDir);
    };

    m_fDirection = fDir;
    static_cast<C037TriceratonFlyingHarnessAI&>(EnemyChr().AIModerator()).OnSetDirection(m_fDirection);

    CPlayerCharacter* pPlayerChr = static_cast<C037TriceratonFlyingHarnessAI&>(EnemyChr().AIModerator()).OnGetTarget();
    if (pPlayerChr)
    {
        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

		if (Math::FEqual(vecPos->x, vecFootPosPlayer.x) &&
			Math::FEqual(vecPos->z, vecFootPosPlayer.z))
			fDir = EnemyChr().Compositor().GetDirection();
		else
			fDir = CEnemyUtils::GetDirection(vecPos, &vecFootPosPlayer);
    };

    EnemyChr().Compositor().RotateDirection(fDir, m_fRotRate);
};


void C037TriceratonFlyingHarness::CMoveStatusObserver::Velocity(const RwV3d* vecPos, const RwV3d* vecAt)
{
    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    Math::Matrix_RotationYawPitchRoll(&matRot, m_fDirection, 0.0f, 0.0f);

    RwV3d vecVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
    RwV3dTransformVector(&vecVelocity, &vecVelocity, &matRot);

    RwV3d vecHeightPos = *vecPos;
    RwV3d vecHeightAt = *vecAt;

    int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);

    if (moveTarget)
    {
        int32 targetNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);

        if (pPlayerChr)
        {
            PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
            CAIUtils::PLAYER_STATE_FLAG playerStateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);
            if (playerStateFlag & CAIUtils::PLAYER_STATE_AERIAL)
                vecHeightAt.y = vecHeightPos.y;
        };
    }
    else
    {
        float fPatrolHeight = EnemyChr().CharacterParameter().m_feature.m_vPatrolOrigin.y;

        if (vecHeightAt.y >= fPatrolHeight)
            fPatrolHeight = vecHeightAt.y;

        vecHeightAt.y = fPatrolHeight;
    };

    float fRadius = EnemyChr().Compositor().GetCollisionParameter().m_fRadius;

    RwV3d vecStep = Math::VECTOR3_ZERO;

    Math::Vec3_Normalize(&vecStep, &vecVelocity);
    Math::Vec3_Scale(&vecStep, &vecStep, (fRadius * 2.0f));

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);
    Math::Vec3_Add(&vecMyPos, &vecMyPos, &vecStep);

    vecMyPos.y += 10.0f;

    float fHeight = CWorldMap::GetMapHeight(&vecMyPos, fRadius);
    fHeight += GetAttackAltitude();

    if (vecHeightAt.y >= fHeight)
        fHeight = vecHeightAt.y;

    vecHeightAt.y = fHeight;

    vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);

    vecVelocity.y = CalcAltitude(vecHeightAt.y, vecMyPos.y, m_fMoveSpeed);
    vecVelocity.y += AltitudeNoise();

    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


//
// *********************************************************************************
//


/*virtual*/ void C037TriceratonFlyingHarness::CFireStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    MotionRegist(MOTIONID_A, ENEMYTYPES::MOTIONNAMES::ATTACK_A);
    MotionRegist(MOTIONID_IDLE, ENEMYTYPES::MOTIONNAMES::IDLE);

    bool bReset = true;
    MotionPlay(MOTIONID_A, bReset);

    m_targetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_offsetNo = 0;
    m_fTimer = 0.0f;
};


/*virtual*/ C037TriceratonFlyingHarness::CFireStatusObserver::RESULT
C037TriceratonFlyingHarness::CFireStatusObserver::Observing(void) /*override*/
{
    DoStoppingVelocity();

    float fPitch = 0.0f;

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_targetNo);
    if (pPlayerChr)
    {
        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

        RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPosEnemy);

        float fDirection = CEnemyUtils::GetDirection(&vecFootPosEnemy, &vecFootPosPlayer);
        float fRotRate = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
        EnemyChr().Compositor().RotateDirection(fDirection, fRotRate);

        RwV3d vecBodyPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetBodyPosition(&vecBodyPosPlayer);

        RwV3d vecBodyPosEnemy = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetBodyPosition(&vecBodyPosEnemy);

        RwV3d vec = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vec, &vecBodyPosPlayer, &vecBodyPosEnemy);

        RwMatrix matRotY;
        RwMatrixSetIdentityMacro(&matRotY);
        Math::Matrix_RotateY(&matRotY, -std::atan2(vec.x, vec.z));
        RwV3dTransformVector(&vec, &vec, &matRotY);

        fPitch = std::atan2(vec.y, vec.z);
    };

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        RwV3d vecOfsPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetOffsetPosition(&vecOfsPos, m_offsetNo);

        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecMyPos);

        Math::Vec3_Add(&vecMyPos, &vecMyPos, &vecOfsPos);

        float fRand = Math::RandFloatRange(-1.0f, 1.0f);
        float fConvergence = (fRand * MATH_DEG2RAD(1.8f));

        float fDir = EnemyChr().Compositor().GetDirection();

        float fYaw = (fDir + fConvergence);

        RwMatrix matDir;
        RwMatrixSetIdentityMacro(&matDir);
        Math::Matrix_RotationYawPitchRoll(&matDir, fYaw, -fPitch, 0.0f);

        RwV3d vecDir = Math::VECTOR3_AXIS_Z;
        RwV3dTransformVector(&vecDir, &vecDir, &matDir);

        uint32 hShot = CShotManager::Shot(MAGICID::ID_LASER_BEAM_TRI, &vecMyPos, &vecDir, &EnemyChr().Compositor(), MATH_DEG2RAD(15.0f));
        ASSERT(hShot);

        m_offsetNo = (1 - m_offsetNo);
    };

    if (MotionCurrent() == MOTIONID_IDLE)
    {
        m_fTimer -= CGameProperty::GetElapsedTime();
        if (m_fTimer <= 0.0f)
            return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C037TriceratonFlyingHarness::CFireStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ bool C037TriceratonFlyingHarness::CFireStatusObserver::OnMotionChange(int32 idOld, int32 idNew) /*override*/
{
    if (idOld == MOTIONID_A)
        m_fTimer = 1.2f;

    return true;
};


//
// *********************************************************************************
//


/*virtual*/ void C037TriceratonFlyingHarness::CDeathStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    MotionRegist(MOTIONID_DEATH_1, ENEMYTYPES::MOTIONNAMES::DEATH1);
    MotionRegist(MOTIONID_DEATH_2, ENEMYTYPES::MOTIONNAMES::DEATH2);

    bool bReset = true;
    MotionPlay(MOTIONID_DEATH_1, bReset);

    RwV3d vecAttackDir = EnemyChr().Compositor().AttackParameter().m_vDirection;
    EnemyChr().Compositor().SetDirectionFromVector(&vecAttackDir);
    
    RwV3d vecAttackVel = EnemyChr().Compositor().AttackParameter().m_vVelocity;
    EnemyChr().Compositor().SetVelocity(&vecAttackVel);

    m_fFallWaitTimer = 0.0f;

    RwV3d vecOffset = Math::VECTOR3_ZERO;
    float fDir = EnemyChr().Compositor().GetDirection();

    m_hSmokeEffect = CEffectManager::PlayTrace(EFFECTID::ID_BLACKSMOKE, new CEnemyTracer(&EnemyChr()), &vecOffset, fDir);
    ASSERT(m_hSmokeEffect);

    if (m_hSmokeEffect)
        CEffectManager::SetScale(m_hSmokeEffect, 1.5f);
};


/*virtual*/ C037TriceratonFlyingHarness::CDeathStatusObserver::RESULT
C037TriceratonFlyingHarness::CDeathStatusObserver::Observing(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    if (IsMotionEnd())
    {
        bool bReset = true;
        MotionPlay(MOTIONID_DEATH_2, bReset);
    };

    const CCharacterCompositor::COLLISIONGROUNDINFO* pGroundInfo = EnemyChr().Compositor().GetCollisionGround();
    ASSERT(pGroundInfo != nullptr);

    if (pGroundInfo->m_bHit)
        return RESULT_END;

    m_fFallWaitTimer += CGameProperty::GetElapsedTime();

    if (m_fFallWaitTimer > 2.0f)
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C037TriceratonFlyingHarness::CDeathStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(true);

    if (m_hSmokeEffect)
    {
        if (!CEffectManager::IsEnd(m_hSmokeEffect))
        {
            CEffectManager::End(m_hSmokeEffect);
            m_hSmokeEffect = 0;
        };
    };

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);

    EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_MACHINE, &vecMyPos, 1.5f);    

    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C037TriceratonFlyingHarness, { EFFECTID::ID_KO_EXPL,
                                                         EFFECTID::ID_LASER_BEAM_TRI,
                                                         EFFECTID::ID_LASER_FIRE_TRI,
                                                         EFFECTID::ID_LASER_HIT_TRI,
                                                         EFFECTID::ID_RIDE_ENGINE,
                                                         EFFECTID::ID_BLACKSMOKE });


C037TriceratonFlyingHarness::C037TriceratonFlyingHarness(void)
: CBaseFlyingEnemyChr(ENEMYID::ID_TRICERATION_FLYING_HARNESS)
, m_specialityConsider()
, m_hEngineEffect(0)
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C037TriceratonFlyingHarnessAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 3.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 4.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 0.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.2f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 0.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 0.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.0f;
    parameter.m_feature.m_iHPMax                = 40;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 40.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 4.0f;
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
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 0, 30, 0 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 4,  0 },
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

    /* C037TriceratonFlyingHarness status */
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,      new CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new CMoveStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_A,     new CFireStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_B,     new CFireStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_C,     new CFireStatusObserver);

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
    SetAttackAltitude(1.2f);    

    /* init effect */
    RwV3d vecOffset = Math::VECTOR3_ZERO; // unused (default)
    float fDir = Compositor().GetDirection();

    m_hEngineEffect = CEffectManager::PlayTrace(EFFECTID::ID_RIDE_ENGINE, new CBurnerTracer(this), &vecOffset, fDir);
    ASSERT(m_hEngineEffect);

    if (m_hEngineEffect)
        CEffectManager::SetScale(m_hEngineEffect, 0.6f);
};


/*virtual*/ C037TriceratonFlyingHarness::~C037TriceratonFlyingHarness(void)
{
    if (m_hEngineEffect)
    {
        if (!CEffectManager::IsEnd(m_hEngineEffect))
        {
            CEffectManager::Finish(m_hEngineEffect);
            m_hEngineEffect = 0;
        };
    };
};


/*virtual*/ void C037TriceratonFlyingHarness::Run(void) /*override*/
{
    CBaseFlyingEnemyChr::Run();

    if (m_hEngineEffect)
    {
        float fDir = Compositor().GetDirection();

        RwMatrix matDir;
        RwMatrixSetIdentityMacro(&matDir);
        Math::Matrix_RotationYawPitchRoll(&matDir, fDir, -MATH_DEG2RAD(72.0f), 0.0f);

        CEffectManager::SetDirection(m_hEngineEffect, &matDir);

        if (GetStatus() == ENEMYTYPES::STATUS_DEATH)
            CEffectManager::SetActive(m_hEngineEffect, false);
    };
};


/*virtual*/ float C037TriceratonFlyingHarness::GetAltitude(void) const /*override*/
{
    float fAltitude = CBaseFlyingEnemyChr::GetAltitude();

    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    Compositor().GetFootPosition(&vecMyFootPos);

    float fHeight = CWorldMap::GetMapHeight(&vecMyFootPos);

    float fAttackAltitude = (GetAttackAltitude() + fHeight + 2.0f);

    if (fAttackAltitude >= fAltitude)
        return fAltitude;

    return fAttackAltitude;
};