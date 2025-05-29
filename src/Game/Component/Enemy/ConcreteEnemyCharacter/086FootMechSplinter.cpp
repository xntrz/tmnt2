#include "086FootMechSplinter.hpp"

#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseSplinterAI.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Module/BarrierModule.hpp"


C086FootMechSplinter::C086FootMechSplinter(void)
: CBaseFootMechSplinter(ENEMYID::ID_FOOT_MECH_SPLINTER)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseSplinterAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.16f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.7f;
    parameter.m_feature.m_fDashMoveSpeed        = 18.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.2f;
    parameter.m_feature.m_fRotateRate           = 1.8f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 500;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 1.0f;
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
        /*  FREQUENCY_GUARD                 0   */  {  10, 30, 60 },
        /*  FREQUENCY_ATTACK_HIGH           1   */  {  10, 30, 50 },
        /*  FREQUENCY_ATTACK_AVOID          2   */  {  20, 50, 80 },

        /****************************************/  
        /*          CBaseSplinterAI freq        */  
        /****************************************/  
        /*  FREQUENCY_ATTACK_NORMAL         3   */  {  30, 60, 80 },
        /*  FREQUENCY_ATTACK_NORMALA        4   */  {  10, 30, 90 },
        /*  FREQUENCY_ATTACK_NORMALB        5   */  {  10, 40, 60 },
        /*  FREQUENCY_ATTACK_B              6   */  {  40, 20, 10 },
        /*  FREQUENCY_ATTACK_KICK           7   */  {  20, 40, 80 },
        /*  FREQUENCY_ATTACK_RUN            8   */  {  60, 30, 10 },
        /*  FREQUENCY_ATTACK_B_CHARGE       9   */  {  40, 30, 20 },
        /*  FREQUENCY_ATTACK_THROW         10   */  {   5, 20, 30 },
        /*  FREQUENCY_ATTACK_SHOT          11   */  {  20, 50, 70 },
        /*  FREQUENCY_ATTACK_SHOT_JUMP     12   */  {  30, 50, 70 },
        /*  FREQUENCY_WAIT                 13   */  { 100, 50, 10 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C086FootMechSplinter::~C086FootMechSplinter(void)
{
    ;
};