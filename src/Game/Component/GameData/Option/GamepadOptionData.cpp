#include "GamepadOptionData.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Misc/Keyboard.hpp"
#include "System/Common/Controller.hpp"


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
    
    SetDefault();
};


void CGamepadOptionData::Terminate(void)
{
    ;
};


void CGamepadOptionData::SetDefault(void)
{
    m_bVibration = true;
    
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_A] = CController::DIGITAL_A;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_B] = CController::DIGITAL_B;    
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_SHURIKEN] = CController::DIGITAL_Y;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_JUMP] = CController::DIGITAL_X;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_DASH] = CController::DIGITAL_LEFT_BUMPER;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_GUARD] = CController::DIGITAL_RIGHT_BUMPER;
    m_auButtonAssign[OPTIONTYPES::BTNFUNC_CHANGE_CHARA] = CController::DIGITAL_RIGHT_TRIGGER;
};


void CGamepadOptionData::Apply(void)
{
    if (!IsValid())
        SetDefault();

    int32 iController = CController::GetPhysicalPort(m_port);
    
    CGamepad::ClearAllButtonFunction(iController);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_ATTACK_A, m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_A]);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_ATTACK_B, m_auButtonAssign[OPTIONTYPES::BTNFUNC_ATTACK_B]);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_SHOT, m_auButtonAssign[OPTIONTYPES::BTNFUNC_SHURIKEN]);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_JUMP, m_auButtonAssign[OPTIONTYPES::BTNFUNC_JUMP]);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_DASH, m_auButtonAssign[OPTIONTYPES::BTNFUNC_DASH]);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_GUARD, m_auButtonAssign[OPTIONTYPES::BTNFUNC_GUARD]);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_SWITCH_CHR, m_auButtonAssign[OPTIONTYPES::BTNFUNC_CHANGE_CHARA]);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_SWITCH_CAM, CController::DIGITAL_LEFT_THUMB);
    CGamepad::AppendButtonFunction(iController, CGamepad::FUNCTION_SWITCH_GAUGE, CController::DIGITAL_RIGHT_THUMB);

#ifdef _TARGET_PC
    if (CKeyboard::GetPort() == m_port)
        CGameData::Option().Keyboard().InitializeGamepadMapping();
#endif
    
    CController::EnableVibration(iController, m_bVibration);
};


bool CGamepadOptionData::IsValid(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_auButtonAssign); ++i)
    {
        if (FLAG_TEST_ANY(m_auButtonAssign[i], ~CController::DIGITAL_MASK))
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
    ASSERT(func >= 0 && func < OPTIONTYPES::BTNFUNCMAX);

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
    ASSERT(func >= 0 && func < OPTIONTYPES::BTNFUNCMAX);

    return m_auButtonAssign[func];
};


uint32 CGamepadOptionData::extractSingleButton(uint32 button) const
{
    for (int32 i = 0; i < CController::DIGITAL_NUM / 2; ++i)
    {
		uint32 uButton = (1 << i);
        if (FLAG_TEST_ANY(uButton, button))
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