#include "KeyboardOptionDataWeb.hpp"

#if defined(TARGET_WEB)

#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/Controller.hpp"
#include "System/Web/WebSpecific.hpp"
#include "System/Web/WebPhysicalController.hpp"


CKeyboardOptionDataWeb::CKeyboardOptionDataWeb(void)
{
    ;
};


CKeyboardOptionDataWeb::~CKeyboardOptionDataWeb(void)
{
    ;
};


void CKeyboardOptionDataWeb::Initialize(void)
{
    struct FIXEDINFO
    {
        uint32 m_uDigital;
        int32 m_scancode;
    };

    static const FIXEDINFO s_aFixedInfo[] =
    {
        { CController::DIGITAL_LUP,     SDL_SCANCODE_UP     },
        { CController::DIGITAL_LDOWN,   SDL_SCANCODE_DOWN   },
        { CController::DIGITAL_LLEFT,   SDL_SCANCODE_LEFT   },
        { CController::DIGITAL_LRIGHT,  SDL_SCANCODE_RIGHT  },
        { CController::DIGITAL_START,   SDL_SCANCODE_RETURN },
        { CController::DIGITAL_SELECT,  SDL_SCANCODE_ESCAPE },
    };

    for (int32 i = 0; i < COUNT_OF(s_aFixedInfo); ++i)
        CWebSpecific::MapDigitalFixed(s_aFixedInfo[i].m_uDigital, s_aFixedInfo[i].m_scancode);

    assignDefaultButton();
    assignDefaultKey();
};


void CKeyboardOptionDataWeb::Terminate(void)
{
    ;
};


void CKeyboardOptionDataWeb::SetDefault(void)
{
    assignDefaultKey();
};


void CKeyboardOptionDataWeb::Apply(void)
{
    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (m_auButtonAssign[i])
            CWebSpecific::MapDigital(m_auButtonAssign[i], m_aiKeyAssign[i]);
    };

    CWebSpecific::MapAnalog(CController::ANALOG_LSTICK_X,
		                   m_aiKeyAssign[OPTIONTYPES::KEYFUNC_LEFT],
		                   m_aiKeyAssign[OPTIONTYPES::KEYFUNC_RIGHT]);
    
    CWebSpecific::MapAnalog(CController::ANALOG_LSTICK_Y,
		                   m_aiKeyAssign[OPTIONTYPES::KEYFUNC_UP],
		                   m_aiKeyAssign[OPTIONTYPES::KEYFUNC_DOWN]);
};


bool CKeyboardOptionDataWeb::IsValid(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_aiKeyAssign); ++i)
    {
        if ((m_aiKeyAssign[i] < 0) ||
            (m_aiKeyAssign[i] > 512))
            return false;
    };

    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (m_auButtonAssign[i] & ~CController::DIGITAL_MASK)
            return false;
    };

    return true;
};


void CKeyboardOptionDataWeb::Snapshot(RAWDATA& rRawData) const
{
    std::memcpy(rRawData.m_aiKeyAssign, m_aiKeyAssign, sizeof(rRawData.m_aiKeyAssign));
};


void CKeyboardOptionDataWeb::Restore(const RAWDATA& rRawData)
{
    std::memcpy(m_aiKeyAssign, rRawData.m_aiKeyAssign, sizeof(m_aiKeyAssign));
};


void CKeyboardOptionDataWeb::AssignKey(OPTIONTYPES::KEYFUNC func, int32 key)
{
    ASSERT(func >= 0);
    ASSERT(func < OPTIONTYPES::KEYFUNCMAX);

    OPTIONTYPES::KEYFUNC funcOld = findFunction(key);

    if ((funcOld != OPTIONTYPES::KEYFUNCMAX) && (funcOld != func))
        m_aiKeyAssign[funcOld] = m_aiKeyAssign[func];

    m_aiKeyAssign[func] = key;
};


bool CKeyboardOptionDataWeb::AssignDownKey(OPTIONTYPES::KEYFUNC func)
{
    ASSERT(func >= 0);
    ASSERT(func < OPTIONTYPES::KEYFUNCMAX);

    int32 iDIKey = CWebSpecific::GetDownKey();

    if (iDIKey == -1)
        return false;

    if (iDIKey == 0)
        return false;

    if (!CWebSpecific::IsKeyValid(iDIKey))
        return false;

    if (m_aiKeyAssign[func] == iDIKey)
        return false;

    AssignKey(func, iDIKey);
    return true;
};


int32 CKeyboardOptionDataWeb::GetAssignedKey(OPTIONTYPES::KEYFUNC func) const
{
    ASSERT(func >= 0);
    ASSERT(func < OPTIONTYPES::KEYFUNCMAX);

    return m_aiKeyAssign[func];
};


const char* CKeyboardOptionDataWeb::GetKeyName(int32 key) const
{
    const char* pszResult = CWebSpecific::GetKeyName(key);
    return (pszResult ? pszResult : "");
};


void CKeyboardOptionDataWeb::AssignButton(void)
{
    CGamepadOptionData* pKeyboardPadOptData = CGameData::Option().GamepadFromPort(CWebSpecific::GetKeyboradPort());
    if (pKeyboardPadOptData)
    {
        for (int32 i = 0; i < OPTIONTYPES::BTNFUNCMAX; ++i)
            m_auButtonAssign[i] = pKeyboardPadOptData->GetAssignedButton(static_cast<OPTIONTYPES::BTNFUNC>(i));
    }
    else
    {
        assignDefaultButton();
    };

    Apply();
};


void CKeyboardOptionDataWeb::assignDefaultKey(void)
{
    static const int32 s_aDefaultKey[] =
    {
        SDL_SCANCODE_F,         //  A
        SDL_SCANCODE_D,         //  B
        SDL_SCANCODE_R,         //  Shot
        SDL_SCANCODE_SPACE,     //  Jump
        SDL_SCANCODE_E,         //  Dash
        SDL_SCANCODE_S,         //  Guard
        SDL_SCANCODE_LSHIFT,    //  Chara change
        SDL_SCANCODE_F1,        //  Camera change
        SDL_SCANCODE_F2,        //  Gauge change
        SDL_SCANCODE_KP_8,      //  Up
        SDL_SCANCODE_KP_2,      //  Down
        SDL_SCANCODE_KP_4,      //  Left
        SDL_SCANCODE_KP_6,      //  Right
    };

    static_assert(COUNT_OF(s_aDefaultKey) == OPTIONTYPES::KEYFUNCMAX, "update me");
    static_assert(COUNT_OF(s_aDefaultKey) == COUNT_OF(m_aiKeyAssign), "update me");

    for (int32 i = 0; i < COUNT_OF(m_aiKeyAssign); ++i)
        m_aiKeyAssign[i] = s_aDefaultKey[i];
};


void CKeyboardOptionDataWeb::assignDefaultButton(void)
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


OPTIONTYPES::KEYFUNC CKeyboardOptionDataWeb::findFunction(int32 key) const
{
    for (int32 i = 0; i < COUNT_OF(m_aiKeyAssign); ++i)
    {
        if (m_aiKeyAssign[i] == key)
            return static_cast<OPTIONTYPES::KEYFUNC>(i);
    };

    return OPTIONTYPES::KEYFUNCMAX;
};

#endif /* defined(TARGET_WEB) */