#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CCuttingGimmickMove;


class CGrassGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_NONE = 0,
        STATE_WAIT,
        STATE_CUT,
        STATE_BREAK,
        STATE_DISAPPEAR,
        STATE_END,
    };

    struct KINDINFO
    {
        const char* m_pszModelName;
        const char* m_pszBreakModelName;        
        float m_fHitRadius;        
    };

public:
    CGrassGimmick(const char* pszName, void* pParam);
    virtual ~CGrassGimmick(void);
    virtual void Draw(void) const override;
    virtual GIMMICKTYPES::FEATURE GetFeatures(void) const override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;

protected:
    void init(void* pParam);
    void waiting(void);
    void cutting(void);
    void breakdown(void);
    void disappear(void);
    void onCut(void);
    void onBreakDown(void);
    void onDisappear(void);
    const KINDINFO& kindinfo(void) const;

protected:
    static const KINDINFO m_aGrassGimmickKindInfoList[];
    CCuttingGimmickMove* m_pGrassMove;
    CNormalGimmickModel m_model;
    float m_fTimer;
    STATE m_state;
    int32 m_subid;
    bool m_bPlayerNearMe;    
};