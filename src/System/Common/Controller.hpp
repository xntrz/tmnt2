#pragma once


class CController
{
public:
	static const int32 CONTROLLER_LOCKED_ON_VIRTUAL 	= -1;
	static const int32 CONTROLLER_UNLOCKED_ON_VIRTUAL 	= -2;

	static const uint32 DIGITAL					= 0x0;
	static const uint32 DIGITAL_UP				= 0x1;
	static const uint32 DIGITAL_DOWN			= 0x2;
	static const uint32 DIGITAL_LEFT			= 0x4;
	static const uint32 DIGITAL_RIGHT			= 0x8;
	static const uint32 DIGITAL_A				= 0x10;
	static const uint32 DIGITAL_B				= 0x20;
	static const uint32 DIGITAL_X				= 0x40;
	static const uint32 DIGITAL_Y				= 0x80;
	static const uint32 DIGITAL_LEFT_BUMPER		= 0x100;
	static const uint32 DIGITAL_RIGHT_BUMPER	= 0x200;
	static const uint32 DIGITAL_LEFT_TRIGGER	= 0x400;
	static const uint32 DIGITAL_RIGHT_TRIGGER 	= 0x800;
	static const uint32 DIGITAL_SELECT			= 0x1000;
	static const uint32 DIGITAL_START			= 0x2000;
	static const uint32 DIGITAL_LEFT_THUMB		= 0x4000;
	static const uint32 DIGITAL_RIGHT_THUMB		= 0x8000;	
	static uint32		DIGITAL_OK;
	static uint32 		DIGITAL_CANCEL;
	static const uint32 DIGITAL_MASK;
	static const uint32 FUNCTIONAL_BEGIN = DIGITAL_A;
	static const int32 	DIGITAL_NUM = sizeof(DIGITAL) << 3;

	enum ANALOG
	{
		ANALOG_LSTICK_X,
		ANALOG_LSTICK_Y,
		ANALOG_RSTICK_X,
		ANALOG_RSTICK_Y,
		ANALOG_RUP,
		ANALOG_RDOWN,
		ANALOG_RLEFT,
		ANALOG_RRIGHT,
		ANALOG_L1,
		ANALOG_L2,
		ANALOG_L3,
		ANALOG_R1,
		ANALOG_R2,
		ANALOG_R3,
		ANALOG_NUM,
	};

	enum STICK
	{
		STICK_LEFT = 0,
		STICK_RIGHT,
		STICK_NUM,
	};

	enum STATE
	{
		STATE_CONNECT = 0,
		STATE_UNCONNECT,
		STATE_MAX,
	};

public:
	static uint32 DigitalFunction(uint32 uDigital);
	static uint32 DigitalButton(uint32 uDigital);
	static int32 Max(void);
	static bool Lock(int32 iController);
	static bool Unlock(int32 iController);
	static bool IsLocked(int32 iController);
	static STATE GetState(int32 iController);
	static int32 GetPhysicalPort(int32 iController);
	static int32 GetController(int32 iPhysicalPort);
	static uint32 GetDigital(int32 iController);
	static uint32 GetDigital(int32 iController, uint32 btn);
	static uint32 GetDigitalRepeat(int32 iController);
	static bool GetDigitalRepeat(int32 iController, uint32 btn);
	static uint32 GetDigitalTrigger(int32 iController);
	static bool GetDigitalTrigger(int32 iController, uint32 btn);
	static uint32 GetDigitalRelease(int32 iController);
	static bool GetDigitalRelease(int32 iController, uint32 btn);
	static int16 GetAnalog(int32 iController, int32 analog);
	static void EnableVibration(int32 iController, bool bEnable);
	static bool StartVibration(int32 iController, uint32 uVibrationType, int32 iVibrationFrame);
	static void Mapping(int32 iController, uint32 btnPhysical, uint32 btnVirtual);
	static uint32 GetMapping(int32 iController, uint32 btnPhysical);
	static void EnableStickToDigitalMapping(int32 iController, STICK stick, bool bEnable);
	static int32 LockTriggeredController(uint32 uButton);
	static int32 LockAndSearchController(int32 iController);
	static void UnlockAllControllers(void);
	static int32 FindTriggeredController(uint32 uButton, bool bUnlocked);
	static void SaveLockedState(void);
	static void RestoreLockedState(void);
};