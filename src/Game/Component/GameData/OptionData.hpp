#pragma once

#include "Option/DisplayOptionData.hpp"
#include "Option/GamepadOptionData.hpp"
#include "Option/KeyboardOptionData.hpp"
#include "Option/PlayOptionData.hpp"
#include "Option/SoundOptionData.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"


class COptionData
{
public:
    struct RAWDATA
    {
        CPlayOptionData::RAWDATA m_play;
        CSoundOptionData::RAWDATA m_sound;
        CDisplayOptionData::RAWDATA m_display;
        CGamepadOptionData::RAWDATA m_aGamepad[6];   // TODO см. IDB (asm код, декомпилятор не видит * 8)
#ifdef _TARGET_PC
        CKeyboardOptionData::RAWDATA m_keyboard;
#endif
    };
    
public:
    COptionData(void);
    ~COptionData(void);
    void Initialize(void);
    void Terminate(void);
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void Apply(void);
    CPlayOptionData& Play(void);
    CSoundOptionData& Sound(void);
    CDisplayOptionData& Display(void);
    CKeyboardOptionData& Keyboard(void);
    CGamepadOptionData& Gamepad(int32 no);
    int32 GamepadNum(void) const;

private:
    CPlayOptionData m_play;
    CSoundOptionData m_sound;
    CDisplayOptionData m_display;
    CKeyboardOptionData m_keyboard;
    CGamepadOptionData* m_paGamepad;
    int32 m_iGamepadNum;    
};