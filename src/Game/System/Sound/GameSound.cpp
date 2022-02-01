#include "GameSound.hpp"


class CGameSoundConfiguration
{
public:
    int32 m_nPauseLevel;
    int32 m_nVolumeSE;
    int32 m_nVolumeBGM;
    int32 m_nVolumeVoice;
    CGameSound::MODE m_mode;
    RwCamera* m_pRwCamera;


    void Initialize(void);
    void Terminate(void);
};


void CGameSoundConfiguration::Initialize(void)
{
    m_nPauseLevel = 0;
    m_nVolumeSE = 0;
    m_nVolumeBGM = 0;
    m_nVolumeVoice = 0;
    m_mode = CGameSound::MODE_MONO;
    m_pRwCamera = nullptr;
};


void CGameSoundConfiguration::Terminate(void)
{
    ;
};


static CGameSoundConfiguration s_GameSoundConfiguration;


/*static*/ void CGameSound::Initialize(void)
{
    s_GameSoundConfiguration.Initialize();
};


/*static*/ void CGameSound::Terminate(void)
{
    s_GameSoundConfiguration.Terminate();
};


/*static*/ void CGameSound::LoadWave(int32 nWaveNo)
{
    ;
};


/*static*/ bool CGameSound::IsLoadEnd(void)
{
    return true;
};


/*static*/ void CGameSound::PlaySE(int32 nSE)
{
    ;
};


/*static*/ void CGameSound::StopSE(int32 nSE)
{
    ;
};


/*static*/ void CGameSound::FadeOutSE(int32 nSE, FADESPEED speed)
{
    ;
};


/*static*/ void CGameSound::PlayBGM(int32 nBGM)
{
    ;
};


/*static*/ void CGameSound::PlayVoice(int32 nVoice, PLAYERID::VALUE idPlayer)
{
    ;
};


/*static*/ bool CGameSound::IsVoicePlaying(void)
{
    return false;
};


/*static*/ bool CGameSound::IsIDPlayering(PLAYERID::VALUE idPlayer)
{
    return false;
};


/*static*/ void CGameSound::FadeOut(FADESPEED speed)
{
    ;
};


/*static*/ void CGameSound::Stop(void)
{
    ;
};


/*static*/ void CGameSound::Pause(void)
{
    ;
};


/*static*/ void CGameSound::Resume(void)
{
    ;
};


/*static*/ void CGameSound::AttachCamera(RwCamera* pRwCamera)
{
    ;
};


/*static*/ void CGameSound::DetachCamera(void)
{
    ;
};


/*static*/ void CGameSound::PlayPositionSE(const RwV3d* pPos, int32 nSE, int32 param)
{
    ;
};


/*static*/ void CGameSound::PlayObjectSE(const CCharacter* pCharacter, int32 nSE, int32 param)
{

};


/*static*/ void CGameSound::PlayObjectSE(const CGimmick* pGimmick, int32 nSE, int32 param)
{

};


/*static*/ void CGameSound::PlayObjectSE(const CEffect* pEffect, int32 nSE, int32 param)
{

};


/*static*/ void CGameSound::PlayObjectSE(const CPlayer* pPlayer, int32 nSE, int32 param)
{

};


/*static*/ void CGameSound::PlayAttackSE(const CCharacter* pCharacter)
{
    ;
};


/*static*/ void CGameSound::PlayDamageSE(void* pParam)
{
    ;
};


/*static*/ void CGameSound::PlayDeathSE(void* pCharacter)
{
    ;
};


/*static*/ void CGameSound::PlayOtherDamageSE(const CGameObject* pObjAttacher, const CGameObject* pObjDefender)
{
    ;
};


/*static*/ void CGameSound::PlayWalkSE(void* pParam)
{
    ;
};


/*static*/ void CGameSound::PlayEffectSE(CEffect* pEffect, EFFECTID::VALUE idEffect)
{
    ;
};


/*static*/ void CGameSound::StopEffectSE(CEffect* pEffect, EFFECTID::VALUE idEffect)
{
    ;
};


/*static*/ void CGameSound::SetMode(MODE mode)
{
    ;
};


/*static*/ CGameSound::MODE CGameSound::GetMode(void)
{
    return s_GameSoundConfiguration.m_mode;
};


/*static*/ void CGameSound::SetBgmConfig(int32 value)
{
    s_GameSoundConfiguration.m_nVolumeBGM = value;
};


/*static*/ int32 CGameSound::GetBgmConfig(void)
{
    return s_GameSoundConfiguration.m_nVolumeBGM;
};


/*static*/ void CGameSound::SetSeConfig(int32 value)
{
    s_GameSoundConfiguration.m_nVolumeSE = value;
};


/*static*/ int32 CGameSound::GetSeConfig(void)
{
    return s_GameSoundConfiguration.m_nVolumeSE;
};


/*static*/ void CGameSound::SetVoiceConfig(int32 value)
{
    s_GameSoundConfiguration.m_nVolumeVoice = value;
};


/*static*/ int32 CGameSound::GetVoiceConfig(void)
{
    return s_GameSoundConfiguration.m_nVolumeVoice;
};


/*static*/ void CGameSound::StageAfter(STAGEID::VALUE idStage)
{
    ;
};


/*static*/ void CGameSound::StageBefore(STAGEID::VALUE idStage)
{
    ;
};


/*static*/ void CGameSound::calcSEPositionInfo(int32& a, int32& b, const RwV3d* c)
{
    ;
};


/*static*/ void CGameSound::getCameraInfo(const RwCamera* pRwCamera, RwV3d& pos, RwV3d& at)
{
    ;
};


/*static*/ int32 CGameSound::getEffectSE(EFFECTID::VALUE idEffect)
{
    return 0;
};


/*static*/ bool CGameSound::isLoopSE(int32 nSE)
{
    return false;
};


/*static*/ void CGameSound::addReferenceCounter(int32 nSE)
{
    ;
};


/*static*/ void CGameSound::subtractReferenceCounter(int32 nSE)
{
    ;
};


/*static*/ void CGameSound::clearReferenceCounter(int32 nSE)
{
    ;
};


/*static*/ void CGameSound::clearReferenceAll(void)
{
    ;
};


/*static*/ int32 CGameSound::convertToVolume(int32 nConfig)
{
    return 0;
};


/*static*/ int32 CGameSound::convertToRawConfig(int32 nVolume)
{
    return 0;
};

