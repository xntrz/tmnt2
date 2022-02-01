#include "PCPhysicalController.hpp"
#include "PCTypedefs.hpp"

#include "System/Common/PhysicalController.hpp"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


struct JOYSTICKINFO
{
	static const int32 STATE_MAX = 32;

	struct STATE
	{
		GUID m_guid;
		IDirectInputDevice8* m_pDevice;
		DIDEVCAPS m_deviceCaps;
		bool m_bVibrationFeature;
		int32 m_numAxes;
		IDirectInputEffect* m_pVibrationEffect;
		TCHAR m_tszName[MAX_PATH];
	};

	STATE* m_apState[STATE_MAX];
	int32 m_count;
};


static JOYSTICKINFO s_aJoystickInfo;
static IDirectInput8* s_pDirectInput8 = nullptr;
static void* s_hWnd = nullptr;


namespace DI8Utils
{
	bool AcquireDevice(IDirectInputDevice8* pDevice)
	{
		HRESULT hResult = S_OK;
		do
		{
			hResult = pDevice->Acquire();
		} while (hResult == DIERR_INPUTLOST);

		return SUCCEEDED(hResult);
	};
};


class CPCKeyboardController : public IPhysicalController
{
private:
	struct DIGITALINFO
	{
		uint32 m_digital;
		int32 m_scancode;
		bool m_bEnabled;
	};


	struct ANALOGINFO
	{
		int32 m_no;
		bool m_bOneAxis;
		int32 m_scancodeX;
		int32 m_scancodeY;
		bool m_bEnabled;
	};


	static const int32 KEYBUFFER_SIZE = 256;
	static const int32 FIXED_DIGITAL_MAX = CController::DIGITAL_NUM;


public:
	CPCKeyboardController(int32 iController)
	: m_numUsedFixedDigital(0)
	, m_pDIDevice(nullptr)
	{
		m_info.m_iPhysicalPort = iController;
	};


	virtual ~CPCKeyboardController(void)
	{
		Close();
	};


	virtual void Close(void) override
	{
		if (m_pDIDevice)
		{
			m_pDIDevice->Unacquire();
			m_pDIDevice->Release();
			m_pDIDevice = nullptr;
		};
	};


	virtual void Update(void) override
	{
		Clear();

		if (!DI8Utils::AcquireDevice(m_pDIDevice))
		{
			m_info.m_state = CController::STATE_UNCONNECT;
			IPhysicalController::Update();
			return;
		};

		m_info.m_state = CController::STATE_CONNECT;

		memset(m_keybuffer, 0x00, sizeof(m_keybuffer));
		if (FAILED(m_pDIDevice->GetDeviceState(sizeof(m_keybuffer), m_keybuffer)))
		{
			IPhysicalController::Update();
			return;
		};

		uint32 digital = 0;
		for (int32 i = 0; i < m_numUsedFixedDigital; ++i)
		{
			if (m_aFixedDigitalInfo[i].m_bEnabled && IsKeyDown(m_aFixedDigitalInfo[i].m_scancode))
				digital |= m_aFixedDigitalInfo[i].m_digital;
		};

		for (int32 i = 0; i < COUNT_OF(m_aDigitalInfo); ++i)
		{
			if (m_aDigitalInfo[i].m_bEnabled && IsKeyDown(m_aDigitalInfo[i].m_scancode))
				digital |= m_aDigitalInfo[i].m_digital;
		};

		for (int32 i = 0; i < COUNT_OF(m_aAnalogInfo); ++i)
		{
			if (m_aAnalogInfo[i].m_bEnabled)
			{
				if(m_aAnalogInfo[i].m_bOneAxis)
				{
					if (IsKeyDown(m_aAnalogInfo[i].m_scancodeX))
						m_info.m_aAnalog[i] = int16(0x7FFF);
					else
						m_info.m_aAnalog[i] = int16(0x0);
				}
				else
				{
					if (IsKeyDown(m_aAnalogInfo[i].m_scancodeY))
						m_info.m_aAnalog[i] = int16(0x7FFF);
					else if (IsKeyDown(m_aAnalogInfo[i].m_scancodeX))
						m_info.m_aAnalog[i] = int16(0x8000);
				};
			};
		};

		m_info.m_digital = digital;

		IPhysicalController::Update();
	};


