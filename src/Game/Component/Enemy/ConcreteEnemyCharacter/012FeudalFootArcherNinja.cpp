#include "012FeudalFootArcherNinja.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Shot/ShotID.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"


static const RwV3d BANDANA_OFFSET = { -0.006f, 0.15f, -0.037f };


namespace
{
    class CArrowTracer : public CEnemyTracer
    {
    public:
        inline CArrowTracer(CEnemyCharacter* pEnemyChr) : CEnemyTracer(pEnemyChr) {};
        inline virtual ~CArrowTracer(void) {};

        virtual void GetPosition(RwV3d* pvPosition) override;
    };
};


//
// *********************************************************************************
//


/*virtual*/ void CArrowTracer::GetPosition(RwV3d* pvPosition) /*override*/
{
    CGameObject* pObj = CGameObjectManager::GetObject(m_hObj);
    if (!pObj)
    {
        *pvPosition = Math::VECTOR3_ZERO;
        return;
    };

    CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pObj);

    RwV3d vecOfsPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetOffsetPosition(&vecOfsPos, 1); // TODO offset no naming

    RwV3d vecPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetPosition(&vecPos);

    Math::Vec3_Add(pvPosition, &vecPos, &vecOfsPos);
};


//
// *********************************************************************************
//


C012FeudalFootArcherNinja::CFireArrowStatusObserver::CFireArrowStatusObserver(const PARAM* pParam)
: CFireStatusObserver(pParam)
{
    ;
};


/*virtual*/ ENEMYTYPES::STATUS C012FeudalFootArcherNinja::CFireArrowStatusObserver::OnEnd(void) /*override*/
{
    SetArrowActive(true);
    return CFireStatusObserver::OnEnd();
};


/*virtual*/ bool C012FeudalFootArcherNinja::CFireArrowStatusObserver::OnMotionChange(int32 idOld, int32 idNew) /*override*/
{
    bool bResult = CFireStatusObserver::OnMotionChange(idOld, idNew);

    if (bResult || (MotionCurrent() != MOTIONID_FIRE))
    {
        SetArrowActive(true);
        bResult = true;
    }
    else
    {
        SetArrowActive(false);
        bResult = false;
    };

    return bResult;
};


void C012FeudalFootArcherNinja::CFireArrowStatusObserver::SetArrowActive(bool bActive)
{
    CModel* pModel = EnemyChr().Compositor().GetModel();
    ASSERT(pModel != nullptr);

    pModel->SetPartsDrawEnable(1, bActive);

    static_cast<C012FeudalFootArcherNinja&>(EnemyChr()).SetArrowActive(bActive);
};


//
// *********************************************************************************
//


DEFINE_NEEDED_EFFECTS_FOR(C012FeudalFootArcherNinja, { EFFECTID::ID_KO_FLASH,
                                                       EFFECTID::ID_FIRE_ARROWSHOT,
                                                       EFFECTID::ID_SLEEP_ARROWSHOT,
                                                       EFFECTID::ID_FIRE_ARROW,
                                                       EFFECTID::ID_SLEEP_ARROW });


