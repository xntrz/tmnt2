#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "AIUtils.hpp"


class CEnemyCharacter;
class CCharacterCompositor;


/* TODO: this functional relate to 6045 group, test it later */
class CPlayerStateWatcher
{
public:
    enum PLAYER_WATCH_FLAG
    {
        PLAYER_WATCH_ATTACK             = (1 << 0),
        PLAYER_WATCH_ATTACK_AERIAL      = (1 << 1),
        PLAYER_WATCH_ATTACK_COMBINATION = (1 << 2),
        PLAYER_WATCH_ATTACK_LEO_DASH    = (1 << 3),
        
        PLAYER_WATCH_SHOOT_KNIFE        = (1 << 4),
        PLAYER_WATCH_SHOOT_CHARGE       = (1 << 5),
        
        PLAYER_WATCH_STATE_CHARGE       = (1 << 6),
        PLAYER_WATCH_STATE_AERIAL       = (1 << 7),
        PLAYER_WATCH_STATE_GUARD        = (1 << 8),
        PLAYER_WATCH_STATE_DOWN         = (1 << 9),
        PLAYER_WATCH_STATE_FLYAWAY      = (1 << 10),
        PLAYER_WATCH_STATE_STUN         = (1 << 11),

        PLAYER_WATCH_ATTACK_ALL         = PLAYER_WATCH_ATTACK
                                        | PLAYER_WATCH_ATTACK_AERIAL
                                        | PLAYER_WATCH_ATTACK_COMBINATION
                                        | PLAYER_WATCH_ATTACK_LEO_DASH,

        PLAYER_WATCH_SHOOT_ALL          = PLAYER_WATCH_SHOOT_KNIFE
                                        | PLAYER_WATCH_SHOOT_CHARGE,

        PLAYER_WATCH_STATE_ALL          = PLAYER_WATCH_STATE_CHARGE
                                        | PLAYER_WATCH_STATE_AERIAL
                                        | PLAYER_WATCH_STATE_GUARD
                                        | PLAYER_WATCH_STATE_DOWN
                                        | PLAYER_WATCH_STATE_FLYAWAY
                                        | PLAYER_WATCH_STATE_STUN,
    };

    DEFINE_ENUM_FLAG_OPS(PLAYER_WATCH_FLAG, friend);
    
public:
    CPlayerStateWatcher(void);
    ~CPlayerStateWatcher(void);
    void Reset(void);
    void Update(void);
    bool IsAttack(int32 iPlayerIndex) const;
    bool IsAttackTrigger(int32 iPlayerIndex) const;
    bool IsAttackLeoDash(int32 iPlayerIndex) const;
    bool IsShootThrowKnife(int32 iPlayerIndex) const;
    bool IsShootChargeAttack(int32 iPlayerIndex) const;
    bool IsGuardState(int32 iPlayerIndex) const;
    bool IsDownState(int32 iPlayerIndex) const;
    bool IsDamagedFlyawayState(int32 iPlayerIndex) const;
    bool IsStunState(int32 iPlayerIndex) const;
    bool IsAllPlayerDownState(void) const;
    bool IsAllPlayerFlyawayOrDownState(void) const;    
    bool IsEnableFlag(int32 iPlayerIndex, uint32 uFlag) const;
    bool IsValidIndex(int32 iPlayerIndex) const;
    uint32 GetWatchFlag(PLAYERTYPES::STATUS eStatus, PLAYERID::VALUE idPlayer) const;

private:    
    uint32 Sub_GetAttackFlag(PLAYERTYPES::STATUS eStatus, PLAYERID::VALUE idPlayer) const;
    uint32 Sub_GetShootFlag(PLAYERTYPES::STATUS eStatus) const;
    uint32 Sub_GetStateFlag(PLAYERTYPES::STATUS eStatus) const;
    
private:
    uint32 m_aCurrentStateFlag[GAMETYPES::PLAYERS_MAX];
    uint32 m_aPrevStateFlag[GAMETYPES::PLAYERS_MAX];
};


/* TODO: this functional relate to 6045 group, test it later */
class CPlayerWatcher : public CPlayerStateWatcher
{
public:
    static const float DIST_MAX;
    
    struct PLAYERDATA
    {
        int32 No;
        RwV3d Position;
        float Length;
    };

