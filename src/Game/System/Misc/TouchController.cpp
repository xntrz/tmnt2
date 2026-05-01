#include "TouchController.hpp"

#include "Game/System/2d/GameFont.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Screen.hpp"

#include "System/Common/InputsDevice.hpp"
#if defined(TARGET_WEB)

#include "System/Web/WebSpecific.hpp"


#define SPRITE_VIRTUAL_SCREEN_STACK_SIZE (4)


static Rt2dBBox s_aGameVirtualScreenStack[SPRITE_VIRTUAL_SCREEN_STACK_SIZE];
static Rt2dBBox s_aRt2dVirtualScreenStack[SPRITE_VIRTUAL_SCREEN_STACK_SIZE];
static int32 s_iVirtualScreenStackDepth = 0;


static void PushVirtualScreen(void)
{
    ASSERT(s_iVirtualScreenStackDepth < SPRITE_VIRTUAL_SCREEN_STACK_SIZE);

    Rt2dBBox* game = &s_aGameVirtualScreenStack[s_iVirtualScreenStackDepth];
    Rt2dBBox* rt2d = &s_aRt2dVirtualScreenStack[s_iVirtualScreenStackDepth];

    ++s_iVirtualScreenStackDepth;

    game->x = CSprite::m_fVirtualScreenX;
    game->y = CSprite::m_fVirtualScreenY;
    game->w = CSprite::m_fVirtualScreenW;
    game->h = CSprite::m_fVirtualScreenH;

    Rt2dDeviceGetMetric(&rt2d->x, &rt2d->y, &rt2d->w, &rt2d->h);
};


static void PopVirtualScreen(void)
{
    ASSERT(s_iVirtualScreenStackDepth > 0);

    Rt2dBBox* game = &s_aGameVirtualScreenStack[s_iVirtualScreenStackDepth - 1];
    Rt2dBBox* rt2d = &s_aRt2dVirtualScreenStack[s_iVirtualScreenStackDepth - 1];

    --s_iVirtualScreenStackDepth;

    Rt2dDeviceSetMetric(rt2d->x, rt2d->y, rt2d->w, rt2d->h);

    CSprite::m_fVirtualScreenX = game->x;
    CSprite::m_fVirtualScreenY = game->y;
    CSprite::m_fVirtualScreenW = game->w;
    CSprite::m_fVirtualScreenH = game->h;
};


static void SetVirtualScreen(float x, float y, float w, float h)
{
    Rt2dDeviceSetMetric(x, y, w, h);
    
    CSprite::m_fVirtualScreenX = x;
    CSprite::m_fVirtualScreenY = y;
    CSprite::m_fVirtualScreenW = w;
    CSprite::m_fVirtualScreenH = h;
};


//static inline bool
//IsRectContainPoint(const RwV2d* pos,
//                   const RwV2d* size,
//                   const RwV2d* point)
//{
//    return (point->x >= pos->x)
//        && (point->y >= pos->y)
//        && (point->x <= (pos->x + size->x))
//        && (point->y <= (pos->y + size->y));
//};


int32 ClampValue(int32 value, int32 clamp)
{
    return ((value < clamp) && (value > -clamp)) ? 0 : value;
};


static inline void GetSDLWindowSize(float* w, float* h)
{
    int32 wi = 0;
    int32 hi = 0;
    SDL_GetWindowSize(CWebSpecific::m_pWindow, &wi, &hi);

    *w = static_cast<float>(wi);
    *h = static_cast<float>(hi);
};

bool IsRectContainPoint(const RwV2d* pos,
    const RwV2d* size,
    const RwV2d* point)
{
    bool isInsideX = (point->x >= pos->x) && (point->x <= pos->x + size->x);
    bool isInsideY = (point->y >= pos->y) && (point->y <= pos->y + size->y);
    return isInsideX && isInsideY;
};


