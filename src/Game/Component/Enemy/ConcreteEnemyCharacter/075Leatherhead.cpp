#include "075Leatherhead.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/075LeatherheadAI.hpp"


C075Leatherhead::C075Leatherhead(void)
: CBaseLeatherheadChr(ENEMYID::ID_LEATHER_HEAD)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C075LeatherheadAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 3.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 3.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 6.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 0.7f;
    parameter.m_feature.m_fRotateRate           = 5.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 3.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.2f;
    parameter.m_feature.m_iHPMax                = 500;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 25.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 4.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_AAA         0   */  { 30, 30, 30 },
        /* FREQUENCY_ATTACK_THROW       1   */  { 20, 20, 20 },
        /* FREQUENCY_ATTACK_TACKLE      2   */  { 30, 30, 30 },
        /* FREQUENCY_ATTACK_TO_AIR      3   */  { 80, 80, 80 },
        /* FREQUENCY_4                  4   */  { 10, 10, 20 },
        /* FREQUENCY_GUARD_N            5   */  { 10, 15, 20 },
        /* FREQUENCY_GUARD_S            6   */  { 15, 20, 25 },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);
};


/*virtual*/ C075Leatherhead::~C075Leatherhead(void)
{
    ;
};