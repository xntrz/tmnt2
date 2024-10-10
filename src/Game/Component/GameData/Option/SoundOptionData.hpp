#pragma once

#include "OptionTypes.hpp"

#include "Game/System/Sound/GameSound.hpp"


class CSoundOptionData
{
public:
    struct RAWDATA
    {
        CGameSound::MODE m_mode;
        int32 m_iVolumeSE;
        int32 m_iVolumeBGM;
        int32 m_iVolumeVoice;
    };

public:
    CSoundOptionData(void);
    ~CSoundOptionData(void);
    void Initialize(void);
    void Terminate(void);
    void SetDefault(void);
    void Apply(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetMode(CGameSound::MODE mode);
    void SetVolume(OPTIONTYPES::VOLUMETYPE volumetype, int32 value);
    CGameSound::MODE GetMode(void) const;
    int32 GetVolume(OPTIONTYPES::VOLUMETYPE volumetype) const;

private:
    CGameSound::MODE m_SdMode;
    CGameSound::MODE m_SdModeDefault;
    int32 m_iVolumeSE;
    int32 m_iVolumeBGM;
    int32 m_iVolumeVoice;
};