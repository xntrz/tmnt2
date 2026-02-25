#include "078EliteFootC.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/BaseEliteFootAI.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ void C078EliteFootC::CSpearStatusObserver::OnStart(void) /*override*/
{
    m_vecSpearPos = EnemyChr().AIThinkOrder().OrderAttack().m_vAt;

    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_C, true);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8445));
};


/*virtual*/ C078EliteFootC::CSpearStatusObserver::RESULT
C078EliteFootC::CSpearStatusObserver::Observing(void) /*override*/
{
    float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
    if (fMotionNowT < 0.6f)
    {
        int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTargetNo);
        if (pPlrChr)
            pPlrChr->GetFootPosition(&m_vecSpearPos);
    };

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        RwV3d vecOrderPos = EnemyChr().AIThinkOrder().OrderAttack().m_vAt;

        m_vecSpearPos.y = vecOrderPos.y;
        YariShot(&m_vecSpearPos);

        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4380));
        CGameSound::PlayPositionSE(&m_vecSpearPos, SDCODE_SE(4381));
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C078EliteFootC::CSpearStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


void C078EliteFootC::CSpearStatusObserver::YariShot(const RwV3d* pvecYariPos)
{
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    float fDir = CPlayerWatcher::GetDirection(&vecFootPos, pvecYariPos);

    RwMatrix matRotY;
    RwMatrixSetIdentity(&matRotY);
    Math::Matrix_RotateY(&matRotY, fDir);

    /* center spear */
    RwV3d vecPosCenter = *pvecYariPos;
    vecPosCenter.y -= 0.1f;

    CShotManager::Shot(SHOTID::ID_SPEAR, &vecPosCenter, 0.0f, &EnemyChr().Compositor(), MATH_DEG2RAD(15.0f), 5.0f);

    /* left spear */
    RwV3d vecOffsetLeft = { 0.5f, 0.0f, 0.0f };
    RwV3dTransformPoint(&vecOffsetLeft, &vecOffsetLeft, &matRotY);

    RwV3d vecPosLeft = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vecPosLeft, &vecPosCenter, &vecOffsetLeft);

    CShotManager::Shot(SHOTID::ID_SPEAR, &vecPosLeft, 0.0f, &EnemyChr().Compositor(), MATH_DEG2RAD(15.0f), 5.0f);

    /* right spear */
    RwV3d vecOffsetRight = { -0.5f, 0.0f, 0.0f };
    RwV3dTransformPoint(&vecOffsetRight, &vecOffsetRight, &matRotY);

    RwV3d vecPosRight = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vecPosRight, &vecPosCenter, &vecOffsetRight);

    CShotManager::Shot(SHOTID::ID_SPEAR, &vecPosRight, 0.0f, &EnemyChr().Compositor(), MATH_DEG2RAD(15.0f), 5.0f);
};


//
// *********************************************************************************
//


C078EliteFootC::C078EliteFootC(void)
: CBaseEliteFootChr(ENEMYID::ID_ELITE_FOOT_C)
{
    /* CBaseEliteFootChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA, new CBaseEliteFootChr::CSwingAttackStatusObserver);

    /* C078EliteFootC status */
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_A, new C078EliteFootC::CSpearStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseEliteFootAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.16f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 1.6f;
    parameter.m_feature.m_fRotateRate           = 1.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 600;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.8f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 15.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 40.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 3.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /****************************************/
        /*          CBaseEliteAI freq           */
        /****************************************/
        /*  FREQUENCY_GUARD                 0   */  {  10, 20, 80 },
        /*  FREQUENCY_ATTACK_HIGH           1   */  {  10, 40, 80 },
        /*  FREQUENCY_ATTACK_AVOID          2   */  {  20, 30, 90 },

        /****************************************/
        /*          CBaseEliteFootAI freq       */
        /****************************************/
        /*  FREQUENCY_DASH                  3   */  {  40, 50, 80 },
        /*  FREQUENCY_ATTACK_SPECIAL        4   */  {   5, 20, 80 },
        /*  FREQUENCY_ATTACK_SPECIAL_EX     5   */  {  10, 40, 50 },
        /*  FREQUENCY_WAIT                  6   */  { 100, 60, 10 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C078EliteFootC::~C078EliteFootC(void)
{
    ;
};


//
// *********************************************************************************
//


C078EliteFootDummyC::C078EliteFootDummyC(void)
: CBaseEliteFootChr(ENEMYID::ID_ELITE_FOOT_DUMMY_C)
{
    /* CBaseEliteFootChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA, new CBaseEliteFootChr::CSwingAttackStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseEliteFootDummyAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.16f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 1.6f;
    parameter.m_feature.m_fRotateRate           = 1.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 90;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.8f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 15.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 40.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 2.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /****************************************/
        /*          CBaseEliteAI freq           */
        /****************************************/
        /*  FREQUENCY_GUARD                 0   */  {   0,  0, 90 },
        /*  FREQUENCY_ATTACK_HIGH           1   */  {  10, 20, 80 },
        /*  FREQUENCY_ATTACK_AVOID          2   */  {   0,  0,  0 },

        /****************************************/
        /*          CBaseEliteFootAI freq       */
        /****************************************/
        /*  FREQUENCY_DASH                  3   */  {   0,  0,  0 },
        /*  FREQUENCY_ATTACK_SPECIAL        4   */  {   5, 20, 80 },
        /*  FREQUENCY_ATTACK_SPECIAL_EX     5   */  {   0,  0,  0 },
        /*  FREQUENCY_WAIT                  6   */  { 100, 60, 10 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C078EliteFootDummyC::~C078EliteFootDummyC(void)
{
    ;
};