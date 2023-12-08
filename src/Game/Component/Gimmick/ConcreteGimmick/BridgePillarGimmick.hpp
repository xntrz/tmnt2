#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CGimmickMotion;
class CCuttingGimmickMove;


class CBridgePillarGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_NONE = 0,
        STATE_WAIT,
        STATE_CUT,
        STATE_DISAPPEAR,
        STATE_END
    };
    
public:
    CBridgePillarGimmick(const char* pszName, void* pParam);
    virtual ~CBridgePillarGimmick(void);    
    virtual bool Query(CGimmickQuery* pQuery) const;
    virtual void PostMove(void);
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype);
    virtual void OnCatchAttack(CHitAttackData* pAttack);

private:
    void init(void* pParam);
    void onCut(void);
    void onBreakdown(void);
    void waiting(void);
    void cutting(void);
    void breakdown(void);
    void setBreakdownHitAttack(int32 Step);
    void setBreakdownSmokeEffect(int32 Step);

private:
    CCuttingGimmickMove* m_pCuttingMove;
    CNormalGimmickModel m_model;
    float m_fTimer;
    float m_fEffectTimer;
    float m_fInvincibleTimer;
    STATE m_eState;
    int32 m_step;
    int32 m_iLife;
    uint32 m_hAtari;
    CGimmickMotion* m_pGimmickMotion;
};