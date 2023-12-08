#include "KeyboardOptionData.hpp"

#ifdef TARGET_PC

#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/Controller.hpp"
#include "System/PC/PCPhysicalControllerKey.hpp"
#include "System/PC/PCSpecific.hpp"


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
    struct FIXEDINFO
    {
        uint32 m_uDigital;
        int32 m_scancode;
    };

    static const FIXEDINFO s_aFixedInfo[] =
    {
        { CController::DIGITAL_LUP,     DIK_UP     },
        { CController::DIGITAL_LDOWN,   DIK_DOWN   },
        { CController::DIGITAL_LLEFT,   DIK_LEFT   },
        { CController::DIGITAL_LRIGHT,  DIK_RIGHT  },
        { CController::DIGITAL_START,   DIK_RETURN },
        { CController::DIGITAL_SELECT,  DIK_ESCAPE },
    };

    for (int32 i = 0; i < COUNT_OF(s_aFixedInfo); ++i)
        CPCSpecific::MapDigitalFixed(s_aFixedInfo[i].m_uDigital, s_aFixedInfo[i].m_scancode);

    assignDefaultButton();
    assignDefaultKey();
};


void CKeyboardOptionData::Terminate(void)
{
    ;
};


void CKeyboardOptionData::SetDefault(void)
{
    assignDefaultKey();
};


void CKeyboardOptionData::Apply(void)
{
    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (m_auButtonAssign[i])
            CPCSpecific::MapDigital(m_auButtonAssign[i], m_aiKeyAssign[i]);
    };

    CPCSpecific::MapAnalog(
        CController::ANALOG_LSTICK_X,
		m_aiKeyAssign[OPTIONTYPES::KEYFUNC_LEFT],
		m_aiKeyAssign[OPTIONTYPES::KEYFUNC_RIGHT]
    );
    
    CPCSpecific::MapAnalog(
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
    
    int32 iDIKey = CPCSpecific::GetDownKey();
    
    if (iDIKey == -1)
        return false;

    if (iDIKey == 0)
        return false;

    if (!CPCSpecific::IsKeyValid(iDIKey))
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
    const char* pszResult = CPCSpecific::GetKeyName(iDIKey);
    return (pszResult ? pszResult : "");
};


void CKeyboardOptionData::AssignButton(void)
{
    int32 iPort = CPCSpecific::GetKeyboradPort();
    
    if ((iPort >= 0) && (iPort < CGameData::Option().GamepadNum()))
    {
        for (int32 i = 0; i < OPTIONTYPES::BTNFUNCMAX; ++i)
            m_auButtonAssign[i] = CGameData::Option().Gamepad(iPort).GetAssignedButton(OPTIONTYPES::BTNFUNC(i));
    }
    else
    {
        assignDefaultButton();
    };

    Apply();
};


void CKeyboardOptionData::assignDefaultKey(void)
{
    static const int32 s_aDefaultKey[] =
    {
        DIK_F,         //  A
        DIK_D,         //  B
        DIK_R,         //  Shot
        DIK_SPACE,     //  Jump
        DIK_E,         //  Dash
        DIK_S,         //  Guard
        DIK_LSHIFT,    //  Chara change
        DIK_F1,        //  Camera change
        DIK_F2,        //  Gauge change
        DIK_NUMPAD8,   //  Up
        DIK_NUMPAD2,   //  Down
        DIK_NUMPAD4,   //  Left
        DIK_NUMPAD6,   //  Right
    };

    static_assert(COUNT_OF(s_aDefaultKey) == OPTIONTYPES::KEYFUNCMAX, "update me");
    static_assert(COUNT_OF(s_aDefaultKey) == COUNT_OF(m_aiKeyAssign), "update me");

    for (int32 i = 0; i < COUNT_OF(m_aiKeyAssign); ++i)
        m_aiKeyAssign[i] = s_aDefaultKey[i];
};


void CKeyboardOptionData::assignDefaultButton(void)
{
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_ATTACK_A]         = CController::DIGITAL_RDOWN;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_ATTACK_B]         = CController::DIGITAL_RLEFT;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_SHURIKEN]         = CController::DIGITAL_RUP;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_JUMP]             = CController::DIGITAL_RRIGHT;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_DASH]             = CController::DIGITAL_L1;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_GUARD]            = CController::DIGITAL_R1;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_CHANGE_CHARA]     = CController::DIGITAL_L2;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_CHANGE_CAMERA]    = CController::DIGITAL_R3;
    m_auButtonAssign[OPTIONTYPES::KEYFUNC_CHANGE_GAUGE]     = CController::DIGITAL_L3;
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

#endif /* TARGET_PC */