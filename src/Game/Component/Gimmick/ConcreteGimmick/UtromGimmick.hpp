#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"

#include "Game/System/Character/Character.hpp"
#include "Game/System/Hit/HitCatchData.hpp"


class CUtromGimmickMove;
class CModuleManager;
class CBodyHitData;
class CGimmickMotion;


class CUtromGimmick final : public CGimmick
{
private:
    friend class CUtromAreaCheckGimmick;
    
    enum STATE
    {
        STATE_WAIT = 0,
        STATE_LIFTED,
        STATE_WARP,
        STATE_WARP_OUT,
        STATE_RELEASE,
    };

    static const int32 UTROM_MAX = 30;

public:
    CUtromGimmick(const char* pszName, void* pParam);
    virtual ~CUtromGimmick(void);
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    virtual void Draw(void) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    virtual void OnCatch(CHitCatchData::RESULT* pResult);
    virtual void OnLift(CCharacter::MSG_LIFT_INFO* pMsgLiftInfo);
    virtual void OnMissThrow(RwV3d* pvVelocity);
    virtual void OnThrow(RwV3d* pvVelocity);

private:    
    void onThrow(RwV3d* pvVelocity, bool bMiss);
    
private:
    static int32 m_iUtromID;
    STATE m_eState;
    float m_fCounter;
    CNormalGimmickModel m_model;
    CBodyHitData* m_pBodyHitData;
    CModuleManager* m_pModuleMan;
    CGimmickMotion* m_pGimmickMotion;
    CUtromGimmickMove* m_pUtromMove;
};


class CUtromAreaCheckGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_WARNING = 0,
        STATE_INIT,
        STATE_ON,
    };

public:
    CUtromAreaCheckGimmick(const char* pszName, void* pParam);
    virtual ~CUtromAreaCheckGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    void AreaCheckInit(void);
    void AreaCheckOn(void);

private:
    STATE m_state;
    RwV3d m_vPosition;
    float m_fRadius;
    float m_fHeight;
    int32 m_iUtromCount;
    char m_szClearTargetName[16];
    CList<CUtromGimmick> m_listUtrom;
};