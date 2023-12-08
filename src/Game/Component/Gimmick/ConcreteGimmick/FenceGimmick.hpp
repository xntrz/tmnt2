#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/MoveStrategy/FenceGimmickMove.hpp"


class CFenceGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_BREAK,
    };

    static const int32 LIFE = 120;

public:
    CFenceGimmick(const char* pszName, void* pParam);
    virtual ~CFenceGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    void EffectCallAtBreak(void);
    void SetVelocityAtBreak(const CGameObject* pObj);
    bool DecreaseMatAlpha(void);
    
private:
    CFenceGimmickMove m_move;
    CNormalGimmickModel m_model;
    int32 m_nLife;
    STATE m_eState;
    uint32 m_hAtari;
};