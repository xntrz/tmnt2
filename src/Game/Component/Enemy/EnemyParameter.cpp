#include "EnemyParameter.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


static const char* ENEMYPARAM_FILEHEADER = "EPB_Ver1.0     ";
static const int32 ENEMYPARAM_MAX = 594;
static const int32 FREQUENCY_MAX = 27;


struct ENEMYPARAMCONTAINER
{
    int32 m_iHeader;
    ENEMYID::VALUE m_eID;
    int32 m_iPattern;
    GAMETYPES::DIFFICULTY m_eDifficulty;
    int32 m_iHP;
    int32 m_iAttack;
    int32 m_iGuard;
    ENEMYTYPES::CHARACTERISTIC m_AICharacteristic;
    uint8 m_auFrequency[FREQUENCY_MAX];
    uint8 m_Padding1;
    RwV3d m_vOrigin;
    float m_fRadius;
};


struct ENEMYPARAM
{
    bool m_bExist;
    ENEMYPARAMCONTAINER m_container;
};


static ENEMYPARAM s_aEnemyParam[ENEMYPARAM_MAX] = { 0 };


static inline void Rate(uint8* pFreqParam, float fRate)
{
    uint8 uFreqParam = *pFreqParam;
	uint8 uResult = uint8(float(uFreqParam) * fRate);
    
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
            pEnemyParam->m_iHP = int32(float(pEnemyParam->m_iHP) * 0.8f);
            Rate(&pEnemyParam->m_auFrequency[6], 0.0f);
            Rate(&pEnemyParam->m_auFrequency[7], 0.0f);
            Rate(&pEnemyParam->m_auFrequency[8], 0.0f);
            Rate(&pEnemyParam->m_auFrequency[9], 0.0f);
            Rate(&pEnemyParam->m_auFrequency[10], 0.0f);
            Rate(&pEnemyParam->m_auFrequency[11], 0.0f);
        }
        break;

    case GAMETYPES::DIFFICULTY_HARD:
        {
            Rate(&pEnemyParam->m_auFrequency[0], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[1], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[2], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[3], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[4], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[5], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[6], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[7], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[8], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[9], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[10], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[11], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[18], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[20], 1.05f);
            Rate(&pEnemyParam->m_auFrequency[21], 1.05f);
            pEnemyParam->m_AICharacteristic.m_fThinkingFrequency*= 1.05f;
            pEnemyParam->m_AICharacteristic.m_fRatioOfActivity  *= 1.05f;
            pEnemyParam->m_AICharacteristic.m_fRatioOfFrontView *= 1.05f;
            pEnemyParam->m_AICharacteristic.m_fRatioOfRearView  *= 1.05f;
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

    const char* ptr = (char*)Buffer;
    
    if (std::strncmp(ptr, ENEMYPARAM_FILEHEADER, std::strlen(ENEMYPARAM_FILEHEADER)))
        ASSERT(false);

    ptr += (std::strlen(ENEMYPARAM_FILEHEADER) + 1);

    int32 ParamNo = 0;
    uint32 ReadPos = 0;
    ENEMYPARAMCONTAINER TmpContainer;
    while (ReadPos < BufferSize)
    {
        std::memcpy(&TmpContainer, &ptr[ReadPos], sizeof(TmpContainer));
        
        if ((TmpContainer.m_iHeader == int32('IBPE')) &&
            (TmpContainer.m_eDifficulty == GAMETYPES::DIFFICULTY_NORMAL))
        {
            ASSERT((TmpContainer.m_eID >= ENEMYID::ID_START) && (TmpContainer.m_eID < ENEMYID::ID_MAX));

            s_aEnemyParam[ParamNo].m_bExist = true;
            std::memcpy(&s_aEnemyParam[ParamNo], &TmpContainer, sizeof(TmpContainer));

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
    ASSERT((index >= 0) && (index < COUNT_OF(s_aEnemyParam)));
    ASSERT(s_aEnemyParam[index].m_bExist);
    return s_aEnemyParam[index].m_container.m_iHP;
};


/*static*/ uint8 CEnemyParameter::GetFrequency(int32 index, int32 num)
{
    ASSERT((index >= 0) && (index < COUNT_OF(s_aEnemyParam)));
    ASSERT(s_aEnemyParam[index].m_bExist);
    ASSERT((num >= 0) && (num < COUNT_OF(ENEMYPARAMCONTAINER::m_auFrequency)));
    return s_aEnemyParam[index].m_container.m_auFrequency[num];
};


/*static*/ int32 CEnemyParameter::GetFrequencyMax(int32 index)
{
    return COUNT_OF(ENEMYPARAMCONTAINER::m_auFrequency);
};


/*static*/ const ENEMYTYPES::CHARACTERISTIC& CEnemyParameter::GetAICharacteristic(int32 index)
{
    ASSERT((index >= 0) && (index < COUNT_OF(s_aEnemyParam)));
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