static bool SDLCALL
TouchEventFilter(void* userdata, SDL_Event* event)
{
    CTouchControllerProcess* pProcess =
        reinterpret_cast<CTouchControllerProcess*>(userdata);

    return pProcess->OnEvent(event);
};


/*static*/ const float CTouchControllerProcess::STICK_RADIUS = 64.0f;
/*static*/ const float CTouchControllerProcess::STICK_THUMB_RADIUS_RATIO = 0.25f;
/*static*/ const RwV2d CTouchControllerProcess::STICK_AREA_SIZE = { 320.0f, 256.0f };
/*static*/ const RwV2d CTouchControllerProcess::STICK_AREA_ANCHOR = {  -320.0f + (320.0f * 0.5f),
                                                                        240.0f - (256.0f * 0.5f) };

/*static*/ const RwV2d CTouchControllerProcess::BUTTON_AREA_ANCHOR = { 320.0f - (((32.0f * 2.0f) * 0.5f) * 3),
                                                                       240.0f - (((32.0f * 2.0f) * 0.5f) * 3), };
/*static*/ const float CTouchControllerProcess::BUTTON_RADIUS = 42.0f;


/*static*/ CProcess* CTouchControllerProcess::Instance(void)
{
    return new CTouchControllerProcess;
};


/*static*/ void CTouchControllerProcess::Initialize(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_TOUCHCONTROLLER, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CTouchControllerProcess::Terminate(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_TOUCHCONTROLLER, PROCESSTYPES::MAIL::TYPE_DETACH);
};


CTouchControllerProcess::CTouchControllerProcess(void)
: m_overlayAlpha(96)
, m_sprite()
, m_numTouchDevices(0)
, m_aTouchDevices(nullptr)
, m_aTouch()
, m_aButton()
, m_stick()
#ifdef _DEBUG
, m_font()
#endif /* _DEBUG */
{
    ;
};


CTouchControllerProcess::~CTouchControllerProcess(void)
{
    ;
};


bool CTouchControllerProcess::Attach(void)
{
    m_aTouchDevices = SDL_GetTouchDevices(&m_numTouchDevices);

    SDL_SetEventFilter(&TouchEventFilter, this);

    std::memset(m_aTouch, 0, sizeof(m_aTouch));
    std::memset(m_aButton, 0, sizeof(m_aButton));
    std::memset(&m_stick, 0, sizeof(m_stick));

    RwV2d digitalIconSize = { 32.0f, 32.0f };

    BUTTON aButton[] =
    {
        { Math::VECTOR2_ZERO, { 0.0f,  (BUTTON_RADIUS * 2.0f) }, nullptr, false, CController::DIGITAL_RUP    },
        { Math::VECTOR2_ZERO, { 0.0f, -(BUTTON_RADIUS * 2.0f) }, nullptr, false, CController::DIGITAL_RDOWN  },
        { Math::VECTOR2_ZERO, { -(BUTTON_RADIUS * 2.0f), 0.0f }, nullptr, false, CController::DIGITAL_RLEFT  },
        { Math::VECTOR2_ZERO, {  (BUTTON_RADIUS * 2.0f), 0.0f }, nullptr, false, CController::DIGITAL_RRIGHT },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_SELECT },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_START  },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_L1     },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_L2     },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_L3     },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_R1     },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_R2     },
        { Math::VECTOR2_ZERO, digitalIconSize, nullptr, false, CController::DIGITAL_R3     },
    };

    static_assert(sizeof(aButton) <= sizeof(m_aButton),
                 "sizeof(src) greater than sizeof(dst)");

    std::memcpy(m_aButton, aButton, sizeof(aButton));

    return true;
};


void CTouchControllerProcess::Detach(void)
{
    if (m_aTouchDevices)
    {
        SDL_free(m_aTouchDevices);
        m_aTouchDevices = nullptr;

        m_numTouchDevices = 0;
    };
};


