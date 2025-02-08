#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameMain/AreaID.hpp"
#include "Game/Component/GameData/GameTime.hpp"


class CAreaRecord
{
public:
    enum STATE : uint8
    {
        STATE_NONE = 0,
        STATE_OPEN,
        STATE_CLEAR,
    };

    enum CLEAR_ROOT
    {
        CLEAR_ROOT_A = 0,
        CLEAR_ROOT_B,
        CLEAR_ROOT_C,
    };

    struct NODEAREA
    {
        STATE                m_state;
        GAMETYPES::CLEARRANK m_clearrank;
        uint32               m_cleartime;
    };

    struct RAWDATA
    {
        AREAID::VALUE        m_AreaSelected;
        uint32               m_rootflagAREA11;
        uint32               m_rootflagAREA25;
        uint32               m_rootflagAREA33;
        STATE                m_aAreaState[AREAID::SELECTABLEMAX];
        GAMETYPES::CLEARRANK m_aAreaClearrank[AREAID::NORMALMAX];
        uint32               m_aAreaCleartime[AREAID::NORMALMAX];
    };

public:
    CAreaRecord(void);
    ~CAreaRecord(void);
    void SetDefault(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void OnNewGame(void);
    bool IsNewGame(void) const;
    void RegistNewArea(void);
    void SetNowClearArea(void);
    AREAID::VALUE GetNowClearArea(void);
    void SetAreaOpened(AREAID::VALUE idArea);
    void SetAreaCleared(AREAID::VALUE idArea, CLEAR_ROOT clearroot);
    void SetAreaRank(AREAID::VALUE idArea, GAMETYPES::CLEARRANK clearrank);
    void UpdateAreaClearTime(AREAID::VALUE idArea, const CGameTime& cleartime);
    STATE GetAreaState(AREAID::VALUE idArea) const;
    bool IsAreaRootCleared(AREAID::VALUE idArea, CLEAR_ROOT clearroot) const;
    GAMETYPES::CLEARRANK GetAreaClearRank(AREAID::VALUE idArea) const;
    uint32 GetAreaClearTime(AREAID::VALUE idArea) const;
    void SetCurrentSelectedArea(AREAID::VALUE idArea);
    AREAID::VALUE GetCurrentSelectedArea(void) const;
    int32 CountRankedArea(GAMETYPES::CLEARRANK clearrank) const;
    GAMETYPES::CLEARRANK CalcTotalClearRank(void) const;
    void CalcTotalClearTime(CGameTime& totalCleartime) const;
    GAMETYPES::CLEARRANK CalcTotalClearTimeRank(void) const;
    
private:
    NODEAREA* getNode(AREAID::VALUE idArea) const;
    bool isSelectableArea(AREAID::VALUE idArea) const;
    void setClearRootFlag(AREAID::VALUE idArea, CLEAR_ROOT root, bool state);
    bool testClearRootFlag(AREAID::VALUE idArea, CLEAR_ROOT root) const;

private:
    AREAID::VALUE m_AreaNow;
    AREAID::VALUE m_AreaSelected;
    mutable NODEAREA m_aNodeArea[AREAID::SELECTABLEMAX];
    uint32 m_rootflagAREA11;
    uint32 m_rootflagAREA25;
    uint32 m_rootflagAREA33;
};