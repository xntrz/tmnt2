#pragma once

#include "Game/Component/Enemy/EnemyID.hpp"


class CEnemy;


class IGameStageExGauge
{
public:
    virtual ~IGameStageExGauge(void) {};
    virtual void Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual void Update(void) = 0;
};


class CBossExGauge : public IGameStageExGauge
{
private:
    static const int32 BOSS_MAX = 2;

public:
    virtual void Start(void) override;
    virtual void Stop(void) override;
    virtual void Update(void) override;
    void HandleEnemyDamaged(CEnemy* pEnemy, int32 nRemainHP);
    
private:
    bool isBoss(ENEMYID::VALUE enemyId, int32 no) const;
    ENEMYID::VALUE getStageBoss(int32 no) const;
    uint32 findBoss(int32 no) const;

private:
    uint32          m_ahBoss[BOSS_MAX];
    ENEMYID::VALUE  m_aBossId[BOSS_MAX];
    int32           m_nBossNum;
};


class CTimerExGauge : public IGameStageExGauge
{
public:
    virtual void Start(void) override;
    virtual void Stop(void) override;
    virtual void Update(void) override;

private:
    uint32 m_hObj;
    float m_fTimeNow;
    float m_fTimeTarget;
};


class CCounterExGauge : public IGameStageExGauge
{
public:
    virtual void Start(void) override;
    virtual void Stop(void) override;
    virtual void Update(void) override;  
};


class CLifeExGauge : public IGameStageExGauge
{
public:
    virtual void Start(void) override;
    virtual void Stop(void) override;
    virtual void Update(void) override;
    uint32 findFugitoid(void);

private:
    uint32 m_hObj;
};


class CThreeRaceExGauge : public IGameStageExGauge
{
public:
    virtual void Start(void) override;
    virtual void Stop(void) override;
    virtual void Update(void) override;
    void HandleEnemyDestroyed(CEnemy* pEnemy);
    
private:
    int32 m_nNumFoot;
    int32 m_nNumPDG;
    int32 m_nNumMafia;
};