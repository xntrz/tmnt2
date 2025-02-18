#include "PCPhysicalController.hpp"
#include "PCSpecific.hpp"

#ifdef _DEBUG
#include "System/Common/Configure.hpp"
#endif /* _DEBUG */

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


struct JOYSTICKSTATE
{
    GUID                 m_guid;
#ifdef _DEBUG
    GUID                 m_guidprod;
#endif /* _DEBUG */
    IDirectInputDevice8* m_pDevice;
    DIDEVCAPS            m_deviceCaps;
    bool                 m_bVibrationFeature;
    int32                m_numAxes;
    IDirectInputEffect*  m_pVibrationEffect;
    TCHAR                m_tszName[MAX_PATH];
};


struct JOYSTICKINFO
{
    JOYSTICKSTATE*  m_apJoystickState[32];
    int32           m_nJoystickCnt;
};


static JOYSTICKINFO s_JoystickInfo;
static IDirectInput8* s_pDirectInput8 = nullptr;
static class CPCKeyboardController* s_pPCKeyboardController = nullptr;


static inline CPCKeyboardController& KeyboardController(void)
{
    ASSERT(s_pPCKeyboardController);
    return *s_pPCKeyboardController;
};


static inline bool AcquireDevice(IDirectInputDevice8* pDevice)
{
    HRESULT hResult = S_OK;
    
    do
    {
        hResult = pDevice->Acquire();
    } while (hResult == DIERR_INPUTLOST);

    return SUCCEEDED(hResult);
};


class CPCKeyboardController final : public IPhysicalController
{
private:
    struct DIGITALINFO
    {
        uint32 m_digital;
        int32  m_scancode;
        bool   m_bEnabled;
    };

    struct ANALOGINFO
    {
        CController::ANALOG m_analog;
        bool  m_bOneAxis;
        int32 m_scancodeX;
        int32 m_scancodeY;
        bool  m_bEnabled;
    };

    static const int32 KEYBUFFER_SIZE = 256;
    static const int32 FIXED_DIGITAL_MAX = CController::DIGITAL_NUM;

public:
    CPCKeyboardController(int32 iController);
    virtual ~CPCKeyboardController(void);
    virtual void Close(void) override;
    virtual void Update(void) override;
    virtual bool Start(void);
    void MapDigital(uint32 btn, int32 iDIKey);
    void MapDigitalFixed(uint32 btn, int32 iDIKey);
    void MapAnalog(CController::ANALOG analog, int32 iDIKeyX, int32 iDIKeyY);
    bool IsKeyDown(int32 iDIKey) const;
    bool IsKeyTrigger(int32 iDIKey) const;
    bool IsKeyNotFixed(int32 iDIKey) const;
    int32 GetDownKey(void) const;

private:
    DIGITALINFO          m_aDigitalFixedInfo[FIXED_DIGITAL_MAX];
    DIGITALINFO          m_aDigitalInfo[CController::DIGITAL_NUM];
    ANALOGINFO           m_aAnalogInfo[CController::ANALOG_NUM];
    uint8                m_keybufferCurr[KEYBUFFER_SIZE];
    uint8                m_keybufferPrev[KEYBUFFER_SIZE];
    int32                m_numUsedFixedDigital;
    IDirectInputDevice8* m_pDIDevice;
};


class CPCGamepadController final : public IPhysicalController
{
public:
    CPCGamepadController(int32 iController);
    virtual ~CPCGamepadController(void);
    virtual void Close(void) override;
    virtual void Update(void) override;
    bool IsButtonDown(int32 idx) const;
    int32 ClampValue(int32 value, int32 clamp) const;

private:
    int32       m_iPort;
    DIJOYSTATE2 m_joystate;
#ifdef _DEBUG
    bool        m_bAxisCorrection;
#endif /* _DEBUG */
};


//
// *********************************************************************************
//


