#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Effect/EffectID.hpp"


class CFallGimmickMove;
class CModuleManager;

class CFallGimmick : public CGimmick
{
public:
    enum SUBID
    {
        SUBID_START = 0,
        SUBID_TSURARA = SUBID_START,
        SUBID_STALACTITE,
        SUBID_CAVEIN,
        SUBID_SCRAP,
        
        SUBID_NUM,
    };

    enum STATE
    {
        STATE_WAIT = 0,
        STATE_SIGN,
        STATE_WAIT_TIMER,
        STATE_FALL,
        STATE_FALL_END,
        STATE_END,
        
        STATE_NUM,
    };

    enum MOVETYPE
    {
        MOVETYPE_START = 0,
        MOVETYPE_STOP = MOVETYPE_START,
        MOVETYPE_BOUND,
        
        MOVETYPE_NUM,
    };

    struct SUBINFO
    {
        const char* m_pszModelName;
        float m_fCollisionRadius;
        float m_fReflection;
        float m_fAttackRadius;
        float m_fFriction;
        int32 m_nPower;
        MOVETYPE m_eMovetype;
        EFFECTID::VALUE m_eFragmentId;
    };

private:
    static const SUBINFO m_aSubInfo[];

public:
    CFallGimmick(const char* pszName, void* pParam);
    virtual ~CFallGimmick(void);
    virtual void Run(void) override;
    virtual void Draw(void) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnTouchedDown(void) override;
    virtual void OnAttackResult(CHitCatchData* pCatch) override;
    void SetBodyHitRadiusAutoChangeEnable(bool bEnable);

    inline void SetPower(int32 nAmount)         { m_nPower = nAmount; };
    inline void SetInterval(float fInterval)    { m_fInterval = fInterval; };
    inline void SetTarget(uint32 target)        { m_target = target; };

private:
    void move(void);
    void fallGimmickCtrl(void);
    void changeStatus(STATE eState);
    void onGround(void);
    void setHitAttack(void);
    bool changeMaterialAlpha(void);
    void breakSign(void);
    bool heightCheck(void);
    void seCtrl(void);

private:
    CNormalGimmickModel m_model;
    CFallGimmickMove* m_pFallMove;
    CModuleManager* m_pModuleManager;
    uint32 m_target;
    SUBID m_eSubid;
    STATE m_eState;
    float m_fTimer;
    float m_fInterval;
    RwV3d m_vPreMovePosition;
    int32 m_nPower;
    float m_fFriction;
    bool m_bStart;
    bool m_bAttack;
    bool m_bTouched;
};