void CTouchControllerProcess::Move(void)
{
    UpdateTouchStartPos();
    UpdateStickHit();
    UpdateButtonHit();
    UpdatePhysicalController();
};


void CTouchControllerProcess::Draw(void) const
{
    DrawStickOverlay();
    DrawButtonOverlay();
#ifdef _DEBUG
    //DrawFingerInfo();
#endif /* _DEBUG */
};


void CTouchControllerProcess::UpdateTouchStartPos(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        if (!m_aTouch[i].bDown)
            continue;

        float x = m_aTouch[i].fWinStartPosX;
        float y = m_aTouch[i].fWinStartPosY;

        WinPosToVirtualScreenPos(&x, &y);

        m_aTouch[i].fStartPosX = x;
        m_aTouch[i].fStartPosY = y;
    };
};


void CTouchControllerProcess::UpdateButtonHit(void)
{
    for (int32 i = 0; i < 4; ++i)
    {
        BUTTON* pButton = &m_aButton[i];

        if (!pButton->digital)
            continue;

        pButton->bIsHold = false;
    };

    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        if (!m_aTouch[i].bDown)
            continue;

        for (int32 j = 0; j < 4; ++j)
        {
            BUTTON* pButton = &m_aButton[j];

            if (!pButton->digital)
                continue;

            RwV2d buttonPos = { (BUTTON_AREA_ANCHOR.x + pButton->vecOffset.x) - ((BUTTON_RADIUS * 2.0f) * 0.5f),
                                (BUTTON_AREA_ANCHOR.y + pButton->vecOffset.y) - ((BUTTON_RADIUS * 2.0f) * 0.5f) };

            RwV2d buttonSize = { (BUTTON_RADIUS * 2.0f),
                                 (BUTTON_RADIUS * 2.0f), };


            RwV2d touchPoint = { m_aTouch[i].fCurrentPosX,
                                 m_aTouch[i].fCurrentPosY };

            buttonPos.x -= CSprite::m_fVirtualScreenX;
            buttonPos.y -= CSprite::m_fVirtualScreenY;

            touchPoint.x -= CSprite::m_fVirtualScreenX;
            touchPoint.y -= CSprite::m_fVirtualScreenY;
            
            if (IsRectContainPoint(&buttonPos, &buttonSize, &touchPoint))
                pButton->bIsHold = true;
        };
    };
};


void CTouchControllerProcess::UpdateStickHit(void)
{
    if (!m_stick.pTouch)
        return;

    m_stick.vecThumbPos = { m_stick.pTouch->fCurrentPosX,
                            m_stick.pTouch->fCurrentPosY };

    RwV2d vecThumbPos = m_stick.vecThumbPos;
    RwV2d vecPos = m_stick.vecPos;

    RwV2d vecDist = { vecThumbPos.x - vecPos.x,
                      vecThumbPos.y - vecPos.y, };

    float fDistSQ = (vecDist.x * vecDist.x +
                     vecDist.y * vecDist.y);
    
    float fRadiusSQ = m_stick.fRadius * m_stick.fRadius;

    if (fDistSQ >= fRadiusSQ)
    {
        float fDist = std::sqrt(fDistSQ);

        float nx = (vecDist.x / fDist) * m_stick.fRadius;
        float ny = (vecDist.y / fDist) * m_stick.fRadius;

        m_stick.vecThumbPos = { vecPos.x + nx,
                                vecPos.y + ny };

        m_stick.lx = nx / m_stick.fRadius;
        m_stick.ly = ny / m_stick.fRadius;
    }
    else
    {
        m_stick.lx = vecDist.x / m_stick.fRadius;
        m_stick.ly = vecDist.y / m_stick.fRadius;
    };
};


