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

    ASSERT(Level < iLevelMax);

    m_nParam = nParam;
    m_pParamTable = (pParam + (Level * m_nParam));
};


void CAIFrequencyParam::DrawFreqTable(void)
{
    // TODO CBaseStoneBiterChr::Draw
};


uint8 CAIFrequencyParam::GetFrequency(int32 No) const
{
    ASSERT(No >= 0);
    ASSERT(No < m_nParam);
    ASSERT(m_pParamTable);

    return m_pParamTable[No];
};