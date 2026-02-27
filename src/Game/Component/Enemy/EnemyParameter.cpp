#include "EnemyParameter.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


static const char* ENEMYPARAM_FILEHEADER = "EPB_Ver1.0     ";
static const int32 ENEMYPARAM_MAX = 594; // (enemy max - 2) * pattern max


struct ENEMYPARAMCONTAINER
{
    int32                       m_iHeader;
    ENEMYID::VALUE              m_eID;
    int32                       m_iPattern;
    GAMETYPES::DIFFICULTY       m_eDifficulty;
    int32                       m_iHP;
    int32                       m_iAttack;
    int32                       m_iGuard;
    ENEMYTYPES::CHARACTERISTIC  m_AICharacteristic;
    uint8                       m_auFrequency[CEnemyParameter::FREQUENCY_MAX];
    uint8                       m_Padding1;
    RwV3d                       m_vOrigin;
    float                       m_fRadius;
};


struct ENEMYPARAM
{
    bool                m_bExist;
    ENEMYPARAMCONTAINER m_container;
};


static ENEMYPARAM s_aEnemyParam[ENEMYPARAM_MAX] = {};


static inline void Rate(uint8* pFreqParam, float fRate)
{
    uint8 uFreqParam = *pFreqParam;
    uint8 uResult = static_cast<uint8>(static_cast<float>(uFreqParam) * fRate);
    
    if (uResult >= 100)
        uResult = 99;
    
    *pFreqParam = uResult;
};


static void CompensationParameter(ENEMYPARAMCONTAINER* pEnemyParam)
{
    GAMETYPES::DIFFICULTY Level = CGameProperty::GetDifficulty();
    switch (Level)
    {
    case GAMETYPES::DIFFICULTY_EASY:
        {
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_A],           0.0f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_B],           0.0f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_SHOT],        0.0f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_JUMP_ATTACK], 0.0f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_RUN_ATTACK],  0.0f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_THROW],       0.0f);

            pEnemyParam->m_iHP = static_cast<int32>(static_cast<float>(pEnemyParam->m_iHP) * 0.8f);
        }
        break;

    case GAMETYPES::DIFFICULTY_HARD:
        {
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_ATTACK_A],          1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_ATTACK_AA],         1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_ATTACK_AAA],        1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_ATTACK_B],          1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_ATTACK_C],          1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_ATTACK_RUN],        1.05f);

            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_A],           1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_B],           1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_SHOT],        1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_JUMP_ATTACK], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_RUN_ATTACK],  1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_GUARD_THROW],       1.05f);

            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE],  1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE],   1.05f);
            Rate(&pEnemyParam->m_auFrequency[CEnemyParameter::FREQUENCY_FIRE_REPEATABLE],   1.05f);

            pEnemyParam->m_AICharacteristic.m_fThinkingFrequency *= 1.05f;
            pEnemyParam->m_AICharacteristic.m_fRatioOfActivity   *= 1.05f;
            pEnemyParam->m_AICharacteristic.m_fRatioOfFrontView  *= 1.05f;
            pEnemyParam->m_AICharacteristic.m_fRatioOfRearView   *= 1.05f;
        }
        break;

    default:
        break;
    };
};


/*static*/ void CEnemyParameter::Initialize(void)
{
    for (int32 i = 0; i < COUNT_OF(s_aEnemyParam); ++i)
    {
        s_aEnemyParam[i].m_bExist = false;
        std::memset(&s_aEnemyParam[i].m_container, 0x00, sizeof(s_aEnemyParam[i].m_container));
    };
};


/*static*/ void CEnemyParameter::Terminate(void)
{
    ;
};


/*static*/ void CEnemyParameter::Read(const void* Buffer, uint32 BufferSize)
{
    CEnemyParameter::Initialize();

    const char* ptr = static_cast<const char*>(Buffer);
    
    if (std::strncmp(ptr, ENEMYPARAM_FILEHEADER, std::strlen(ENEMYPARAM_FILEHEADER)))
        ASSERT(false);

    ptr += (std::strlen(ENEMYPARAM_FILEHEADER) + 1);

    int32 ParamNo = 0;
    uint32 ReadPos = 0;
    ENEMYPARAMCONTAINER tempContainer;
    while (ReadPos < BufferSize)
    {
        std::memcpy(&tempContainer, &ptr[ReadPos], sizeof(tempContainer));

        static char signature[] = { 'E', 'P', 'B', 'I' };
        static_assert(sizeof(signature) == sizeof(tempContainer.m_iHeader), "sizeof should match");

        if ((tempContainer.m_iHeader == *reinterpret_cast<int32*>(signature)) &&
            (tempContainer.m_eDifficulty == GAMETYPES::DIFFICULTY_NORMAL))
        {
            ASSERT(tempContainer.m_eID >= ENEMYID::ID_START);
            ASSERT(tempContainer.m_eID <  ENEMYID::ID_MAX);

            s_aEnemyParam[ParamNo].m_bExist = true;
            std::memcpy(&s_aEnemyParam[ParamNo].m_container, &tempContainer, sizeof(tempContainer));

			CompensationParameter(&s_aEnemyParam[ParamNo].m_container);

            ++ParamNo;
        };

        ReadPos += sizeof(ENEMYPARAMCONTAINER);
    };
};


/*static*/ int32 CEnemyParameter::Search(ENEMYID::VALUE id, int32 pattern)
{
    for (int32 i = 0; i < COUNT_OF(s_aEnemyParam); ++i)
    {
        if (!s_aEnemyParam[i].m_bExist)
            continue;

        if (s_aEnemyParam[i].m_container.m_eID != id)
            continue;

        if (s_aEnemyParam[i].m_container.m_iPattern == pattern)
            return i;
    };

    return -1;
};


/*static*/ int32 CEnemyParameter::GetHP(int32 index)
{
    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(s_aEnemyParam));
    ASSERT(s_aEnemyParam[index].m_bExist);

    return s_aEnemyParam[index].m_container.m_iHP;
};


/*static*/ uint8 CEnemyParameter::GetFrequency(int32 index, int32 num)
{
    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(s_aEnemyParam));
    ASSERT(s_aEnemyParam[index].m_bExist);
    ASSERT(num >= 0);
    ASSERT(num < COUNT_OF(ENEMYPARAMCONTAINER::m_auFrequency));

    return s_aEnemyParam[index].m_container.m_auFrequency[num];
};


/*static*/ int32 CEnemyParameter::GetFrequencyMax(int32 index)
{
    return COUNT_OF(ENEMYPARAMCONTAINER::m_auFrequency);
};


/*static*/ const ENEMYTYPES::CHARACTERISTIC& CEnemyParameter::GetAICharacteristic(int32 index)
{
    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(s_aEnemyParam));
    ASSERT(s_aEnemyParam[index].m_bExist);

    return s_aEnemyParam[index].m_container.m_AICharacteristic;
};


/*static*/ bool CEnemyParameter::Exist(ENEMYID::VALUE id)
{
    for (int32 i = 0; i < COUNT_OF(s_aEnemyParam); ++i)
    {
        if (!s_aEnemyParam[i].m_bExist)
            continue;

        if (s_aEnemyParam[i].m_container.m_eID != id)
            continue;

        return true;
    };

    return false;
};