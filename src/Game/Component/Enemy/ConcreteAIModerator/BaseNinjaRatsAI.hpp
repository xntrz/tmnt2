#pragma once

#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CBaseNinjaRatsAI : public CAIModerator
{
public:
    static const int32 RAT_MAX = 4;

    struct ORDERTYPE
    {
        /*
         *  NOTE: WAIT & MOVE order type should be compatible with CCommonEnemyObserver::CMoveStatus
         */

        enum WAIT
        {
            WAIT_IDLE = 0,
            WAIT_TURN_POS,
            WAIT_TURN_NO,
        };

        enum MOVE
        {
            MOVE_WALK_POS = 0,
            MOVE_WALK_NO,
            MOVE_RUN_POS,
            MOVE_RUN_NO,
            MOVE_JUMP,
            MOVE_DASH,
        };

        enum ATTACK
        {
            ATTACK_CHAIN = 0,
            ATTACK_SINGLE,
            ATTACK_STUN,
            ATTACK_JUMP,
        };
    };

    enum STRATEGY
    {
        STRATEGY_WAIT = 0,
        STRATEGY_BARRAGE,
        STRATEGY_PATTERN,
        STRATEGY_FOLLOW,
        STRATEGY_MAN_TO_MAN,
        STRATEGY_HAPPY,

        STRATEGYNUM,
    };

    struct STATE
    {
        STRATEGY strategyExec;
        STRATEGY strategyThink;
        float    fWaitTime;
        int32    runCnt;
        float    fRotDir;
        float    fRotRad;
        float    fDegForRotRad;
        RwV3d    avecTargetPos[RAT_MAX];
        int32    targetPlayerNo;
        bool     abSdPlayAttack[RAT_MAX];
        bool     abSdPlayHappy[RAT_MAX];
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseNinjaRatsAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseNinjaRatsAI(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void ThinkStrategy(void);
    void ChangeStrategy(STRATEGY strategy);
    void CheckNormalAttack(void);
    void ExecuteWaitStrategy(void);
    void ExecuteWaitTactics(void);
    void InitBarrgeStrategy(void);
    void ExecuteBarrageStrategy(void);
    void ExecuteBarrageTactics(void);
    void ExecutePatternStrategy(void);
    void ExecutePatternTactics(void);
    void InitFollowStrategy(void);
    void ExecuteFollowStrategy(void);
    void ExecuteFollowTactics(void);
    void InitManToManStrategy(void);
    void ExecuteManToManStrategy(void);
    void ExecuteManToManTactics(void);
    void InitHappyStrategy(void);
    void ExecuteHappyStrategy(void);
    void ExecuteHappyTactics(void);
    int32 GetTargetRatNo(int32 playerNo) const;
    void GetNearestFrontPlrDist(int32* nearestPlayerNo, float* pfNearestPlayerDist) const;
    void GetFarthestFrontPlrDist(int32* farthestPlayerNo, float* pfFarthestPlayerDist) const;
    int32 GetFirstStunnedPlr(void) const;
    void PlayHappySE(void);
    void PlayAttackSE(void);

private:
    static STATE m_state;
    static int32 m_ratNum;
    int32 m_ratNo;
    bool m_bJumpAttack;
    float m_fRotRad;
    float m_fRotDir;
    int32 m_targetPlayerNo;
};