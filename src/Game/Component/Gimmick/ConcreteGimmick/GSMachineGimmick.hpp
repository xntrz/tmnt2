#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CGSMachineGimmickMove;
class CGimmickMotion;
class CBodyHitData;


class CGSMachineGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_WAIT = 0,
        STATE_CRUISE,
        STATE_PRECHARGE,
        STATE_CHARGE,
        STATE_POSTCHARGE,
        STATE_SLEEP,
        STATE_STOP,
    };
    
public:
    CGSMachineGimmick(const char* pszName, void* pParam);
    virtual ~CGSMachineGimmick(void);
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnAttackResult(CHitCatchData* pCatch) override;
    void Wait(void);
    void Cruise(void);
    void PreCharge(void);
    void Charge(void);
    void PostCharge(void);
    void Sleep(void);
    void Stop(void);
    void CommonUpdate(void);
    void EffectStart(void);
    void EffectStop(void);
    void EffectUpdate(void);
    void SetGSState(STATE eState);
    float RateControl(float fCur, float fAim, float dt) const;
    float GetPathTimerateByMPS(float fMPS) const;
    
private:
    CGSMachineGimmickMove* m_pGSMachineGimmickMove;
    CBodyHitData* m_pBodyHitData;
    CNormalGimmickModel m_model;
    CGimmickMotion* m_pGimmickMotion;
    char m_szPathName[8];
    RwV3d m_vNearestPlayerPosition;
    float m_fDistance;
    float m_fNearestPathT;
    float m_fAimPlayrate;
    float m_fAimPathTimerate;
    float m_fPlayrate;
    float m_fPathTimerate;
    uint32 m_hEffectWaterSplash;
    STATE m_eState;
    float m_fStepTimer;
    int32 m_subid;
    bool m_bIsVib;
};