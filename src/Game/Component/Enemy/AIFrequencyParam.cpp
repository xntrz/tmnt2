#include "AIFrequencyParam.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"


CAIFrequencyParam::CAIFrequencyParam(void)
: m_nParam(0)
, m_pParamTable(nullptr)
{
    ;
};


CAIFrequencyParam::~CAIFrequencyParam(void)
{
    ;
};


void CAIFrequencyParam::Initialize(uint8* pParam, int32 nParam, int32 iLevelMax)
{
    GAMETYPES::DIFFICULTY Level = CGameProperty::GetDifficulty();
    ASSERT(Level >= iLevelMax);
    m_nParam = nParam;
    m_pParamTable = (pParam + (Level * m_nParam));
};


uint8 CAIFrequencyParam::GetFrequency(int32 No)
{
    ASSERT((No >= 0) && (No < m_nParam));
    ASSERT(m_pParamTable);
    return m_pParamTable[No];
};