	virtual bool Start(void)
	{
		ASSERT(s_pDirectInput8);

		bool bResult = false;
		try
		{
			if (FAILED(s_pDirectInput8->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL)))
				throw std::exception();

			if (FAILED(m_pDIDevice->SetDataFormat(&c_dfDIKeyboard)))
				throw std::exception();

			if (FAILED(m_pDIDevice->SetCooperativeLevel(HWND(s_hWnd), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
				throw std::exception();
			
			HRESULT hResult = S_OK;
			do
			{
				hResult = m_pDIDevice->Acquire();
			} while (hResult == DIERR_INPUTLOST);

			if (FAILED(hResult))
				throw std::exception();

			bResult = true;
		}
		catch (std::exception& e)
		{
			REF(e);
		};

		if (bResult)
		{
			uint32 aDigital[] =
			{
				CController::DIGITAL_UP,
				CController::DIGITAL_DOWN,
				CController::DIGITAL_LEFT,
				CController::DIGITAL_RIGHT,
				CController::DIGITAL_A,
				CController::DIGITAL_B,
				CController::DIGITAL_X,
				CController::DIGITAL_Y,
				CController::DIGITAL_LEFT_BUMPER,
				CController::DIGITAL_RIGHT_BUMPER,
				CController::DIGITAL_LEFT_TRIGGER,
				CController::DIGITAL_RIGHT_TRIGGER,
				CController::DIGITAL_SELECT,
				CController::DIGITAL_START,
				CController::DIGITAL_LEFT_THUMB,
				CController::DIGITAL_RIGHT_THUMB,
			};

			for (int32 i = 0; i < COUNT_OF(m_aDigitalInfo); ++i)
			{
				m_aDigitalInfo[i].m_bEnabled = false;
				m_aDigitalInfo[i].m_digital = aDigital[i];
			};

			for (int32 i = 0; i < COUNT_OF(m_aAnalogInfo); ++i)
			{
				m_aAnalogInfo[i].m_bEnabled = false;
				m_aAnalogInfo[i].m_no = i;
				if (i < 0 || i > 3)
				{
					m_aAnalogInfo[i].m_bOneAxis = true;
					m_aAnalogInfo[i].m_scancodeX = -1;
				}
				else
				{
					m_aAnalogInfo[i].m_bOneAxis = false;
					m_aAnalogInfo[i].m_scancodeX = -1;
					m_aAnalogInfo[i].m_scancodeY = -1;
				};
			};
		};

		return bResult;
	};


	void MapDigital(uint32 btn, int32 iDIKey)
	{
		for (int32 i = 0; i < COUNT_OF(m_aDigitalInfo); ++i)
		{
			if (m_aDigitalInfo[i].m_digital == btn)
			{
				m_aDigitalInfo[i].m_bEnabled = (iDIKey >= 0);
				m_aDigitalInfo[i].m_scancode = iDIKey;
				return;
			};
		};
	};


	void MapDigitalFixed(uint32 btn, int32 iDIKey)
	{
		ASSERT(m_numUsedFixedDigital < FIXED_DIGITAL_MAX);
		m_aFixedDigitalInfo[m_numUsedFixedDigital].m_bEnabled = true;
		m_aFixedDigitalInfo[m_numUsedFixedDigital].m_digital = btn;
		m_aFixedDigitalInfo[m_numUsedFixedDigital].m_scancode = iDIKey;
		++m_numUsedFixedDigital;
	};


	void MapAnalog(int32 no, int32 iDIKeyX, int32 iDIKeyY)
	{
		ASSERT(iDIKeyX >= 0 && iDIKeyX < 256);
		ASSERT(iDIKeyY >= 0 && iDIKeyY < 256);
		ASSERT(no >= 0 && no < CController::ANALOG_NUM);

		for (int32 i = 0; i < COUNT_OF(m_aAnalogInfo); ++i)
		{
			if (m_aAnalogInfo[i].m_no == no)
			{
				m_aAnalogInfo[i].m_bEnabled = (iDIKeyX >= 0 && iDIKeyY >= 0);
				m_aAnalogInfo[i].m_scancodeX = iDIKeyX;
				m_aAnalogInfo[i].m_scancodeY = iDIKeyY;
			};
		};
	};


	bool IsKeyDown(int32 iDIKey) const
	{
		ASSERT(iDIKey >= 0 && iDIKey < KEYBUFFER_SIZE);
		return (m_keybuffer[iDIKey] & 0x80) != 0;
	};


	bool IsKeyNotFixed(int32 iDIKey) const
	{
		if (!m_numUsedFixedDigital)
			return true;

		for (int32 i = 0; i < m_numUsedFixedDigital; ++i)
		{
			if (m_aFixedDigitalInfo[i].m_scancode == iDIKey)
				return false;
		};

		return true;
	};


	int32 GetDownKey(void) const
	{
		for (int32 i = 0; i < KEYBUFFER_SIZE; ++i)
		{
			if (CPCPhysicalController::IsKeyValid(i) && IsKeyDown(i))
				return i;			
		};

		return -1;
	};


private:
	DIGITALINFO m_aFixedDigitalInfo[FIXED_DIGITAL_MAX];
	DIGITALINFO m_aDigitalInfo[CController::DIGITAL_NUM];
	ANALOGINFO m_aAnalogInfo[CController::ANALOG_NUM];
	int8 m_keybuffer[KEYBUFFER_SIZE];
	int32 m_numUsedFixedDigital;
	IDirectInputDevice8* m_pDIDevice;
};


class CPCGamepadController : public IPhysicalController
{
public:
	CPCGamepadController(int32 iController)
	: m_joystickNo(iController)
	{
		m_info.m_iPhysicalPort = iController;
	};


	virtual ~CPCGamepadController(void)
	{
		;
	};


	virtual void Close(void) override
	{
		delete this;
	};


	virtual void Update(void) override
	{
		Clear();

		JOYSTICKINFO::STATE* pJoystickState = s_aJoystickInfo.m_apState[m_joystickNo];
		IDirectInputDevice8* pDevice = pJoystickState->m_pDevice;
		if (!pJoystickState || !pDevice)
		{
			m_info.m_state = CController::STATE_UNCONNECT;
			IPhysicalController::Update();
			return;
		};

		m_info.m_state = CController::STATE_CONNECT;
		
		if (FAILED(pDevice->Poll()))
		{
			if (!DI8Utils::AcquireDevice(pDevice))
			{
				IPhysicalController::Update();
				return;
			};
		};

		memset(&m_joystate, 0x00, sizeof(m_joystate));
		if (FAILED(pDevice->GetDeviceState(sizeof(DIJOYSTATE2), &m_joystate)))
		{
			IPhysicalController::Update();
			return;
		};

		uint32 digital = 0;

		if (LOWORD(m_joystate.rgdwPOV[0]) != WORD(0xFFFF))
		{
			enum
			{
				DPAD_UP			= 0,
				DPAD_UP_RIGHT	= 1,
				DPAD_UP_LEFT	= 7,
				DPAD_RIGHT		= 2,
				DPAD_LEFT		= 6,
				DPAD_DOWN		= 4,
				DPAD_DOWN_RIGHT = 3,
				DPAD_DOWN_LEFT	= 5,
			};

			switch (m_joystate.rgdwPOV[0] / 4500)
			{
			case DPAD_UP:
				digital |= (CController::DIGITAL_UP);
				break;
			
			case DPAD_UP_RIGHT:
				digital |= (CController::DIGITAL_UP | CController::DIGITAL_RIGHT);
				break;
			
			case DPAD_UP_LEFT:
				digital |= (CController::DIGITAL_UP | CController::DIGITAL_LEFT);
				break;

			case DPAD_RIGHT:
				digital |= (CController::DIGITAL_RIGHT);
				break;

			case DPAD_LEFT:
				digital |= (CController::DIGITAL_LEFT);
				break;

			case DPAD_DOWN:
				digital |= (CController::DIGITAL_DOWN);
				break;

			case DPAD_DOWN_RIGHT:
				digital |= (CController::DIGITAL_DOWN | CController::DIGITAL_RIGHT);
				break;

			case DPAD_DOWN_LEFT:
				digital |= (CController::DIGITAL_DOWN | CController::DIGITAL_LEFT);
				break;

			default:
				ASSERT(false);
				break;
			};
		};

		if (IsButtonDown(0))
			digital |= CController::DIGITAL_A;
		if (IsButtonDown(1))
			digital |= CController::DIGITAL_B;
		if (IsButtonDown(2))
			digital |= CController::DIGITAL_X;
		if (IsButtonDown(3))
			digital |= CController::DIGITAL_Y;
		if (IsButtonDown(4))
			digital |= CController::DIGITAL_LEFT_BUMPER;
		if (IsButtonDown(5))
			digital |= CController::DIGITAL_RIGHT_BUMPER;
		if (IsButtonDown(6))
			digital |= CController::DIGITAL_LEFT_TRIGGER;
		if (IsButtonDown(7))
			digital |= CController::DIGITAL_RIGHT_TRIGGER;
		if (IsButtonDown(8))
			digital |= CController::DIGITAL_SELECT;
		if (IsButtonDown(9))
			digital |= CController::DIGITAL_START;
		if (IsButtonDown(10))
			digital |= CController::DIGITAL_LEFT_THUMB;
		if (IsButtonDown(11))
			digital |= CController::DIGITAL_RIGHT_THUMB;

		const int32 DEADZONE = int32(float(TYPEDEF::SINT16_MAX) * 0.25f);

		m_info.m_aAnalog[CController::ANALOG_LSTICK_X]	= int16(Clamp(m_joystate.lX, DEADZONE));
		m_info.m_aAnalog[CController::ANALOG_LSTICK_Y]	= int16(Clamp(-1 - m_joystate.lY, DEADZONE));
		m_info.m_aAnalog[CController::ANALOG_RSTICK_X]	= int16(Clamp(m_joystate.lRz, DEADZONE));
		m_info.m_aAnalog[CController::ANALOG_RSTICK_Y]	= int16(Clamp(-1 - m_joystate.rglSlider[0], DEADZONE));
		m_info.m_aAnalog[CController::ANALOG_RUP]		= (digital & CController::DIGITAL_A ? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_RDOWN] 	= (digital & CController::DIGITAL_B ? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_RLEFT] 	= (digital & CController::DIGITAL_X ? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_RRIGHT] 	= (digital & CController::DIGITAL_Y ? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_L1]		= (digital & CController::DIGITAL_LEFT_BUMPER	? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_L2]		= (digital & CController::DIGITAL_LEFT_TRIGGER	? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_L3]		= (digital & CController::DIGITAL_LEFT_THUMB	? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_R1]		= (digital & CController::DIGITAL_RIGHT_BUMPER	? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_R2]		= (digital & CController::DIGITAL_RIGHT_TRIGGER ? TYPEDEF::SINT16_MAX : 0);
		m_info.m_aAnalog[CController::ANALOG_R3]		= (digital & CController::DIGITAL_RIGHT_THUMB	? TYPEDEF::SINT16_MAX : 0);

		if(m_bVibrateEnable)
		{
			if(!m_bVibrate && m_iVibFrame > 0)
			{
				if (pJoystickState->m_bVibrationFeature && 
					pJoystickState->m_pVibrationEffect)
				{
					int32 type = int32(m_uVibType);
					int32 direction[2] = { int32(m_uVibType), int32(m_uVibType) };

					DIEFFECT effect;
					memset(&effect, 0x00, sizeof(effect));
					effect.dwSize = sizeof(effect);
					effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
					effect.lpEnvelope = 0;
					effect.dwStartDelay = 0;
					effect.cbTypeSpecificParams = sizeof(type);
					effect.lpvTypeSpecificParams = &type;
					effect.rglDirection = LPLONG(direction);
					pJoystickState->m_pVibrationEffect->SetParameters(&effect, DIEP_START | DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS);

					m_bVibrate = true;
				};
			}
			else
			{
				if (!--m_iVibFrame)
				{
					if (pJoystickState->m_bVibrationFeature &&
						pJoystickState->m_pVibrationEffect)
						pJoystickState->m_pVibrationEffect->Stop();

					m_bVibrate = false;
					m_iVibFrame = 0;
					m_uVibType = 0;
				};
			};
		};

		m_info.m_digital = digital;

		IPhysicalController::Update();
	};


	bool IsButtonDown(int32 idx) const
	{
		return (m_joystate.rgbButtons[idx] & 0x80) != 0;
	};


	int32 Clamp(int32 A, int32 B) const
	{
		if (A < B && -B < A)
			A = 0;
		return A;
	};


private:
	int32 m_joystickNo;
	DIJOYSTATE2 m_joystate;
};


static CPCKeyboardController* s_pPCKeyboardController = nullptr;


static inline CPCKeyboardController& KeyboardController(void)
{
	ASSERT(s_pPCKeyboardController);
	return *s_pPCKeyboardController;
};


static BOOL FAR PASCAL EnumAxisCallback(LPCDIDEVICEOBJECTINSTANCE lpObject, LPVOID lpParameter)
{
	JOYSTICKINFO::STATE* pJoystickState = (JOYSTICKINFO::STATE*)lpParameter;
	ASSERT(pJoystickState);

	DIPROPRANGE range;
	memset(&range, 0x00, sizeof(range));
	range.lMin = int16(0x8000);
	range.lMax = int16(0x7FFF);
	range.diph.dwObj = lpObject->dwType;
	range.diph.dwHow = DIPH_BYID;
	range.diph.dwSize = sizeof(range);
	range.diph.dwHeaderSize = sizeof(range.diph);
	
	return SUCCEEDED(pJoystickState->m_pDevice->SetProperty(DIPROP_RANGE, &range.diph));
};


static BOOL FAR PASCAL EnumFeedbackCallback(LPCDIDEVICEOBJECTINSTANCE lpObject, LPVOID lpParameter)
{
	JOYSTICKINFO::STATE* pJoystickState = (JOYSTICKINFO::STATE*)lpParameter;
	ASSERT(pJoystickState);

	pJoystickState->m_bVibrationFeature = true;
	if ((lpObject->dwType & DIDFT_FFACTUATOR) != 0)
		++pJoystickState->m_numAxes;

	return TRUE;
};


static BOOL FAR PASCAL EnumDeviceCallback(LPCDIDEVICEINSTANCE lpDevice, LPVOID lpParameter)
{
	BOOL bResult = TRUE;
	int32 deviceType = lpDevice->dwDevType & 0xFF;

	if (deviceType == DI8DEVTYPE_GAMEPAD ||
		deviceType == DI8DEVTYPE_JOYSTICK)
	{
		JOYSTICKINFO::STATE* pJoystickState = nullptr;
		try
		{
			pJoystickState = new JOYSTICKINFO::STATE;
			if (!pJoystickState)
				throw std::exception();

			memset(pJoystickState, 0x00, sizeof(*pJoystickState));
			_tcscpy(pJoystickState->m_tszName, lpDevice->tszProductName);
			pJoystickState->m_guid = lpDevice->guidInstance;

			if (FAILED(s_pDirectInput8->CreateDevice(pJoystickState->m_guid, &pJoystickState->m_pDevice, NULL)))
				throw std::exception();

			if (FAILED(pJoystickState->m_pDevice->SetDataFormat(&c_dfDIJoystick2)))
				throw std::exception();

			if (FAILED(pJoystickState->m_pDevice->SetCooperativeLevel(HWND(s_hWnd), DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
				throw std::exception();

			pJoystickState->m_deviceCaps.dwSize = sizeof(pJoystickState->m_deviceCaps);
			if (FAILED(pJoystickState->m_pDevice->GetCapabilities(&pJoystickState->m_deviceCaps)))
				throw std::exception();

			if (FAILED(pJoystickState->m_pDevice->EnumObjects(EnumAxisCallback, pJoystickState, DIDFT_AXIS)))
				throw std::exception();

			if (FAILED(pJoystickState->m_pDevice->EnumObjects(EnumFeedbackCallback, pJoystickState, DIDFT_FFACTUATOR | DIDFT_AXIS)))
				throw std::exception();

			if (pJoystickState->m_bVibrationFeature)
			{
				DIPROPDWORD prop;
				memset(&prop, 0x00, sizeof(prop));
				prop.diph.dwHow = DIPH_DEVICE;
				prop.diph.dwObj = 0;
				prop.diph.dwSize = sizeof(prop);
				prop.diph.dwHeaderSize = sizeof(prop.diph);
				prop.dwData = 0;

				if (FAILED(pJoystickState->m_pDevice->SetProperty(DIPROP_AUTOCENTER, &prop.diph)))
					throw std::exception();

				if (pJoystickState->m_numAxes > 2)
					pJoystickState->m_numAxes = 2;

				int32 aAxes[4] = { 0, 4, 0, 0 };

				DIEFFECT effect;
				memset(&effect, 0x00, sizeof(effect));
				effect.dwGain = 10000;
				effect.rglDirection = LPLONG(&aAxes[2]);
				effect.dwDuration = -1;
				effect.dwTriggerButton = -1;
				effect.cAxes = pJoystickState->m_numAxes;
				effect.dwSize = sizeof(effect);
				effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
				effect.dwSamplePeriod = 0;
				effect.dwTriggerRepeatInterval = 0;
				effect.rgdwAxes = LPDWORD(aAxes);
				effect.lpEnvelope = 0;
				effect.dwStartDelay = 0;

				int32 param = 10000;
				effect.cbTypeSpecificParams = sizeof(param);
				effect.lpvTypeSpecificParams = &param;

				if (FAILED(pJoystickState->m_pDevice->CreateEffect(GUID_ConstantForce, &effect, &pJoystickState->m_pVibrationEffect, NULL)))
					throw std::exception();
			};

			s_aJoystickInfo.m_apState[s_aJoystickInfo.m_count++] = pJoystickState;
			bResult = (s_aJoystickInfo.m_count < JOYSTICKINFO::STATE_MAX);
		}
		catch (std::exception& e)
		{
			REF(e);

			if (pJoystickState->m_pVibrationEffect)
			{
				pJoystickState->m_pVibrationEffect->Release();
				pJoystickState->m_pVibrationEffect = nullptr;
			};

			if (pJoystickState->m_pDevice)
			{
				pJoystickState->m_pDevice->Release();
				pJoystickState->m_pDevice = nullptr;
			};

			if (pJoystickState)
			{
				delete pJoystickState;
				pJoystickState = nullptr;
			};
		};
	};

	return bResult;
};


/*static*/ int32 CPCPhysicalController::PHYSICALCONTROLLER_MAX = 3;


/*static*/ bool CPCPhysicalController::Initialize(void* hWnd)
{
	s_hWnd = hWnd;

	if (DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&s_pDirectInput8, NULL) == DI_OK)
	{
 		if (s_pDirectInput8->EnumDevices(DI8DEVCLASS_ALL, EnumDeviceCallback, NULL, 0) == DI_OK)
		{
			s_pPCKeyboardController = new CPCKeyboardController(s_aJoystickInfo.m_count);
			if (s_pPCKeyboardController)
			{
				if (s_pPCKeyboardController->Start())
				{
					PHYSICALCONTROLLER_MAX = ++s_aJoystickInfo.m_count;
					return true;
				};
			};
		};
	};

	if (s_pPCKeyboardController)
	{
		delete s_pPCKeyboardController;
		s_pPCKeyboardController = nullptr;
	};

	s_pDirectInput8->Release();
	s_pDirectInput8 = nullptr;

	return false;
};


/*static*/ void CPCPhysicalController::Terminate(void)
{
	for (int32 i = 0; i < s_aJoystickInfo.m_count; ++i)
	{
		JOYSTICKINFO::STATE* pJoystickState = s_aJoystickInfo.m_apState[i];
		if (!pJoystickState)
			continue;

		ASSERT(pJoystickState->m_pDevice);

		pJoystickState->m_pDevice->Unacquire();

		if (pJoystickState->m_pVibrationEffect)
		{
			pJoystickState->m_pVibrationEffect->Stop();
			pJoystickState->m_pVibrationEffect->Release();
			pJoystickState->m_pVibrationEffect = nullptr;
		};

		pJoystickState->m_pDevice->Release();
		pJoystickState->m_pDevice = nullptr;

		delete pJoystickState;
		s_aJoystickInfo.m_apState[i] = nullptr;
	};

	s_aJoystickInfo.m_count = 0;

	if (s_pPCKeyboardController)
	{
		delete s_pPCKeyboardController;
		s_pPCKeyboardController = nullptr;
	};

	if (s_pDirectInput8)
	{
		s_pDirectInput8->Release();
		s_pDirectInput8 = nullptr;
	};
};


/*static*/ IPhysicalController* CPCPhysicalController::New(int32 iController)
{
	if (!s_pPCKeyboardController || KeyboardController().Info().m_iPhysicalPort != iController)
	{
		ASSERT(iController < PHYSICALCONTROLLER_MAX);
		return new CPCGamepadController(iController);
	};

	return s_pPCKeyboardController;
};


/*static*/ void CPCPhysicalController::MapDigital(uint32 btn, int32 iDIKey)
{
	KeyboardController().MapDigital(btn, iDIKey);
};


/*static*/ void CPCPhysicalController::MapDigitalFixed(uint32 btn, int32 iDIKey)
{
	KeyboardController().MapDigitalFixed(btn, iDIKey);
};


/*static*/ void CPCPhysicalController::MapAnalog(int32 no, int32 iDIKeyX, int32 iDIKeyY)
{
	KeyboardController().MapAnalog(no, iDIKeyX, iDIKeyY);
};


/*static*/ bool CPCPhysicalController::IsKeyDown(int32 iDIKey)
{
	return KeyboardController().IsKeyDown(iDIKey);
};


/*static*/ bool CPCPhysicalController::IsKeyValid(int32 iDIKey)
{
	static int8 s_aValidKeyTable[] =
	{
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	};

	return KeyboardController().IsKeyNotFixed(iDIKey) && s_aValidKeyTable[iDIKey];
};


/*static*/ bool CPCPhysicalController::IsKeyNotFixed(int32 iDIKey)
{
	return KeyboardController().IsKeyNotFixed(iDIKey);
};


/*static*/ int32 CPCPhysicalController::GetDownKey(void)
{
	return KeyboardController().GetDownKey();
};


/*static*/ int32 CPCPhysicalController::GetPort(void)
{
	return KeyboardController().Info().m_iPhysicalPort;
};


/*static*/ const char* CPCPhysicalController::GetKeyName(int32 iDIKey)
{
	static const char* s_apszKeyNameTable[]=
	{
		"NULL",
		"NULL",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",
		"dash",
		"NULL",
		"back",
		"tab",
		"q",
		"w",
		"e",
		"r",
		"t",
		"y",
		"u",
		"i",
		"o",
		"p",
		"lbrace",
		"rbrace",
		"NULL",
		"lctrl",
		"a",
		"s",
		"d",
		"f",
		"g",
		"h",
		"j",
		"k",
		"l",
		"colon",
		"NULL",
		"NULL",
		"lshift",
		"bkslash",
		"z",
		"x",
		"c",
		"v",
		"b",
		"n",
		"m",
		"comma",
		"period",
		"slash",
		"rshift",
		"numpad star",
		"lalt",
		"space",
		"capslock",
		"f1",
		"f2",
		"f3",
		"f4",
		"f5",
		"f6",
		"f7",
		"f8",
		"f9",
		"f10",
		"numlock",
		"scroll",
		"numpad 7",
		"numpad 8",
		"numpad 9",
		"numpad minus",
		"numpad 4",
		"numpad 5",
		"numpad 6",
		"numpad plus",
		"numpad 1",
		"numpad 2",
		"numpad 3",
		"numpad 0",
		"numpad del",
		"NULL",
		"NULL",
		"NULL",
		"f11",
		"f12",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"rctrl",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"numpad slash",
		"NULL",
		"NULL",
		"ralt",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"home",
		"up",
		"pgup",
		"NULL",
		"left",
		"NULL",
		"right",
		"NULL",
		"end",
		"down",
		"pgdown",
		"ins",
		"del",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"lwin",
		"rwin",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		"NULL",
	};

	if (iDIKey >= 0 && iDIKey < 256)
		return s_apszKeyNameTable[iDIKey];
	else
		return nullptr;
};