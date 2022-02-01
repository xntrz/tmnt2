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


class CGameSound
{
public:
    static const int32 VOLUME_MAX = 10;
    
    enum FADESPEED
    {
        FADESPEED_SLOW = 0,
        FADESPEED_NORMAL,
        FADESPEED_FAST,
    };

    enum MODE
    {
        MODE_MONO = 0,
        MODE_STEREO,
        MODE_SURROUND,
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
    static bool IsIDPlayering(PLAYERID::VALUE idPlayer);
    static void FadeOut(FADESPEED speed);
    static void Stop(void);
    static void Pause(void);
    static void Resume(void);
    static void AttachCamera(RwCamera* pRwCamera);
    static void DetachCamera(void);
    static void PlayPositionSE(const RwV3d* pPos, int32 nSE, int32 param);
    static void PlayObjectSE(const CCharacter* pCharacter, int32 nSE, int32 param = 0);
    static void PlayObjectSE(const CGimmick* pGimmick, int32 nSE, int32 param = 0);
    static void PlayObjectSE(const CEffect* pEffect, int32 nSE, int32 param = 0);
    static void PlayObjectSE(const CPlayer* pPlayer, int32 nSE, int32 param = 0);
    static void PlayAttackSE(const CCharacter* pCharacter);
    static void PlayDamageSE(void* pParam);
    static void PlayDeathSE(void* pCharacter);
    static void PlayOtherDamageSE(const CGameObject* pObjAttacher, const CGameObject* pObjDefender);
    static void PlayWalkSE(void* pParam);
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
    static void calcSEPositionInfo(int32& a, int32& b, const RwV3d* c);
    static void getCameraInfo(const RwCamera* pRwCamera, RwV3d& pos, RwV3d& at);
    static int32 getEffectSE(EFFECTID::VALUE idEffect);
    static bool isLoopSE(int32 nSE);
    static void addReferenceCounter(int32 nSE);
    static void subtractReferenceCounter(int32 nSE);
    static void clearReferenceCounter(int32 nSE);
    static void clearReferenceAll(void);
    static int32 convertToVolume(int32 nConfig);
    static int32 convertToRawConfig(int32 nVolume);
};