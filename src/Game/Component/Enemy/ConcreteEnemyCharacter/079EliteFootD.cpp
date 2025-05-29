#include "079EliteFootD.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/BaseEliteFootAI.hpp"


C079EliteFootD::C079EliteFootD(void)
: CBaseEliteFootChr(ENEMYID::ID_ELITE_FOOT_D)
{
    /* CBaseEliteFootChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA, new CBaseEliteFootChr::CSpinAttackStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseEliteFootAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 1.8f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 2.0f;
    parameter.m_feature.m_fRotateRate           = 2.0f;
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
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 2.5f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /****************************************/
        /*          CBaseEliteAI freq           */
        /****************************************/
        /*  FREQUENCY_GUARD                 0   */  {   5, 10, 80 },
        /*  FREQUENCY_ATTACK_HIGH           1   */  {  20, 50, 80 },
        /*  FREQUENCY_ATTACK_AVOID          2   */  {  10, 30, 40 },

        /****************************************/
        /*          CBaseEliteFootAI freq       */
        /****************************************/
        /*  FREQUENCY_DASH                  3   */  {  20, 30, 70 },
        /*  FREQUENCY_ATTACK_SPECIAL        4   */  {  10, 30, 60 },
        /*  FREQUENCY_ATTACK_SPECIAL_EX     5   */  {  60, 40, 20 },
        /*  FREQUENCY_WAIT                  6   */  { 100, 60, 10 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C079EliteFootD::~C079EliteFootD(void)
{
    ;
};


//
// *********************************************************************************
//


C079EliteFootDummyD::C079EliteFootDummyD(void)
: CBaseEliteFootChr(ENEMYID::ID_ELITE_FOOT_DUMMY_D)
{
    /* CBaseEliteFootChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA, new CBaseEliteFootChr::CSpinAttackStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseEliteFootDummyAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 1.8f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 2.0f;
    parameter.m_feature.m_fRotateRate           = 2.0f;
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
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 25.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 40.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 2.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /****************************************/
        /*          CBaseEliteAI freq           */
        /****************************************/
        /*  FREQUENCY_GUARD                 0   */  {   0,  0, 80 },
        /*  FREQUENCY_ATTACK_HIGH           1   */  {  10, 40, 80 },
        /*  FREQUENCY_ATTACK_AVOID          2   */  {   0,  0,  0 },

        /****************************************/
        /*          CBaseEliteFootAI freq       */
        /****************************************/
        /*  FREQUENCY_DASH                  3   */  {   0,  0,  0 },
        /*  FREQUENCY_ATTACK_SPECIAL        4   */  {  20, 40, 80 },
        /*  FREQUENCY_ATTACK_SPECIAL_EX     5   */  {   5, 20, 30 },
        /*  FREQUENCY_WAIT                  6   */  { 100, 80, 10 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C079EliteFootDummyD::~C079EliteFootDummyD(void)
{

};