void CTouchControllerProcess::DrawButtonOverlay(void) const
{
    CSprite::PushRenderStates();
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);

    for (int32 i = 0; i < 4; ++i)
    {
        const BUTTON* pButton = &m_aButton[i];

        if (!pButton->digital)
            continue;

        //RwUInt8 alpha = (pButton->) static_cast<RwUInt8>(static_cast<float>(m_overlayAlpha) * 0.5f)

        RwUInt8 a = (pButton->bIsHold ? m_overlayAlpha / 2 : m_overlayAlpha);

        m_sprite.Move(BUTTON_AREA_ANCHOR.x + pButton->vecOffset.x,
                      BUTTON_AREA_ANCHOR.y + pButton->vecOffset.y);
        m_sprite.Resize(BUTTON_RADIUS * 2.0f, BUTTON_RADIUS * 2.0f);
        m_sprite.SetRGBA({ 64, 64, 64, a });

        if (pButton->pTexture)
            m_sprite.SetTexture(pButton->pTexture);
        else
            RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, NULL);
        
        m_sprite.Draw();

        if (!pButton->pTexture)
            RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
    };
    
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    CSprite::PopRenderStates();
};


void CTouchControllerProcess::DrawStickOverlay(void) const
{
    CSprite::PushRenderStates();
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, NULL);

//    m_sprite.Move(STICK_AREA_ANCHOR.x, STICK_AREA_ANCHOR.y);
//    m_sprite.Resize(STICK_AREA_SIZE.x, STICK_AREA_SIZE.y);
//    m_sprite.SetRGBA({ 255, 0, 0, m_overlayAlpha });
//    m_sprite.Draw();
//
//    m_sprite.Move(-160.0f, 120.0f);
//    m_sprite.Resize(32.0f, 32.0f);
//    m_sprite.SetRGBA({ 255, 255, 0, m_overlayAlpha });
//    m_sprite.Draw();

    if (m_stick.pTouch)
    {
        RwV2d vecSize = { m_stick.fRadius * 2.0f,
                          m_stick.fRadius * 2.0f };

        m_sprite.Move(m_stick.vecPos.x, m_stick.vecPos.y);
        m_sprite.Resize(vecSize.x, vecSize.y);
        m_sprite.SetRGBA({ 64, 64, 64, m_overlayAlpha });
        m_sprite.Draw();

        RwUInt8 a = m_overlayAlpha / 2;
        RwUInt8 c = 64 * 2;

        m_sprite.Move(m_stick.vecThumbPos.x, m_stick.vecThumbPos.y);
        m_sprite.Resize(vecSize.x * STICK_THUMB_RADIUS_RATIO,
            vecSize.y * STICK_THUMB_RADIUS_RATIO);
        m_sprite.SetRGBA({ c, c, c , a });
        m_sprite.Draw();
    };

    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    CSprite::PopRenderStates();
};


void CTouchControllerProcess::DrawFingerInfo(void) const
{
#ifdef _DEBUG
    m_font.Color({ 0xFF, 0xFF, 0xFF, 0xFF });
    m_font.Background({ 0x00, 0x00, 0x00, 0xFF });
    m_font.Position(400, 250);
    m_font.SetAutoStep(0, 20);

    char szTextBuffer[256];
    szTextBuffer[0] = '\0';

    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        const TOUCH* pFingerInfo = &m_aTouch[i];

        if (!pFingerInfo->bDown)
            continue;

        std::sprintf(szTextBuffer, "%d -- %.2f %.2f -- %.2f %.2f",
                     pFingerInfo->id,
                     pFingerInfo->fCurrentPosX, pFingerInfo->fCurrentPosY,
                     pFingerInfo->fStartPosX, pFingerInfo->fStartPosY);

        m_font.Print(szTextBuffer);
    };
#endif /* _DEBUG */
};


