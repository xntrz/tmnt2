#include "OptionData.hpp"

#include "System/Common/Controller.hpp"

#if defined(TARGET_PC)
#include "Option/KeyboardOptionDataWin.hpp"
#elif defined(TARGET_WEB)
#include "Option/KeyboardOptionDataWeb.hpp"
#endif


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

#if defined(TMNT2_FEATURE_KEYBOARD)

#if defined(TARGET_PC)
    m_pKeyboard = new CKeyboardOptionDataWin;
#elif defined(TARGET_WEB)
    m_pKeyboard = new CKeyboardOptionDataWeb;
#endif
    if (m_pKeyboard)
        m_pKeyboard->Initialize();

#endif /* defined(TMNT2_FEATURE_KEYBOARD) */
    
    Apply();
};


void COptionData::Terminate(void)
{
#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
    {
        m_pKeyboard->Terminate();
        delete m_pKeyboard;
        m_pKeyboard = nullptr;
    };
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */

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

#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
        m_pKeyboard->Snapshot(rRawData.m_keyboard);
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */
};


void COptionData::Restore(const RAWDATA& rRawData)
{
    m_play.Restore(rRawData.m_play);
    m_sound.Restore(rRawData.m_sound);
    m_display.Restore(rRawData.m_display);

    ASSERT(m_iGamepadNum < COUNT_OF(rRawData.m_aGamepad));
    
    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Restore(rRawData.m_aGamepad[i]);

#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
        m_pKeyboard->Restore(rRawData.m_keyboard);
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */
};


void COptionData::Apply(void)
{
    m_play.Apply();
    m_sound.Apply();
    m_display.Apply();

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_paGamepad[i].Apply();

#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
        m_pKeyboard->Apply();
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */
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


#if defined(TMNT2_FEATURE_KEYBOARD)
IKeyboardOptionData& COptionData::Keyboard(void)
{
    ASSERT(m_pKeyboard != nullptr);
    return *m_pKeyboard;
};
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */


CGamepadOptionData& COptionData::Gamepad(int32 controller)
{
    ASSERT(controller >= 0);
    ASSERT(controller < CController::Max());

    int32 physicalPort = CController::GetPhysicalPort(controller);
    ASSERT(physicalPort < m_iGamepadNum);

    return m_paGamepad[physicalPort];
};


CGamepadOptionData* COptionData::GamepadFromPort(int32 port)
{
    if ((port >= 0) && (port < m_iGamepadNum))
        return &m_paGamepad[port];

    return nullptr;
};


int32 COptionData::GamepadNum(void) const
{
    return m_iGamepadNum;
};

