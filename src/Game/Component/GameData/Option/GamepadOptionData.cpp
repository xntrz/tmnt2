#include "GamepadOptionData.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Controller.hpp"

#ifdef TARGET_PC
#include "System/PC/PCSpecific.hpp"
#endif /* TARGET_PC */


CGamepadOptionData::CGamepadOptionData(void)
{
    ;
};


CGamepadOptionData::~CGamepadOptionData(void)
{
    ;
};


void CGamepadOptionData::Initialize(int32 port)
{
    m_port = port;

#ifdef TARGET_PC    
    //
    //  "WEAK ATTACK->OK  JUMP->CANCEL"
    //
    IGamepad::DIGITAL_OK = IPadFunctionMask(IGamepad::FUNCTION_ATTACK_A) | IGamepad::DIGITAL_START;
    IGamepad::DIGITAL_CANCEL = IPadFunctionMask(IGamepad::FUNCTION_JUMP) | IGamepad::DIGITAL_SELECT;
#endif /* TARGET_PC */
    
    SetDefault();
};


void CGamepadOptionData::Terminate(void)
{
    ;
};


void CGamepadOptionData::SetDefault(void)
{
    m_bVibration = true;
    
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_A]     = IGamepad::DIGITAL_RDOWN;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_B]     = IGamepad::DIGITAL_RLEFT;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_SHURIKEN]     = IGamepad::DIGITAL_RUP;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_JUMP]         = IGamepad::DIGITAL_RRIGHT;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_DASH]         = IGamepad::DIGITAL_L1;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_GUARD]        = IGamepad::DIGITAL_R1;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_CHANGE_CHARA] = IGamepad::DIGITAL_L2;
};


void CGamepadOptionData::Apply(void)
{
    if (!IsValid())
        SetDefault();

    int32 iController = IGamepad::GetController(m_port);
    
    IGamepad::ClearAllButtonFunction(iController);
    
    IGamepad::AppendButtonFunction(iController, m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_A],        IGamepad::FUNCTION_ATTACK_A);
    IGamepad::AppendButtonFunction(iController, m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_B],        IGamepad::FUNCTION_ATTACK_B);
    IGamepad::AppendButtonFunction(iController, m_auButtonAssign[OPTIONTYPES::BTNFUNC_SHURIKEN],        IGamepad::FUNCTION_SHOT);
    IGamepad::AppendButtonFunction(iController, m_auButtonAssign[OPTIONTYPES::BTNFUNC_JUMP],            IGamepad::FUNCTION_JUMP);
    IGamepad::AppendButtonFunction(iController, m_auButtonAssign[OPTIONTYPES::BTNFUNC_DASH],            IGamepad::FUNCTION_DASH);
    IGamepad::AppendButtonFunction(iController, m_auButtonAssign[OPTIONTYPES::BTNFUNC_GUARD],           IGamepad::FUNCTION_GUARD);
    IGamepad::AppendButtonFunction(iController, m_auButtonAssign[OPTIONTYPES::BTNFUNC_CHANGE_CHARA],    IGamepad::FUNCTION_SWITCH_CHR);
#if defined(TARGET_PS2)
    IGamepad::AppendButtonFunction(iController, IGamepad::DIGITAL_LUP,      IGamepad::FUNCTION_SWITCH_CAM);
    IGamepad::AppendButtonFunction(iController, IGamepad::DIGITAL_LDOWN,    IGamepad::FUNCTION_SWITCH_GAUGE);
#elif defined(TARGET_PC)
    IGamepad::AppendButtonFunction(iController, IGamepad::DIGITAL_R3,       IGamepad::FUNCTION_SWITCH_CAM);
    IGamepad::AppendButtonFunction(iController, IGamepad::DIGITAL_L3,       IGamepad::FUNCTION_SWITCH_GAUGE);
#else
#error Not implemented for current target
#endif    

#ifdef TARGET_PC
    if (m_port == CPCSpecific::GetKeyboradPort())
        CGameData::Option().Keyboard().AssignButton();
#endif /* TARGET_PC */
    
    IGamepad::EnableVibration(iController, m_bVibration);
};


bool CGamepadOptionData::IsValid(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (m_auButtonAssign[i] & ~IGamepad::DIGITAL_MASK)
            return false;
    };

    return true;
};


void CGamepadOptionData::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_bVibration = m_bVibration;
    std::memcpy(rRawData.m_auButtonAssign, m_auButtonAssign, sizeof(rRawData.m_auButtonAssign));
};


void CGamepadOptionData::Restore(const RAWDATA& rRawData)
{
    m_bVibration = rRawData.m_bVibration;
    std::memcpy(m_auButtonAssign, rRawData.m_auButtonAssign, sizeof(m_auButtonAssign));
};


void CGamepadOptionData::SetEnableVibration(bool bSet)
{
    m_bVibration = bSet;
};


void CGamepadOptionData::AssignButton(OPTIONTYPES::BTNFUNC func, uint32 uButton)
{
    ASSERT(func >= 0);
    ASSERT(func < OPTIONTYPES::BTNFUNCMAX);

    uint32 uSingleButton = extractSingleButton(uButton);
    OPTIONTYPES::BTNFUNC funcOld = findFunction(uSingleButton);
    
    if (funcOld != OPTIONTYPES::BTNFUNCMAX && funcOld != func)
        m_auButtonAssign[funcOld] = m_auButtonAssign[func];
    
    m_auButtonAssign[func] = uButton;
};


bool CGamepadOptionData::IsEnabledVibration(void) const
{
    return m_bVibration;
};


uint32 CGamepadOptionData::GetAssignedButton(OPTIONTYPES::BTNFUNC func) const
{
    ASSERT(func >= 0);
    ASSERT(func < OPTIONTYPES::BTNFUNCMAX);

    return m_auButtonAssign[func];
};


uint32 CGamepadOptionData::extractSingleButton(uint32 button) const
{
    for (int32 i = 0; i < IGamepad::DIGITAL_NUM / 2; ++i)
    {
		uint32 uButton = (1 << i);
        if (uButton & button)
            return uButton;
    };

	return 0;
};


OPTIONTYPES::BTNFUNC CGamepadOptionData::findFunction(uint32 button) const
{
    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (m_auButtonAssign[i] == button)
            return OPTIONTYPES::BTNFUNC(i);
    };

    return OPTIONTYPES::BTNFUNCMAX;
};