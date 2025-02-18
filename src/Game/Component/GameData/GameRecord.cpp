#include "GameRecord.hpp"

#include "System/Common/SystemTime.hpp"


CGameRecord::CGameRecord(void)
{
    ;
};


CGameRecord::~CGameRecord(void)
{
    ;
};


void CGameRecord::Initialize(void)
{
    SetDefault();
};


void CGameRecord::Terminate(void)
{
    ;
};


void CGameRecord::SetDefault(void)
{
    m_uTimestampInit = CSystemTime::Instance().GetLocalTime().GetTimevalue();
    m_item.SetDefault();
    m_antique.SetDefault();
    m_database.SetDefault();
    m_nexus.SetDefault();
    m_area.SetDefault();
    m_secret.SetDefault();
};


bool CGameRecord::IsValid(void) const
{
    if (!m_item.IsValid())
        return false;

    if (!m_antique.IsValid())
        return false;

    if (!m_database.IsValid())
        return false;

    if (!m_nexus.IsValid())
        return false;

    if (!m_area.IsValid())
        return false;

    if (!m_secret.IsValid())
        return false;

    return true;
};


void CGameRecord::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_uTimestampInit = m_uTimestampInit;
    m_item.Snapshot(rRawData.m_item);
    m_antique.Snapshot(rRawData.m_antique);
    m_database.Snapshot(rRawData.m_database);
    m_nexus.Snapshot(rRawData.m_nexus);
    m_area.Snapshot(rRawData.m_area);
    m_secret.Snapshot(rRawData.m_secret);
};


void CGameRecord::Restore(const RAWDATA& rRawData, bool bNotSavedFlag)
{
    m_uTimestampInit = rRawData.m_uTimestampInit;
    m_item.Restore(rRawData.m_item);
    m_antique.Restore(rRawData.m_antique);
    m_database.Restore(rRawData.m_database);
    m_nexus.Restore(rRawData.m_nexus);
    m_area.Restore(rRawData.m_area);
    m_secret.Restore(rRawData.m_secret);

    if (bNotSavedFlag)
        m_secret.SetNotSavedFlagOff();
};


uint32 CGameRecord::GetInitDate(void) const
{
    return m_uTimestampInit;
};


int32 CGameRecord::GetAchievedPoint(void) const
{
    //
    //  Total achieved points in percent
    //
    int32 result = 0;

    //
    //  Calc unlocked character points
    //  Max points: 8
    //
    for (int32 i = 0; i < 4; ++i)
    {
        SECRETID::VALUE idSecret = SECRETID::VALUE(SECRETID::ID_CHARACTER_CASEY + i);
        if (Secret().IsUnlockedSecret(idSecret))
            result += 2;
    };

    //
    //  Calc antique taken points
    //  Max points: 5
    //
    int32 nNumTakenAntique = 0;

    for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
    {
        if (Antique().IsAntiqueTaken(ANTIQUEID::VALUE(i)))
            ++nNumTakenAntique;
    };

    if (nNumTakenAntique >= 13)
        ++result;
    if (nNumTakenAntique >= 27)
        ++result;
    if (nNumTakenAntique >= 41)
        ++result;
    if (nNumTakenAntique >= 55)
        result += 2;

    //
    //  Calc database points
    //  Max points: 5
    //
    int32 nDBCounter = Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_CHARACTER);
    nDBCounter += Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ENEMY);
    nDBCounter += Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ART);
    nDBCounter += Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ETC);

    if (nDBCounter >= 9)
        ++result;
    if (nDBCounter >= 19)
        ++result;
    if (nDBCounter >= 29)
        ++result;
    if (nDBCounter >= 39)
        result += 2;

    //
    //  Calc area clear points
    //  Max points: 53
    //  NOTE: exclude home area --> (AREAID::NORMALMAX - AREAID::NORMALSTART) - 1
    //
    for (int32 i = AREAID::NORMALSTART; i < AREAID::NORMALMAX; ++i)
    {
        if (i == AREAID::HOME)
            continue;

        if (Area().GetAreaClearRank(AREAID::VALUE(i)) > GAMETYPES::CLEARRANK_NONE)
            ++result;
    };

    //
    //  Calc crystal taken points
    //  Max points: 24
    //
    for (int32 i = 1; i < GAMETYPES::CRYSTALTYPE_NUM; ++i)
    {
        int32 nCryNum = Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE(i));
        result += (2 * (nCryNum / 10));
    };

    //
    //  Calc nexus clear points
    //  Max points: 5
    //
    if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_KITTY_OPEN) >= CNexusRecord::STATE_CLEAR)
        ++result;

    if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN) >= CNexusRecord::STATE_CLEAR)
        ++result;

    if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT) >= CNexusRecord::STATE_CLEAR)
        ++result;

    if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS) >= CNexusRecord::STATE_CLEAR)
        result += 2;

    ASSERT(result >= 0);
    ASSERT(result <= 100);

    return result;
};