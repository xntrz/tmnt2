#pragma once


class CController
{
public:
    static const int32 CONTROLLER_LOCKED_ON_VIRTUAL		= -1;
    static const int32 CONTROLLER_UNLOCKED_ON_VIRTUAL 	= -2;

    static const uint32 DIGITAL			= 0;
    static const uint32 DIGITAL_LUP 	= 0x1;
    static const uint32 DIGITAL_LDOWN 	= 0x2;
    static const uint32 DIGITAL_LLEFT 	= 0x4;
    static const uint32 DIGITAL_LRIGHT 	= 0x8;
    static const uint32 DIGITAL_RUP 	= 0x10;
    static const uint32 DIGITAL_RDOWN 	= 0x20;
    static const uint32 DIGITAL_RLEFT 	= 0x40;
    static const uint32 DIGITAL_RRIGHT 	= 0x80;
    static const uint32 DIGITAL_SELECT 	= 0x100;
    static const uint32 DIGITAL_START 	= 0x200;
    static const uint32 DIGITAL_L1 		= 0x400;
    static const uint32 DIGITAL_L2 		= 0x800;
    static const uint32 DIGITAL_L3 		= 0x1000;
    static const uint32 DIGITAL_R1 		= 0x2000;
    static const uint32 DIGITAL_R2 		= 0x4000;
    static const uint32 DIGITAL_R3 		= 0x8000;

    static uint32		DIGITAL_OK;
    static uint32 		DIGITAL_CANCEL;
    
    static const uint32 DIGITAL_MASK;
    
    static const int32 	DIGITAL_NUM = BITSOF(DIGITAL);
    
    enum ANALOG
    {
        ANALOG_LSTICK_X = 0,
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
    static int32 Max(void);
    static bool Lock(int32 iController);
    static bool Unlock(int32 iController);
    static bool IsLocked(int32 iController);
    static STATE GetState(int32 iController);
    static int32 GetPhysicalPort(int32 iController);
    static int32 GetController(int32 iPhysicalPort);
    static uint32 GetDigital(int32 iController);
    static bool GetDigital(int32 iController, uint32 btn);
    static uint32 GetDigitalRepeat(int32 iController);
    static bool GetDigitalRepeat(int32 iController, uint32 btn);
    static uint32 GetDigitalTrigger(int32 iController);
    static bool GetDigitalTrigger(int32 iController, uint32 btn);
    static uint32 GetDigitalRelease(int32 iController);
    static bool GetDigitalRelease(int32 iController, uint32 btn);
    static int16 GetAnalog(int32 iController, ANALOG analog);
    static void EnableStickToDigitalMapping(int32 iController, STICK stick, bool bEnable);
    static void EnableVibration(int32 iController, bool bEnable);
    static bool StartVibration(int32 iController, uint32 uVibMax, int32 iVibFrame);
    static void Mapping(int32 iController, uint32 btnPhysical, uint32 btnVirtual);
    static uint32 GetMapping(int32 iController, uint32 btnPhysical);
    static void Mapping(int32 iController, ANALOG analogPhysical, ANALOG analogVirtual);
    static ANALOG GetMapping(int32 iController, ANALOG analogPhysical);
};


class IPhysicalController
{
public:
    struct INFO
    {
        int32 m_iPhysicalPort;
        CController::STATE m_eState;
        uint32 m_digital;
        uint32 m_digitalOld;
        uint32 m_digitalTrigger;
        uint32 m_digitalRelease;
        uint32 m_digitalRepeat;
        uint8 m_aDigitalRepeatTime[BITSOF(CController::DIGITAL)];
        uint8 m_aDigitalRepeatDelay[BITSOF(CController::DIGITAL)];
        int16 m_aAnalog[CController::ANALOG_NUM];
        uint32 m_uVibMax;
        int32 m_iVibFrame;
        bool m_bVibrate;
        bool m_bVibEnable;
        bool m_abStickToDigital[CController::STICK_NUM];
    };

public:
    static uint32 ConvertAnalogStickToDigital(CController::STICK stick, int16 x, int16 y);
    
    IPhysicalController(void);
    virtual ~IPhysicalController(void);
    virtual void Close(void) = 0;
    virtual void Update(void);
    virtual void Clear(void);
    virtual void SetVibration(uint32 uVibMax, int32 iVibFrame);
    void EnableVibration(bool bEnable);
    void EnableStickToDigitalMapping(CController::STICK stick, bool bEnable);
    uint32* GetDigitalMapping(uint32 digitalPhysical);
    CController::ANALOG* GetAnalogMapping(CController::ANALOG analogPhysical);

    inline INFO& Info(void) { return m_info; };

protected:
    INFO m_info;
    uint32 m_aDigitalMapping[BITSOF(CController::DIGITAL)];
    CController::ANALOG m_aAnalogMapping[CController::ANALOG_NUM];
};