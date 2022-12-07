#include "KeyboardOptionData.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/Keyboard.hpp"
#include "System/Common/Controller.hpp"
#include "System/PC/PCPhysicalControllerScancode.hpp"


CKeyboardOptionData::CKeyboardOptionData(void)
{
    ;
};


CKeyboardOptionData::~CKeyboardOptionData(void)
{
    ;
};


void CKeyboardOptionData::Initialize(void)
{
    SetDefault();
};


void CKeyboardOptionData::Terminate(void)
{
    ;
};


void CKeyboardOptionData::SetDefault(void)
{
    struct FIXEDINFO
    {
        uint32 m_uDigital;
        int32 m_scancode;
    };

    static const FIXEDINFO s_aFixedInfo[] =
    {
        { CController::DIGITAL_UP,      KEYCODE::KEY_UP     },
        { CController::DIGITAL_DOWN,    KEYCODE::KEY_DOWN   },
        { CController::DIGITAL_LEFT,    KEYCODE::KEY_LEFT   },
        { CController::DIGITAL_RIGHT,   KEYCODE::KEY_RIGHT  },
        { CController::DIGITAL_START,   KEYCODE::KEY_RETURN },
        { CController::DIGITAL_SELECT,  KEYCODE::KEY_ESCAPE },
    };

    for (int32 i = 0; i < COUNT_OF(s_aFixedInfo); ++i)
        CKeyboard::MapDigitalFixed(s_aFixedInfo[i].m_uDigital, s_aFixedInfo[i].m_scancode);

    assignDefaultButton();
    assignDefaultKey();
    Apply();
};


void CKeyboardOptionData::Apply(void)
{
    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (m_auButtonAssign[i])
            CKeyboard::MapDigital(m_auButtonAssign[i], m_aiKeyAssign[i]);
    };

    CKeyboard::MapAnalog(
        CController::ANALOG_LSTICK_X,
		m_aiKeyAssign[OPTIONTYPES::KEYFUNC_LEFT],
		m_aiKeyAssign[OPTIONTYPES::KEYFUNC_RIGHT]
    );
    
    CKeyboard::MapAnalog(
        CController::ANALOG_LSTICK_Y,
		m_aiKeyAssign[OPTIONTYPES::KEYFUNC_UP],
		m_aiKeyAssign[OPTIONTYPES::KEYFUNC_DOWN]
    );
};


bool CKeyboardOptionData::IsValid(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_aiKeyAssign); ++i)
    {
        if (m_aiKeyAssign[i] < 0 ||
            m_aiKeyAssign[i] > 256)
            return false;        
    };

    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (FLAG_TEST_ANY(m_auButtonAssign[i], ~CController::DIGITAL_MASK))
            return false;
    };

    return true;
};


void CKeyboardOptionData::Snapshot(RAWDATA& rRawData) const
{
    std::memcpy(rRawData.m_aiKeyAssign, m_aiKeyAssign, sizeof(rRawData.m_aiKeyAssign));
};


void CKeyboardOptionData::Restore(const RAWDATA& rRawData)
{
    std::memcpy(m_aiKeyAssign, rRawData.m_aiKeyAssign, sizeof(m_aiKeyAssign));
};


void CKeyboardOptionData::AssignKey(OPTIONTYPES::KEYFUNC func, int32 iDIKey)
{
    ASSERT(func >= 0 && func < OPTIONTYPES::KEYFUNCMAX);

    OPTIONTYPES::KEYFUNC funcOld = findFunction(iDIKey);

    if (funcOld != OPTIONTYPES::KEYFUNCMAX && funcOld != func)
        m_aiKeyAssign[funcOld] = m_aiKeyAssign[func];

    m_aiKeyAssign[func] = iDIKey;
};


