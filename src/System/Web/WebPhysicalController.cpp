#include "WebPhysicalController.hpp"
#include "WebSpecific.hpp"

#include "System/Common/Configure.hpp"

#include <SDL3/SDL.h>


static class CWebJoystickController* s_apJoystickController[32];
static int32 s_numJoystickController;


static inline bool IsLatchModeRequired(void)
{
    return true;
};


class CWebKeyboardController final : public IPhysicalController
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

    static const int32 FIXED_DIGITAL_MAX = CController::DIGITAL_NUM;

public:
    static bool SDLEventHandler(void* userdata, SDL_Event* event);

    CWebKeyboardController(int32 iController);
    virtual ~CWebKeyboardController(void);
    virtual void Close(void) override;
    virtual void Update(void) override;
    bool Start(void);
    void MapDigital(uint32 btn, int32 sdlScancode);
    void MapDigitalFixed(uint32 btn, int32 sdlScancode);
    void MapAnalog(CController::ANALOG analog, int32 sdlScancodeX, int32 sdlScancodeY);
    bool IsKeyDown(int32 sdlScancode) const;
    bool IsKeyTrigger(int32 sdlScancode) const;
    bool IsKeyNotFixed(int32 sdlScancode) const;
    int32 GetDownKey(void) const;

private:
    DIGITALINFO  m_aDigitalFixedInfo[FIXED_DIGITAL_MAX];
    DIGITALINFO  m_aDigitalInfo[CController::DIGITAL_NUM];
    ANALOGINFO   m_aAnalogInfo[CController::ANALOG_NUM];
    int32        m_numUsedFixedDigital;
    const bool*  m_keybuffer;
    bool*        m_keybufferCurr;
    bool*        m_keybufferPrev;
    int32        m_keybufferSize;
    bool         m_bLatchMode;
    bool*        m_keybufferUpNextFrame;
    int32*       m_keybufferFrameCnt;
};


class CWebJoystickController final : public IPhysicalController
{
public:
    static bool SDLEventHandler(void* userdata, SDL_Event* event);

    CWebJoystickController(int32 iController);
    virtual ~CWebJoystickController(void);
    virtual void Close(void) override;
    virtual void Update(void) override;
    bool Start(SDL_JoystickID joystickID);
    void Connect(SDL_JoystickID joystickID);
    void Reconnect(SDL_JoystickID joystickID);
    int32 ClampValue(int32 value, int32 clamp) const;
    bool CompareGUID(SDL_GUID guid) const;
    bool IsFree(void) const;
    void DbgNoteConnection(bool bReconnect) const;

private:
    int32           m_iPort;
    SDL_Joystick*   m_pJoystick;
    SDL_GUID        m_guid;
    int32           m_numButtons;
    int32           m_numAxes;
    int32           m_numHats;
    bool            m_vibAvailable;
    bool            m_bLatchMode;
#ifdef _DEBUG
    uint16          m_vendorId;
    uint16          m_productId;
    bool            m_bAxisCorrection;
#endif /* _DEBUG */
};


class CWebTouchController final : public IPhysicalController
{
public:
    CWebTouchController(int32 iController);
    virtual ~CWebTouchController(void);
    virtual void Close(void) override;
    virtual void Update(void) override;
};


//
// *********************************************************************************
//


/*static*/ bool CWebKeyboardController::SDLEventHandler(void* userdata, SDL_Event* event)
{
    CWebKeyboardController* controller =
        reinterpret_cast<CWebKeyboardController*>(userdata);

    if (controller->m_bLatchMode)
    {
        switch (event->type)
        {
        case SDL_EVENT_KEY_DOWN:
            {
                controller->m_keybufferCurr[event->key.scancode] = true;
                controller->m_keybufferUpNextFrame[event->key.scancode] = false;
                controller->m_keybufferFrameCnt[event->key.scancode] = 0;
            }
            break;

        case SDL_EVENT_KEY_UP:
            {
                if (controller->m_keybufferCurr[event->key.scancode])
                {
                    if (controller->m_keybufferFrameCnt[event->key.scancode] > 0)
                        controller->m_keybufferCurr[event->key.scancode] = false;
                    else
                        controller->m_keybufferUpNextFrame[event->key.scancode] = true;
                };
            }
            break;

        default:
            break;
        };
    };

    return true;
};


