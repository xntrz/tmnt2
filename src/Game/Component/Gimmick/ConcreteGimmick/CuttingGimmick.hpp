#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CCuttingGimmickMove;


class CCuttingGimmickModel : public CNormalGimmickModel
{
public:
    enum STATE
    {
        STATE_NORMAL = 0,
        STATE_DESTROY,
        STATE_END,
    };

    enum CUTMODELKIND
    {
        CUTMODELKIND_NORMAL = 0,
        CUTMODELKIND_ATARI,
        CUTMODELKIND_NORMAL_DOWN,
        CUTMODELKIND_NORMAL_UP,
    };

public:
    CCuttingGimmickModel(void);
    virtual ~CCuttingGimmickModel(void);
    virtual void Draw(void) const override;
    void SetState(STATE state);
    
protected:
    STATE m_state;
};


class CCuttingGimmick : public CGimmick
{
protected:
    static const int32 NUM_ATTACK_TO_BREAK;

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
        const char* m_pszAtariModelName;
        const char* m_pszBreakDModelName;
        const char* m_pszBreakUModelName;
        float m_fSlideTime;
        float m_fHitRadius;
        float m_fCollisionRadius;
        bool m_bLighting;
        int32 m_nSE;
    };
    
public:    
    CCuttingGimmick(const char* pszName, void* pParam);
    virtual ~CCuttingGimmick(void);
    virtual void Draw(void) const override;
    virtual GIMMICKTYPES::FEATURE GetFeatures(void) const override;
    virtual bool Query(CGimmickQuery* pQuery) const override;
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
    static const KINDINFO m_aCuttingGimmickKindInfoList[];
    CCuttingGimmickMove* m_pCuttingMove;
    CCuttingGimmickModel m_model;
    float m_fTimer;
    STATE m_state;
    int32 m_subid;
    uint32 m_hAtari;
    int32 m_nNumAttack;
    float m_fInvinsibleTimer;
};