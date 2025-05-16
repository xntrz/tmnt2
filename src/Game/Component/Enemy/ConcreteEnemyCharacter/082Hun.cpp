#include "082Hun.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/082HunAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


namespace HUN_082
{
    static int32 REVENGE_DAMAGE_VALUE[] = { 60, 60, 50 };
    static_assert(COUNT_OF(REVENGE_DAMAGE_VALUE) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");
};


C082Hun::C082Hun(void)
: CBaseHunChr(ENEMYID::ID_HUN_A)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C082HunAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 2.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.8f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 0.7f;
    parameter.m_feature.m_fRotateRate           = 5.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 10.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 3.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.2f;
    parameter.m_feature.m_iHPMax                = 300;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.25f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.8f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 20.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 5.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_ATTACK_A                 0     */  { 20, 20, 20  },
        /* FREQUENCY_ATTACK_B                 1     */  { 20, 20, 20  },
        /* FREQUENCY_ATTACK_C                 2     */  { 20, 20, 20  },
        /* FREQUENCY_ATTACK_D                 3     */  { 20, 20, 20  },
        /* FREQUENCY_ATTACK_D_FOR_ESCAPE      4     */  { 40, 40, 40  },
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);

    /* init hun */
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    InitRevengeMode(HUN_082::REVENGE_DAMAGE_VALUE[difficulty]);
};


/*virtual*/ C082Hun::~C082Hun(void)
{
    ;
};