void CTouchControllerProcess::UpdatePhysicalController(void)
{
    int32 port = CWebSpecific::GetTouchPort();
    if (port == -1)
        return;

    int32 controller = CController::GetController(port);
    if (controller == -1)
        return;

    IPhysicalController& touch = CInputsDevice::Instance().ControllerResource(controller);
    touch.Clear();

    IPhysicalController::INFO& info = touch.Info();

    info.m_eState = CController::STATE_CONNECT;

    uint32 digital = 0;

    for (int32 i = 0; i < 4; ++i)
    {
        if (m_aButton[i].bIsHold)
            digital |= m_aButton[i].digital;
    };

    if (m_stick.pTouch)
    {
        float lx = (m_stick.lx * static_cast<float>(TYPEDEF::SINT16_MAX));
        float ly = (m_stick.ly * static_cast<float>(TYPEDEF::SINT16_MAX));

        const float DEADZONE = (static_cast<float>(TYPEDEF::SINT16_MAX) * 0.25f);

        info.m_aAnalog[CController::ANALOG_LSTICK_X]    = static_cast<int16>(ClampValue(lx, DEADZONE));
        info.m_aAnalog[CController::ANALOG_LSTICK_Y]    = static_cast<int16>(ClampValue(-1 - ly, DEADZONE));
        info.m_aAnalog[CController::ANALOG_RSTICK_X]    = 0;
        info.m_aAnalog[CController::ANALOG_RSTICK_Y]    = 0;
        info.m_aAnalog[CController::ANALOG_RUP]         = (digital & CController::DIGITAL_RUP    ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_RDOWN]       = (digital & CController::DIGITAL_RDOWN  ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_RLEFT]       = (digital & CController::DIGITAL_RLEFT  ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_RRIGHT]      = (digital & CController::DIGITAL_RRIGHT ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_L1]          = (digital & CController::DIGITAL_L1     ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_L2]          = (digital & CController::DIGITAL_L2     ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_L3]          = (digital & CController::DIGITAL_L3     ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_R1]          = (digital & CController::DIGITAL_R1     ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_R2]          = (digital & CController::DIGITAL_R2     ? TYPEDEF::SINT16_MAX : 0);
        info.m_aAnalog[CController::ANALOG_R3]          = (digital & CController::DIGITAL_R3     ? TYPEDEF::SINT16_MAX : 0);
    };

    if (info.m_bVibEnable)
    {
        if (!info.m_bVibrate && (info.m_iVibFrame > 0))
        {
            int32 ms = info.m_iVibFrame * static_cast<int32>(CScreen::Framerate());
            CWebSpecific::VibrateMobile(ms);

            info.m_bVibrate = true;
        }
        else
        {
            if (!--info.m_iVibFrame)
            {
                CWebSpecific::VibrateMobile(0);
                info.m_bVibrate = false;
                info.m_iVibFrame = 0;
                info.m_uVibMax = 0;
            };
        };
    };

    info.m_digital = digital;

    touch.Update();
};


bool CTouchControllerProcess::OnEvent(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_EVENT_FINGER_DOWN:
        OnTouchEvent(TOUCH_EVENT_DOWN, event->tfinger.fingerID, event->tfinger.x, event->tfinger.y);
        break;

    case SDL_EVENT_FINGER_MOTION:
        OnTouchEvent(TOUCH_EVENT_MOTION, event->tfinger.fingerID, event->tfinger.x, event->tfinger.y);
        break;

    case SDL_EVENT_FINGER_UP:
        OnTouchEvent(TOUCH_EVENT_UP, event->tfinger.fingerID, event->tfinger.x, event->tfinger.y);
        break;

    default:
        break;
    };

    return true;
};


