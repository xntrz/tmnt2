#include "Controller.hpp"
#include "InputsDevice.hpp"


/*static*/ uint32 CController::DIGITAL_OK = CController::DIGITAL_START | CController::DIGITAL_A;
/*static*/ uint32 CController::DIGITAL_CANCEL = CController::DIGITAL_SELECT | CController::DIGITAL_B;
/*static*/ const uint32 CController::DIGITAL_MASK =
	CController::DIGITAL_UP				|
	CController::DIGITAL_DOWN 			|
	CController::DIGITAL_LEFT 			|
	CController::DIGITAL_RIGHT 			|
	CController::DIGITAL_A 				|
	CController::DIGITAL_B 				|
	CController::DIGITAL_X 				|
	CController::DIGITAL_Y 				|
	CController::DIGITAL_LEFT_BUMPER 	|
	CController::DIGITAL_RIGHT_BUMPER 	|
	CController::DIGITAL_LEFT_TRIGGER 	|
	CController::DIGITAL_RIGHT_TRIGGER 	|
	CController::DIGITAL_SELECT 		|
	CController::DIGITAL_START 			|
	CController::DIGITAL_LEFT_THUMB 	|
	CController::DIGITAL_RIGHT_THUMB;

/*static*/ uint32 CController::DigitalFunction(uint32 uDigital)
{
	return ((uDigital >> 16) & 0xFFFF);
};


/*static*/ uint32 CController::DigitalButton(uint32 uDigital)
{
	return (uDigital & 0xFFFF);
};


/*static*/ int32 CController::Max(void)
{
	return CInputsDevice::Instance().Max();
};


/*static*/ bool CController::Lock(int32 iController)
{
	ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
	ASSERT(iController < Max());
	
	return CInputsDevice::Instance().Lock(iController);
};


/*static*/ bool CController::Unlock(int32 iController)
{
	ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
	ASSERT(iController < Max());
	
	return CInputsDevice::Instance().Unlock(iController);
};


/*static*/ bool CController::IsLocked(int32 iController)
{
	ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
	ASSERT(iController < Max());
	
	return CInputsDevice::Instance().IsLocked(iController);
};


/*static*/ CController::STATE CController::GetState(int32 iController)
{
	ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
	ASSERT(iController < Max());
	
	return CInputsDevice::Instance().ControllerResource(iController).Info().m_state;
};


/*static*/ int32 CController::GetPhysicalPort(int32 iController)
{
	ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
	ASSERT(iController < Max());
	
	return CInputsDevice::Instance().ControllerResource(iController).Info().m_iPhysicalPort;
};


/*static*/ int32 CController::GetController(int32 iPhysicalPort)
{
	for (int32 i = 0; i < Max(); i++)
	{
		if (GetPhysicalPort(i) == iPhysicalPort)
			return i;
	};

	return -1;
};


/*static*/ uint32 CController::GetDigital(int32 iController)
{
	switch (iController)
	{
	case CONTROLLER_LOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetLockedVirtualController().m_digital;
		
	case CONTROLLER_UNLOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetUnlockedVirtualController().m_digital;

	default:
		return CInputsDevice::Instance().ControllerResource(iController).Info().m_digital;
	};
};


/*static*/ uint32 CController::GetDigital(int32 iController, uint32 btn)
{
	return (GetDigital(iController) & btn) != 0;
};


/*static*/ uint32 CController::GetDigitalRepeat(int32 iController)
{
	switch (iController)
	{
	case CONTROLLER_LOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetLockedVirtualController().m_digitalRepeat;

	case CONTROLLER_UNLOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetUnlockedVirtualController().m_digitalRepeat;

	default:
		return CInputsDevice::Instance().ControllerResource(iController).Info().m_digitalRepeat;
	};
};


/*static*/ bool CController::GetDigitalRepeat(int32 iController, uint32 btn)
{
	return (GetDigitalRepeat(iController) & btn) != 0;
};


/*static*/ uint32 CController::GetDigitalTrigger(int32 iController)
{
	switch (iController)
	{
	case CONTROLLER_LOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetLockedVirtualController().m_digitalTrigger;

	case CONTROLLER_UNLOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetUnlockedVirtualController().m_digitalTrigger;

	default:
		return CInputsDevice::Instance().ControllerResource(iController).Info().m_digitalTrigger;
	};
};


/*static*/ bool CController::GetDigitalTrigger(int32 iController, uint32 btn)
{
	return (GetDigitalTrigger(iController) & btn) != 0;
};


/*static*/ uint32 CController::GetDigitalRelease(int32 iController)
{
	switch (iController)
	{
	case CONTROLLER_LOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetLockedVirtualController().m_digitalRelease;

	case CONTROLLER_UNLOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetUnlockedVirtualController().m_digitalRelease;

	default:
		return CInputsDevice::Instance().ControllerResource(iController).Info().m_digitalRelease;
	};
};