CWebKeyboardController::CWebKeyboardController(int32 iController)
: m_aDigitalFixedInfo()
, m_aDigitalInfo()
, m_aAnalogInfo()
, m_numUsedFixedDigital(0)
, m_keybufferCurr(nullptr)
, m_keybufferPrev(nullptr)
, m_keybufferSize(0)
, m_bLatchMode(IsLatchModeRequired())
, m_keybufferUpNextFrame(nullptr)
, m_keybufferFrameCnt(nullptr)
{
    std::memset(m_aDigitalInfo, 0, sizeof(m_aDigitalInfo));
    std::memset(m_aDigitalFixedInfo, 0, sizeof(m_aDigitalFixedInfo));
    std::memset(m_aAnalogInfo, 0, sizeof(m_aAnalogInfo));

    m_info.m_iPhysicalPort = iController;

    SDL_AddEventWatch(&SDLEventHandler, this);
};


CWebKeyboardController::~CWebKeyboardController(void)
{
    SDL_RemoveEventWatch(&SDLEventHandler, this);

    if (m_keybufferCurr)
    {
        if (m_keybufferFrameCnt)
        {
            delete[] m_keybufferFrameCnt;
            m_keybufferFrameCnt = 0;
        };

        if (m_keybufferUpNextFrame)
        {
            delete[] m_keybufferUpNextFrame;
            m_keybufferUpNextFrame = nullptr;
        };

        if (m_keybufferCurr)
        {
            delete[] m_keybufferCurr;
            m_keybufferCurr = nullptr;
        };

        if (m_keybufferPrev)
        {
            delete[] m_keybufferPrev;
            m_keybufferPrev = nullptr;
        };

        m_keybufferSize = 0;
    };    
};


void CWebKeyboardController::Close(void)
{

};