void CTouchControllerProcess::OnTouchEvent(TOUCH_EVENT event, SDL_FingerID id, float x, float y)
{
    float w = 0.0f;
    float h = 0.0f;
    GetSDLWindowSize(&w, &h);

    x *= w;
    y *= h;

    switch (event)
    {
    case TOUCH_EVENT_DOWN:
        {
            ASSERT(!IsTouchExist(id));

            for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
            {
                if (!m_aTouch[i].bDown)
                {
                    m_aTouch[i].bDown = true;
                    m_aTouch[i].bTrigger = true;
                    m_aTouch[i].id = id;
                    m_aTouch[i].fWinStartPosX = x;
                    m_aTouch[i].fWinStartPosY = y;

                    WinPosToVirtualScreenPos(&x, &y);

                    m_aTouch[i].fStartPosX = x;
                    m_aTouch[i].fStartPosY = y;
                    m_aTouch[i].fCurrentPosX = x;
                    m_aTouch[i].fCurrentPosY = y;

                    if (IsStickTouch(&m_aTouch[i]))
                    {
                        m_stick.pTouch = &m_aTouch[i];
                        m_stick.vecPos = { m_stick.pTouch->fCurrentPosX,
                                           m_stick.pTouch->fCurrentPosY };

                        m_stick.vecThumbPos = m_stick.vecPos;
                        m_stick.fRadius = STICK_RADIUS;
                    };

                    break;
                };
            };
        }
        break;

    case TOUCH_EVENT_MOTION:
        {
            ASSERT(IsTouchExist(id));

            for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
            {
                if (m_aTouch[i].id == id)
                {
                    ASSERT(m_aTouch[i].bDown);

                    WinPosToVirtualScreenPos(&x, &y);

                    m_aTouch[i].bTrigger = false;
                    m_aTouch[i].fCurrentPosX = x;
                    m_aTouch[i].fCurrentPosY = y;

                    break;
                };
            };
        }
        break;

    case TOUCH_EVENT_UP:
        {
            ASSERT(IsTouchExist(id));

            for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
            {
                if (m_aTouch[i].id == id)
                {
                    ASSERT(m_aTouch[i].bDown);

                    if (&m_aTouch[i] == m_stick.pTouch)
                        m_stick.pTouch = nullptr;

                    m_aTouch[i].bDown = false;
                    m_aTouch[i].bTrigger = false;
                    m_aTouch[i].id = 0;
                    m_aTouch[i].fWinStartPosX = 0.0f;
                    m_aTouch[i].fWinStartPosY = 0.0f;
                    m_aTouch[i].fStartPosX = 0.0f;
                    m_aTouch[i].fStartPosY = 0.0f;
                    m_aTouch[i].fCurrentPosX = 0.0f;
                    m_aTouch[i].fCurrentPosY = 0.0f;

                    break;
                };
            };
        }
        break;

    default:
        break;
    };
};


bool CTouchControllerProcess::IsTouchExist(SDL_FingerID id) const
{
    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        if (m_aTouch[i].id == id)
            return true;
    };

    return false;
};


void CTouchControllerProcess::WinPosToVirtualScreenPos(float* x, float* y) const
{
    float w = 0.0f;
    float h = 0.0f;
    GetSDLWindowSize(&w, &h);

    *x = CSprite::m_fVirtualScreenX + ((CSprite::m_fVirtualScreenW / w) * (*x));
    *y = CSprite::m_fVirtualScreenY + ((CSprite::m_fVirtualScreenH / h) * (*y));
};


bool CTouchControllerProcess::IsStickTouch(const TOUCH* pTouch) const
{
    RwV2d stickAreaPos = { STICK_AREA_ANCHOR.x - (STICK_AREA_SIZE.x * 0.5f),
                           STICK_AREA_ANCHOR.y - (STICK_AREA_SIZE.y * 0.5f) };

    RwV2d stickAreaSize = STICK_AREA_SIZE;

    RwV2d touchPoint = { pTouch->fCurrentPosX,
                         pTouch->fCurrentPosY };

    stickAreaPos.x -= CSprite::m_fVirtualScreenX;
    stickAreaPos.y -= CSprite::m_fVirtualScreenY;

    touchPoint.x -= CSprite::m_fVirtualScreenX;
    touchPoint.y -= CSprite::m_fVirtualScreenY;

    if (IsRectContainPoint(&stickAreaPos, &stickAreaSize, &touchPoint))
        return true;

    return false;
};

#endif /*  defined(TARGET_WEB) */