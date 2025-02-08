#include "SoundOptionData.hpp"


CSoundOptionData::CSoundOptionData(void)
{
    ;
};


CSoundOptionData::~CSoundOptionData(void)
{
    ;
};


void CSoundOptionData::Initialize(void)
{
    m_SdMode        = CGameSound::GetMode();
    m_SdModeDefault = CGameSound::GetMode();
    m_iVolumeSE     = CGameSound::GetSeConfig();
    m_iVolumeBGM    = CGameSound::GetBgmConfig();
    m_iVolumeVoice  = CGameSound::GetVoiceConfig();
};


void CSoundOptionData::Terminate(void)
{
    ;
};


void CSoundOptionData::SetDefault(void)
{
    m_SdMode        = m_SdModeDefault;
    m_iVolumeSE     = CGameSound::VOLUME_MAX;
    m_iVolumeBGM    = CGameSound::VOLUME_MAX;
    m_iVolumeVoice  = CGameSound::VOLUME_MAX;
};


void CSoundOptionData::Apply(void)
{
    if (!IsValid())
        SetDefault();

    CGameSound::SetMode(m_SdMode);
    CGameSound::SetSeConfig(m_iVolumeSE);
    CGameSound::SetBgmConfig(m_iVolumeBGM);
    CGameSound::SetVoiceConfig(m_iVolumeVoice);
};


bool CSoundOptionData::IsValid(void) const
{
    if ((m_SdMode < CGameSound::MODE_MONO) || (m_SdMode > CGameSound::MODE_SURROUND))
        return false;

    if ((m_iVolumeSE < 0) && (m_iVolumeSE > CGameSound::VOLUME_MAX))
        return false;
    
    if ((m_iVolumeBGM < 0) && (m_iVolumeBGM > CGameSound::VOLUME_MAX))
        return false;
    
    if ((m_iVolumeVoice < 0) && (m_iVolumeVoice > CGameSound::VOLUME_MAX))
        return false;

    return true;
};


void CSoundOptionData::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_mode         = m_SdMode;
    rRawData.m_iVolumeSE    = m_iVolumeSE;
    rRawData.m_iVolumeBGM   = m_iVolumeBGM;
    rRawData.m_iVolumeVoice = m_iVolumeVoice;
};


void CSoundOptionData::Restore(const RAWDATA& rRawData)
{
    m_SdMode        = rRawData.m_mode;
    m_iVolumeSE     = rRawData.m_iVolumeSE;
    m_iVolumeBGM    = rRawData.m_iVolumeBGM;
    m_iVolumeVoice  = rRawData.m_iVolumeVoice;
};


void CSoundOptionData::SetMode(CGameSound::MODE mode)
{
    m_SdMode = mode;
};


void CSoundOptionData::SetVolume(OPTIONTYPES::VOLUMETYPE volumetype, int32 value)
{
    switch (volumetype)
    {
    case OPTIONTYPES::VOLUMETYPE_SE:
        m_iVolumeSE = value;
        break;

    case OPTIONTYPES::VOLUMETYPE_BGM:
        m_iVolumeBGM = value;
        break;

    case OPTIONTYPES::VOLUMETYPE_VOICE:
        m_iVolumeVoice = value;
        break;

    default:
        ASSERT(false);
        break;
    };
};


CGameSound::MODE CSoundOptionData::GetMode(void) const
{
    return m_SdMode;
};


int32 CSoundOptionData::GetVolume(OPTIONTYPES::VOLUMETYPE volumetype) const
{
    switch (volumetype)
    {
    case OPTIONTYPES::VOLUMETYPE_SE:
        return m_iVolumeSE;
        
    case OPTIONTYPES::VOLUMETYPE_BGM:
        return m_iVolumeBGM;
        
    case OPTIONTYPES::VOLUMETYPE_VOICE:
        return m_iVolumeVoice;

    default:
        ASSERT(false);
        return 0;
    };
};

