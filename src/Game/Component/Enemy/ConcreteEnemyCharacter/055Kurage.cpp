#include "055Kurage.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/055KurageAI.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseGunnerEnemyChr.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ void C055Kurage::CMoveStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    m_order               = EnemyChr().AIThinkOrder().GetOrder();
    m_orderTarget         = AIOT::GetTarget(EnemyChr().AIThinkOrder().OrderMove().m_iMoveType);
    m_fMoveSpeed          = EnemyChr().Feature().m_fWalkMoveSpeed;
    m_fRotRate            = EnemyChr().Feature().m_fRotateRate;
    m_fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fDirection          = static_cast<C055KurageAI&>(EnemyChr().AIModerator()).OnGetDirection();
    m_bTargetChanged      = false;

    RwV3d vecOrderPos = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vecOrderPos);

    float fMapHeight = CWorldMap::GetMapHeight(&vecOrderPos);
    float fAltitude = static_cast<C055Kurage&>(EnemyChr()).GetAltitude() - fMapHeight;

    if (m_orderTarget)
        fAltitude = static_cast<C055Kurage&>(EnemyChr()).GetAttackAltitude();

    m_fAltitude = fAltitude;

    if (m_orderTarget)
    {
        MotionRegist(MOTIONID_A1, ENEMYTYPES::MOTIONNAMES::A1);
        MotionRegist(MOTIONID_A2, ENEMYTYPES::MOTIONNAMES::A2);
        MotionRegist(MOTIONID_A3, ENEMYTYPES::MOTIONNAMES::A3);

        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4332));
    }
    else
    {
        MotionRegist(MOTIONID_A1, ENEMYTYPES::MOTIONNAMES::IDLE);
    };

    MotionPlay(MOTIONID_A1);
};


/*virtual*/ C055Kurage::CMoveStatusObserver::RESULT
C055Kurage::CMoveStatusObserver::Observing(void) /*override*/
{
    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order != m_order)
        return RESULT_END;

    if (IsMotionEnd())
        return RESULT_END;

    UpdateMotion();
    UpdateDirection();
    UpdateVelocity();

	return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C055Kurage::CMoveStatusObserver::OnEnd(void) /*override*/
{
    CGameSound::StopSE(SDCODE_SE(4332));
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ bool C055Kurage::CMoveStatusObserver::OnMotionChange(int32 idOld, int32 idNew) /*override*/
{
	if (m_orderTarget)
	{
		if (idOld == MOTIONID_A2)
			MotionPlay(MOTIONID_A2);
	}
	else
	{
        MotionPlay(idOld);
	};

    return true;
};


/*virtual*/ float C055Kurage::CMoveStatusObserver::AltitudeNoise(void)
{
    return 0.0f;
};


void C055Kurage::CMoveStatusObserver::UpdateMotion(void)
{
    if (m_bTargetChanged)
        return;

    int32 orderTarget = AIOT::GetTarget(EnemyChr().AIThinkOrder().OrderMove().m_iMoveType);
    if (orderTarget != m_orderTarget)
    {
        m_bTargetChanged = true;

        if (m_orderTarget)
        {
            /* motion was applied as attack to target and now its empty - play end of
               attack motion to trigger end of status and reattach move status as idle */
            bool bReset = true;
            MotionPlay(MOTIONID_A3, bReset);
            m_fAltitude = GetAltitude();            
        }
        else
        {
            /* motion was applied as idle and target now changed - stop motion to trigger
               end of current status and reattach move status as attack */
            MotionStop();
        };
    };
};


void C055Kurage::CMoveStatusObserver::UpdateDirection(void)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecPos);

    RwV3d vecAt = Math::VECTOR3_ZERO;
    CBaseFlyingEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vecAt);

    float fDir = 0.0f;

    if (Math::FEqual(vecPos.x, vecAt.x) &&
        Math::FEqual(vecPos.z, vecAt.z))
    {
        fDir = EnemyChr().Compositor().GetDirection();
    }
    else
    {
        EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

        float fPrevDir = EnemyChr().Compositor().GetDirection();

        fDir = CEnemyUtils::GetDirection(&vecPos, &vecAt);
        EnemyChr().Compositor().SetDirection(m_fDirection);
        EnemyChr().Compositor().RotateDirection(fDir, m_fRotRate);

        fDir = EnemyChr().Compositor().GetDirection();

        EnemyChr().Compositor().SetDirection(fPrevDir);

        EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
    };

    m_fDirection = fDir;
    static_cast<C055KurageAI&>(EnemyChr().AIModerator()).OnSetDirection(m_fDirection);
};


