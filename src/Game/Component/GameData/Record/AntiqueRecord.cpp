#include "AntiqueRecord.hpp"

#include "Game/Component/GameMain/AreaInfo.hpp"
#include "System/Common/SystemTime.hpp"


CAntiqueRecord::CAntiqueRecord(void)
{
    ;
};


CAntiqueRecord::~CAntiqueRecord(void)
{
    ;
};


void CAntiqueRecord::SetDefault(void)
{
    m_shopstate = SHOPSTATE_NONE;

    for (int32 i = 0; i < COUNT_OF(m_aNodeAntique); ++i)
    {
        m_aNodeAntique[i].m_state = STATE_NONE;
        m_aNodeAntique[i].m_takenDate = 0;
    };
};


bool CAntiqueRecord::IsValid(void) const
{
    if ((m_shopstate < SHOPSTATE_NONE) ||
        (m_shopstate > SHOPSTATE_COMPLETED))
    {
        OUTPUT(" %s is failed: shopstate\n", __FUNCTION__);
        return false;
    };

    for (int32 i = 0; i < COUNT_OF(m_aNodeAntique); ++i)
    {
        if ((m_aNodeAntique[i].m_state < STATE_NONE) ||
            (m_aNodeAntique[i].m_state > STATE_IDENTIFIED))
        {
            OUTPUT(" %s is failed: antique node state\n", __FUNCTION__);
            return false;
        };
    };

    OUTPUT(" %s ...OK!\n", __FUNCTION__);
    return true;
};


void CAntiqueRecord::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_shopstate = m_shopstate;

    for (int32 i = 0; i < COUNT_OF(m_aNodeAntique); ++i)
    {
        rRawData.m_abAntiqueState[i]     = m_aNodeAntique[i].m_state;
        rRawData.m_auAntiqueTakenDate[i] = m_aNodeAntique[i].m_takenDate;
    };
};


void CAntiqueRecord::Restore(const RAWDATA& rRawData)
{
    m_shopstate = rRawData.m_shopstate;

    for (int32 i = 0; i < COUNT_OF(m_aNodeAntique); ++i)
    {
        m_aNodeAntique[i].m_state       = rRawData.m_abAntiqueState[i];
        m_aNodeAntique[i].m_takenDate   = rRawData.m_auAntiqueTakenDate[i];
    };
};


void CAntiqueRecord::SetShopState(SHOPSTATE shopstate)
{
    m_shopstate = shopstate;
};


CAntiqueRecord::SHOPSTATE CAntiqueRecord::GetShopState(void) const
{
    return m_shopstate;
};


bool CAntiqueRecord::IsNewAntiqueExisted(void) const
{
    return (CountUnidentifiedAntique() > 0);
};


int32 CAntiqueRecord::CountUnidentifiedAntique(void) const
{
    int32 iResult = 0;

    for (int32 i = 0; i < COUNT_OF(m_aNodeAntique); ++i)
    {
        if (m_aNodeAntique[i].m_state == STATE_UNIDENTIFIED)
            ++iResult;
    };

    return iResult;
};


bool CAntiqueRecord::CheckJustCompleted(void)
{
    if (GetShopState() == SHOPSTATE_COMPLETED)
        return false;

    for (int32 i = 1; i < COUNT_OF(m_aNodeAntique); ++i)
    {
        if (m_aNodeAntique[i].m_state != STATE_IDENTIFIED)
            return false;
    };

    SetShopState(SHOPSTATE_COMPLETED);

    return true;
};


void CAntiqueRecord::SetAntiqueTaken(ANTIQUEID::VALUE idAntique)
{
    NODEANTIQUE& node = getAntiqueNode(idAntique);
    
    ASSERT(node.m_state == STATE_NONE);

    node.m_state     = STATE_UNIDENTIFIED;
    node.m_takenDate = CSystemTime::Instance().GetLocalTime().GetTimevalue();
};


void CAntiqueRecord::SetAntiqueIdentified(ANTIQUEID::VALUE idAntique)
{
    ASSERT(getAntiqueNode(idAntique).m_state == STATE_UNIDENTIFIED);
    
    getAntiqueNode(idAntique).m_state = STATE_IDENTIFIED;
};


bool CAntiqueRecord::IsAntiqueTaken(ANTIQUEID::VALUE idAntique) const
{
    return getAntiqueNode(idAntique).m_state > STATE_NONE;
};


bool CAntiqueRecord::IsAntiqueIdentified(ANTIQUEID::VALUE idAntique) const
{
    return getAntiqueNode(idAntique).m_state > STATE_UNIDENTIFIED;
};


void CAntiqueRecord::GetAntiqueTakenDate(ANTIQUEID::VALUE idAntique, CTimeObj& date)
{
    date.Init(getAntiqueNode(idAntique).m_takenDate);
};


bool CAntiqueRecord::IsAreaAntiqueTaken(AREAID::VALUE idArea) const
{
    ANTIQUEID::VALUE idAreaAntiq = CAreaInfo::GetAntiqueID(idArea);
    if (idAreaAntiq == ANTIQUEID::ID_NONE)
        return false;

    ASSERT(idAreaAntiq >= ANTIQUEID::ID_FIRST);
    ASSERT(idAreaAntiq <= ANTIQUEID::ID_MAX);

    return (getAntiqueNode(idAreaAntiq).m_state > STATE_NONE);
};


CAntiqueRecord::NODEANTIQUE& CAntiqueRecord::getAntiqueNode(ANTIQUEID::VALUE idAntique)
{
    ASSERT(idAntique >= 0);
    ASSERT(idAntique < ANTIQUEID::ID_MAX);

    return m_aNodeAntique[idAntique];
};


const CAntiqueRecord::NODEANTIQUE& CAntiqueRecord::getAntiqueNode(ANTIQUEID::VALUE idAntique) const
{
    ASSERT(idAntique >= 0);
    ASSERT(idAntique < ANTIQUEID::ID_MAX);

    return m_aNodeAntique[idAntique];
};
