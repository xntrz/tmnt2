#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameData/GameTime.hpp"


class CNexusRecord
{
public:
    enum NEXUSSTATE
    {
        NEXUSSTATE_NONE = 0,
        NEXUSSTATE_OPEN,
        NEXUSSTATE_NORMAL,
    };

    enum STATE
    {
        STATE_NONE = 0,
        STATE_OPEN,
        STATE_NORMAL,
        STATE_CLEAR,
    };
    
    struct NODETOUR
    {
        STATE m_state;
        CGameTime m_cleartime;
    };

    struct NODESTAGE
    {
        int16 m_numPlayed;
        int16 m_numCleared;
        CGameTime m_cleartime;
    };

    struct RAWDATA
    {
        NEXUSSTATE m_nexusstate;
        NODETOUR m_aNodeTour[GAMETYPES::NEXUSID_NUM];
        NODESTAGE m_aNodeStage[GAMETYPES::STAGE_MAX * GAMETYPES::NEXUSID_NUM];
    };

public:
    CNexusRecord(void);
    ~CNexusRecord(void);
    void SetDefault(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetNexusState(NEXUSSTATE nexusstate);
    NEXUSSTATE GetNexusState(void) const;
    bool IsNewTournamentExisted(void) const;
    void SetTournamentState(GAMETYPES::NEXUSID idNexus, STATE state);
    STATE GetTournamentState(GAMETYPES::NEXUSID idNexus) const;
    void UpdateTournamentClearTime(GAMETYPES::NEXUSID idNexus, const CGameTime& cleartime);
    const CGameTime& GetTournamentClearTime(GAMETYPES::NEXUSID idNexus) const;
    int32 GetTournamentPlayedStage(GAMETYPES::NEXUSID idNexus) const;
    void SetStagePlayed(GAMETYPES::NEXUSID idNexus, int32 stageNo);
    void UpdateStageClearTime(GAMETYPES::NEXUSID idNexus, int32 stageNo, const CGameTime& cleartime);
    int32 GetStagePlayCount(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    int32 GetStageClearCount(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    int32 GetStageClearPercent(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    const CGameTime& GetStageClearTime(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    bool IsStagePlayed(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;

private:
    int32 getTourIndex(GAMETYPES::NEXUSID idNexus) const;
    int32 getStageIndex(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;

private:
    NEXUSSTATE m_nexusstate;
    NODETOUR m_aNodeTour[GAMETYPES::NEXUSID_NUM];
    NODESTAGE m_aNodeStage[GAMETYPES::STAGE_MAX * GAMETYPES::NEXUSID_NUM];
};