#include "PlayOptionData.hpp"


CPlayOptionData::CPlayOptionData(void)
{
    ;
};


CPlayOptionData::~CPlayOptionData(void)
{
    ;
};


void CPlayOptionData::Initialize(void)
{
    SetDefault();    
};


void CPlayOptionData::Terminate(void)
{
    ;
};


void CPlayOptionData::SetDefault(void)
{
    m_difficulty = GAMETYPES::DIFFICULTY_NORMAL;
    m_bAutosaveFlag = true;
};


void CPlayOptionData::Apply(void)
{
	if (!IsValid())
	{
		OUTPUT("%s is invalid! Set to default!\n", __FUNCTION__);
		SetDefault();
	};
};


bool CPlayOptionData::IsValid(void) const
{
    ASSERT(m_difficulty >= GAMETYPES::DIFFICULTY_EASY);
    ASSERT(m_difficulty <  GAMETYPES::DIFFICULTY_NUM);

    if ((m_difficulty <  GAMETYPES::DIFFICULTY_EASY) ||
        (m_difficulty >= GAMETYPES::DIFFICULTY_NUM))
        return false;

    return true;
};


void CPlayOptionData::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_difficulty    = m_difficulty;
    rRawData.m_bAutosaveFlag = m_bAutosaveFlag;
};


void CPlayOptionData::Restore(const RAWDATA& rRawData)
{
    m_difficulty    = rRawData.m_difficulty;
    m_bAutosaveFlag = rRawData.m_bAutosaveFlag;
};


void CPlayOptionData::SetDifficulty(GAMETYPES::DIFFICULTY difficulty)
{
    ASSERT(difficulty >= GAMETYPES::DIFFICULTY_EASY);
    ASSERT(difficulty <  GAMETYPES::DIFFICULTY_NUM);

    m_difficulty = difficulty;
};


void CPlayOptionData::SetEnableAutosave(bool bSet)
{
    m_bAutosaveFlag = bSet;
};


GAMETYPES::DIFFICULTY CPlayOptionData::GetDifficulty(void) const
{
    return m_difficulty;
};


bool CPlayOptionData::IsAutosaveEnabled(void) const
{
    return m_bAutosaveFlag;
};