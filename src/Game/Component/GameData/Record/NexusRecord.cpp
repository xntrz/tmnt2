#include "NexusRecord.hpp"

#include "Game/Component/GameData/GameTime.hpp"


CNexusRecord::CNexusRecord(void)
{
    ;
};


CNexusRecord::~CNexusRecord(void)
{
    ;
};


void CNexusRecord::SetDefault(void)
{
    m_nexusstate = NEXUSSTATE_OPEN;

    for (int32 i = 0; i < COUNT_OF(m_aNodeTour); ++i)
    {
        m_aNodeTour[i].m_state = STATE_NONE;
        m_aNodeTour[i].m_cleartime = 0;
    };

    for (int32 i = 0; i < COUNT_OF(m_aNodeStage); ++i)
    {
        m_aNodeStage[i].m_iNumPlayed = 0;
        m_aNodeStage[i].m_iNumCleared = 0;
        m_aNodeStage[i].m_cleartime = 0;
    };    
};


bool CNexusRecord::IsValid(void) const
{
    if (m_nexusstate < 0 || m_nexusstate > NEXUSSTATE_NORMAL)
    {
        OUTPUT(" %s is failed: nexus state\n", __FUNCTION__);
        return false;
    };

    for (int32 i = 0; i < COUNT_OF(m_aNodeTour); ++i)
    {
        if (m_aNodeTour[i].m_state < 0 ||
            m_aNodeTour[i].m_state > STATE_CLEAR)
        {
            OUTPUT(" %s is failed: tour node state\n", __FUNCTION__);
            return false;
        };

        if (m_aNodeTour[i].m_state != STATE_CLEAR &&
            (m_aNodeTour[i].m_cleartime.GetTotalSecond() > 0))
        {
            OUTPUT(" %s is failed: tour node clear time\n", __FUNCTION__);
            return false;
        };
    };

    for (int32 i = 0; i < COUNT_OF(m_aNodeStage); ++i)
    {
        if (m_aNodeStage[i].m_iNumPlayed < 0)
        {
            OUTPUT(" %s is failed: node stage num played\n", __FUNCTION__);
            return false;
        };

        if (m_aNodeStage[i].m_iNumCleared < 0 ||
            m_aNodeStage[i].m_iNumCleared > m_aNodeStage[i].m_iNumPlayed)
        {
            OUTPUT(" %s is failed: node stage num cleared\n", __FUNCTION__);
            return false;
        };

        if (!m_aNodeStage[i].m_iNumCleared &&
            (m_aNodeStage[i].m_cleartime.GetTotalSecond() > 0))
        {
            OUTPUT(" %s is failed: node stage clear time\n", __FUNCTION__);
            return false;
        };
    };

    OUTPUT(" %s ...OK\n", __FUNCTION__);
    return true;
};


void CNexusRecord::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_nexusstate = m_nexusstate;

    for (int32 i = 0; i < COUNT_OF(m_aNodeTour); ++i)
    {
        rRawData.m_aNodeTour[i].m_state = m_aNodeTour[i].m_state;
        rRawData.m_aNodeTour[i].m_cleartime = m_aNodeTour[i].m_cleartime;
    };

    for (int32 i = 0; i < COUNT_OF(m_aNodeStage); ++i)
    {
        rRawData.m_aNodeStage[i].m_iNumPlayed = m_aNodeStage[i].m_iNumPlayed;
        rRawData.m_aNodeStage[i].m_iNumCleared = m_aNodeStage[i].m_iNumCleared;
        rRawData.m_aNodeStage[i].m_cleartime = m_aNodeStage[i].m_cleartime;
    };
};


void CNexusRecord::Restore(const RAWDATA& rRawData)
{
    m_nexusstate = rRawData.m_nexusstate;

    for (int32 i = 0; i < COUNT_OF(m_aNodeTour); ++i)
    {
        m_aNodeTour[i].m_state = rRawData.m_aNodeTour[i].m_state;
        m_aNodeTour[i].m_cleartime = rRawData.m_aNodeTour[i].m_cleartime;
    };

    for (int32 i = 0; i < COUNT_OF(m_aNodeStage); ++i)
    {
        m_aNodeStage[i].m_iNumPlayed = rRawData.m_aNodeStage[i].m_iNumPlayed;
        m_aNodeStage[i].m_iNumCleared = rRawData.m_aNodeStage[i].m_iNumCleared;
        m_aNodeStage[i].m_cleartime = rRawData.m_aNodeStage[i].m_cleartime;
    };
};