C012FeudalFootArcherNinja::C012FeudalFootArcherNinja(void)
: CBaseGunnerEnemyChr(ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER)
, m_specialityConsider()
, m_hArrowEffect(0)
{
    /* init status shared data */
    SetSharedData(&m_specialityConsider);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseGunnerAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 1.9f;
    parameter.m_feature.m_fRunMoveSpeed         = 4.4f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 2.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 9.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 15;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.2f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.15f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.05f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.4f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 10.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 8.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A               0  */    { 0, 0,   0 },   
        /* FREQUENCY_ATTACK_AA              1  */    { 0, 0,   0 },
        /* FREQUENCY_ATTACK_AAA             2  */    { 0, 0,   0 },
        /* FREQUENCY_ATTACK_B               3  */    { 0, 0,   0 },
        /* FREQUENCY_ATTACK_C               4  */    { 0, 0,   0 },
        /* FREQUENCY_ATTACK_RUN             5  */    { 0, 0,   0 },
        /* FREQUENCY_GUARD_A                6  */    { 0, 0,   0 },
        /* FREQUENCY_GUARD_B                7  */    { 0, 0,   0 },
        /* FREQUENCY_GUARD_SHOT             8  */    { 0, 0,   0 },
        /* FREQUENCY_GUARD_JUMP_ATTACK      9  */    { 0, 0,   0 },
        /* FREQUENCY_GUARD_RUN_ATTACK       10 */    { 0, 0,   0 },
        /* FREQUENCY_GUARD_THROW            11 */    { 0, 0,   0 },
        /* FREQUENCY_MOVE_APPROACH          12 */    { 0, 0,   0 },
        /* FREQUENCY_MOVE_FORMATION         13 */    { 0, 0,   0 },
        /* FREQUENCY_TRIGGER_ATTACK         14 */    { 0, 0,   0 },
        /* FREQUENCY_TRIGGER_MOVE           15 */    { 0, 0,   0 },
        /* FREQUENCY_TRIGGER_ESCAPE         16 */    { 0, 0,   0 },
        /* FREQUENCY_TRIGGER_REDUCTION_HP   17 */    { 0, 0,   0 },
        /* FREQUENCY_FIRE_CONVERGENCE       18 */    { 0, 80,  0 },
        /* FREQUENCY_FIRE_RANGE             19 */    { 0, 10,  0 },
        /* FREQUENCY_FIRE_RANGE_RATE        20 */    { 0, 40,  0 },
        /* FREQUENCY_FIRE_REPEATABLE        21 */    { 0, 15,  0 },
        /* FREQUENCY_FIRE_REPEATCOUNT       22 */    { 0, 2,   0 },
        /* FREQUENCY_FIRE_REPEATCHASE       23 */    { 0, 0,   0 },
        /* FREQUENCY_COMMON_ATTACKINTERVAL  24 */    { 0, 2,   0 },
        /* FREQUENCY_COMMON_1               25 */    { 0, 0,   0 },
        /* FREQUENCY_COMMON_2               26 */    { 0, 0,   0 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = true;
    Initialize(&parameter, bReplace);

    /* CBaseGeneralEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_TURN,       new CBaseGeneralEnemyChr::CTurnStatusObserver(120.0f, ENEMYTYPES::MOTIONNAMES::TURNL, ENEMYTYPES::MOTIONNAMES::TURNR));
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK_LEFT,  new CBaseGeneralEnemyChr::CSideWalkStatusObserver(1.8f, ENEMYTYPES::MOTIONNAMES::WALKL));
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK_RIGHT, new CBaseGeneralEnemyChr::CSideWalkStatusObserver(1.9f, ENEMYTYPES::MOTIONNAMES::WALKR));   // 1.9 ??
    
    /* C012FeudalFootArcherNinja status */
    int32 repeatCount  = static_cast<int32>(FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCOUNT));
    bool  bRepeatChase = (FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_REPEATCHASE) != 0);
    int32 arrowType    = static_cast<int32>(FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_2));

    SHOTID::VALUE aShotId[] =
    {
        SHOTID::ID_ARROW,
        SHOTID::ID_ARROW_FIRE,
        SHOTID::ID_ARROW_SLEEP,
    };

    ASSERT(arrowType >= 0);
    ASSERT(arrowType < COUNT_OF(aShotId));

    CFireStatusObserver::PARAM fireParam;
    fireParam.type          = CFireStatusObserver::PARAM::TYPE_SHOT;
    fireParam.shotId        = aShotId[arrowType];
    fireParam.repeatCount   = repeatCount;
    fireParam.bRepeatChase  = bRepeatChase;
    fireParam.boneId        = 0;

    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_A,     new C012FeudalFootArcherNinja::CFireArrowStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_B,     new C012FeudalFootArcherNinja::CFireArrowStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_C,     new C012FeudalFootArcherNinja::CFireArrowStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_AA,    new C012FeudalFootArcherNinja::CFireArrowStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_BB,    new C012FeudalFootArcherNinja::CFireArrowStatusObserver(&fireParam));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_CC,    new C012FeudalFootArcherNinja::CFireArrowStatusObserver(&fireParam));

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

    /* init modules */
    CCharacterCompositor* pChrCompositor = &Compositor();

    CBandanaModule* pBandanaModule = new CBandanaModule(pChrCompositor,
                                                        pChrCompositor->GetModel(),
                                                        11,
                                                        &BANDANA_OFFSET,
                                                        CBandanaModule::BANDANACOLOR_DARKGREY);

    pChrCompositor->IncludeModule(pBandanaModule);

    /* init arrow effect */
    if ((fireParam.shotId == SHOTID::ID_ARROW_FIRE) ||
        (fireParam.shotId == SHOTID::ID_ARROW_SLEEP))
    {
        RwV3d vecOffset = Math::VECTOR3_ZERO; // unused (default)

        EFFECTID::VALUE effectId =
            (fireParam.shotId == SHOTID::ID_ARROW_FIRE ? EFFECTID::ID_FIRE_ARROW : EFFECTID::ID_SLEEP_ARROW);

        m_hArrowEffect = CEffectManager::PlayTrace(effectId, new CArrowTracer(this), &vecOffset);
        ASSERT(m_hArrowEffect);
    };
};


/*virtual*/ C012FeudalFootArcherNinja::~C012FeudalFootArcherNinja(void)
{
    if (m_hArrowEffect)
    {
        if (!CEffectManager::IsEnd(m_hArrowEffect))
        {
            CEffectManager::Finish(m_hArrowEffect);
            m_hArrowEffect = 0;
        };
    };
};


/*virtual*/ void C012FeudalFootArcherNinja::SetArrowActive(bool bActive) const
{
    if (m_hArrowEffect)
        CEffectManager::SetActive(m_hArrowEffect, bActive);
};