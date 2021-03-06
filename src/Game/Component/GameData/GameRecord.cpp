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
    m_uTimestampInit = CSystemTime::Instance().GetLocalTime().GetTimevalue();
    m_antique.SetDefault();
    m_area.SetDefault();
    m_database.SetDefault();
    m_item.SetDefault();
    m_nexus.SetDefault();
    m_secret.SetDefault();
};


void CGameRecord::Terminate(void)
{
    ;
};


void CGameRecord::SetDefault(void)
{
    m_uTimestampInit = CSystemTime::Instance().GetLocalTime().GetTimevalue();
    m_antique.SetDefault();
    m_area.SetDefault();
    m_database.SetDefault();
    m_item.SetDefault();
    m_nexus.SetDefault();
    m_secret.SetDefault();
};


bool CGameRecord::IsValid(void) const
{
    if (!m_antique.IsValid()    ||
        !m_area.IsValid()       ||
        !m_database.IsValid()   ||
        !m_item.IsValid()       ||
        !m_nexus.IsValid()      ||
        !m_secret.IsValid())
        return false;

    return true;
};


void CGameRecord::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_uTimestampInit = m_uTimestampInit;
    m_antique.Snapshot(rRawData.m_antique);
    m_area.Snapshot(rRawData.m_area);
    m_database.Snapshot(rRawData.m_database);
    m_item.Snapshot(rRawData.m_item);
    m_nexus.Snapshot(rRawData.m_nexus);
    m_secret.Snapshot(rRawData.m_secret);
};


void CGameRecord::Restore(const RAWDATA& rRawData, bool bNotSavedFlag)
{
    m_uTimestampInit = rRawData.m_uTimestampInit;
    m_antique.Restore(rRawData.m_antique);
    m_area.Restore(rRawData.m_area);
    m_database.Restore(rRawData.m_database);
    m_item.Restore(rRawData.m_item);
    m_nexus.Restore(rRawData.m_nexus);
    m_secret.Restore(rRawData.m_secret);

    if (bNotSavedFlag)
        m_secret.SetNotSavedFlagOff();
};


uint32 CGameRecord::GetInitDate(void) const
{
    return m_uTimestampInit;
};


uint32 CGameRecord::GetAchievedPoint(void)
{
    uint32 uResult = 0;

    {
		//
		//	8
		//

        for (int32 i = 0; i < 4; ++i)
        {
            SECRETID::VALUE idSecret = SECRETID::VALUE(SECRETID::ID_CHARACTER_CASEY + i);
            if (Secret().IsUnlockedSecret(idSecret))
                uResult += 2;
        };
    }

    {
        //
        //  4
        //
        
        int32 nNumTakenAntique = 0;
        
        for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
        {
            if (Antique().IsAntiqueTaken(ANTIQUEID::VALUE(i)))
                ++nNumTakenAntique;
        };
        
        if (nNumTakenAntique >= 13)
            ++uResult;
        if (nNumTakenAntique >= 27)
            ++uResult;
        if (nNumTakenAntique >= 41)
            ++uResult;
        if (nNumTakenAntique >= 55)
            uResult += 2;
    }

    {
        //
        //  5
        //
        
        int32 nDBCounter = Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_CHARACTER);
        nDBCounter += Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ENEMY);
        nDBCounter += Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ART);
        nDBCounter += Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ETC);
        
        if (nDBCounter >= 9)
            ++uResult;
        if (nDBCounter >= 19)
            ++uResult;
        if (nDBCounter >= 29)
            ++uResult;
        if (nDBCounter >= 39)
            uResult += 2;
    }

    {
        //
        //  53
        //
        
        for (int32 i = AREAID::NORMALSTART; i < AREAID::NORMALMAX; ++i)
        {
            if (i == AREAID::HOME)
                continue;

            if (Area().GetAreaClearRank(AREAID::VALUE(i)) > GAMETYPES::CLEARRANK_NONE)
                ++uResult;
        };
    }

    {
        //
        //  24
        //
        
        for (int32 i = 1; i < GAMETYPES::CRYSTALTYPE_NUM; ++i)
        {
            int32 nCryNum = Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE(i));
            uResult += (2 * (nCryNum / 10));
        };
    }

    {
        //
        //  5
        //
        
        if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_KITTY_OPEN) >= CNexusRecord::STATE_CLEAR)
            ++uResult;

        if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN) >= CNexusRecord::STATE_CLEAR)
            ++uResult;

        if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT) >= CNexusRecord::STATE_CLEAR)
            ++uResult;

        if (Nexus().GetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS) >= CNexusRecord::STATE_CLEAR)
            uResult += 2;
    }

    return uResult;
};


CAntiqueRecord& CGameRecord::Antique(void)
{
    return m_antique;
};


CAreaRecord& CGameRecord::Area(void)
{
    return m_area;
};


CDatabaseRecord& CGameRecord::Database(void)
{
    return m_database;
};


CItemRecord& CGameRecord::Item(void)
{
    return m_item;
};


CNexusRecord& CGameRecord::Nexus(void)
{
    return m_nexus;
};


CSecretRecord& CGameRecord::Secret(void)
{
    return m_secret;
};

