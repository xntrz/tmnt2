#include "OptionData.hpp"

#include "Game/System/Misc/ControllerMisc.hpp"
#include "System/Common/Controller.hpp"


#if defined(TMNT2_FEATURE_KEYBOARD)
    #if defined(TARGET_PC)
        #include "Option/KeyboardOptionDataWin.hpp"
        #define CKeyboardOptionData CKeyboardOptionDataWin
    #elif defined(TARGET_WEB)
        #include "Option/KeyboardOptionDataWeb.hpp"
        #define CKeyboardOptionData CKeyboardOptionDataWeb
    #endif
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */


COptionData::COptionData(void)
: m_play()
, m_sound()
, m_display()
, m_pGamepad(nullptr)
, m_iGamepadNum(0)
, m_pKeyboard(nullptr)
, m_pTouch(nullptr)
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

    if (m_iGamepadNum > 0)
        m_pGamepad = new CGamepadOptionData[m_iGamepadNum];

    m_play.Initialize();
    m_sound.Initialize();
    m_display.Initialize();

    int32 iTouchPort = -1;
    for (int32 i = 0; i < m_iGamepadNum; ++i)
    {
        m_pGamepad[i].Initialize(i);

        if (ControllerIsTouch(i))
            iTouchPort = i;
    };

#if defined(TMNT2_FEATURE_KEYBOARD)
    m_pKeyboard = new CKeyboardOptionData;
    if (m_pKeyboard)
        m_pKeyboard->Initialize();
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    if (iTouchPort != -1)
    {
        m_pTouch = new CTouchOptionData;
        if (m_pTouch)
            m_pTouch->Initialize(iTouchPort);
    };
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */    

    Apply();
};


void COptionData::Terminate(void)
{
#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    if (m_pTouch)
    {
        m_pTouch->Terminate();
        delete m_pTouch;
        m_pTouch = nullptr;
    };
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */    

#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
    {
        m_pKeyboard->Terminate();
        delete m_pKeyboard;
        m_pKeyboard = nullptr;
    };
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_pGamepad[i].Terminate();
    
    m_display.Terminate();
    m_sound.Terminate();
    m_play.Terminate();

    if (m_pGamepad)
    {
        delete[] m_pGamepad;
        m_pGamepad = nullptr;
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
        m_pGamepad[i].Snapshot(rRawData.m_aGamepad[i]);

#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
        m_pKeyboard->Snapshot(rRawData.m_keyboard);
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    if (m_pTouch)
        m_pTouch->Snapshot(rRawData.m_touch);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */  
};


void COptionData::Restore(const RAWDATA& rRawData)
{
    m_play.Restore(rRawData.m_play);
    m_sound.Restore(rRawData.m_sound);
    m_display.Restore(rRawData.m_display);

    ASSERT(m_iGamepadNum < COUNT_OF(rRawData.m_aGamepad));
    
    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_pGamepad[i].Restore(rRawData.m_aGamepad[i]);

#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
        m_pKeyboard->Restore(rRawData.m_keyboard);
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    if (m_pTouch)
        m_pTouch->Restore(rRawData.m_touch);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */  
};


void COptionData::Apply(void)
{
    m_play.Apply();
    m_sound.Apply();
    m_display.Apply();

    for (int32 i = 0; i < m_iGamepadNum; ++i)
        m_pGamepad[i].Apply();

#if defined(TMNT2_FEATURE_KEYBOARD)
    if (m_pKeyboard)
        m_pKeyboard->Apply();
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    if (m_pTouch)
        m_pTouch->Apply();
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */  
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


CGamepadOptionData& COptionData::Gamepad(int32 controller)
{
    ASSERT(controller >= 0);
    ASSERT(controller < CController::Max());

    int32 physicalPort = CController::GetPhysicalPort(controller);
    ASSERT(physicalPort < m_iGamepadNum);

    return m_pGamepad[physicalPort];
};


CGamepadOptionData* COptionData::GamepadFromPort(int32 port)
{
    if ((port >= 0) && (port < m_iGamepadNum))
        return &m_pGamepad[port];

    return nullptr;
};


int32 COptionData::GamepadNum(void) const
{
    return m_iGamepadNum;
};


#if defined(TMNT2_FEATURE_KEYBOARD)
IKeyboardOptionData& COptionData::Keyboard(void)
{
    ASSERT(m_pKeyboard != nullptr);
    return *m_pKeyboard;
};
#endif /* defined(TMNT2_FEATURE_KEYBOARD) */


#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
CTouchOptionData& COptionData::Touch(void)
{
    ASSERT(m_pTouch != nullptr);
    return *m_pTouch;
};
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */  