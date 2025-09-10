#pragma once

#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class C091DrakoAI : public CAIModerator
{
public:
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
            ATTACK_ENERGY_BALL = 0,
            ATTACK_TAIL,
            ATTACK_SOMERSAULT,
            ATTACK_RUSH,
            ATTACK_ANTI_AIR,
            ATTACK_BEAM,
            ATTACK_QUAKE,
            ATTACK_THROW,
        };
    };

    enum AIPATTERN
    {
        AIPATTERN_HP_HIGH = 0,
        AIPATTERN_HP_MEDIUM,
        AIPATTERN_HP_LOW,

        AIPATTERN_DEFAULT = AIPATTERN_HP_HIGH,
    };

    struct PLAYER_DATA
    {
        RwV3d vecFootPos;
        float fDirection;
        PLAYERTYPES::STATUS eStatus;
    };

    struct PLAYER_INFO
    {
        enum SIDE : int16
        {
            SIDE_FRONT = 0,
            SIDE_LEFT,
            SIDE_RIGHT,
            SIDE_BACK,
        };

        float   fDistance;
        float   fDirection;
        SIDE    eSide;
        bool    bIsAttack;
        bool    bIsAttackCore;
        bool    bIsJump;
        bool    bIsDown;
        bool    bIsGuard;
        bool    bIsTrouble;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C091DrakoAI(CEnemyCharacter* pEnemyChr);
    virtual ~C091DrakoAI(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void GuardProc(void);
    bool ThinkProc(void);
    void ReserveOrderProc(void);
    void CheckPlayerStatus(void);
    void CheckAIPattern(void);
    void CheckDisableOrder(void);
    void SetReservedOrder(void);
    void ReserveOrderOnDamaged(void);
    void ReserveOrderOnGuard(void);
    void ReserveOrderDefault(void);
    void ReserveOrderDefault_OnHpHigh(void);
    void ReserveOrderDefault_OnHpMedium(void);
    void ReserveOrderDefault_OnHpLow(void);
    void ReserveIdle(void);
    void ReserveWalkNearestPlayer(void);
    void ReserveRunNearestPlayer(void);
    void ReserveGuard(void);
    void ReserveAttackEnergyBall(void);
    void ReserveAttackTail(void);
    void ReserveAttackSomersault(void);
    void ReserveAttackRush(void);
    void ReserveAttackAntiAir(void);
    void ReserveAttackBeam(void);
    void ReserveAttackQuake(void);
    void ReserveAttackThrow(void);
    bool CanBeam(void) const;
    bool CanQuake(void) const;
    float GetRandNormal(void);
    bool IsAttackStatus(PLAYERTYPES::STATUS status) const;
    bool IsJumpStatus(PLAYERTYPES::STATUS status) const;
    bool IsDownStatus(PLAYERTYPES::STATUS status) const;
    bool IsGuardStatus(PLAYERTYPES::STATUS status) const;
    int32 GetNearestPlayerNo(void) const;
    int32 GetNearBackPlayerNum(void) const;
    int32 GetNearSidePlayerNum(void) const;
    int32 GetNearFrontPlayerNum(void) const;
    int32 GetMiddleFrontPlayerNum(void) const;
    int32 GetMiddlePlayerNum(void) const;
    bool IsNearAttackPlayer(void) const;
    bool IsNearAttackCorePlayer(void) const;
    bool IsNearFrontDownPlayerOnly(void) const;
    bool IsNearFrontGuardPlayerOnly(void) const;
    bool CheckDebugOrder(void);
    
private:
    CRandNormal m_rand;
    AIPATTERN m_eAIPattern;
    float m_fThinkTimer;
    int32 m_numPlayers;
    PLAYER_DATA m_aPlayerData[GAMETYPES::PLAYERS_MAX];
    PLAYER_INFO m_aPlayerInfo[GAMETYPES::PLAYERS_MAX];
    CAIThinkOrder::ORDER m_eOrder;
    int32 m_iOrderType;
    int32 m_iOrderTargetNo;
    RwV3d m_vecOrderPos;
    float m_fGuardTimer;
    ENEMYTYPES::STATUS m_eMyPrevStatus;
    float m_fTimeoutBeam;
    float m_fTimeoutQuake;
    float m_fHpRatioPrev;
};