#include "OptionData.hpp"

#include "System/Common/Controller.hpp"


COptionData::COptionData(void)
{
    ;
};


COptionData::~COptionData(void)
{
    ;
};


void COptionData::Initialize(void)
{
    m_iGamepadNum = CController::Max();
    ASSERT(m_iGamepadNum > 0);
    ASSERT(m_iGamepadNum <= COUNT_OF(RAWDATA::m_aGamepad));

    m_paGamepad = new CGamepadOptionData[m_iGamepadNum];
    ASSERT(m_paGamepad);

    m_play.Initialize();
    m_sound.Initialize();
    m_display.Initialize();

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Initialize(i);

#ifdef TARGET_PC
    m_keyboard.Initialize();
#endif /* TARGET_PC */

    Apply();
};


void COptionData::Terminate(void)
{
#ifdef TARGET_PC
    m_keyboard.Terminate();
#endif /* TARGET_PC */

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Terminate();
    
    m_display.Terminate();
    m_sound.Terminate();
    m_play.Terminate();

    if (m_paGamepad)
    {
        delete[] m_paGamepad;
        m_paGamepad = nullptr;
        m_iGamepadNum = 0;
    };
};


void COptionData::Snapshot(RAWDATA& rRawData) const
{
    m_play.Snapshot(rRawData.m_play);
    m_sound.Snapshot(rRawData.m_sound);
    m_display.Snapshot(rRawData.m_display);

    ASSERT(m_iGamepadNum < COUNT_OF(rRawData.m_aGamepad));

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Snapshot(rRawData.m_aGamepad[i]);

#ifdef TARGET_PC
    m_keyboard.Snapshot(rRawData.m_keyboard);
#endif /* TARGET_PC */ 
};


void COptionData::Restore(const RAWDATA& rRawData)
{
    m_play.Restore(rRawData.m_play);
    m_sound.Restore(rRawData.m_sound);
    m_display.Restore(rRawData.m_display);

    ASSERT(m_iGamepadNum < COUNT_OF(rRawData.m_aGamepad));
    
    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Restore(rRawData.m_aGamepad[i]);

#ifdef TARGET_PC
    m_keyboard.Restore(rRawData.m_keyboard);
#endif /* TARGET_PC */    
};


void COptionData::Apply(void)
{
    m_play.Apply();
    m_sound.Apply();
    m_display.Apply();

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Apply();

#ifdef TARGET_PC
    m_keyboard.Apply();
#endif /* TARGET_PC */    
};


CPlayOptionData& COptionData::Play(void)
{
    return m_play;
};


CSoundOptionData& COptionData::Sound(void)
{
    return m_sound;
};


CDisplayOptionData& COptionData::Display(void)
{
    return m_display;
};


CKeyboardOptionData& COptionData::Keyboard(void)
{
    return m_keyboard;
};


CGamepadOptionData& COptionData::Gamepad(int32 no)
{
    ASSERT(no >= 0);
    ASSERT(no < CController::Max());

#if defined(TARGET_PC)
    return m_paGamepad[no];
#elif defined(TARGET_PS2)
    int32 iPort = CController::GetPhysicalPort(no);
    return m_paGamepad[iPort];
#else    
#error Not implemented for current target
#endif
};


int32 COptionData::GamepadNum(void) const
{
    return m_iGamepadNum;
};