    enum PLAYER_DATA_TYPE
    {
        PLAYER_DATA_TEMP = 0,
        PLAYER_DATA_NEAR,
        PLAYER_DATA_SUIT,
        PLAYER_DATA_ATTACK,
        PLAYER_DATA_TGT,
        PLAYER_DATA_MAX,
    };

    enum TARGET_TYPE
    {
        TARGET_TYPE_TEMP = 0,
        TARGET_TYPE_NEAR,
        TARGET_TYPE_SUIT,
        TARGET_TYPE_ATTACKLAST,
        TARGET_TYPE_ROCKON,        
    };

public:
    static float GetDistanceFromPlayer(int32 iPlayerIndex, RwV3d* pvPosition);
    static float GetDirection(const RwV3d* pvecPos, const RwV3d* pvecAt);

    CPlayerWatcher(void);
    ~CPlayerWatcher(void);
    void Initialize(CEnemyCharacter* pEnemyChara);
    void Update(void);
    bool IsAttackWithinRange(float fLength, float fFrontAngle);
    bool IsAttackTriggerWithinRange(float fLength, float fFrontAngle);
    bool IsShootShuriken(float fLength);
    bool IsShootShurikenTrigger(float fLength);
    bool IsChargeAttack(float fLength);
    bool IsHighAttack(float fLength, float fLowSide, float fHighSide);
    bool IsSuitableArea(float fViewAngle, float fLength = -1.0f);
    bool IsViewArea(CAIUtils::PLAYER_STATE_FLAG state);
    bool IsPlayerAttack(int32 iPlayerIndex);
    bool IsPlayerAttackTrigger(int32 iPlayerIndex);
    bool IsPlayerAttackTriggerWithinRange(int32 iPlayerIndex, float fLength, float fFrontAngle);
    bool IsPlayerShootShurikenTrigger(int32 iPlayerIndex, float fLength);
    bool IsPlayerViewArea(int32 iPlayerIndex);
    bool IsPlayerLookAtMe(int32 iPlayerIndex, float fAngle);
    bool IsPlayerInTheFront(int32 iPlayerIndex, float fFrontAngle);
    bool IsPlayerWithinRange(int32 iPlayerIndex, float fLength);
    bool IsPlayerSuitRange(int32 iPlayerIndex, float fRate);
    bool IsPlayerActionRange(int32 iPlayerIndex, float fRate);
    bool IsPlayerStop(int32 iPlayerIndex);
    bool IsDonBarrier(float fLength);
    bool IsLeoDash(float fLength);
    int32 GetPlayerNumThere(RwV3d* pvCenter, float fRadius);
    bool IsTogether(int32 iPlayerNum, float fRadius);
    int32 GetPlayerNoFromID(PLAYERID::VALUE id);
    int32 GetNearPlayer(RwV3d* pvCenter, float* pfDistNearest, RwV3d* pvPosNearest, CAIUtils::PLAYER_STATE_FLAG state);
    void SetTargetType(TARGET_TYPE type);
    void ClearTargetType(void);
    void SetPlayerData(int32 iPlayerIndex, PLAYER_DATA_TYPE type);
    PLAYERDATA* GetPlayerData(PLAYER_DATA_TYPE type);
    CEnemyCharacter& EnemyChara(void);
    int32 PlayerIndexCorrection(int32 iPlayerIndex);
    void GetTargetTypePosition(RwV3d* pvPos) const;
    int32 GetTargetTypeNo(void) const;

private:
    PLAYERDATA m_aPlayerData[PLAYER_DATA_MAX];
    int32 m_LastAttackType;
    RwV3d m_vMyPos;
    RwV3d m_vMyPosOld;
    float m_fMyAngle;
    TARGET_TYPE m_eTargetType;
    CEnemyCharacter* m_pEnemyChara;
    bool m_aPlayerJump[GAMETYPES::PLAYERS_MAX];
    float m_aPlayerKnife[GAMETYPES::PLAYERS_MAX];
    PLAYERTYPES::STATUS m_aPlayerStatusCurr[GAMETYPES::PLAYERS_MAX];
    PLAYERTYPES::STATUS m_aPlayerStatusPrev[GAMETYPES::PLAYERS_MAX];
};