void CWebKeyboardController::Update(void)
{
    Clear();

    m_info.m_eState = CController::STATE_CONNECT;

    if (m_bLatchMode)
    {
        for (int32 i = 0; i < m_keybufferSize; ++i)
        {
            if (m_keybufferUpNextFrame[i] && (m_keybufferFrameCnt[i] != 0))
            {
                m_keybufferUpNextFrame[i] = false;
                m_keybufferCurr[i] = false;
            };

            ++m_keybufferFrameCnt[i];
        };
    }
    else
    {
        std::memcpy(m_keybufferPrev, m_keybufferCurr, sizeof(*m_keybuffer) * m_keybufferSize);

        SDL_PumpEvents();

        int32 numKeys = 0;
        m_keybuffer = SDL_GetKeyboardState(&numKeys);
        if (m_keybuffer)
            std::memcpy(m_keybufferCurr, m_keybuffer, sizeof(*m_keybuffer) * Min(m_keybufferSize, numKeys));
        else
            std::memset(m_keybufferCurr, 0, sizeof(*m_keybuffer) * m_keybufferSize);
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


bool CWebKeyboardController::Start(void)
{
    int32 numKeys = 0;
    m_keybuffer = SDL_GetKeyboardState(&numKeys);

    if (!m_keybuffer || (numKeys == 0))
        return false;

    m_keybufferPrev = new bool[numKeys];
    std::memset(m_keybufferPrev, 0, sizeof(*m_keybuffer) * numKeys);

    m_keybufferCurr = new bool[numKeys];
    std::memset(m_keybufferCurr, 0, sizeof(*m_keybuffer) * numKeys);

    m_keybufferSize = numKeys;

    m_keybufferUpNextFrame = new bool[numKeys];
    std::memset(m_keybufferUpNextFrame, 0, sizeof(*m_keybuffer) * numKeys);

    m_keybufferFrameCnt = new int32[numKeys];
    std::memset(m_keybufferFrameCnt, 0, sizeof(*m_keybufferFrameCnt) * numKeys);

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


void CWebKeyboardController::MapDigital(uint32 btn, int32 sdlScancode)
{
    for (int32 i = 0; i < COUNT_OF(m_aDigitalInfo); ++i)
    {
        if (m_aDigitalInfo[i].m_digital == btn)
        {
            m_aDigitalInfo[i].m_bEnabled = (sdlScancode >= 0);
            m_aDigitalInfo[i].m_scancode = sdlScancode;
            return;
        };
    };
};


void CWebKeyboardController::MapDigitalFixed(uint32 btn, int32 sdlScancode)
{
    ASSERT(m_numUsedFixedDigital < FIXED_DIGITAL_MAX);
    
    m_aDigitalFixedInfo[m_numUsedFixedDigital].m_bEnabled = true;
    m_aDigitalFixedInfo[m_numUsedFixedDigital].m_digital = btn;
    m_aDigitalFixedInfo[m_numUsedFixedDigital].m_scancode = sdlScancode;
    
    ++m_numUsedFixedDigital;
};


void CWebKeyboardController::MapAnalog(CController::ANALOG analog, int32 sdlScancodeX, int32 sdlScancodeY)
{
    ASSERT(sdlScancodeX >= 0);
    ASSERT(sdlScancodeX < m_keybufferSize);

    ASSERT(sdlScancodeY >= 0);
    ASSERT(sdlScancodeY < m_keybufferSize);

    ASSERT(analog >= 0);
    ASSERT(analog < CController::ANALOG_NUM);

    for (int32 i = 0; i < COUNT_OF(m_aAnalogInfo); ++i)
    {
        if (m_aAnalogInfo[i].m_analog == analog)
        {
            m_aAnalogInfo[i].m_bEnabled = (sdlScancodeX >= 0 && sdlScancodeY >= 0);
            m_aAnalogInfo[i].m_scancodeX = sdlScancodeX;
            m_aAnalogInfo[i].m_scancodeY = sdlScancodeY;
        };
    };
};


bool CWebKeyboardController::IsKeyDown(int32 sdlScancode) const
{
    ASSERT(sdlScancode >= 0);
    ASSERT(sdlScancode < m_keybufferSize);

    return (m_keybufferCurr[sdlScancode]);
};


bool CWebKeyboardController::IsKeyTrigger(int32 sdlScancode) const
{
    ASSERT(sdlScancode >= 0);
    ASSERT(sdlScancode < m_keybufferSize);

    if (m_bLatchMode)
        return (m_keybufferCurr[sdlScancode] && (m_keybufferFrameCnt[sdlScancode] == 1));

    return ((!m_keybufferPrev[sdlScancode]) &&
            (m_keybufferCurr[sdlScancode]));
};


bool CWebKeyboardController::IsKeyNotFixed(int32 sdlScancode) const
{
    for (int32 i = 0; i < m_numUsedFixedDigital; ++i)
    {
        if (m_aDigitalFixedInfo[i].m_scancode == sdlScancode)
            return false;
    };

    return true;
};


int32 CWebKeyboardController::GetDownKey(void) const
{
    for (int32 i = 0; i < m_keybufferSize; ++i)
    {
        if (IsKeyDown(i))
            return i;
    };

    return -1;
};


//
// *********************************************************************************
//


/*static*/ bool CWebJoystickController::SDLEventHandler(void* userdata, SDL_Event* event)
{
    CWebJoystickController* controller =
        reinterpret_cast<CWebJoystickController*>(userdata);

    if (controller->m_bLatchMode)
    {
        switch (event->type)
        {
        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
            break;

        case SDL_EVENT_JOYSTICK_BUTTON_UP:
            break;

        default:
            break;
        };
    };

    return true;
};


CWebJoystickController::CWebJoystickController(int32 iController)
: m_iPort(iController)
, m_pJoystick(nullptr)
, m_guid({})
, m_numButtons(0)
, m_numAxes(0)
, m_numHats(0)
, m_vibAvailable(false)
, m_bLatchMode(IsLatchModeRequired())
#ifdef _DEBUG
, m_vendorId(0)
, m_productId(0)
, m_bAxisCorrection(false)
#endif /* _DEBUG */
{
    m_info.m_iPhysicalPort = iController;
#ifdef _DEBUG
    m_bAxisCorrection = CConfigure::CheckArg("padfix");
#endif /* _DEBUG */

    SDL_AddEventWatch(&SDLEventHandler, this);
};


CWebJoystickController::~CWebJoystickController(void)
{
    SDL_RemoveEventWatch(&SDLEventHandler, this);

    if (m_pJoystick)
    {
        SDL_CloseJoystick(m_pJoystick);
        m_pJoystick = nullptr;
    };
};


void CWebJoystickController::Close(void)
{
    delete this;
};


void CWebJoystickController::Update(void)
{
    Clear();

    if (!m_pJoystick)
    {
        m_info.m_eState = CController::STATE_UNCONNECT;
        IPhysicalController::Update();
        return;
    };

    SDL_Joystick* pJoystick = m_pJoystick;

    m_guid = SDL_GetJoystickGUID(pJoystick);
    m_numAxes = SDL_GetNumJoystickAxes(pJoystick);
    m_numHats = SDL_GetNumJoystickHats(pJoystick);
    m_numButtons = SDL_GetNumJoystickButtons(pJoystick);
    m_vibAvailable = false;

    m_info.m_eState = (SDL_JoystickConnected(pJoystick) ? CController::STATE_CONNECT :
                                                          CController::STATE_UNCONNECT);

    if (m_info.m_eState == CController::STATE_UNCONNECT)
    {
        IPhysicalController::Update();
        return;
    };

    uint32 digital = 0;

    if (m_numHats > 0)
    {
        for (int32 i = 0; i < m_numHats; ++i)
        {
            uint8 hat = SDL_GetJoystickHat(pJoystick, i);

            if (hat & SDL_HAT_UP)
                digital |= CController::DIGITAL_LUP;

            if (hat & SDL_HAT_DOWN)
                digital |= CController::DIGITAL_LDOWN;

            if (hat & SDL_HAT_LEFT)
                digital |= CController::DIGITAL_LLEFT;

            if (hat & SDL_HAT_RIGHT)
                digital |= CController::DIGITAL_LRIGHT;
        };
    };

    static const uint32 s_aJoyBtn2DigitalTbl[] =
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

    if (m_numButtons > 0)
    {
        int32 numButtons = Min(COUNT_OF(s_aJoyBtn2DigitalTbl), m_numButtons);
        for (int32 i = 0; i < numButtons; ++i)
        {
            if (SDL_GetJoystickButton(pJoystick, i))
                digital |= s_aJoyBtn2DigitalTbl[i];
        };
    };

    int16 aAnalog[32] = {};

    if (m_numAxes > 0)
    {
        int32 numAxes = Min(COUNT_OF(aAnalog), m_numAxes);
        for (int32 i = 0; i < numAxes; ++i)
            aAnalog[i] = SDL_GetJoystickAxis(pJoystick, i);
    };

    m_info.m_digital = digital;

#ifdef _DEBUG
    if (m_bAxisCorrection)
    {
        //
        //	VID_0810&PID_0001 gamepad case:
        // 		- correcting right stick axis values
        //
        if ((m_vendorId  == 0x0810) &&
            (m_productId == 0x0001))
        {
            aAnalog[3] = aAnalog[2];
            aAnalog[2] = aAnalog[5];
        };
    };
#endif /* _DEBUG */	

	const int16 DEADZONE = static_cast<int16>(static_cast<float>(TYPEDEF::SINT16_MAX) * 0.25f);

    m_info.m_aAnalog[CController::ANALOG_LSTICK_X] = ClampValue(aAnalog[0], DEADZONE);
    m_info.m_aAnalog[CController::ANALOG_LSTICK_Y] = ClampValue(-1 - aAnalog[1], DEADZONE);
    m_info.m_aAnalog[CController::ANALOG_RSTICK_X] = ClampValue(aAnalog[2], DEADZONE);
    m_info.m_aAnalog[CController::ANALOG_RSTICK_Y] = ClampValue(-1 - aAnalog[3], DEADZONE);
    m_info.m_aAnalog[CController::ANALOG_RUP]      = (digital & CController::DIGITAL_RUP    ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_RDOWN]    = (digital & CController::DIGITAL_RDOWN  ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_RLEFT]    = (digital & CController::DIGITAL_RLEFT  ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_RRIGHT]   = (digital & CController::DIGITAL_RRIGHT ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_L1]       = (digital & CController::DIGITAL_L1     ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_L2]       = (digital & CController::DIGITAL_L2     ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_L3]       = (digital & CController::DIGITAL_L3     ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_R1]       = (digital & CController::DIGITAL_R1     ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_R2]       = (digital & CController::DIGITAL_R2     ? TYPEDEF::SINT16_MAX : 0);
    m_info.m_aAnalog[CController::ANALOG_R3]       = (digital & CController::DIGITAL_R3     ? TYPEDEF::SINT16_MAX : 0);

    if (m_vibAvailable)
    {
        // TODO vibration
    };

    IPhysicalController::Update();
};


bool CWebJoystickController::Start(SDL_JoystickID joystickID)
{
    if (joystickID == 0)
        return true;

    SDL_Joystick* pJoystick = SDL_OpenJoystick(joystickID);
    if (!pJoystick)
    {
        OUTPUT("SDL_OpenJoystick failed: %s\n", SDL_GetError());
        return false;
    };

    SDL_UpdateJoysticks();

    m_pJoystick = pJoystick;
    m_guid = SDL_GetJoystickGUID(pJoystick);
    m_numAxes = SDL_GetNumJoystickAxes(pJoystick);
    m_numHats = SDL_GetNumJoystickHats(pJoystick);
    m_numButtons = SDL_GetNumJoystickButtons(pJoystick);
    m_vibAvailable = false;    // TODO vibration

#ifdef _DEBUG
    uint16 version = 0;
    uint16 crc16 = 0;
    SDL_GetJoystickGUIDInfo(m_guid, &m_vendorId, &m_productId, &version, &crc16);    
#endif /* _DEBUG */

    return false;
};


void CWebJoystickController::Connect(SDL_JoystickID joystickID)
{
    Start(joystickID);
    DbgNoteConnection(false);
};


void CWebJoystickController::Reconnect(SDL_JoystickID joystickID)
{
    if (m_pJoystick)
    {
        SDL_CloseJoystick(m_pJoystick);
        m_pJoystick = nullptr;
    };

    Start(joystickID);
    DbgNoteConnection(true);
};


int32 CWebJoystickController::ClampValue(int32 value, int32 clamp) const
{
    return ((value < clamp) && (value > -clamp)) ? 0 : value;
};


bool CWebJoystickController::CompareGUID(SDL_GUID guid) const
{
    if (!std::memcmp(&m_guid, &guid, sizeof(m_guid)))
        return true;

    return false;
};


bool CWebJoystickController::IsFree(void) const
{
    return (m_pJoystick == nullptr);
};


void CWebJoystickController::DbgNoteConnection(bool bReconnect) const
{
#ifdef _DEBUG    
    OUTPUT("JOYSTICK %s!\n", bReconnect ? "RECONNECT" : "CONNECT");
    OUTPUT("    VID&PID: 0x%" PRIx16 " -- 0x%" PRIx16 "\n", m_vendorId, m_productId);
    OUTPUT("    Port: %" PRId32 "\n", m_iPort);
    OUTPUT("    Num hats: %" PRId32 "\n", m_numHats);
    OUTPUT("    Num axes: %" PRId32 "\n", m_numAxes);
    OUTPUT("    Num btns: %" PRId32 "\n", m_numButtons);
    OUTPUT("    Vibration: %s\n", m_vibAvailable ? "YES" : "NO");
#endif /* _DEBUG */
};


//
// *********************************************************************************
//


CWebTouchController::CWebTouchController(int32 iController)
{
    m_info.m_iPhysicalPort = iController;
};


CWebTouchController::~CWebTouchController(void)
{
    ;
};


void CWebTouchController::Close(void)
{
    ;
};


void CWebTouchController::Update(void)
{
    IPhysicalController::Update();
};


//
// *********************************************************************************
//


static CWebKeyboardController* s_pWebKeyboardController = nullptr;
static CWebTouchController* s_pWebTouchController = nullptr;


static inline CWebKeyboardController& KeyboardController(void)
{
    ASSERT(s_pWebKeyboardController);
    return *s_pWebKeyboardController;
};


static inline CWebTouchController& TouchController(void)
{
    ASSERT(s_pWebTouchController);
    return *s_pWebTouchController;
};


static inline IPhysicalController* CreatePrimaryController(int32 port)
{
    if (CWebSpecific::IsMobilePlatform())
    {
        s_pWebTouchController = new CWebTouchController(port);
        return s_pWebTouchController;
    }
    else
    {
        s_pWebKeyboardController = new CWebKeyboardController(port);
        if (s_pWebKeyboardController)
        {
            if (s_pWebKeyboardController->Start())
                return s_pWebKeyboardController;
            
            delete s_pWebKeyboardController;
            s_pWebKeyboardController = nullptr;
        };
    };

    return nullptr;
};


static inline void DestroyPrimaryController(void)
{
    if (s_pWebKeyboardController)
    {
        delete s_pWebKeyboardController;
        s_pWebKeyboardController = nullptr;
    };

    if (s_pWebTouchController)
    {
        delete s_pWebTouchController;
        s_pWebTouchController = nullptr;
    };
};


static inline IPhysicalController* GetPrimaryController(void)
{
    if (CWebSpecific::IsMobilePlatform())
        return s_pWebTouchController;

    return s_pWebKeyboardController;
};


static inline bool IsPrimaryController(int32 iController)
{
    IPhysicalController::INFO* pInfo = nullptr;

    if (CWebSpecific::IsMobilePlatform())
        pInfo = &s_pWebTouchController->Info();
    else
        pInfo = &s_pWebKeyboardController->Info();

    return (pInfo->m_iPhysicalPort == iController);
};


/*static*/ int32 CWebPhysicalController::PHYSICALCONTROLLER_MAX = 0;


/*static*/ bool CWebPhysicalController::Initialize(void)
{
    // create primary at port 0
    if (CreatePrimaryController(0))
    {
        // count primary controller
        PHYSICALCONTROLLER_MAX += 1;

        // pre count joystick controllers
        PHYSICALCONTROLLER_MAX += 3;

        return true;
    };

    return false;
};


/*static*/ void CWebPhysicalController::Terminate(void)
{
    DestroyPrimaryController();

    PHYSICALCONTROLLER_MAX = 0;
};


/*static*/ bool CWebPhysicalController::OnJoystickAdd(uint32 joystickID)
{
    SDL_GUID guid = SDL_GetJoystickGUIDForID(joystickID);

    for (int32 i = 0; i < s_numJoystickController; ++i)
    {
        CWebJoystickController* pWebJoystickController = s_apJoystickController[i];
        if (pWebJoystickController->CompareGUID(guid))
        {
            pWebJoystickController->Reconnect(joystickID);
            return true;
        };
    };

    for (int32 i = 0; i < s_numJoystickController; ++i)
    {
        CWebJoystickController* pWebJoystickController = s_apJoystickController[i];
        if (pWebJoystickController->IsFree())
        {
            pWebJoystickController->Connect(joystickID);
            return true;
        };
    };

    return false;
};


/*static*/ IPhysicalController* CWebPhysicalController::Open(int32 iController,
                                                             uint32 joystickID /*= 0*/)
{
    IPhysicalController* pPhysicalController = nullptr;
    
    if (IsPrimaryController(iController))
    {
        pPhysicalController = GetPrimaryController();
    }
    else
    {
        CWebJoystickController* pWebJoystickController = new CWebJoystickController(iController);
        if (pWebJoystickController)
        {
            if (pWebJoystickController->Start(joystickID))
            {
                s_apJoystickController[s_numJoystickController++] = pWebJoystickController;
                pPhysicalController = pWebJoystickController;
            }
            else
            {
                delete pWebJoystickController;
                pWebJoystickController = nullptr;
            };
        };
    };

    return pPhysicalController;
};


/*static*/ void CWebPhysicalController::MapDigital(uint32 btn, int32 sdlScancode)
{
    if (s_pWebKeyboardController)
        KeyboardController().MapDigital(btn, sdlScancode);
};


/*static*/ void CWebPhysicalController::MapDigitalFixed(uint32 btn, int32 sdlScancode)
{
    if (s_pWebKeyboardController)
        KeyboardController().MapDigitalFixed(btn, sdlScancode);
};


/*static*/ void CWebPhysicalController::MapAnalog(CController::ANALOG analog, int32 sdlScancodeX, int32 sdlScancodeY)
{
    if (s_pWebKeyboardController)
        KeyboardController().MapAnalog(analog, sdlScancodeX, sdlScancodeY);
};


/*static*/ bool CWebPhysicalController::IsKeyDown(int32 sdlScancode)
{
    if (s_pWebKeyboardController)
        return KeyboardController().IsKeyDown(sdlScancode);
    
    return false;
};


/*static*/ bool CWebPhysicalController::IsKeyTrigger(int32 sdlScancode)
{
    if (s_pWebKeyboardController)
        return KeyboardController().IsKeyTrigger(sdlScancode);

    return false;
};


/*static*/ bool CWebPhysicalController::IsKeyNotFixed(int32 sdlScancode)
{
    if (s_pWebKeyboardController)
        return KeyboardController().IsKeyNotFixed(sdlScancode);
    
    return false;
};


/*static*/ int32 CWebPhysicalController::GetDownKey(void)
{
    if (s_pWebKeyboardController)
        return KeyboardController().GetDownKey();
    
    return -1;
};


/*static*/ int32 CWebPhysicalController::GetPort(void)
{
    if (s_pWebKeyboardController)
        return KeyboardController().Info().m_iPhysicalPort;
    
    return -1;
};


/*static*/ int32 CWebPhysicalController::GetTouchPort(void)
{
    if (s_pWebTouchController)
        return TouchController().Info().m_iPhysicalPort;

    return -1;
};