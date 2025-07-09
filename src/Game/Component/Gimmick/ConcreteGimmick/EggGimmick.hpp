#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/MoveStrategy/NormalGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GeneratorObj.hpp"


class CModuleManager;


class CEggGimmick final : public CGimmick
{
public:
    enum STATE
    {
        STATE_SHAKE = 0,
        STATE_GENERATE_ENEMY,
        STATE_ERASE_EGG,
        STATE_END,
    };

    enum SHAKESTATE
    {
        SHAKESTATE_IDLE = 0,
        SHAKESTATE_LONG_RANGE,
        SHAKESTATE_MIDDLE_RANGE,
        SHAKESTATE_SHORT_RANGE,
        
        SHAKESTATENUM,
    };

public:
    CEggGimmick(const char* pszName, void* pParam);
    virtual ~CEggGimmick(void);
    virtual void Run(void) override;
    virtual void Draw(void) const override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    void eggCtrl(void);
    void shakeMoveGrandCtrl(void);
    void shakeMoveCtrl(void);
    void changeShakeState(SHAKESTATE shakeState);
    void setHitAttack(void);
    bool birth(void);
    void birthForce(void);
    bool enemyGenerate(void);
    bool eraseEgg(void);

private:
    CSingleEnemyGenerator   m_generator;
    CNormalGimmickModel     m_model;
    CNormalGimmickMove      m_move;
    CModuleManager*         m_pModuleMan;
    float                   m_fNearestPlayerDist;
    float                   m_fTimer;
    STATE                   m_state;
    SHAKESTATE              m_shakeState;
    int32                   m_nShake;
};