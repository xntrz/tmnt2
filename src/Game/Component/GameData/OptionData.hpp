#pragma once

#include "Option/DisplayOptionData.hpp"
#include "Option/GamepadOptionData.hpp"
#include "Option/KeyboardOptionData.hpp"
#include "Option/PlayOptionData.hpp"
#include "Option/SoundOptionData.hpp"
#include "Option/TouchOptionData.hpp"


class COptionData
{
public:
    struct RAWDATA
    {
        CPlayOptionData::RAWDATA     m_play;
        CSoundOptionData::RAWDATA    m_sound;
        CDisplayOptionData::RAWDATA  m_display;
        CGamepadOptionData::RAWDATA  m_aGamepad[6];

#if defined(TMNT2_FEATURE_KEYBOARD)
        IKeyboardOptionData::RAWDATA m_keyboard;
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
        CTouchOptionData::RAWDATA    m_touch;
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */
    };

    static_assert(std::is_pod<RAWDATA>::value, "option RAWDATA should be POD type");

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
    CGamepadOptionData& Gamepad(int32 controller);
    CGamepadOptionData* GamepadFromPort(int32 port);
    int32 GamepadNum(void) const;

#if defined(TMNT2_FEATURE_KEYBOARD)
    IKeyboardOptionData& Keyboard(void);
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */
    
#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    CTouchOptionData& Touch(void);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */    

private:
    CPlayOptionData      m_play;
    CSoundOptionData     m_sound;
    CDisplayOptionData   m_display;
    CGamepadOptionData*  m_pGamepad;
    int32                m_iGamepadNum;
    IKeyboardOptionData* m_pKeyboard;
    CTouchOptionData*    m_pTouch;
};