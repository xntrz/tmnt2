#pragma once

#include "EnemyTestTypes.hpp"

#ifdef _DEBUG

#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/Sequence/Stage/StageSeqState.hpp"


class CLoadEnemyTestStageSeqState : public IStageSeqState
{
private:
    enum STEP
    {
        STEP_INIT = 0,
        STEP_LOAD_SOUND,
        STEP_LOAD_DATA,
        STEP_END,
    };

public:
    static const char* MakeFilepath(const char* pszFilename);
    static bool IsBoxLoadEnable(void);

    CLoadEnemyTestStageSeqState();
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;
    void LoadData(void);
    void LoadPlayers(void);
    void LoadStage(void);
    void LoadItems(void);
    void LoadEnemy(void);
    void LoadEnemyNeededEffects(ENEMYID::VALUE enemyId);
    void Regist(int32 fileId);
    void Regist(const char* pszFilename);

private:
    int32                       m_step;
    ENEMYTESTTYPES::TEST_CTX    m_testCtx;
};


class CPlayEnemyTestStageSeqState : public IStageSeqState
{
private:
    enum STEP
    {
        STEP_PLAYING = 0,
        STEP_FADEOUT,
        STEP_FADEOUT_WAIT,
        STEP_END,
    };

public:
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;
    virtual void OnDraw(const CStageBaseSequence* pSeq) const override;
    void UpdateItems(float dt);
    void CreateItems(void);
    int32 CountItems(void);
    void InitMenu(void);
    void ControlAI(bool bEnable);
    void CreateEnemy(int32 index);
    void CreateGimmicks(void);

private:
    static CPlayEnemyTestStageSeqState* m_pPlaySeqState;
    int32                               m_step;
    float                               m_fItemBoxRespawnWait;
    ENEMYTESTTYPES::TEST_CTX            m_testCtx;
    bool                                m_bAIState;
    const char*                         m_apszEnemyName[COUNT_OF(ENEMYTESTTYPES::TEST_CTX::enemyId)];    
    int32                               m_nextEnemyHP;
    int32                               m_nextEnemyPattern;
    float                               m_fNextEnemyPatrolRadius;
};

#endif /* _DEBUG */