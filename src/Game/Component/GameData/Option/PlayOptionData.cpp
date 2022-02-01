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
    m_difficulty = GAMETYPES::DIFFICULTY_EASY;
    m_bAutosaveFlag = true;
    m_bHelpFlag = true;
    m_bClassicInput = false;
};


void CPlayOptionData::Apply(void)
{
    ;
};


bool CPlayOptionData::IsValid(void) const
{
    ASSERT(m_difficulty >= GAMETYPES::DIFFICULTY_EASY && m_difficulty < GAMETYPES::DIFFICULTY_NUM);

    if (m_difficulty < GAMETYPES::DIFFICULTY_EASY ||
        m_difficulty >= GAMETYPES::DIFFICULTY_NUM)
        return false;

    return true;
};


void CPlayOptionData::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_difficulty = m_difficulty;
    rRawData.m_bAutosaveFlag = m_bAutosaveFlag;
    rRawData.m_bHelpFlag = m_bHelpFlag;
    rRawData.m_bClassicInput = m_bClassicInput;
};


void CPlayOptionData::Restore(const RAWDATA& rRawData)
{
    m_difficulty = rRawData.m_difficulty;
    m_bAutosaveFlag = rRawData.m_bAutosaveFlag;
    m_bHelpFlag = rRawData.m_bHelpFlag;
    m_bClassicInput = rRawData.m_bClassicInput;
};


void CPlayOptionData::SetDifficulty(GAMETYPES::DIFFICULTY difficulty)
{
    ASSERT(difficulty >= GAMETYPES::DIFFICULTY_EASY && difficulty <= GAMETYPES::DIFFICULTY_HARD);
    m_difficulty = difficulty;
};


void CPlayOptionData::SetEnableAutosave(bool bSet)
{
    m_bAutosaveFlag = bSet;
};


void CPlayOptionData::SetEnableHelp(bool bSet)
{
    m_bHelpFlag = bSet;
};


void CPlayOptionData::SetClassicInput(bool bSet)
{
    //
    //  Converts 3 buttons inputs to 2 buttons (CPlayerCharacter::CheckAttackConnec):
    //      A -> A
    //      B -> A
    //      C -> B
    // 
    
    m_bClassicInput = bSet;
};


GAMETYPES::DIFFICULTY CPlayOptionData::GetDifficulty(void) const
{
    return m_difficulty;
};


bool CPlayOptionData::IsAutosaveEnabled(void) const
{
    return m_bAutosaveFlag;
};


bool CPlayOptionData::IsHelpEnabled(void) const
{
    return m_bHelpFlag;
};


bool CPlayOptionData::IsClassicInput(void) const
{
    return m_bClassicInput;
};