void C055Kurage::CMoveStatusObserver::UpdateVelocity(void)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    RwV3d vecAt = Math::VECTOR3_ZERO;
    CBaseFlyingEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vecAt);

    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    Math::Matrix_RotationYawPitchRoll(&matRot, m_fDirection, 0.0f, 0.0f);

    RwV3d vecVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
    RwV3dTransformVector(&vecVelocity, &vecVelocity, &matRot);

    RwV3d vecHeightPos = vecPos;
    RwV3d vecHeightAt = vecAt;

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
        float fHeight = CWorldMap::GetMapHeight(&vecHeightAt);

        if (fHeight < fPatrolHeight)
            vecHeightAt = EnemyChr().CharacterParameter().m_feature.m_vPatrolOrigin;
    };

    float fHeightAt = CWorldMap::GetMapHeight(&vecHeightAt);
    float fHeightPos = CWorldMap::GetMapHeight(&vecHeightPos);

    float fHeight = fHeightPos;

    if (fHeightAt >= fHeightPos)
        fHeight = fHeightAt;

    vecVelocity.y = CalcAltitude(m_fAltitude + fHeight, vecPos.y, m_fMoveSpeed);
    vecVelocity.y += AltitudeNoise();

    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


//
// *********************************************************************************
//


/*virtual*/ void C055Kurage::CAttackBombingStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_B, bForce);
};


/*virtual*/ C055Kurage::CAttackBombingStatusObserver::RESULT
C055Kurage::CAttackBombingStatusObserver::Observing(void) /*override*/
{
    DoStoppingVelocity();

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        CGameSound::PlayAttackSE(&EnemyChr().Compositor());

        RwV3d vecPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecPos);

        float fDir = EnemyChr().Compositor().GetDirection();

        uint32 hShot = CShotManager::Shot(SHOTID::ID_BOMB, &vecPos, fDir, &EnemyChr().Compositor(), MATH_DEG2RAD(15.0f), 5.0f);
        ASSERT(hShot);
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C055Kurage::CAttackBombingStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C055Kurage, { EFFECTID::ID_KO_EXPL,
                                        EFFECTID::ID_BLACKSMOKE });


C055Kurage::C055Kurage(void)
: CBaseFlyingEnemyChr(ENEMYID::ID_KURAGE)
, m_specialityConsider()
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C055KurageAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.4f;

    parameter.m_feature.m_fWalkMoveSpeed        = 4.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 0.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 1.2f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 0.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 0.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.0f;
    parameter.m_feature.m_iHPMax                = 15;
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
        /* FREQUENCY_ATTACK_A               0  */    { 0, 10, 0 },   
        /* FREQUENCY_ATTACK_AA              1  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_AAA             2  */    { 0, 0,  0 },
        /* FREQUENCY_ATTACK_B               3  */    { 0, 50, 0 },
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
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,      new CBaseFlyingEnemyChr::CDeathStatusObserver(EFFECT_GENERIC::DEATHTYPE_MONSTER, 1.0f));

    /* C055Kurage status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new CMoveStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,   new CAttackBombingStatusObserver);

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

    /* play work sound */
    CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4333));
};


/*virtual*/ C055Kurage::~C055Kurage(void)
{
    CGameSound::StopSE(SDCODE_SE(4333));
};


/*virtual*/ void C055Kurage::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    if (GetStatus() == ENEMYTYPES::STATUS_WALK)
    {
        const int32 HIT_MAX = 10;

        if (++m_hitCount >= HIT_MAX)
        {
            static_cast<C055KurageAI&>(AIModerator()).OnSetActionOfRun();
            m_hitCount = 0;
        };
    };

    CBaseFlyingEnemyChr::OnMessageAttackResult(pCatch);
};