void CNexusRecord::SetNexusState(NEXUSSTATE nexusstate)
{
    m_nexusstate = nexusstate;
};


CNexusRecord::NEXUSSTATE CNexusRecord::GetNexusState(void) const
{
    return m_nexusstate;
};


bool CNexusRecord::IsNewTournamentExisted(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_aNodeTour); ++i)
    {
        if (m_aNodeTour[i].m_state == STATE_OPEN)
            return true;
    };

    return false;
};


void CNexusRecord::SetTournamentState(GAMETYPES::NEXUSID idNexus, STATE state)
{
    ASSERT(state >= 0 && state <= STATE_CLEAR);
    
    NODETOUR& nodetour = m_aNodeTour[getTourIndex(idNexus)];

    if (nodetour.m_state < state)
        nodetour.m_state = state;
};


CNexusRecord::STATE CNexusRecord::GetTournamentState(GAMETYPES::NEXUSID idNexus) const
{
    return m_aNodeTour[getTourIndex(idNexus)].m_state;
};


void CNexusRecord::UpdateTournamentClearTime(GAMETYPES::NEXUSID idNexus, const CGameTime& cleartime)
{
    NODETOUR& nodetour = m_aNodeTour[getTourIndex(idNexus)];

    if (nodetour.m_cleartime < cleartime)
        nodetour.m_cleartime = cleartime;
};


const CGameTime& CNexusRecord::GetTournamentClearTime(GAMETYPES::NEXUSID idNexus) const
{
    return m_aNodeTour[getTourIndex(idNexus)].m_cleartime;
};


int32 CNexusRecord::GetTournamentPlayedStage(GAMETYPES::NEXUSID idNexus) const
{
    int32 iResult = 0;
    
    for (int32 i = 0; i < GAMETYPES::STAGE_MAX; ++i)
    {
        if (m_aNodeStage[getStageIndex(idNexus, i)].m_iNumPlayed <= 0)
            break;

        ++iResult;
    };

    return iResult;
};


void CNexusRecord::SetStagePlayed(GAMETYPES::NEXUSID idNexus, int32 stageNo)
{
    ++m_aNodeStage[getStageIndex(idNexus, stageNo)].m_iNumPlayed;
};


void CNexusRecord::UpdateStageClearTime(GAMETYPES::NEXUSID idNexus, int32 stageNo, const CGameTime& cleartime)
{
    NODESTAGE& nodestage = m_aNodeStage[getStageIndex(idNexus, stageNo)];

	++nodestage.m_iNumCleared;

    if (nodestage.m_cleartime < cleartime)
        nodestage.m_cleartime = cleartime;
};


int32 CNexusRecord::GetStagePlayCount(GAMETYPES::NEXUSID idNexus, int32 stageNo) const
{
    return m_aNodeStage[getStageIndex(idNexus, stageNo)].m_iNumPlayed;
};


int32 CNexusRecord::GetStageClearCount(GAMETYPES::NEXUSID idNexus, int32 stageNo) const
{
    return m_aNodeStage[getStageIndex(idNexus, stageNo)].m_iNumCleared;
};


int32 CNexusRecord::GetStageClearPercent(GAMETYPES::NEXUSID idNexus, int32 stageNo) const
{
    const NODESTAGE& nodestage = m_aNodeStage[getStageIndex(idNexus, stageNo)];

    if (nodestage.m_iNumPlayed)
        return int32(float(nodestage.m_iNumCleared) * 100.0f / float(nodestage.m_iNumPlayed));
    else
        return 0;
};


const CGameTime& CNexusRecord::GetStageClearTime(GAMETYPES::NEXUSID idNexus, int32 stageNo) const
{
	return m_aNodeStage[getStageIndex(idNexus, stageNo)].m_cleartime;
};


bool CNexusRecord::IsStagePlayed(GAMETYPES::NEXUSID idNexus, int32 stageNo) const
{
    return GetStagePlayCount(idNexus, stageNo) > 0;
};


int32 CNexusRecord::getTourIndex(GAMETYPES::NEXUSID idNexus) const
{
    ASSERT(idNexus >= 0 && idNexus < GAMETYPES::NEXUSID_NUM);
    
    return int32(idNexus);
};


int32 CNexusRecord::getStageIndex(GAMETYPES::NEXUSID idNexus, int32 stageNo) const
{
    ASSERT(idNexus >= 0 && idNexus < GAMETYPES::NEXUSID_NUM);
    ASSERT(stageNo >= 0 && stageNo < GAMETYPES::STAGE_MAX);

    return (idNexus * GAMETYPES::STAGE_MAX) + stageNo;
};

