#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameData/GameTime.hpp"


class CNexusRecord
{
public:
    enum NEXUSSTATE
    {
        NEXUSSTATE_NONE = 0,// nexus tournament door at home not available to enter
        NEXUSSTATE_OPEN,    // nexus tournament door just open, but not entered yet (used for set RED text in home)
        NEXUSSTATE_NORMAL,  // nexus tournament door open, and entered at least once
    };

    enum STATE
    {
        STATE_NONE = 0, // "closed" state, tournament not available
        STATE_OPEN,     // "new" state, tournament just open but not started yet
        STATE_NORMAL,   // "open" state, tournament open and played atleast once
        STATE_CLEAR,    // "complete" state, tournament open and fully completed
    };
    
    struct NODETOUR
    {
        STATE  m_state;
        uint32 m_cleartime;
    };

    struct NODESTAGE
    {
        int16  m_numPlayed;
        int16  m_numCleared;
        uint32 m_cleartime;
    };

    struct RAWDATA
    {
        NEXUSSTATE m_nexusstate;
        NODETOUR   m_aNodeTour[GAMETYPES::NEXUSID_NUM];
        NODESTAGE  m_aNodeStage[GAMETYPES::STAGE_MAX * GAMETYPES::NEXUSID_NUM];
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
    CGameTime GetTournamentClearTime(GAMETYPES::NEXUSID idNexus) const;
    int32 GetTournamentPlayedStage(GAMETYPES::NEXUSID idNexus) const;
    void SetStagePlayed(GAMETYPES::NEXUSID idNexus, int32 stageNo);
    void UpdateStageClearTime(GAMETYPES::NEXUSID idNexus, int32 stageNo, const CGameTime& cleartime);
    int32 GetStagePlayCount(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    int32 GetStageClearCount(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    int32 GetStageClearPercent(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    CGameTime GetStageClearTime(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;
    bool IsStagePlayed(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;

private:
    int32 getTourIndex(GAMETYPES::NEXUSID idNexus) const;
    int32 getStageIndex(GAMETYPES::NEXUSID idNexus, int32 stageNo) const;

private:
    NEXUSSTATE m_nexusstate;
    NODETOUR m_aNodeTour[GAMETYPES::NEXUSID_NUM];
    NODESTAGE m_aNodeStage[GAMETYPES::STAGE_MAX * GAMETYPES::NEXUSID_NUM];
};