CPCKeyboardController::CPCKeyboardController(int32 iController)
: m_numUsedFixedDigital(0)
, m_pDIDevice(nullptr)
{
    std::memset(m_aDigitalInfo, 0x00, sizeof(m_aDigitalInfo));
    std::memset(m_aDigitalFixedInfo, 0x00, sizeof(m_aDigitalFixedInfo));
    std::memset(m_aAnalogInfo, 0x00, sizeof(m_aAnalogInfo));

    m_info.m_iPhysicalPort = iController;	
};


CPCKeyboardController::~CPCKeyboardController(void)
{
    if (m_pDIDevice)
    {
        m_pDIDevice->Unacquire();
        m_pDIDevice->Release();
        m_pDIDevice = nullptr;
    };
};


void CPCKeyboardController::Close(void)
{
    if (m_pDIDevice)
    {
        m_pDIDevice->Unacquire();
        m_pDIDevice->Release();
        m_pDIDevice = nullptr;
    };
};


void CPCKeyboardController::Update(void)
{
    Clear();

    if (!AcquireDevice(m_pDIDevice))
    {
        m_info.m_eState = CController::STATE_UNCONNECT;
        IPhysicalController::Update();
        return;
    };

    m_info.m_eState = CController::STATE_CONNECT;

    std::memcpy(m_keybufferPrev, m_keybufferCurr, sizeof(m_keybufferCurr));
    std::memset(m_keybufferCurr, 0x00, sizeof(m_keybufferCurr));
    if (FAILED(m_pDIDevice->GetDeviceState(sizeof(m_keybufferCurr), m_keybufferCurr)))
    {
        IPhysicalController::Update();
        return;
    };

    uint32 digital = 0;
    
    for (int32 i = 0; i < m_numUsedFixedDigital; ++i)
    {
        if (m_aDigitalFixedInfo[i].m_bEnabled && IsKeyDown(m_aDigitalFixedInfo[i].m_scancode))
            digital |= m_aDigitalFixedInfo[i].m_digital;
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
            if (m_aAnalogInfo[i].m_bOneAxis)
            {
                if (IsKeyDown(m_aAnalogInfo[i].m_scancodeX))
                    m_info.m_aAnalog[i] = TYPEDEF::SINT16_MAX;
                else
                    m_info.m_aAnalog[i] = 0;
            }
            else
            {
                if (IsKeyDown(m_aAnalogInfo[i].m_scancodeY))
                    m_info.m_aAnalog[i] = TYPEDEF::SINT16_MAX;
                else if (IsKeyDown(m_aAnalogInfo[i].m_scancodeX))
                    m_info.m_aAnalog[i] = TYPEDEF::SINT16_MIN;
            };
        };
    };

    m_info.m_digital = digital;

    IPhysicalController::Update();
};


