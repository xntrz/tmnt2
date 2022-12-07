#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"


class CMadCarGimmickManager final : public CGimmick
{
private:
    enum STATE
    {
        STATE_DEMO = 0,
        STATE_BATTLE_1,
        STATE_BATTLE_1_END,
        STATE_BATTLE_2,
        STATE_BATTLE_2_END,
        
        STATE_NUM,
    };

    enum SUBID
    {
        SUBID_START = 0,
        SUBID_BRIDGE_CHASE = SUBID_START,   // boss chase traffic lane
        SUBID_BRIDGE_FIGHT_L,               // boss fight left traffic lane
        SUBID_BRIDGE_FIGHT_R,               // boss fight right traffic lane
        
        SUBID_NUM,
    };

public:
    static float GetPathLength(const char* pszPathName);
    static bool IsPathDirection(const char* pszPathName);

    CMadCarGimmickManager(const char* pszName, void* pParam);
    virtual ~CMadCarGimmickManager(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE type) override;    

private:
    int32 m_subid;
    STATE m_eState;
    float m_fTimer;
    int32 m_anCarCreated[4];
    int32 m_nRndCnt;
    int32 m_nPathStart;
    int32 m_nPathEnd;
    float m_fTimerSE;
};