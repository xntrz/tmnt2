#pragma once

#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class C087SlashuurAI : public CAIModerator
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
            ATTACK_A = 0,
            ATTACK_AA,
            ATTACK_AAB,
            ATTACK_AAC,
            ATTACK_B,
            ATTACK_ESCAPE,
            ATTACK_TELEPORT,
            ATTACK_TELEPORT_SCYTHE,
            ATTACK_TELEPORT_DRAIN,
            ATTACK_SHOT,
            ATTACK_RUN,
            ATTACK_THROW,
        };
    };

    enum AIPATTERN
    {
        AIPATTERN_TELEPORT_ATTACK = 0,
        AIPATTERN_TELEPORT_SCYTHE,
        AIPATTERN_TELEPORT_DRAIN,

        AIPATTERN_DEFAULT = AIPATTERN_TELEPORT_ATTACK,
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

    C087SlashuurAI(CEnemyCharacter* pEnemyChr);
    virtual ~C087SlashuurAI(void);
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
    void ReserveOrderOnAttackA(void);
    void ReserveOrderOnAttackAA(void);
    void ReserveOrderDefault(void);
    void ReserveIdle(void);
    void ReserveWalkNearestPlayer(void);
    void ReserveRunNearestPlayer(void);
    void ReserveEscapeDash(void);
    void ReserveGuard(void);
    void ReserveAttack(int32 iAttackOrderType);
    void ReserveNormalAttack(void);
    void ReserveNormalAttackB(void);    
    void ReserveRunAttack(void);
    void ReserveTeleportationAttack(void);
    void ReserveTeleportationScythe(void);
    void ReserveTeleportationDrain(void);
    void ReserveEscapeAttack(void);
    void ReserveShot(void);
    bool CanDrain(void) const;
    bool CanRunAttack(void) const;
    bool CanEscape(void) const;
    bool CanShot(void) const;
    bool CanTeleportation(void) const;
    float GetRandNormal(void);
    bool IsAttackStatus(PLAYERTYPES::STATUS status) const;
    bool IsJumpStatus(PLAYERTYPES::STATUS status) const;
    bool IsDownStatus(PLAYERTYPES::STATUS status) const;
    bool IsGuardStatus(PLAYERTYPES::STATUS status) const;
    bool IsTroubleStatus(PLAYERTYPES::STATUS status) const;
    int32 GetNearestPlayerNo(void) const;
    int32 GetNearPlayerNum(void) const;
    int32 GetNearBackPlayerNum(void) const;
    int32 GetNearSidePlayerNum(void) const;
    int32 GetNearFrontPlayerNum(void) const;
    int32 GetNearNotFrontPlayerNum(void) const;
    int32 GetMiddleFrontPlayerNum(void) const;
    int32 GetMiddleNotFrontPlayerNum(void) const;
    int32 GetFarFrontPlayerNum(void) const;
    bool IsNearAttackPlayer(void) const;
    bool IsNearAttackCorePlayer(void) const;
    bool IsMiddleFrontAttackPlayer(void) const;
    bool IsNearFrontDownPlayerOnly(void) const;
    bool IsMiddleFrontDownPlayerOnly(void) const;
    bool IsNearFrontTroublePlayerOnly(void) const;
    bool IsNearSideTroublePlayerOnly(void) const;
    bool IsTroublePlayerOnly(void) const;
    bool CanEscapeDashDir(float fDir) const;
    float GetEscapeDashDir(void) const;
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
    int32 m_iTeleportReps;
    float m_fTimeoutTeleport;
    float m_fTimeoutShot;
    float m_fTimeoutEscape;
    float m_fTimeoutRunAttack;
    float m_fHpRatioPrev;
    bool m_bDrainAbility;
};