bool CPCKeyboardController::Start(void)
{
    ASSERT(s_pDirectInput8);

    if (FAILED(s_pDirectInput8->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL)))
        return false;

    if (FAILED(m_pDIDevice->SetDataFormat(&c_dfDIKeyboard)))
        return false;

    if (FAILED(m_pDIDevice->SetCooperativeLevel(CPCSpecific::m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
        return false;

    //
    // NOTE: Uncomment this to return initialization failure if keyboard focus is not on window
    //
    
    /*
    if (!AcquireDevice(m_pDIDevice))
        return false;
    */
    
    for (int32 i = 0; i < COUNT_OF(m_aDigitalInfo); ++i)
    {
        m_aDigitalFixedInfo[i].m_bEnabled = false;
        m_aDigitalFixedInfo[i].m_digital = 0;
        m_aDigitalFixedInfo[i].m_scancode = -1;
    };

    uint32 aDigital[] =
    {
        CController::DIGITAL_LUP,
        CController::DIGITAL_LDOWN,
        CController::DIGITAL_LLEFT,
        CController::DIGITAL_LRIGHT,
        CController::DIGITAL_RUP,
        CController::DIGITAL_RDOWN,
        CController::DIGITAL_RLEFT,
        CController::DIGITAL_RRIGHT,
        CController::DIGITAL_SELECT,
        CController::DIGITAL_START,
        CController::DIGITAL_L1,
        CController::DIGITAL_L2,
        CController::DIGITAL_L3,
        CController::DIGITAL_R1,
        CController::DIGITAL_R2,
        CController::DIGITAL_R3,
    };

    for (int32 i = 0; i < COUNT_OF(m_aDigitalInfo); ++i)
    {
        m_aDigitalInfo[i].m_bEnabled = false;
        m_aDigitalInfo[i].m_digital = aDigital[i];
        m_aDigitalInfo[i].m_scancode = -1;
    };

    for (int32 i = 0; i < COUNT_OF(m_aAnalogInfo); ++i)
    {
        m_aAnalogInfo[i].m_bEnabled = false;
        m_aAnalogInfo[i].m_analog = CController::ANALOG(i);
        m_aAnalogInfo[i].m_bOneAxis = (i >= 0 && i <= 3 ? false : true);
        m_aAnalogInfo[i].m_scancodeX = -1;
        m_aAnalogInfo[i].m_scancodeY = -1;
    };
    
    return true;
};


void CPCKeyboardController::MapDigital(uint32 btn, int32 iDIKey)
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


void CPCKeyboardController::MapDigitalFixed(uint32 btn, int32 iDIKey)
{
    ASSERT(m_numUsedFixedDigital < FIXED_DIGITAL_MAX);
    
    m_aDigitalFixedInfo[m_numUsedFixedDigital].m_bEnabled = true;
    m_aDigitalFixedInfo[m_numUsedFixedDigital].m_digital = btn;
    m_aDigitalFixedInfo[m_numUsedFixedDigital].m_scancode = iDIKey;
    
    ++m_numUsedFixedDigital;
};


void CPCKeyboardController::MapAnalog(CController::ANALOG analog, int32 iDIKeyX, int32 iDIKeyY)
{
    ASSERT(iDIKeyX >= 0);
    ASSERT(iDIKeyX < KEYBUFFER_SIZE);

    ASSERT(iDIKeyY >= 0);
    ASSERT(iDIKeyY < KEYBUFFER_SIZE);

    ASSERT(analog >= 0);
    ASSERT(analog < CController::ANALOG_NUM);

    for (int32 i = 0; i < COUNT_OF(m_aAnalogInfo); ++i)
    {
        if (m_aAnalogInfo[i].m_analog == analog)
        {
            m_aAnalogInfo[i].m_bEnabled = (iDIKeyX >= 0 && iDIKeyY >= 0);
            m_aAnalogInfo[i].m_scancodeX = iDIKeyX;
            m_aAnalogInfo[i].m_scancodeY = iDIKeyY;
        };
    };
};


bool CPCKeyboardController::IsKeyDown(int32 iDIKey) const
{
    ASSERT(iDIKey >= 0);
    ASSERT(iDIKey < KEYBUFFER_SIZE);

    return ((m_keybufferCurr[iDIKey] & 0x80) != 0);
};


bool CPCKeyboardController::IsKeyTrigger(int32 iDIKey) const
{
    ASSERT(iDIKey >= 0);
    ASSERT(iDIKey < KEYBUFFER_SIZE);

    return ((m_keybufferPrev[iDIKey] & 0x80) == 0) &&
           ((m_keybufferCurr[iDIKey] & 0x80) != 0);
};


bool CPCKeyboardController::IsKeyNotFixed(int32 iDIKey) const
{
    for (int32 i = 0; i < m_numUsedFixedDigital; ++i)
    {
        if (m_aDigitalFixedInfo[i].m_scancode == iDIKey)
            return false;
    };

    return true;
};


int32 CPCKeyboardController::GetDownKey(void) const
{
    for (int32 i = 0; i < KEYBUFFER_SIZE; ++i)
    {
        if (IsKeyDown(i))
            return i;
    };

    return -1;
};


//
// *********************************************************************************
//


CPCGamepadController::CPCGamepadController(int32 iController)
: m_iPort(iController)
, m_joystate({})
#ifdef _DEBUG
, m_bAxisCorrection(false)
#endif /* _DEBUG */
{
    m_info.m_iPhysicalPort = iController;

#ifdef _DEBUG
    m_bAxisCorrection = CConfigure::CheckArg("padfix");
#endif /* _DEBUG */
};


CPCGamepadController::~CPCGamepadController(void)
{
    ;
};


void CPCGamepadController::Close(void)
{
    delete this;
};


void CPCGamepadController::Update(void)
{
    Clear();

    JOYSTICKSTATE* pJoystickState = s_JoystickInfo.m_apJoystickState[m_iPort];
    
    IDirectInputDevice8* pDevice = pJoystickState->m_pDevice;
    if (!pJoystickState || !pDevice)
    {
        m_info.m_eState = CController::STATE_UNCONNECT;
        IPhysicalController::Update();
        return;
    };

    if (FAILED(pDevice->Poll()))
    {
        if (!AcquireDevice(pDevice))
        {
            m_info.m_eState = CController::STATE_UNCONNECT;
            IPhysicalController::Update();
            return;
        };
    };

    m_info.m_eState = CController::STATE_CONNECT;

    std::memset(&m_joystate, 0x00, sizeof(m_joystate));
    if (FAILED(pDevice->GetDeviceState(sizeof(DIJOYSTATE2), &m_joystate)))
    {
        IPhysicalController::Update();
        return;
    };

    uint32 digital = 0;

    if (LOWORD(m_joystate.rgdwPOV[0]) != WORD(0xFFFF))
    {
        uint32 PovDigital[] =
        {
            CController::DIGITAL_LUP,
            CController::DIGITAL_LUP 	| CController::DIGITAL_LRIGHT,
            CController::DIGITAL_LRIGHT,
            CController::DIGITAL_LRIGHT | CController::DIGITAL_LDOWN,
            CController::DIGITAL_LDOWN,
            CController::DIGITAL_LDOWN 	| CController::DIGITAL_LLEFT,
            CController::DIGITAL_LLEFT,
            CController::DIGITAL_LLEFT 	| CController::DIGITAL_LUP,
        };

        int32 Index = int32(m_joystate.rgdwPOV[0] / 4500);
        
        if (Index >= 0 && Index < COUNT_OF(PovDigital))
            digital |= PovDigital[Index];
    };

    uint32 BtnDigital[] =
    {
        CController::DIGITAL_RDOWN,
        CController::DIGITAL_RLEFT,
        CController::DIGITAL_RRIGHT,
        CController::DIGITAL_RUP,
        CController::DIGITAL_L1,
        CController::DIGITAL_L2,
        CController::DIGITAL_R1,
        CController::DIGITAL_R2,
        CController::DIGITAL_SELECT,
        CController::DIGITAL_START,
        CController::DIGITAL_L3,
        CController::DIGITAL_R3,		
    };

    for (int32 i = 0; i < COUNT_OF(BtnDigital); ++i)
    {
        if (IsButtonDown(i))
            digital |= BtnDigital[i];
    };

#ifdef _DEBUG
    if (m_bAxisCorrection)
    {
        switch (pJoystickState->m_guidprod.Data1)
        {
        case MAKELONG(0x0810, 0x0001):
            {
                //
                //	VID_0810&PID_0001 gamepad case:
                // 		- correcting right stick axis values
                //
                m_joystate.rglSlider[0] = m_joystate.lZ;
            }
            break;
        };
    };
#endif /* _DEBUG */	

    const int32 DEADZONE = int32(float(TYPEDEF::SINT16_MAX) * 0.25f);

    m_info.m_aAnalog[CController::ANALOG_LSTICK_X] 	= int16(ClampValue(m_joystate.lX, DEADZONE));
    m_info.m_aAnalog[CController::ANALOG_LSTICK_Y] 	= int16(ClampValue(-1 - m_joystate.lY, DEADZONE));
    m_info.m_aAnalog[CController::ANALOG_RSTICK_X] 	= int16(ClampValue(m_joystate.lRz, DEADZONE));
    m_info.m_aAnalog[CController::ANALOG_RSTICK_Y] 	= int16(ClampValue(-1 - m_joystate.rglSlider[0], DEADZONE));
    m_info.m_aAnalog[CController::ANALOG_RUP] 		= (digital & CController::DIGITAL_RUP 	? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_RDOWN] 	= (digital & CController::DIGITAL_RDOWN ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_RLEFT] 	= (digital & CController::DIGITAL_RLEFT ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_RRIGHT] 	= (digital & CController::DIGITAL_RRIGHT? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_L1] 		= (digital & CController::DIGITAL_L1 	? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_L2] 		= (digital & CController::DIGITAL_L2 	? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_L3] 		= (digital & CController::DIGITAL_L3 	? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_R1] 		= (digital & CController::DIGITAL_R1 	? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_R2] 		= (digital & CController::DIGITAL_R2 	? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_R3] 		= (digital & CController::DIGITAL_R3 	? TYPEDEF::SINT16_MAX : 0);

    //
    //	TESTME will test it someday
    //
    if (m_info.m_bVibEnable)
    {
        if (!m_info.m_bVibrate && (m_info.m_iVibFrame > 0))
        {
            if (pJoystickState->m_bVibrationFeature && pJoystickState->m_pVibrationEffect)
            {
                int32 type = int32(m_info.m_uVibMax);
                int32 direction[2] = { int32(m_info.m_uVibMax), int32(m_info.m_uVibMax) };

                DIEFFECT effect;
                std::memset(&effect, 0x00, sizeof(effect));
                effect.dwSize = sizeof(effect);
                effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
                effect.lpEnvelope = 0;
                effect.dwStartDelay = 0;
                effect.cbTypeSpecificParams = sizeof(type);
                effect.lpvTypeSpecificParams = &type;
                effect.rglDirection = LPLONG(direction);

                pJoystickState->m_pVibrationEffect->SetParameters(&effect, DIEP_START | DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS);

                m_info.m_bVibrate = true;
            };
        }
        else
        {
            if (!--m_info.m_iVibFrame)
            {
                if (pJoystickState->m_bVibrationFeature && pJoystickState->m_pVibrationEffect)
                    pJoystickState->m_pVibrationEffect->Stop();

                m_info.m_bVibrate = false;
                m_info.m_iVibFrame = 0;
                m_info.m_uVibMax = 0;
            };
        };
    };

    m_info.m_digital = digital;

    IPhysicalController::Update();
};


bool CPCGamepadController::IsButtonDown(int32 idx) const
{
    return ((m_joystate.rgbButtons[idx] & static_cast<BYTE>(0x80)) != 0);
};


int32 CPCGamepadController::ClampValue(int32 value, int32 clamp) const
{
    return ((value < clamp) && (value > -clamp)) ? 0 : value;
};


static BOOL FAR PASCAL EnumAxisCallback(LPCDIDEVICEOBJECTINSTANCE lpObject, LPVOID lpParameter)
{
    JOYSTICKSTATE* pJoystickState = static_cast<JOYSTICKSTATE*>(lpParameter);
    ASSERT(pJoystickState);

    DIPROPRANGE range;
    memset(&range, 0x00, sizeof(range));
    range.lMin = TYPEDEF::SINT16_MIN;
    range.lMax = TYPEDEF::SINT16_MAX;
    range.diph.dwObj = lpObject->dwType;
    range.diph.dwHow = DIPH_BYID;
    range.diph.dwSize = sizeof(range);
    range.diph.dwHeaderSize = sizeof(range.diph);
    
    return SUCCEEDED(pJoystickState->m_pDevice->SetProperty(DIPROP_RANGE, &range.diph));
};


static BOOL FAR PASCAL EnumFeedbackCallback(LPCDIDEVICEOBJECTINSTANCE lpObject, LPVOID lpParameter)
{
    JOYSTICKSTATE* pJoystickState = static_cast<JOYSTICKSTATE*>(lpParameter);
    ASSERT(pJoystickState);

    pJoystickState->m_bVibrationFeature = true;
    if ((lpObject->dwType & DIDFT_FFACTUATOR) != 0)
        ++pJoystickState->m_numAxes;

    return TRUE;
};


static BOOL FAR PASCAL EnumDeviceCallback(LPCDIDEVICEINSTANCE lpDevice, LPVOID lpParameter)
{
    DWORD dwDevType = DIDFT_GETTYPE(lpDevice->dwDevType);

    if ((dwDevType != DI8DEVTYPE_GAMEPAD) &&
        (dwDevType != DI8DEVTYPE_JOYSTICK))
        return TRUE;

    JOYSTICKSTATE* pJoystickState = new JOYSTICKSTATE;

    std::memset(pJoystickState, 0x00, sizeof(*pJoystickState));
    _tcscpy(pJoystickState->m_tszName, lpDevice->tszProductName);
    pJoystickState->m_guid = lpDevice->guidInstance;	
#ifdef _DEBUG
    pJoystickState->m_guidprod = lpDevice->guidProduct;
#endif /* _DEBUG */

    if (FAILED(s_pDirectInput8->CreateDevice(pJoystickState->m_guid, &pJoystickState->m_pDevice, NULL)))
        goto label_failure;

    if (FAILED(pJoystickState->m_pDevice->SetDataFormat(&c_dfDIJoystick2)))
        goto label_failure;

    if (FAILED(pJoystickState->m_pDevice->SetCooperativeLevel(CPCSpecific::m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
        goto label_failure;

    pJoystickState->m_deviceCaps.dwSize = sizeof(pJoystickState->m_deviceCaps);
    if (FAILED(pJoystickState->m_pDevice->GetCapabilities(&pJoystickState->m_deviceCaps)))
        goto label_failure;

    if (FAILED(pJoystickState->m_pDevice->EnumObjects(EnumAxisCallback, pJoystickState, DIDFT_AXIS)))
        goto label_failure;

    if (FAILED(pJoystickState->m_pDevice->EnumObjects(EnumFeedbackCallback, pJoystickState, DIDFT_FFACTUATOR | DIDFT_AXIS)))
        goto label_failure;

    if (pJoystickState->m_bVibrationFeature)
    {
        DIPROPDWORD prop;
        std::memset(&prop, 0x00, sizeof(prop));		
        prop.diph.dwHow 		= DIPH_DEVICE;
        prop.diph.dwObj 		= 0;
        prop.diph.dwSize 		= sizeof(prop);
        prop.diph.dwHeaderSize 	= sizeof(prop.diph);
        prop.dwData 			= 0;

        if (FAILED(pJoystickState->m_pDevice->SetProperty(DIPROP_AUTOCENTER, &prop.diph)))
            goto label_failure;

        if (pJoystickState->m_numAxes > 2)
            pJoystickState->m_numAxes = 2;

        int32 aAxes[4] = { 0, 4, 0, 0 };

        DIEFFECT effect;
        std::memset(&effect, 0x00, sizeof(effect));		
        effect.dwGain 					= 10000;
        effect.rglDirection 			= LPLONG(&aAxes[2]);
        effect.dwDuration 				= INFINITE;
        effect.dwTriggerButton 			= DIEB_NOTRIGGER;
        effect.cAxes 					= pJoystickState->m_numAxes;
        effect.dwSize 					= sizeof(effect);
        effect.dwFlags 					= DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        effect.dwSamplePeriod 			= 0;
        effect.dwTriggerRepeatInterval 	= 0;
        effect.rgdwAxes 				= LPDWORD(aAxes);
        effect.lpEnvelope 				= 0;
        effect.dwStartDelay 			= 0;

        int32 param = 10000;
        effect.cbTypeSpecificParams = sizeof(param);
        effect.lpvTypeSpecificParams = &param;

        if (FAILED(pJoystickState->m_pDevice->CreateEffect(GUID_ConstantForce, &effect, &pJoystickState->m_pVibrationEffect, NULL)))
            goto label_failure;
    };

    s_JoystickInfo.m_apJoystickState[s_JoystickInfo.m_nJoystickCnt++] = pJoystickState;

    return (s_JoystickInfo.m_nJoystickCnt < COUNT_OF(s_JoystickInfo.m_apJoystickState));

label_failure:
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
    
    return TRUE;
};


/*static*/ int32 CPCPhysicalController::PHYSICALCONTROLLER_MAX = 3;


/*static*/ bool CPCPhysicalController::Initialize(void)
{
    HRESULT hr = DirectInput8Create(GetModuleHandle(NULL),
                                    DIRECTINPUT_VERSION,
                                    IID_IDirectInput8,
                                    reinterpret_cast<LPVOID*>(&s_pDirectInput8), NULL);
    if (SUCCEEDED(hr))
    {
        if (s_pDirectInput8->EnumDevices(DI8DEVCLASS_ALL, EnumDeviceCallback, NULL, 0) == DI_OK)
        {
            s_pPCKeyboardController = new CPCKeyboardController(s_JoystickInfo.m_nJoystickCnt);
            if (s_pPCKeyboardController)
            {
                if (s_pPCKeyboardController->Start())
                {
                    PHYSICALCONTROLLER_MAX = ++s_JoystickInfo.m_nJoystickCnt;
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
    for (int32 i = 0; i < s_JoystickInfo.m_nJoystickCnt; ++i)
    {
        JOYSTICKSTATE* pJoystickState = s_JoystickInfo.m_apJoystickState[i];
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
        s_JoystickInfo.m_apJoystickState[i] = nullptr;
    };

    s_JoystickInfo.m_nJoystickCnt = 0;

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


/*static*/ IPhysicalController* CPCPhysicalController::Open(int32 iController)
{
    IPhysicalController* pPhysicalController = s_pPCKeyboardController;

    if(KeyboardController().Info().m_iPhysicalPort != iController)
    {
        ASSERT(iController < PHYSICALCONTROLLER_MAX);
        pPhysicalController = new CPCGamepadController(iController);
    };

    return pPhysicalController;
};


/*static*/ void CPCPhysicalController::MapDigital(uint32 btn, int32 iDIKey)
{
    if (s_pPCKeyboardController)
        KeyboardController().MapDigital(btn, iDIKey);
};


/*static*/ void CPCPhysicalController::MapDigitalFixed(uint32 btn, int32 iDIKey)
{
    if (s_pPCKeyboardController)
        KeyboardController().MapDigitalFixed(btn, iDIKey);
};


/*static*/ void CPCPhysicalController::MapAnalog(CController::ANALOG analog, int32 iDIKeyX, int32 iDIKeyY)
{
    if (s_pPCKeyboardController)
        KeyboardController().MapAnalog(analog, iDIKeyX, iDIKeyY);
};


/*static*/ bool CPCPhysicalController::IsKeyDown(int32 iDIKey)
{
    if (s_pPCKeyboardController)
        return KeyboardController().IsKeyDown(iDIKey);
    
    return false;
};


/*static*/ bool CPCPhysicalController::IsKeyTrigger(int32 iDIKey)
{
    if (s_pPCKeyboardController)
        return KeyboardController().IsKeyTrigger(iDIKey);

    return false;
};


/*static*/ bool CPCPhysicalController::IsKeyNotFixed(int32 iDIKey)
{
    if (s_pPCKeyboardController)
        return KeyboardController().IsKeyNotFixed(iDIKey);
    
    return false;
};


/*static*/ int32 CPCPhysicalController::GetDownKey(void)
{
    if (s_pPCKeyboardController)
        return KeyboardController().GetDownKey();
    
    return -1;
};


/*static*/ int32 CPCPhysicalController::GetPort(void)
{
    if (s_pPCKeyboardController)
        return KeyboardController().Info().m_iPhysicalPort;
    
    return CController::Max();
};