bool CKeyboardOptionData::AssignDownKey(OPTIONTYPES::KEYFUNC func)
{
    ASSERT(func >= 0 && func < OPTIONTYPES::KEYFUNCMAX);
    
    int32 iDIKey = CKeyboard::GetDownKey();
    if (!iDIKey || !CKeyboard::IsKeyValid(iDIKey))
        return false;

    if (m_aiKeyAssign[func] == iDIKey)
        return false;

    AssignKey(func, iDIKey);
    return true;
};


int32 CKeyboardOptionData::GetAssignedKey(OPTIONTYPES::KEYFUNC func) const
{
    ASSERT(func >= 0 && func < OPTIONTYPES::KEYFUNCMAX);
    return m_aiKeyAssign[func];
};


const char* CKeyboardOptionData::GetKeyName(int32 iDIKey) const
{
    static const char* EMPTYKEY = "";

    const char* pszResult = CKeyboard::GetKeyName(iDIKey);
    
    if (!pszResult)
        pszResult = EMPTYKEY;

    return pszResult;
};


void CKeyboardOptionData::InitializeGamepadMapping(void)
{
    int32 iPort = CKeyboard::GetPort();
    if ((iPort >= 0) && (iPort < CGameData::Option().GamepadNum()))
    {

        for (int32 i = 0; i < OPTIONTYPES::BTNFUNCMAX; ++i)
            m_auButtonAssign[i] = CGameData::Option().Gamepad(iPort).GetAssignedButton(OPTIONTYPES::BTNFUNC(i));
    }
    else
    {
        
    };

    Apply();
};


void CKeyboardOptionData::assignDefaultKey(void)
{
    static const uint32 s_aiDefaultKey[] =
    {
        KEYCODE::KEY_F,         //  A
        KEYCODE::KEY_D,         //  B
        KEYCODE::KEY_R,         //  Shuriken
        KEYCODE::KEY_SPACE,     //  Jump
        KEYCODE::KEY_E,         //  Dash
        KEYCODE::KEY_S,         //  Guard
        KEYCODE::KEY_LSHIFT,    //  Chara change
        KEYCODE::KEY_F1,        //  Camera change
        KEYCODE::KEY_F2,        //  Gauge change
        KEYCODE::KEY_NUMPAD8,   //  Up
        KEYCODE::KEY_NUMPAD2,   //  Down
        KEYCODE::KEY_NUMPAD4,   //  Left
        KEYCODE::KEY_NUMPAD6,   //  Right
    };

    static_assert(COUNT_OF(s_aiDefaultKey) == OPTIONTYPES::KEYFUNCMAX, "update me");
    static_assert(COUNT_OF(s_aiDefaultKey) == COUNT_OF(m_aiKeyAssign), "update me");
    
    for (int32 i = 0; i < COUNT_OF(m_aiKeyAssign); ++i)
        m_aiKeyAssign[i] = s_aiDefaultKey[i];
};


void CKeyboardOptionData::assignDefaultButton(void)
{
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_ATTACK_A] = CController::DIGITAL_A;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_ATTACK_B] = CController::DIGITAL_B;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_SHURIKEN] = CController::DIGITAL_Y;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_JUMP] = CController::DIGITAL_X;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_DASH] = CController::DIGITAL_LEFT_BUMPER;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_GUARD] = CController::DIGITAL_RIGHT_BUMPER;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_CHANGE_CHARA] = CController::DIGITAL_RIGHT_TRIGGER;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_CHANGE_CAMERA] = CController::DIGITAL_LEFT_THUMB;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_CHANGE_GAUGE] = CController::DIGITAL_RIGHT_THUMB;
};


OPTIONTYPES::KEYFUNC CKeyboardOptionData::findFunction(int32 key) const
{
    for (int32 i = 0; i < COUNT_OF(m_aiKeyAssign); ++i)
    {
        if (m_aiKeyAssign[i] == key)
            return OPTIONTYPES::KEYFUNC(i);
    };

    return OPTIONTYPES::KEYFUNCMAX;
};