#include "083Hun.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/083HunAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


namespace HUN_083
{
    static int32 REVENGE_DAMAGE_VALUE[] = { 50, 50, 40 };
    static_assert(COUNT_OF(REVENGE_DAMAGE_VALUE) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");
};


C083Hun::C083Hun(void)
: CBaseHunChr(ENEMYID::ID_HUN_B)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C083HunAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 3.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.8f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 0.7f;
    parameter.m_feature.m_fRotateRate           = 5.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 9.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 3.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.2f;
    parameter.m_feature.m_iHPMax                = 600;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 20.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 3.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /*  FREQUENCY_ATTACK_A              0   */      { 50, 50, 50 },
        /*  FREQUENCY_ATTACK_B              1   */      { 20, 20, 20 },
        /*  FREQUENCY_ATTACK_C              2   */      { 15, 15, 15 },
        /*  FREQUENCY_ATTACK_D              3   */      { 40, 40, 40 },
        /*  FREQUENCY_ATTACK_D_FOR_ESCAPE   4   */      { 50, 50, 50 },
        /*  FREQUENCY_ATTACK_THROW          5   */      { 30, 30, 30 },
        /*  FREQUENCY_ATTACK_F              6   */      { 20, 20, 20 },
        /*  FREQUENCY_MOVE                  7   */      { 20, 20, 20 }, // chance for attack F connect from run
        /*  FREQUENCY_ATTACK_F_ON_TALL_OBJ  8   */      { 50, 50, 50 },
        /*  FREQUENCY_REVENGE_NEAR          9   */      { 30, 30, 30 }, // chance for D attack, B otherwise
        /*  FREQUENCY_REVENGE_MIDDLE        10  */      { 40, 40, 40 }, // chance for C attack, F otherwise
        /*  FREQUENCY_GUARD_N               11  */      {  5,  5, 10 }, // normal mode guard
        /*  FREQUENCY_GUARD_B               12  */      {  0,  0,  0 }, // burst mode guard
    };

    parameter.m_iFrequencyMax = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);

    /* init hun */
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    InitRevengeMode(HUN_083::REVENGE_DAMAGE_VALUE[difficulty]);
};


/*virtual*/ C083Hun::~C083Hun(void)
{
    ;
};


/*virtual*/ void C083Hun::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    float fHpRatio = CEnemyUtils::GetEnemyHPRate(this);

    bool bIsInBurstMode = (m_bBurstModeActie);
    bool bIsDead = (TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS));
    bool bIsHpHigh = (fHpRatio > 0.3f);
    bool bIsDown = (GetStatus() == BASEHUN::STATUS_BURST_FLYAWAY);

    if (bIsInBurstMode || bIsDead || bIsHpHigh || bIsDown)
    {
        CBaseHunChr::OnMessageReceivedDamage(iAmount);
    }
    else
    {
        /* TODO: game event SetEnemyDamaged not fired in this case */
        CharacterParameter().m_feature.m_iHP -= iAmount;
        SetStatus(BASEHUN::STATUS_BURST_FLYAWAY);
    };
};