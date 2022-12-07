#pragma once

#include "Game/Component/GameMain/StageID.hpp"
#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Component/Effect/EffectID.hpp"


class CPlayer;
class CEffect;
class CGimmick;
class CCharacter;
class CGameObject;
class CEffect;


typedef struct tagSE_WALK_PARAM
{
    int32 Type;
    int32 Id;
    RwV3d Pos;
    int32 MoveType;
    int32 GroundType;
    int32 GroundParam;    
} SE_WALK_PARAM;


typedef struct tagSE_DAMAGE_PARAM
{
    RwV3d Pos;
    CGameObject* Attacker;
    int32 AttackerType;         //  GAMEOBJECTTYPE field
    int32 AttackerSubType;      //  CharacterType (for Character obj), Id (for Effect/Gimmick/Shot)
    int32 AttackerId;           //  CharacterId (for EnemyCharacter ENEMYID, for PlayerCharacter PLAYERID)
    const char* AttackerMotion; //  Motion name (valid only if obj type is Character)
    int32 DefenderType;         //  CharacterType only
    int32 DefenderId;           //  CharacterId (for EnemyCharacter ENEMYID, for PlayerCharacter PLAYERID)
    int32 AttackResult;         //  CHARACTERTYPES::ATTACKRESULTTYPE
} SE_DAMAGE_PARAM;


typedef struct tagSE_ATTACK_PARAM
{
    int32 Type;
    int32 Id;
    RwV3d Pos;
    const char* Motion;
} SE_ATTACK_PARAM;


class CGameSound
{
public:
    static const int32 VOLUME_MAX = 10;
    
    enum FADESPEED
    {
        FADESPEED_SLOW = 0,
        FADESPEED_NORMAL,
        FADESPEED_FAST,
        FADESPEEDNUM,
    };

    enum MODE
    {
        MODE_MONO = 0,
        MODE_STEREO,
        MODE_SURROUND,

        MODE_NUM,
    };

    struct VOICE_HIST
    {
        int32 m_nVoiceCode;
        PLAYERID::VALUE m_PlayerID;
    };

    struct LOOP_SE
    {
        const int32 m_nSeCode;
        int32 m_nRefCount;
    };

public:
    static void Initialize(void);
    static void Terminate(void);
    static void LoadWave(int32 nWaveNo);
    static bool IsLoadEnd(void);
    static void PlaySE(int32 nSE);
    static void StopSE(int32 nSE);
    static void FadeOutSE(int32 nSE, FADESPEED speed);
    static void PlayBGM(int32 nBGM);
    static void PlayVoice(int32 nVoice, PLAYERID::VALUE idPlayer);
    static bool IsVoicePlaying(void);
    static bool IsIDPlaying(PLAYERID::VALUE idPlayer);
    static void FadeOut(FADESPEED speed);
    static void Stop(void);
    static void Pause(void);
    static void Resume(void);
    static void AttachCamera(RwCamera* pCamera);
    static void DetachCamera(void);
    static void PlayPositionSE(const RwV3d* pPos, int32 nSE, int32 param);
    static void PlayObjectSE(const CCharacter* pCharacter, int32 nSE, int32 param = 0);
    static void PlayObjectSE(const CGimmick* pGimmick, int32 nSE, int32 param = 0);
    static void PlayObjectSE(const CEffect* pEffect, int32 nSE, int32 param = 0);
    static void PlayObjectSE(const CPlayer* pPlayer, int32 nSE, int32 param = 0);
    static void PlayAttackSE(const CCharacter* pCharacter);
    static void PlayDamageSE(SE_DAMAGE_PARAM* pParam);
    static void PlayDeathSE(const CCharacter* pCharacter);
    static void PlayOtherDamageSE(const CGameObject* pObjAttacker, const CGameObject* pObjDefender);
    static void PlayWalkSE(SE_WALK_PARAM* pParam);
    static void PlayEffectSE(CEffect* pEffect, EFFECTID::VALUE idEffect);
    static void StopEffectSE(CEffect* pEffect, EFFECTID::VALUE idEffect);
    static void SetMode(MODE mode);
    static MODE GetMode(void);
    static void SetBgmConfig(int32 value);
    static int32 GetBgmConfig(void);
    static void SetSeConfig(int32 value);
    static int32 GetSeConfig(void);
    static void SetVoiceConfig(int32 value);
    static int32 GetVoiceConfig(void);
    static void StageAfter(STAGEID::VALUE idStage);
    static void StageBefore(STAGEID::VALUE idStage);

private:
    static bool calcSEPositionInfo(const RwV3d* pvPos, int32& dist, int32& rot);
    static void getCameraInfo(const RwCamera* pRwCamera, RwV3d& pos, RwV3d& at);
    static int32 getEffectSE(EFFECTID::VALUE idEffect);
    static bool isLoopSE(int32 nSE);
    static void addReferenceCounter(int32 nSE);
    static bool subtractReferenceCounter(int32 nSE);
    static void clearReferenceCounter(int32 nSE);
    static void clearReferenceAll(void);
    static int32 convertToVolume(int32 nConfig);
    static int32 convertToRawConfig(int32 nVolume);

private:
    static int32 m_nPauseLevel;
    static int32 m_nSeConfig;
    static int32 m_nBgmConfig;
    static int32 m_nVoiceConfig;
    static MODE m_mode;
    static RwCamera* m_pCamera;
    static VOICE_HIST m_aVoiceHist[2];
    static int32 m_nVoiceHistSide;
    static LOOP_SE m_aLoopSe[];
};