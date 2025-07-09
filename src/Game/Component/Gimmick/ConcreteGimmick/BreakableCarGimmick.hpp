#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CBreakableCarGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_EXPLOSION_SIGN,
        STATE_EXPLOSION,
        STATE_SMOLDER,
        STATE_END,
    };

    enum SMOKESTATE
    {
        SMOKESTATE_IDLE = 0,
        SMOKESTATE_GENERATE,
        SMOKESTATE_WAIT,
    };
        
public:
    CBreakableCarGimmick(const char* pszName, void* pParam);
    virtual ~CBreakableCarGimmick(void);
    virtual void Run(void) override;    
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    void carControl(void);
    bool smokeControl(bool bWaitErase);
    bool explosionControl(void);
    void setHitAttack(void);

private:
    int32               m_nLife;
    float               m_fTimer;
    float               m_fTimerExplosionInterval;
    STATE               m_eState;
    SMOKESTATE          m_eSmokeState;
    CNormalGimmickModel m_model;
    uint32              m_ahEffect[3];
    uint32              m_hAtari;
};