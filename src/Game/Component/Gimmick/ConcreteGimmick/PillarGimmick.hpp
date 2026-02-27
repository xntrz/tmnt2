#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"


class CPillarGimmickMove;


class CPillarGimmick final : public CGimmick
{
private:
    enum SHKSTEP
    {
        SHKSTEP_IDLE = 0,
        SHKSTEP_MOVE,
    };

    enum BRKSTEP
    {
        BRKSTEP_IDLE = 0,
        BRKSTEP_STEADY,
        BRKSTEP_MOVE,
        BRKSTEP_END,
    };
    
public:
    CPillarGimmick(const char* pszName, void* pParam);
    virtual ~CPillarGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    void onShake(void);
    void onCrumble(void);
    void shake(float dt);
    void crumble(float dt);
    
private:
    CPillarGimmickMove* m_pPillarMove;
    CNormalGimmickModel m_model;
    RwV3d m_vPrePosition;
    RwV3d m_vOrgPosition;
    float m_fTimer;
    float m_fRad;
    float m_fEffectTime;
    int32 m_step;
    uint32 m_hAtari;
};


class CSwitchPillarGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_OFF = 0,
        STATE_ON,

        STATENUM,
    };
    
public:
    CSwitchPillarGimmick(const char* pszName, void* pParam);
    virtual ~CSwitchPillarGimmick(void);        
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    bool isPlayerPreOnSwitchPillar(const char* pszName) const;

private:
    CNormalGimmickModel m_model;
    uint32 m_hAtari;
    RwV3d m_vOrgPosition;
    STATE m_eState;
    int32 m_nRidePlayer;
    char m_aszOnPlayerName[GAMETYPES::PLAYERS_MAX][GAMEOBJECTTYPES::GO_NAME_MAX];
    char m_aszPreOnPlayerName[GAMETYPES::PLAYERS_MAX][GAMEOBJECTTYPES::GO_NAME_MAX];
};