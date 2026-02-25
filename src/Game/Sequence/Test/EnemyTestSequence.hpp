#pragma once

#ifdef _DEBUG

#include "Game/Sequence/Stage/StageBaseSequence.hpp"
#include "Game/Sequence/Stage/StageSeqState.hpp"


class CLoadEnemyTestSeqState : public CLoadTestSeqState
{
public:
    CLoadEnemyTestSeqState(MAPID::VALUE idMap, ENEMYID::VALUE idEnemy);
    CLoadEnemyTestSeqState(STAGEID::VALUE idStage, ENEMYID::VALUE idEnemy);
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;

private:
    ENEMYID::VALUE m_idEnemy;
};


class CPlayEnemyTestSeqState : public CPlayTestSeqState
{
public:
    CPlayEnemyTestSeqState(ENEMYID::VALUE idEnemy);
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;

private:
    ENEMYID::VALUE m_idEnemy;
};


class CEnemyTestSequence : public CStageBaseSequence
{
public:
    static CProcess* Instance(void);

    CEnemyTestSequence(void);
    virtual ~CEnemyTestSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnStateDetached(STATE state) override;
};

#endif /* _DEBUG */