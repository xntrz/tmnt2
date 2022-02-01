#include "DatabaseRecord.hpp"


CDatabaseRecord::CDatabaseRecord(void)
{
    ;
};


CDatabaseRecord::~CDatabaseRecord(void)
{
    ;
};


void CDatabaseRecord::SetDefault(void)
{
    m_dbstate = DBSTATE_NONE;
    std::memset(m_aState, STATE_NONE, sizeof(m_aState));
};


bool CDatabaseRecord::IsValid(void) const
{
    if (m_dbstate < 0 && m_dbstate > DBSTATE_NORMAL)
    {
        OUTPUT("[GAME] %s is failed: db state\n", __FUNCTION__);
        return false;
    };

    for (int32 i = 0; i < COUNT_OF(m_aState); ++i)
    {
        if (m_aState[i] > STATE_READ)
        {
            OUTPUT("[GAME] %s is failed: node state\n", __FUNCTION__);
            return false;
        };
    };

    OUTPUT("[GAME] %s ...OK!\n", __FUNCTION__);
    return true;
};


void CDatabaseRecord::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_dbstate = m_dbstate;
    std::memcpy(rRawData.m_aState, m_aState, sizeof(rRawData.m_aState));
};


void CDatabaseRecord::Restore(const RAWDATA& rRawData)
{
    m_dbstate = rRawData.m_dbstate;
    std::memcpy(m_aState, rRawData.m_aState, sizeof(m_aState));
};


void CDatabaseRecord::SetDatabaseState(DBSTATE dbstate)
{
    m_dbstate = dbstate;
};


CDatabaseRecord::DBSTATE CDatabaseRecord::GetDatabaseState(void) const
{
    return m_dbstate;
};


bool CDatabaseRecord::IsNewItemExisted(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_aState); ++i)
    {
        if (getItemState(DBITEMID::VALUE(i)) == STATE_UNREAD)
            return true;
    };

    return false;
};


int32 CDatabaseRecord::CountUnlockedItemInGroup(GROUP group) const
{
    ASSERT(group >= 0 && group < GROUP_NUM);

    int32 iResult = 0;
    DBITEMID::VALUE idItemBase = DBITEMID::ID_NONE;
    DBITEMID::VALUE idItemMax = DBITEMID::ID_NONE;

    switch (group)
    {
    case GROUP_CHARACTER:
        idItemBase = DBITEMID::CHARACTERSTART;
        idItemMax = DBITEMID::CHARACTERMAX;
        break;

    case GROUP_ENEMY:
        idItemBase = DBITEMID::ENEMYSTART;
        idItemMax = DBITEMID::ENEMYMAX;
        break;

    case GROUP_ART:
        idItemBase = DBITEMID::ARTSTART;
        idItemMax = DBITEMID::ARTMAX;
        break;

    case GROUP_ETC:
        idItemBase = DBITEMID::ETCSTART;
        idItemMax = DBITEMID::ETCMAX;
        break;

    default:
        ASSERT(false);
        break;
    };

    for (int32 i = idItemBase; i < idItemMax; ++i)
    {
        if (IsItemUnlocked(DBITEMID::VALUE(i)))
            ++iResult;
    };

    return iResult;
};


bool CDatabaseRecord::IsItemReadInGroup(GROUP group) const
{
    ASSERT(group >= 0 && group < GROUP_NUM);

    bool bResult = true;
    DBITEMID::VALUE idItemBase = DBITEMID::ID_NONE;
    DBITEMID::VALUE idItemMax = DBITEMID::ID_NONE;
    
    switch (group)
    {
    case GROUP_CHARACTER:
        idItemBase = DBITEMID::CHARACTERSTART;
        idItemMax = DBITEMID::CHARACTERMAX;
        break;

    case GROUP_ENEMY:
        idItemBase = DBITEMID::ENEMYSTART;
        idItemMax = DBITEMID::ENEMYMAX;
        break;

    case GROUP_ART:
        idItemBase = DBITEMID::ARTSTART;
        idItemMax = DBITEMID::ARTMAX;
        break;

    case GROUP_ETC:
        idItemBase = DBITEMID::ETCSTART;
        idItemMax = DBITEMID::ETCMAX;
        break;

    default:
        ASSERT(false);
        break;
    };

    for (int32 i = idItemBase; i < idItemMax; ++i)
    {
        if (IsItemUnlocked(DBITEMID::VALUE(i)) && !IsItemRead(DBITEMID::VALUE(i)))
            bResult = false;
    };

    return bResult;
};


void CDatabaseRecord::SetItemUnlocked(DBITEMID::VALUE idItem)
{
    ASSERT(getItemState(idItem) == STATE_NONE);

    setItemState(idItem, STATE_UNREAD);
};


void CDatabaseRecord::SetItemRead(DBITEMID::VALUE idItem)
{
    ASSERT(getItemState(idItem) == STATE_UNREAD);
    
    setItemState(idItem, STATE_READ);
};


bool CDatabaseRecord::IsItemUnlocked(DBITEMID::VALUE idItem) const
{
    return (getItemState(idItem) != STATE_NONE);
};


bool CDatabaseRecord::IsItemRead(DBITEMID::VALUE idItem) const
{
    return (getItemState(idItem) > STATE_UNREAD);
};


CDatabaseRecord::STATE CDatabaseRecord::getItemState(DBITEMID::VALUE idItem) const
{
    ASSERT(idItem > DBITEMID::ID_NONE && idItem < DBITEMID::ID_MAX);

    return m_aState[idItem];
};


void CDatabaseRecord::setItemState(DBITEMID::VALUE idItem, STATE state)
{
    ASSERT(idItem > DBITEMID::ID_NONE && idItem < DBITEMID::ID_MAX);
    ASSERT(idItem >= 0 && idItem < COUNT_OF(m_aState));

    m_aState[idItem] = state;
    
    OUTPUT("[GAME] %s proc! Item: %d, state: %d\n", __FUNCTION__, idItem, state);
};

