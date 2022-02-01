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

    m_paGamepad = new CGamepadOptionData[ 8 ];
    ASSERT(m_paGamepad);

    m_play.Initialize();
    m_sound.Initialize();
    m_display.Initialize();

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Initialize(i);

#ifdef _TARGET_PC
    m_keyboard.Initialize();
#endif

    Apply();
};


void COptionData::Terminate(void)
{
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

#ifdef _TARGET_PC
    m_keyboard.Snapshot(rRawData.m_keyboard);
#endif 
};


void COptionData::Restore(const RAWDATA& rRawData)
{
    m_play.Restore(rRawData.m_play);
    m_sound.Restore(rRawData.m_sound);
    m_display.Restore(rRawData.m_display);

    ASSERT(m_iGamepadNum < COUNT_OF(rRawData.m_aGamepad));
    
    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Restore(rRawData.m_aGamepad[i]);

#ifdef _TARGET_PC
    m_keyboard.Restore(rRawData.m_keyboard);
#endif    
};


void COptionData::Apply(void)
{
    m_play.Apply();
    m_sound.Apply();
    m_display.Apply();

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Apply();

#ifdef _TARGET_PC
    m_keyboard.Apply();
#endif    
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
    ASSERT(no >= 0 && no < CController::Max());
    
    int32 iPort = CController::GetPhysicalPort(no);
    ASSERT(iPort >= 0 && iPort < m_iGamepadNum);

    return *&m_paGamepad[iPort];
};


int32 COptionData::GamepadNum(void) const
{
    return m_iGamepadNum;
};