/*static*/ bool CController::GetDigitalRelease(int32 iController, uint32 btn)
{
	return (GetDigitalRelease(iController) & btn) != 0;
};


/*static*/ int16 CController::GetAnalog(int32 iController, int32 analog)
{
	ASSERT(analog < ANALOG_NUM);
	
	switch (iController)
	{
	case CONTROLLER_LOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetLockedVirtualController().m_aAnalog[analog];

	case CONTROLLER_UNLOCKED_ON_VIRTUAL:
		return CInputsDevice::Instance().GetUnlockedVirtualController().m_aAnalog[analog];

	default:
		ASSERT(iController < Max());
		ASSERT(analog >= 0 && analog < COUNT_OF(CInputsDevice::Instance().ControllerResource(iController).Info().m_aAnalog));
		return CInputsDevice::Instance().ControllerResource(iController).Info().m_aAnalog[analog];
	};
};


/*static*/ void CController::EnableVibration(int32 iController, bool bEnable)
{
	ASSERT(iController < Max());
	
	if (!bEnable)
		CInputsDevice::Instance().ControllerResource(iController).SetVibration(0, 0);
	
	CInputsDevice::Instance().ControllerResource(iController).SetVibrationEnable(bEnable);
};


/*static*/ bool CController::StartVibration(int32 iController, uint32 uVibrationType, int32 iVibrationFrame)
{
	ASSERT(iController < Max());
	
	CInputsDevice::Instance().ControllerResource(iController).SetVibration(uVibrationType, iVibrationFrame);
	return true;
};


/*static*/ void CController::Mapping(int32 iController, uint32 btnPhysical, uint32 btnVirtual)
{
	ASSERT(iController < Max());

	*CInputsDevice::Instance().ControllerResource(iController).GetDigitalMapping(btnPhysical) = btnVirtual;
};


/*static*/ uint32 CController::GetMapping(int32 iController, uint32 btnPhysical)
{
	ASSERT(iController < Max());
	
	return *CInputsDevice::Instance().ControllerResource(iController).GetDigitalMapping(btnPhysical);
};


/*static*/ void CController::EnableStickToDigitalMapping(int32 iController, STICK stick, bool bEnable)
{
	if (iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL)
	{
		ASSERT(iController < Max());
		
		CInputsDevice::Instance().ControllerResource(iController).EnableStickToDigitalMapping(stick, bEnable);
	};
};


/*static*/ int32 CController::LockTriggeredController(uint32 uButton)
{
	for (int32 i = 0; i < Max(); ++i)
	{
		if (GetDigitalTrigger(i, uButton) && !IsLocked(i))
			return LockAndSearchController(i);
	};

	return CController::CONTROLLER_LOCKED_ON_VIRTUAL;
};


/*static*/ int32 CController::LockAndSearchController(int32 iController)
{
	if (iController < 0 || iController > Max())
		iController = 0;

	if (Lock(iController))
		return GetController(GetPhysicalPort(iController));
	else
		return CController::CONTROLLER_LOCKED_ON_VIRTUAL;
};


/*static*/ void CController::UnlockAllControllers(void)
{
	for (int32 i = 0; i < Max(); ++i)
	{
		if (IsLocked(i))
			Unlock(i);
	};
};


/*static*/ int32 CController::FindTriggeredController(uint32 uButton, bool bUnlocked)
{
	for (int32 i = 0; i < Max(); ++i)
	{
		if (GetDigitalTrigger(i, uButton) && (bUnlocked || !IsLocked(i)))
			return i;
	};

	return CController::CONTROLLER_LOCKED_ON_VIRTUAL;
};


static int32 s_aiLockedPhysicalPort[4] =
{
	CController::CONTROLLER_LOCKED_ON_VIRTUAL,
	CController::CONTROLLER_LOCKED_ON_VIRTUAL,
	CController::CONTROLLER_LOCKED_ON_VIRTUAL,
	CController::CONTROLLER_LOCKED_ON_VIRTUAL,
};


/*static*/ void CController::SaveLockedState(void)
{
	for (int32 i = 0; i < Max(); ++i)
	{
		if (IsLocked(i))
			s_aiLockedPhysicalPort[i] = GetPhysicalPort(i);
		else
			s_aiLockedPhysicalPort[i] = CController::CONTROLLER_LOCKED_ON_VIRTUAL;
	};
};


/*static*/ void CController::RestoreLockedState(void)
{
	UnlockAllControllers();
	
	for (int32 i = 0; i < COUNT_OF(s_aiLockedPhysicalPort); ++i)
	{
		if (s_aiLockedPhysicalPort[i] >= 0)
			Lock(s_aiLockedPhysicalPort[i]);		
	};
};

