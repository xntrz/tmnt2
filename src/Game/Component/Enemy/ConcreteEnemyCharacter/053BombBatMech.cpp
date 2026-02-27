#include "053BombBatMech.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/053BombBatMechAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Hit/HitCatchData.hpp"


/*virtual*/ void C053BombBatMech::CBodyBlowStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    MotionRegist(MOTIONID_IDLE, ENEMYTYPES::MOTIONNAMES::IDLE);
    MotionRegist(MOTIONID_B1,   ENEMYTYPES::MOTIONNAMES::BODYBLOW_B1);
    MotionRegist(MOTIONID_B2,   ENEMYTYPES::MOTIONNAMES::BODYBLOW_B2);
    MotionPlay(MOTIONID_IDLE);

    m_targetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_fTimer = Math::RandFloatRange(0.5f, 1.5f);
    m_ahEffect[0] = 0;
    m_ahEffect[1] = 0;

    float fDirection = EnemyChr().Compositor().GetDirection();

    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

    m_ahEffect[0] = CEffectManager::Play(EFFECTID::ID_ALL_TAME2, &vecBodyPos, fDirection);
    ASSERT(m_ahEffect[0]);

    if (m_ahEffect[0])
        CEffectManager::SetScale(m_ahEffect[0], 0.5f);
};


/*virtual*/ C053BombBatMech::CBodyBlowStatusObserver::RESULT
C053BombBatMech::CBodyBlowStatusObserver::Observing(void) /*override*/
{
    m_fTimer -= CGameProperty::GetElapsedTime();

    if (IsMotionEnd())
        return RESULT_END;

    int32 motionId = MotionCurrent();

    switch (motionId)
    {
    case MOTIONID_IDLE:
        {
            if (m_fTimer >= 0.0f)
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
            }
            else
            {
                bool bReset = true;
                MotionPlay(MOTIONID_B1, bReset);

                EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);

                float fMoveSpeed = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed * 8.0f;
                RwV3d vecVelocity = { 0.0f, 0.0f, fMoveSpeed };
                EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
                EnemyChr().Compositor().SetVelocity(&vecVelocity);

                m_fTimer = Math::RandFloatRange(0.2f, 0.6f);

                if (!CEffectManager::IsEnd(m_ahEffect[0]))
                {
                    CEffectManager::End(m_ahEffect[0]);
                    m_ahEffect[0] = 0;
                };

                float fDir = EnemyChr().Compositor().GetDirection();
                RwV3d vecOffset = Math::VECTOR3_ZERO;

                EFFECTID::VALUE aEffectId[] =
                {
                    EFFECTID::ID_ALL_DASH1,
                    EFFECTID::ID_ALL_DASH2,
                };

                static_assert(COUNT_OF(aEffectId) == COUNT_OF(m_ahEffect), "should equal");

                for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
                {
                    m_ahEffect[i] = CEffectManager::PlayTrace(aEffectId[i], new CEnemyTracer(&EnemyChr()), &vecOffset, fDir);
                    ASSERT(m_ahEffect[i]);

                    if (m_ahEffect[i])
                        CEffectManager::SetScale(m_ahEffect[i], 0.5f);
                };
            };
        }
        break;

    case MOTIONID_B1:
        {
            DoStoppingVelocity();

            if (m_fTimer < 0.0f)
            {
                bool bReset = true;
                MotionPlay(MOTIONID_B2, bReset);
            };

            const CCharacterCompositor::COLLISIONWALLINFO* pWallInfo = EnemyChr().Compositor().GetCollisionWall();
            ASSERT(pWallInfo != nullptr);

            if (pWallInfo->m_bHit)
            {
                bool bReset = true;
                MotionPlay(MOTIONID_B2, bReset);
            };
        }
        break;

    case MOTIONID_B2:
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C053BombBatMech::CBodyBlowStatusObserver::OnEnd(void) /*override*/
{
    for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
    {
        if (!m_ahEffect[i])
            continue;

        if (!CEffectManager::IsEnd(m_ahEffect[i]))
        {
            CEffectManager::End(m_ahEffect[i]);
            m_ahEffect[i] = 0;
        };
    };

    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ bool
C053BombBatMech::CBodyBlowStatusObserver::OnMotionChange(int32 idOld, int32 idNew) /*override*/
{
    if (idOld == MOTIONID_IDLE)
    {
        MotionPlay(MOTIONID_IDLE);

        return false;
    };

    if (idOld == MOTIONID_B1)
    {
        MotionPlay(MOTIONID_B1);

        float endT = EnemyChr().Compositor().GetMotionEndTime();
        EnemyChr().Compositor().SetMotionTime(endT);
        EnemyChr().Compositor().StopMotion();

        return false;
    };

    return true;
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C053BombBatMech, { EFFECTID::ID_KO_EXPL,
                                             EFFECTID::ID_ALL_DASH1,
                                             EFFECTID::ID_ALL_DASH2,
                                             EFFECTID::ID_ALL_TAME2 });


C053BombBatMech::C053BombBatMech(void)
: CBaseBatChr(ENEMYID::ID_BOMB_BAT_MECH)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C053BombBatMechAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 0.8f;

    parameter.m_feature.m_fWalkMoveSpeed        = 5.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 0.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 2.4f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 0.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 0.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.0f;
    parameter.m_feature.m_iHPMax                = 30;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.8f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 40.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 1.0f;
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
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 0,  0 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    { 0, 20, 0 },
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
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,  new CBaseFlyingEnemyChr::CDeathStatusObserver(EFFECT_GENERIC::DEATHTYPE_MACHINE, 0.4f));

    /* C053BombBatMech status */
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_B, new CBodyBlowStatusObserver);

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


/*virtual*/ C053BombBatMech::~C053BombBatMech(void)
{
    ;
};


/*virtual*/ void C053BombBatMech::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    if (GetStatus() == ENEMYTYPES::STATUS_FIRE_B)
    {
        CHitCatchData::RESULT catchResult = pCatch->GetResult();

        if ((catchResult == CHitCatchData::RESULT_GUARD) ||
            (catchResult == CHitCatchData::RESULT_GUARDIMPACT))
        {
            SetStatus(ENEMYTYPES::STATUS_DEATH);
        }
        else
        {
            RwV3d vecPos = Math::VECTOR3_ZERO;
            Compositor().GetPosition(&vecPos);

            EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_MACHINE, &vecPos, 0.75f);
            
            SetStatus(ENEMYTYPES::STATUS_QUIT);
        };
    }
    else
    {
        CBaseBatChr::OnMessageAttackResult(pCatch);
    };
};