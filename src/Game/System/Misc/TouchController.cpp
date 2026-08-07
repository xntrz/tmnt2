//
//  Useful link: https://www.figma.com/
//  To fast virtual screen layout with alignments, coords and so on
//

#include "TouchController.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Sequence/Menu/OptionsSequence.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/InputsDevice.hpp"

#if defined(TARGET_WEB)

#include "System/Web/WebSpecific.hpp"


// size of all textures used in touch controller
#define TEX_W (256.0f)
#define TEX_H (256.0f)


// for CSprite::SetUV(x, x, x, x)
#define TEX_UV_RECT(x, y, w, h)         \
      (x) / TEX_W,                      \
      (y) / TEX_H,                      \
      ((x) / TEX_W) + ((w) / TEX_W),    \
      ((y) / TEX_H) + ((h) / TEX_H)


static const int32 VIRTUAL_SCREEN_STACK_SIZE = 4;
static Rt2dBBox s_aVirtualScreenStack[VIRTUAL_SCREEN_STACK_SIZE];
static int32 s_iVirtualScreenStackDepth = 0;


static inline void PushVirtualScreen(float x, float y, float w, float h)
{
    ASSERT(s_iVirtualScreenStackDepth < VIRTUAL_SCREEN_STACK_SIZE);

    Rt2dBBox* vs = &s_aVirtualScreenStack[s_iVirtualScreenStackDepth];

    ++s_iVirtualScreenStackDepth;

    vs->x = CSprite::m_fVirtualScreenX;
    vs->y = CSprite::m_fVirtualScreenY;
    vs->w = CSprite::m_fVirtualScreenW;
    vs->h = CSprite::m_fVirtualScreenH;

    CSprite::m_fVirtualScreenX = x;
    CSprite::m_fVirtualScreenY = y;
    CSprite::m_fVirtualScreenW = w;
    CSprite::m_fVirtualScreenH = h;
};


static inline void PopVirtualScreen(void)
{
    ASSERT(s_iVirtualScreenStackDepth > 0);

    Rt2dBBox* vs = &s_aVirtualScreenStack[s_iVirtualScreenStackDepth - 1];

    --s_iVirtualScreenStackDepth;

    CSprite::m_fVirtualScreenX = vs->x;
    CSprite::m_fVirtualScreenY = vs->y;
    CSprite::m_fVirtualScreenW = vs->w;
    CSprite::m_fVirtualScreenH = vs->h;
};


static inline void PushVirtualScreenEx(float x, float y, float w, float h)
{
    PushVirtualScreen(x, y, w, h);
    CSystem2D::Reset();
};


static inline void PopVirtualScreenEx(void)
{
    PopVirtualScreen();
    CSystem2D::Reset();
};


static inline int32 FramesCorrection(int32 frames)
{
    float dt = static_cast<float>(CScreen::TimerStride());
    float fFramerateOrg = static_cast<float>(CScreen::Framerate());
    float fFramerateCur = (1.0f / dt);

    return static_cast<int32>(static_cast<float>(frames) * (fFramerateCur / fFramerateOrg));
};


static inline int32 GetTouchController(void)
{
    int32 port = CWebSpecific::GetTouchPort();
    if (port == -1)
        return -1;

    return CController::GetController(port);
};


static inline IGamepad::FUNCTION GetFunctionForDigital(int32 controller, uint32 digital)
{
    if (controller == -1)
        return IGamepad::FUNCTION_MAX;
    
    IPhysicalController& physicalController = CInputsDevice::Instance().ControllerResource(controller);

    uint32 function = 0;
    if ((digital & 0xFFFF) != 0)
    {
        function = *physicalController.GetDigitalMapping(digital);
        function &= (0xFFFF0000);
    }
    else
    {
        function = (digital & 0xFFFF0000);
    };

    for (int32 i = 0; i < IGamepad::FUNCTION_MAX; ++i)
    {
        uint32 functionMask = IPadFunctionMask(i);

        if ((function & functionMask) == functionMask)
            return static_cast<IGamepad::FUNCTION>(i);
    };

    return IGamepad::FUNCTION_MAX;
};


static inline uint32 GetDigitalForFunction(int32 controller, IGamepad::FUNCTION func)
{
    if (controller == -1)
        return 0;

    IPhysicalController& physicalController = CInputsDevice::Instance().ControllerResource(controller);

    for (int32 i = 0; i < (CController::DIGITAL_NUM / 2); ++i)
    {
        uint32 mapping = *physicalController.GetDigitalMapping((1 << i));
        if (mapping == 0)
            continue;
        
        uint32 function = (mapping & 0xFFFF0000);
        uint32 funcMask = IPadFunctionMask(func);

        if ((function & funcMask) == funcMask)
        {
            uint32 digital = (mapping & 0x0000FFFF);
            return digital;
        };
    };

    return 0;
};


static inline const char* GetFunctionText(IGamepad::FUNCTION func)
{
    int32 iCurrentlySeqLabel = CSequence::GetCurrently();
    switch (iCurrentlySeqLabel)
    {    
    case PROCLABEL_SEQ_CHARASELECT:
        {
            switch (func)
            {
            case IGamepad::FUNCTION_DASH:  return "Nexus\nCostume";
            case IGamepad::FUNCTION_GUARD: return "Japan\nCostume";
            default: break;
            };
        }
        break;

    case PROCLABEL_SEQ_RIDESTAGE:
        {
            switch (func)
            {
            case IGamepad::FUNCTION_DASH:  return "Roll\nLeft";
            case IGamepad::FUNCTION_GUARD: return "Roll\nRight";
            default: break;
            };
        }
        break;

    default:
        break;
    };

    static const char* s_apszBaseFunctionName[] =
    {
        "WEAK\nATTACK",
        "STRONG\nATTACK",
        "SHOT",
        "JUMP",
        "DASH",
        "GUARD",
        "SWITCH",
        "CAMERA",
        "GAUGE",
    };

    return s_apszBaseFunctionName[func];
};


static bool SDLCALL SDLEventFilter(void* userdata, SDL_Event* event)
{
    CTouchController* pProcess =
        reinterpret_cast<CTouchController*>(userdata);

    return pProcess->OnSDLEvent(event);
};


CTouchController::CSpriteEx::CSpriteEx(void)
{
    ;
};


CTouchController::CSpriteEx::~CSpriteEx(void)
{
    ;
};


void CTouchController::CSpriteEx::Move(float x, float y)
{
    if (m_bLeftHanded) {
        x = (CSprite::m_fVirtualScreenW - x);
    };
    
    CSprite::Move(x, y);
};


void CTouchController::CSpriteEx::Resize(float w, float h)
{
    if (m_bLeftHanded) {
        w = -w;
    };

    CSprite::ResizeStrict(w, h);
};


/*static*/ const Rt2dBBox CTouchController::VIRTUAL_SCREEN_SIZE = { 0.0f, 0.0f, 1280.0f, 720.0f };
/*static*/ const float CTouchController::STICK_RADIUS = 112.0f;
/*static*/ const float CTouchController::BUTTON_RADIUS = 48.0f;
/*static*/ const float CTouchController::BUTTON_HIT_RADIUS_SCALE = 1.20f;
/*static*/ const RwV2d CTouchController::CONTROL_OFFSET = { 64.0f, 38.0f };
/*static*/ int32 CTouchController::m_iRefCount = 0;
/*static*/ bool CTouchController::m_bDebugDraw = false;
/*static*/ bool CTouchController::m_bLeftHanded = false;
/*static*/ bool CTouchController::m_bFloating = true;
/*static*/ bool CTouchController::m_bDynamic = true;
/*static*/ bool CTouchController::m_bLatchMode = true;


/*static*/ CProcess* CTouchController::Instance(void)
{
    return new CTouchController;
};


/*static*/ void CTouchController::Initialize(CProcess* pSender)
{
    if (!m_iRefCount++)
        pSender->Mail().Send(PROCLABEL_TOUCHCONTROLLER, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CTouchController::Terminate(CProcess* pSender)
{
    if (m_iRefCount > 0)
    {
        if (!--m_iRefCount)
            pSender->Mail().Send(PROCLABEL_TOUCHCONTROLLER, PROCESSTYPES::MAIL::TYPE_DETACH);
    };
};


CTouchController::CTouchController(void)
: m_fAlphaRatio(0.4f)
, m_sprite()
, m_aTouch()
, m_aButton()
, m_aStick()
, m_tap()
, m_pTexStick(nullptr)
, m_pTexButton(nullptr)
, m_bOptionChanged(false)
, m_bGestureMode(false)
#ifdef _DEBUG
, m_font()
#endif /* _DEBUG */
{
    ;
};


CTouchController::~CTouchController(void)
{
    ;
};


bool CTouchController::Attach(void)
{
    if (!SDL_AddEventWatch(&SDLEventFilter, this))
    {
        OUTPUT("SDL_AddEventWatch failed: %s\n", SDL_GetError());
        return false;
    };

    CTextureManager::SetCurrentTextureSet("touch");
    m_pTexStick = CTextureManager::GetRwTexture("stick.png");
    m_pTexButton = CTextureManager::GetRwTexture("button.png");

    TouchInit();
    ButtonInit();
    StickInit();
    TapInit();

    return true;
};


void CTouchController::Detach(void)
{
    SDL_RemoveEventWatch(&SDLEventFilter, this);
};


void CTouchController::Move(void)
{
    OptionUpdate();
    ButtonUpdateLayout();
    StickUpdateArea();

    PushVirtualScreen(VIRTUAL_SCREEN_SIZE.x,
                      VIRTUAL_SCREEN_SIZE.y,
                      VIRTUAL_SCREEN_SIZE.w,
                      VIRTUAL_SCREEN_SIZE.h);

    TouchUpdate();
    TapUpdate();
    ButtonUpdate();
    StickUpdate();
    UpdatePhysicalController();
    TouchPostUpdate();

    PopVirtualScreen();
};


void CTouchController::Draw(void) const
{
    if (!CWebSpecific::IsInFullscreen())
        return;

    PushVirtualScreenEx(VIRTUAL_SCREEN_SIZE.x,
                        VIRTUAL_SCREEN_SIZE.y,
                        VIRTUAL_SCREEN_SIZE.w,
                        VIRTUAL_SCREEN_SIZE.h);

    CSprite::PushRenderStates();
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    
    TouchDraw();
    ButtonDraw();
    ButtonDrawName();
    StickDraw();
    
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    CSprite::PopRenderStates();

    PopVirtualScreenEx();
};


void CTouchController::StickInit(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aStick); ++i)
    {
        STICK* pStick = &m_aStick[i];

        pStick->area = { 0.0f, 0.0f, 0.0f, 0.0f };
        pStick->pos = Math::VECTOR2_ZERO;
        pStick->thumbPos = Math::VECTOR2_ZERO;
        pStick->fRadius = STICK_RADIUS;
        pStick->x = 0.0f;
        pStick->y = 0.0f;
        pStick->pTouch = nullptr;
    };

    StickUpdateArea();
};


void CTouchController::StickUpdateArea(void)
{
    static const Rt2dBBox s_aStickAreaDynamic[CController::STICK_NUM] =
    {
        // x           y        w         h
        { 0.0f,     216.0f,   640.0f,   504.0f  },
        { 640.0f,   216.0f,   320.0f,   504.0f  },
    };

    static const Rt2dBBox s_aStickAreaStatic[CController::STICK_NUM] =
    {
        // x           y        w         h
        { 128.0f,    500.0f,   64.0f,   64.0f  },
        { 858.0f,    500.0f,   64.0f,   64.0f  },
    };

    int32 stickNum = CController::STICK_NUM;
    stickNum = Min(stickNum, COUNT_OF(m_aStick));

    for (int32 i = 0; i < stickNum; ++i)
    {
        m_aStick[i].area = (m_bDynamic ? s_aStickAreaDynamic[i] :
                                         s_aStickAreaStatic[i]);
    };
};


void CTouchController::StickUpdate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aStick); ++i)
    {
        STICK* pStick = &m_aStick[i];

        pStick->x = 0.0f;
        pStick->y = 0.0f;

        if (m_bDynamic)
        {
            if (!pStick->pTouch)
                continue;
        }
        else
        {
            pStick->pos = RwV2d{ pStick->area.x + (pStick->area.w * 0.5f),
                                 pStick->area.y + (pStick->area.h * 0.5f) };

            if (!pStick->pTouch)
            {
                pStick->thumbPos = pStick->pos;
                continue;
            };

            if (m_bLeftHanded)
                pStick->pos.x = (CSprite::m_fVirtualScreenW - pStick->pos.x);
        };

        pStick->thumbPos = pStick->pTouch->currentPos;

        RwV2d dist = RwV2d{ pStick->thumbPos.x - pStick->pos.x,
                            pStick->thumbPos.y - pStick->pos.y };

        float fDistSQ = (dist.x * dist.x +
                         dist.y * dist.y);

        float fRadiusSQ = (pStick->fRadius * pStick->fRadius);

        if (fDistSQ >= fRadiusSQ)
        {
            float fDist = std::sqrt(fDistSQ);

            float nx = ((dist.x / fDist) * pStick->fRadius);
            float ny = ((dist.y / fDist) * pStick->fRadius);

            pStick->thumbPos = RwV2d{ pStick->pos.x + nx,
                                      pStick->pos.y + ny };

            if (m_bFloating && m_bDynamic)
            {
                const float fFollowSpeed = 5.0f;
                const float dt = CScreen::TimerStride();

                float fOverdist = (fDist - pStick->fRadius);
                float fLerpFactor = (fFollowSpeed * dt);

                if (fLerpFactor > 1.0f)
                    fLerpFactor = 1.0f;

                pStick->pos.x += ((dist.x / fDist) * fOverdist * fLerpFactor);
                pStick->pos.y += ((dist.y / fDist) * fOverdist * fLerpFactor);
            };

            pStick->x = (nx / pStick->fRadius);
            pStick->y = (ny / pStick->fRadius);
        }
        else
        {
            pStick->x = (dist.x / pStick->fRadius);
            pStick->y = (dist.y / pStick->fRadius);
        };
    };
};


void CTouchController::StickDraw(void) const
{
#ifdef _DEBUG
    if (m_bDebugDraw)
    {
        m_sprite.SetOffset(0.0f, 0.0f);
        for (int32 i = 0; i < COUNT_OF(m_aStick); ++i)
        {
            const STICK* pStick = &m_aStick[i];

            m_sprite.SetTextureEmpty();
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);

            m_sprite.SetRGBA(255, 0, 0, 32);
            m_sprite.Move(pStick->area.x, pStick->area.y);
            m_sprite.Resize(pStick->area.w, pStick->area.h);
            m_sprite.Draw();
        };
        m_sprite.SetOffset(0.5f, 0.5f);
    };
#endif /* _DEBUG */

    m_sprite.SetTextureEmpty();
    if (m_pTexStick)
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexStick));

    uint8 alpha = static_cast<uint8>(255.0f * m_fAlphaRatio);

    for (int32 i = 0; i < COUNT_OF(m_aStick); ++i)
    {
        const STICK* pStick = &m_aStick[i];

        bool bLeftHanded = m_bLeftHanded;
        if (m_bDynamic)
        {
            if (!pStick->pTouch)
                continue;
        };

        if (pStick->pTouch)
        {
            /* touch contains actually finger pos on screen
               so no need to invert x axis for sprites when
               in left handed mode */
            m_bLeftHanded = false;
        };

        RwV2d size = RwV2d{ pStick->fRadius * 2.0f,
                            pStick->fRadius * 2.0f };

        m_sprite.SetUV(TEX_UV_RECT(0.0f, 0.0f, 156.0f, 156.0f));
        m_sprite.Move(pStick->pos.x, pStick->pos.y);
        m_sprite.Resize(size.x, size.y);
        m_sprite.SetRGBA(255, 255, 255, alpha);
        m_sprite.Draw();
        m_sprite.ResetUV();

        float fPadding = 16.0f;

        m_sprite.SetUV(TEX_UV_RECT(156.0f - fPadding,
                                   156.0f - fPadding,
                                    52.0f + (fPadding * 2.0f),
                                    52.0f + (fPadding * 2.0f)));
        m_sprite.Move(pStick->thumbPos.x, pStick->thumbPos.y);
        m_sprite.Resize((size.x + fPadding + fPadding) * 0.50f,
                        (size.y + fPadding + fPadding) * 0.50f);
        m_sprite.SetRGBA(255, 255, 255, 200);
        m_sprite.Draw();
        m_sprite.ResetUV();

        m_bLeftHanded = bLeftHanded;        
    };
};


void CTouchController::ButtonInit(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        BUTTON* pButton = &m_aButton[i];

        pButton->digital = 0;
        pButton->pos = Math::VECTOR2_ZERO;
        pButton->fRadius = 0.0f;
        pButton->pTexture = nullptr;
        pButton->bEnabled = false;
        pButton->bDown = false;
        pButton->holdDelay = 0;
        pButton->delay = 0;
    };
};


void CTouchController::ButtonUpdate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        BUTTON* pButton = &m_aButton[i];
        if (pButton->bDown && (pButton->holdDelay > 0))
        {
            if (pButton->delay == 0)
            {
                pButton->delay = pButton->holdDelay;                
            }
            else if (pButton->delay > 0)
            {
                if (--pButton->delay == 0)
                    pButton->bDown = false;
            };                
        }
        else
        {
            pButton->bDown = false;
        };
    };

    if (m_bGestureMode)
        ButtonUpdateGesture();
    else
        ButtonUpdateNormal();
};


void CTouchController::ButtonUpdateGesture(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        BUTTON* pButton = &m_aButton[i];

        if (!pButton->digital)
            continue;

        if (!pButton->bEnabled)
            continue;

        if (pButton->digital == CController::DIGITAL_START)
        {
            pButton->bDown = m_tap.trigger[0];
        }
        else if (pButton->digital == CController::DIGITAL_SELECT)
        {
            pButton->bDown = m_tap.trigger[1];
        };
    };
};


void CTouchController::ButtonUpdateNormal(void)
{
    float fScaleW = static_cast<float>(CScreen::Width()) / CSprite::m_fVirtualScreenW;
    float fScaleH = static_cast<float>(CScreen::Height()) / CSprite::m_fVirtualScreenH;
    float fScale = Min(fScaleW, fScaleH);

    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        BUTTON* pButton = &m_aButton[i];

        if (!pButton->digital || !pButton->bEnabled)
            continue;

        float x = pButton->pos.x;
        float y = pButton->pos.y;
        ButtonPosCorrection(&x, &y, ANCHOR_POS_BOTTOM_RIGHT | ANCHOR_FLAG_REAL_SCREEN);

        RwV2d buttonPos = { x + (pButton->fRadius * fScale),
                            y + (pButton->fRadius * fScale) };

        float buttonRadius = (pButton->fRadius * BUTTON_HIT_RADIUS_SCALE) * fScale;

        for (int32 j = 0; j < COUNT_OF(m_aTouch); ++j)
        {
            TOUCH* pTouch = &m_aTouch[j];

            if (!pTouch->bDown)
                continue;

            //if (!pTouch->bTrigger)
            //    continue;

            if (pTouch->bStickHit)
                continue;

            RwV2d touchPoint = pTouch->currentPos;
            touchPoint.x *= fScaleW;
            touchPoint.y *= fScaleH;

            if (IsCircleContainPoint(&buttonPos, buttonRadius, &touchPoint, true))
            {
                pButton->bDown = true;
            };
        };
    };
};


void CTouchController::ButtonDraw(void) const
{
    m_sprite.SetTextureEmpty();
    if (m_pTexButton)
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexButton));

    uint8 alpha = static_cast<uint8>(255.0f * m_fAlphaRatio);

    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        const BUTTON* pButton = &m_aButton[i];

        if (!pButton->digital || !pButton->bEnabled)
            continue;

        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.SetUV(TEX_UV_RECT(0.0f, 0.0f, 64.0f, 64.0f));

        float x = pButton->pos.x;
        float y = pButton->pos.y;

        if ((pButton->digital == CController::DIGITAL_START) ||
            (pButton->digital == CController::DIGITAL_SELECT))
        {
            ButtonPosCorrection(&x, &y, ANCHOR_POS_TOP_LEFT);
        }
        else
        {
            ButtonPosCorrection(&x, &y, ANCHOR_POS_BOTTOM_RIGHT);
        };

        m_sprite.Move(x, y);
        m_sprite.Resize((pButton->fRadius * 2.0f), (pButton->fRadius * 2.0f));

        if (pButton->bDown)
            m_sprite.SetRGBA(64, 255, 64, 128);
        else
            m_sprite.SetRGBA(255, 255, 255, alpha);

        m_sprite.Draw();
        m_sprite.ResetUV();
        m_sprite.SetOffset(0.5f, 0.5f);
    };
};


void CTouchController::ButtonDrawName(void) const
{
    uint8 alpha = static_cast<uint8>(255.0f * m_fAlphaRatio);

    float fScaleW = static_cast<float>(CScreen::Width()) / CSprite::m_fVirtualScreenW;
    float fScaleH = static_cast<float>(CScreen::Height()) / CSprite::m_fVirtualScreenH;
    float fScale = Min(fScaleW, fScaleH);

    float fHeight = 18.0f * (fScale / fScaleH);

    CGameFont::SetHeight(fHeight);
    CGameFont::SetRGBA(255, 255, 255, alpha);

    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        const BUTTON* pButton = &m_aButton[i];

        if (!pButton->digital)
            continue;

        if (!pButton->bEnabled)
            continue;

        if (pButton->fRadius <= 0.0f)
            continue;

        float x = pButton->pos.x + pButton->fRadius;
        float y = pButton->pos.y + pButton->fRadius;

        if (pButton->digital == CController::DIGITAL_OK)
        {
            ButtonPosCorrection(&x, &y, ANCHOR_POS_BOTTOM_RIGHT);
            ButtonDrawCenteredText("OK", pButton->fRadius, fHeight, x, y);
        }
        else if (pButton->digital == CController::DIGITAL_CANCEL)
        {
            ButtonPosCorrection(&x, &y, ANCHOR_POS_BOTTOM_RIGHT);
            ButtonDrawCenteredText("CANCEL", pButton->fRadius, fHeight, x, y);
        }
        else if (pButton->digital == CController::DIGITAL_START)
        {
            ButtonPosCorrection(&x, &y, ANCHOR_POS_TOP_LEFT);
            ButtonDrawCenteredText("PAUSE", pButton->fRadius, fHeight, x, y);
        }
        else if (pButton->digital == CController::DIGITAL_SELECT)
        {
            ButtonPosCorrection(&x, &y, ANCHOR_POS_TOP_LEFT);
            ButtonDrawCenteredText("DEBUG\nMENU", pButton->fRadius, fHeight, x, y);
        }
        else
        {
            ButtonPosCorrection(&x, &y, ANCHOR_POS_BOTTOM_RIGHT);

            int32 controller = GetTouchController();
            IGamepad::FUNCTION function = GetFunctionForDigital(controller, pButton->digital);

            if (function != IGamepad::FUNCTION_MAX)
            {
                const char* pszText = GetFunctionText(function);
                ButtonDrawCenteredText(pszText, pButton->fRadius, fHeight, x, y);
            };
        };
    };
};


void CTouchController::ButtonDrawCenteredText(const char* pszText, float r,
                                              float h, float x, float y) const
{
    int32 lineNum = 1;

    const char* pszPos = std::strchr(pszText, '\n');
    while (pszPos)
    {
        pszPos = std::strchr(pszPos + 1, '\n');
        ++lineNum;
    };

    if (lineNum == 1)
        ButtonDrawSinglelineCenteredText(pszText, r, h, x, y);
    else
        ButtonDrawMultilineCenteredText(pszText, lineNum, r, h, x, y);
};


void CTouchController::ButtonDrawSinglelineCenteredText(const char* pszText, float r,
                                                        float h, float x, float y) const
{
    ButtonDrawCenteredString(pszText, r, h, x, y);
};


void CTouchController::ButtonDrawMultilineCenteredText(const char* pszText, int32 lineNum,
                                                       float r, float h, float x, float y) const
{
    const float fLineSpacing = 1.2f;
    const float fLineStep = h * fLineSpacing;
    const float fTotalHeight = fLineStep * lineNum;

    float fCurrentY = y - (fTotalHeight * 0.5f) + (fLineStep * 0.5f);

    const char* pszStart = pszText;
    while (pszStart && (*pszStart != '\0'))
    {
        const char* pszEnd = std::strchr(pszStart, '\n');

        char szCurrentLine[256];
        szCurrentLine[0] = '\0';

        if (pszEnd)
        {
            size_t len = pszEnd - pszStart;
            len = Min(len, sizeof(szCurrentLine) - 1);

            std::strncpy(szCurrentLine, pszStart, len);
            szCurrentLine[len] = '\0';

            pszStart = pszEnd + 1;
        }
        else
        {
            std::strncpy(szCurrentLine, pszStart, sizeof(szCurrentLine) - 1);
            szCurrentLine[sizeof(szCurrentLine) - 1] = '\0';

            pszStart = nullptr;
        };
        
        ButtonDrawCenteredString(szCurrentLine, r, h, x, fCurrentY);

        fCurrentY += fLineStep;
    };
};


void CTouchController::ButtonDrawCenteredString(const char* pszText, float r,
                                                float h, float x, float y) const
{
    if (m_bLeftHanded) {
        x = (CSprite::m_fVirtualScreenW - x);
    };

    CGameFont::SetHeight(h);

    float w = CGameFont::GetStringWidth(pszText);
    float s = ((r * 2.0f) / w);

    if (s > 1.0f)
        s = 1.0f;

    h *= (s * 0.85f);

    CGameFont::SetHeight(h);

    float xx = CGameFont::GetStringWidth(pszText, h) * -0.5f;
    CGameFont::Show(pszText, x + xx, -CSprite::m_fVirtualScreenH + y);
};


void CTouchController::ButtonPosCorrection(float* x, float* y, uint32 anchor) const
{
    if ((*x == 0.0f) && (*y == 0.0f))
        return;

    float fScaleW = static_cast<float>(CScreen::Width()) / CSprite::m_fVirtualScreenW;
    float fScaleH = static_cast<float>(CScreen::Height()) / CSprite::m_fVirtualScreenH;
    float fScale = Min(fScaleW, fScaleH);

    float fDistToRight = CSprite::m_fVirtualScreenW - (*x);
    float fDistToBottom = CSprite::m_fVirtualScreenH - (*y);
    float fDistToLeft = *x;
    float fDistToTop = *y;

    float fRealScrX = 0.0f;
    float fRealScrY = 0.0f;

    int32 anchorPos = (anchor & ANCHOR_MASK_POS);
    switch (anchorPos)
    {
    case ANCHOR_POS_TOP_LEFT:
        fRealScrX = (fDistToLeft * fScale);
        fRealScrY = (fDistToTop * fScale);
        break;

    case ANCHOR_POS_BOTTOM_RIGHT:
        fRealScrX = static_cast<float>(CScreen::Width()) - (fDistToRight * fScale);
        fRealScrY = static_cast<float>(CScreen::Height()) - (fDistToBottom * fScale);
        break;

    default:
        ASSERT(false);
        break;
    };

    int32 anchorFlag = (anchor & ANCHOR_MASK_FLAG);
    if (anchorFlag & ANCHOR_FLAG_REAL_SCREEN)
    {
        *x = fRealScrX;
        *y = fRealScrY;
    }
    else
    {
        *x = fRealScrX / fScaleW;
        *y = fRealScrY / fScaleH;
    };
};


void CTouchController::ButtonUpdateLayout(void)
{
    if (!CDataLoader::IsLoadEnd() || CScreenFade::IsDrawing())
    {
        ButtonSetLayoutEmpty();
        return;
    };

    int32 iCurrentlySeqLabel = CSequence::GetCurrently();
    switch (iCurrentlySeqLabel)
    {
    case PROCLABEL_SEQ_UNLOCK:
    case PROCLABEL_SEQ_MOVIE:
    case PROCLABEL_SEQ_SAVELOADCHECK:
    case PROCLABEL_SEQ_SAVELOADMENUSAVE:
    case PROCLABEL_SEQ_SAVELOADMENULOAD:
    case PROCLABEL_SEQ_SAVELOADAUTO:        
    case PROCLABEL_SEQ_TITLE:
        {
            ButtonSetLayoutMenu(MENU_BUTTON_OK);
        }
        break;

    case PROCLABEL_SEQ_CHARASELECT:
        {
            // WEAK ATTACK -> SELECT/OK & JUMP -> BACK/CANCEL
            ButtonSetLayoutMenu(MENU_BUTTON_WEAK_ATTACK | MENU_BUTTON_JUMP |
                                MENU_BUTTON_COSTUME_NEX | MENU_BUTTON_COSTUME_SAM);
        }
        break;

    case PROCLABEL_SEQ_OPTIONS:
    case PROCLABEL_SEQ_ANTIQUESHOP:
    case PROCLABEL_SEQ_DATABASESEQ:
    case PROCLABEL_SEQ_DATABASEVIEWER:
    case PROCLABEL_SEQ_DATABASE:
        {
            if (iCurrentlySeqLabel == PROCLABEL_SEQ_OPTIONS)
            {
                if (COptionsSequence::IsControlConfigure())
                {
                    ButtonSetLayoutNormal(false);
                    break;
                };
            };

            // WEAK ATTACK -> SELECT/OK & JUMP -> BACK/CANCEL
            ButtonSetLayoutMenu(MENU_BUTTON_WEAK_ATTACK | MENU_BUTTON_JUMP);
        }
        break;

    case PROCLABEL_SEQ_RESULT:
        {
            ButtonSetLayoutMenu(MENU_BUTTON_WEAK_ATTACK);
        }
        break;

    case PROCLABEL_SEQ_ENBU:
        {
            if (IsInteractive())
                ButtonSetLayoutMenu(MENU_BUTTON_OK);
        }
        break;

    case PROCLABEL_SEQ_STAFFROLL:
        {
            // unskippable
        }
        break;

    case PROCLABEL_SEQ_AREA:
        {
            // GUARD -> MENU & WEAK ATTACK -> OK
            ButtonSetLayoutMenu(MENU_BUTTON_GUARD | MENU_BUTTON_WEAK_ATTACK);
        }
        break;

    case PROCLABEL_SEQ_NEXUSMENU:
        {
            // WEAK ATTACK -> OK & GUARD -> INFO/BACK & JUMP -> HOME/BACK
            ButtonSetLayoutMenu(MENU_BUTTON_WEAK_ATTACK | MENU_BUTTON_JUMP | MENU_BUTTON_GUARD);
        }
        break;

    case PROCLABEL_SEQ_HOMESTAGE:
        {
            ButtonSetLayoutMenu(MENU_BUTTON_OK);
        }
        break;

    case PROCLABEL_SEQ_NORMALSTAGE:
    case PROCLABEL_SEQ_RIDESTAGE:
    case PROCLABEL_SEQ_NEXUSSTAGE:
        {
            if (!IsInteractive())
            {
                // introduction case - ok for skip
                ButtonSetLayoutMenu(MENU_BUTTON_OK);
                break;
            };

            STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();
            if (idStage == STAGEID::ID_NONE)
            {
                ButtonSetLayoutEmpty();
                break;
            };

            if (IsStageRetToMenu())
            {
                ButtonSetLayoutEmpty();
                break;
            };

            if (IsStagePaused())
            {
                if (IsStageMenuPause())
                {
                    // WEAK ATTACk -> SELECT & JUMP -> BACK
                    ButtonSetLayoutMenu(MENU_BUTTON_WEAK_ATTACK | MENU_BUTTON_JUMP);
                }
                else
                {
                    // tutorial case - ok for ok
                    ButtonSetLayoutMenu(MENU_BUTTON_OK);
                };
            }
            else
            {
                if (CStageInfo::IsRideShotStage(idStage))
                    ButtonSetLayoutRideShot();
                else if (CStageInfo::IsRideJumpStage(idStage))
                    ButtonSetLayoutRideJump();
                else
                    ButtonSetLayoutNormal();
            };
        }
        break;

    case PROCLABEL_SEQ_TR_COMINGSOON:
    case PROCLABEL_SEQ_TR_PADINFO:
        {
            ButtonSetLayoutMenu(MENU_BUTTON_OK);
        }
        break;

    default:
        {
            if (IsInteractive())
                ButtonSetLayoutMenu(MENU_BUTTON_OK | MENU_BUTTON_CANCEL);
        }
        break;
    };
};


void CTouchController::ButtonSetLayout(const BUTTON_LAYOUT* pLayout, int32 count,
                                       const int32* pHoldDelayTable /*= nullptr*/)
{
    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        BUTTON* pButton = &m_aButton[i];

        if (i >= count)
        {
            pButton->digital = 0;
            pButton->pos = Math::VECTOR2_ZERO;
            pButton->fRadius = 0.0f;
            pButton->pTexture = nullptr;
            pButton->bEnabled = false;
            pButton->bDown = false;
            pButton->holdDelay = 0;
            pButton->delay = 0;
        }
        else
        {
            pButton->digital = pLayout[i].digital;
            pButton->pos = pLayout[i].pos;
            pButton->fRadius = pLayout[i].fRadius * 1.1f;
            pButton->bEnabled = (pButton->digital ? true : false);
            pButton->holdDelay = (pHoldDelayTable ? pHoldDelayTable[i] : 0);
        };
    };
};


void CTouchController::ButtonSetLayoutEmpty(void)
{
    static const BUTTON_LAYOUT s_aButtonLayout[COUNT_OF(m_aButton)] =
    {
        { CController::DIGITAL_START,   {    0.0f,   0.0f }, 0.0f    }, // START
        { CController::DIGITAL_SELECT,  {    0.0f,   0.0f }, 0.0f    }, // SELECT
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
        { 0, { 0.0f, 0.0f }, 0.0f },
    };

    ButtonSetLayout(s_aButtonLayout, COUNT_OF(s_aButtonLayout));
};


void CTouchController::ButtonSetLayoutMenu(int32 menuButtonFlag)
{
    const float radiusB = 48.0f;
    const RwV2d offset = CONTROL_OFFSET;
    const RwV2d anchor = { 735.0f - offset.x,
                           476.0f - offset.y };

    static BUTTON_LAYOUT s_aButtonLayout[] =
    {
        { 0, { (anchor.x + 396.0f), (anchor.y + 0.0f) }, radiusB },
        { 0, { (anchor.x + 264.0f), (anchor.y + 0.0f) }, radiusB },
        { 0, { (anchor.x + 132.0f), (anchor.y + 0.0f) }, radiusB },
        { 0, { (anchor.x +   0.0f), (anchor.y + 0.0f) }, radiusB },
    };

    int32 buttonCount = 0;
    int32 controller = GetTouchController();

#define buttonIndex \
    (buttonCount++ % COUNT_OF(s_aButtonLayout))

    if (menuButtonFlag & MENU_BUTTON_WEAK_ATTACK)
        s_aButtonLayout[buttonIndex].digital = GetDigitalForFunction(controller, IGamepad::FUNCTION_ATTACK_A);

    if (menuButtonFlag & MENU_BUTTON_JUMP)
        s_aButtonLayout[buttonIndex].digital = GetDigitalForFunction(controller, IGamepad::FUNCTION_JUMP);

    if (menuButtonFlag & MENU_BUTTON_GUARD)
        s_aButtonLayout[buttonIndex].digital = GetDigitalForFunction(controller, IGamepad::FUNCTION_GUARD);

    if (menuButtonFlag & MENU_BUTTON_OK)
        s_aButtonLayout[buttonIndex].digital = CController::DIGITAL_OK;

    if (menuButtonFlag & MENU_BUTTON_CANCEL)
        s_aButtonLayout[buttonIndex].digital = CController::DIGITAL_CANCEL;

    if ((menuButtonFlag & MENU_BUTTON_COSTUME_NEX) && IsCostumeAvailable(GAMETYPES::COSTUME_NEXUS))
        s_aButtonLayout[buttonIndex].digital = GetDigitalForFunction(controller, IGamepad::FUNCTION_DASH);

    if ((menuButtonFlag & MENU_BUTTON_COSTUME_SAM) && IsCostumeAvailable(GAMETYPES::COSTUME_SAMURAI))
        s_aButtonLayout[buttonIndex].digital = GetDigitalForFunction(controller, IGamepad::FUNCTION_GUARD);

#undef buttonIndex
    
    ButtonSetLayout(s_aButtonLayout, buttonCount);
};


void CTouchController::ButtonSetLayoutNormal(bool bSetPause /*= true*/)
{
    const float radiusS = 48.0f;
    const float radiusB = 56.0f;

    const RwV2d offset = CONTROL_OFFSET;
    const RwV2d anchor = { 864.0f - offset.x,
                           199.0f - offset.y };

    const RwV2d arc = { 8.0f, 8.0f }; // arc offset to left-up
    const RwV2d main = { 10.0f, 10.0f }; // main offset away from each other (down and right)

    static BUTTON_LAYOUT s_aButtonLayout[] =
    {
        { CController::DIGITAL_RUP,     { (anchor.x + 192.0f) - arc.x,          (anchor.y + 215.0f) - (arc.y * 2.0f) }, radiusS        }, // SHOT
        { CController::DIGITAL_L1,      { (anchor.x + 110.0f) - (arc.x * 2.0f), (anchor.y + 297.0f) - arc.y          }, radiusS        }, // DASH
        { CController::DIGITAL_R1,      { (anchor.x + 304.0f),                  (anchor.y + 177.0f) - arc.y          }, radiusS,       }, // GUARD
        { CController::DIGITAL_RRIGHT,  { (anchor.x +  72.0f) - arc.x,          (anchor.y + 409.0f)                  }, radiusS        }, // JUMP

        { CController::DIGITAL_RDOWN,   { (anchor.x + 192.0f),                  (anchor.y + 393.0f) + main.y         }, radiusB        }, // ATTACK WEAK
        { CController::DIGITAL_RLEFT,   { (anchor.x + 288.0f) + main.x,         (anchor.y + 297.0f)                  }, radiusB        }, // ATTACK STRONG
        
        { CController::DIGITAL_L2,      { (anchor.x + 304.0f),                  (anchor.y +   0.0f)                  }, radiusS        }, // SWITCH CHR
        { CController::DIGITAL_START,   { (             0.0f),                  (             0.0f)                  }, radiusS * 1.0f }, // PAUSE
        { 0,                            { (112.0f),                             (0.0f)                               }, radiusS * 1.0f }, // DEBUG MENU
    };

    static int32 s_aButtonHoldDelay[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    static_assert(COUNT_OF(s_aButtonLayout) == COUNT_OF(s_aButtonHoldDelay), "checkout");

    s_aButtonLayout[7].digital = (bSetPause ? CController::DIGITAL_START : 0);
#ifdef _DEBUG
    if (Info().IsProcessExist(PROCLABEL_DBGMENU))
        s_aButtonLayout[8].digital = CController::DIGITAL_SELECT;
#endif /* _DEBUG */
    
    s_aButtonHoldDelay[2] = 15;

    ButtonSetLayout(s_aButtonLayout, COUNT_OF(s_aButtonLayout), s_aButtonHoldDelay);
};


void CTouchController::ButtonSetLayoutRideJump(void)
{
    const float radiusS = 48.0f;
    const float radiusB = 56.0f;
    const RwV2d offset = CONTROL_OFFSET;
    const RwV2d anchor = { 1040.0f - offset.x,
                            199.0f - offset.y };

    static const BUTTON_LAYOUT s_aButtonLayout[] =
    {
        { CController::DIGITAL_RRIGHT,  { (anchor.x +  56.0f), (anchor.y + 377.0f) }, radiusB        }, // JUMP
        { CController::DIGITAL_L1,      { (anchor.x +   0.0f), (anchor.y + 273.0f) }, radiusS        }, // TURN LEFT
        { CController::DIGITAL_R1,      { (anchor.x + 128.0f), (anchor.y + 273.0f) }, radiusS        }, // TURN RIGHT
        
        { CController::DIGITAL_L2,      { (anchor.x + 128.0f), (anchor.y +   0.0f) }, radiusS        }, // SWITCH CHR
        { CController::DIGITAL_START,   {                0.0f,                0.0f }, radiusS * 1.0f }, // PAUSE
    };

    ButtonSetLayout(s_aButtonLayout, COUNT_OF(s_aButtonLayout));
};


void CTouchController::ButtonSetLayoutRideShot(void)
{
    const float radiusS = 48.0f;
    const float radiusB = 56.0f;
    const RwV2d offset = CONTROL_OFFSET;
    const RwV2d anchor = { 1040.0f - offset.x,
                            199.0f - offset.y };

    static const BUTTON_LAYOUT s_aButtonLayout[] =
    {
        { CController::DIGITAL_RUP,     { (anchor.x +  56.0f), (anchor.y + 377.0f) }, radiusB        }, // SHOT
        { CController::DIGITAL_L1,      { (anchor.x +   0.0f), (anchor.y + 273.0f) }, radiusS        }, // ROLL LEFT
        { CController::DIGITAL_R1,      { (anchor.x + 128.0f), (anchor.y + 273.0f) }, radiusS        }, // ROLL RIGHT
        
        { CController::DIGITAL_L2,      { (anchor.x + 128.0f), (anchor.y +   0.0f) }, radiusS        }, // SWITCH CHR
        { CController::DIGITAL_START,   {                0.0f,                0.0f }, radiusS * 1.0f }, // PAUSE
    };

    ButtonSetLayout(s_aButtonLayout, COUNT_OF(s_aButtonLayout));
};


void CTouchController::TouchInit(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
        TouchClear(&m_aTouch[i]);
};


void CTouchController::TouchUpdate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        TOUCH* pTouch = &m_aTouch[i];

        if (!pTouch->bInUse)
            continue;

        if (pTouch->bDown)
        {
            pTouch->startPos = pTouch->startPosWin;
            ToVirtualScreenPos(&pTouch->startPos);
        };

        if (m_bOptionChanged)
        {
            TouchClear(pTouch);
            TouchFree(pTouch);
        };
    };
};


void CTouchController::TouchPostUpdate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
        m_aTouch[i].bTrigger = false;

    if (m_bLatchMode)
    {
        for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
        {
            if (m_aTouch[i].bLock && m_aTouch[i].bUp)
            {
                m_aTouch[i].bLock = false;
                TouchEvent(TOUCH::EVENT_UP, m_aTouch[i].id, 0.0f, 0.0f);
            };
        };
    };
};


void CTouchController::TouchDraw(void) const
{
#ifdef _DEBUG
    if (!m_bDebugDraw)
        return;
    
    float w = 0.0f;
    float h = 0.0f;
    GetSDLWindowSize(&w, &h);

    m_font.Color({ 0xFF, 0xFF, 0xFF, 0xFF });
    m_font.Background({ 0x00, 0x00, 0x00, 0xFF });
    m_font.Position(static_cast<int32>(w * 0.5f) - 100,
                    static_cast<int32>(h * 0.5f) - 20);
    m_font.SetAutoStep(0, 20);

    char szTextBuffer[256];
    szTextBuffer[0] = '\0';

    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        const TOUCH* pTouch = &m_aTouch[i];

        if (!pTouch->bDown)
            continue;

        std::sprintf(szTextBuffer, "%llu -- %.2f %.2f -- %.2f %.2f",
                     pTouch->id,
                     pTouch->currentPos.x, pTouch->currentPos.y,
                     pTouch->startPos.x, pTouch->startPos.y);

        m_font.Print(szTextBuffer);
    };
#endif /* _DEBUG */
};


CTouchController::TOUCH* CTouchController::TouchAlloc(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        if (!m_aTouch[i].bInUse)
        {
            m_aTouch[i].bInUse = true;
            return &m_aTouch[i];
        };
    };

    return nullptr;
};


void CTouchController::TouchFree(TOUCH* pTouch)
{
    pTouch->bInUse = false;
};


void CTouchController::TouchEventProxy(TOUCH::EVENT type, SDL_FingerID id, float x, float y)
{
    float w = 0.0f;
    float h = 0.0f;
    GetSDLWindowSize(&w, &h);

    x *= w;
    y *= h;

    PushVirtualScreen(VIRTUAL_SCREEN_SIZE.x,
                      VIRTUAL_SCREEN_SIZE.y,
                      VIRTUAL_SCREEN_SIZE.w,
                      VIRTUAL_SCREEN_SIZE.h);

    TouchEvent(type, id, x, y);

    PopVirtualScreen();
};


void CTouchController::TouchEvent(TOUCH::EVENT type, SDL_FingerID id, float x, float y)
{
    switch (type)
    {
    case TOUCH::EVENT_DOWN:
        {
            TOUCH* pTouch = TouchAlloc();
            if (pTouch)
            {
                pTouch->bDown = true;
                pTouch->bTrigger = true;
                pTouch->id = id;
                pTouch->startPosWin = RwV2d{ x, y };

                pTouch->startPos = RwV2d{ x, y };
                ToVirtualScreenPos(&pTouch->startPos);

                pTouch->currentPos = RwV2d{ x, y };
                ToVirtualScreenPos(&pTouch->currentPos);

                STICK* pStick = TouchCheckHitStickArea(pTouch);
                if (pStick)
                {
                    pStick->pTouch = pTouch;
                    pStick->pos = pTouch->currentPos;
                    pStick->thumbPos = pStick->pos;

                    pTouch->bStickHit = true;
                }
                else
                {
                    if (m_bLatchMode)
                    {
                        ASSERT(pTouch->bUp == false);
                        ASSERT(pTouch->bLock == false);
                        pTouch->bLock = true;
                    };
                };
            };
        }
        break;

    case TOUCH::EVENT_MOTION:
        {
            TOUCH* pTouch = TouchFind(id);
            if (pTouch)
            {
                pTouch->bTrigger = false;

                pTouch->currentPos = RwV2d{ x, y };
                ToVirtualScreenPos(&pTouch->currentPos);
            };
        }
        break;

    case TOUCH::EVENT_UP:
        {
            TouchForceFullscreenOnLost();

            TOUCH* pTouch = TouchFind(id);
            if (pTouch)
            {
                if (m_bLatchMode)
                {
                    if (pTouch->bLock)
                    {
                        pTouch->bUp = true;
                        break;
                    };
                };

                float dx = (x - pTouch->startPosWin.x);
                float dy = (y - pTouch->startPosWin.y);

                float distSQ = (dx * dx +
                                dy * dy);

                if (distSQ <= 15.0f)
                    TapOccurred();

                TouchClear(pTouch);
                TouchFree(pTouch);
            };
        }
        break;

    default:
        break;
    };
};


CTouchController::TOUCH* CTouchController::TouchFind(SDL_FingerID id)
{
    for (int32 i = 0; i < COUNT_OF(m_aTouch); ++i)
    {
        if (m_aTouch[i].id == id)
            return &m_aTouch[i];
    };

    return nullptr;
};


void CTouchController::TouchClear(TOUCH* pTouch)
{
    for (int32 i = 0; i < COUNT_OF(m_aStick); ++i)
    {
        if (m_aStick[i].pTouch == pTouch)
        {
            m_aStick[i].pTouch = nullptr;
            break;
        };
    };
    
    pTouch->bDown = false;
    pTouch->bTrigger = false;
    pTouch->bStickHit = false;
    pTouch->id = 0;
    pTouch->startPosWin = Math::VECTOR2_ZERO;
    pTouch->startPos = Math::VECTOR2_ZERO;
    pTouch->currentPos = Math::VECTOR2_ZERO;
    pTouch->bLock = false;
    pTouch->bUp = false;
};


CTouchController::STICK*
CTouchController::TouchCheckHitStickArea(const TOUCH* pTouch)
{
    for (int32 i = 0; i < COUNT_OF(m_aStick); ++i)
    {
        STICK* pStick = &m_aStick[i];

        RwV2d stickAreaPos = RwV2d{ pStick->area.x,
                                    pStick->area.y };

        RwV2d stickAreaSize = RwV2d{ pStick->area.w,
                                     pStick->area.h };
        
        RwV2d touchPoint = pTouch->currentPos;

        if (IsRectContainPoint(&stickAreaPos, &stickAreaSize, &touchPoint, false))
            return pStick;    
    };

    return nullptr;
};


void CTouchController::TouchForceFullscreenOnLost(void)
{
    bool bUserActive = false;

//    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
//        bUserActive |= ((m_aButton[i].bDown) && (m_aButton[i].delay == 0) ? true : false);
//
//    for (int32 i = 0; i < COUNT_OF(m_aStick); ++i)
//        bUserActive |= (m_aStick[i].pTouch ? true : false);

    if (!CWebSpecific::IsInFullscreen())
        bUserActive |= true;

    if (bUserActive)
    {
        if (!CWebSpecific::IsInFullscreen())
            CWebSpecific::RequestFullscreen(true);
    };
};


void CTouchController::TapInit(void)
{
    std::memset(&m_tap, 0, sizeof(m_tap));
};


void CTouchController::TapUpdate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_tap.trigger); ++i)
        m_tap.trigger[i] = false;

    if (m_tap.counter > 0)
    {
        uint64 nowTimestamp = TapGetNowTimestamp();
        uint64 elapsed = nowTimestamp - m_tap.startTimestamp;

        if (elapsed >= TAP_DURATION)
        {
            m_tap.trigger[m_tap.counter - 1] = true;
            m_tap.counter = 0;
        };
    };    
};


void CTouchController::TapOccurred(void)
{
    if (m_tap.counter < TAP_MAX)
    {
        ++m_tap.counter;
        m_tap.startTimestamp = TapGetNowTimestamp();
    };
};


uint64 CTouchController::TapGetNowTimestamp(void) const
{
    return SDL_GetTicks();
};


void CTouchController::OptionUpdate(void)
{
    m_bOptionChanged = false;

    bool bLeftHanded = false;
    bool bFloating = false;
    bool bDynamic = false;

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    bLeftHanded = CGameData::Option().Touch().IsLeftHanded();
    bFloating = CGameData::Option().Touch().IsFloating();
    bDynamic = CGameData::Option().Touch().IsDynamic();
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */ 

    if (bLeftHanded != m_bLeftHanded)
    {
        m_bOptionChanged |= true;
        m_bLeftHanded = bLeftHanded;
    };

    if (bFloating != m_bFloating)
    {
        m_bOptionChanged |= true;
        m_bFloating = bFloating;
    };

    if (bDynamic != m_bDynamic)
    {
        m_bOptionChanged |= true;
        m_bDynamic = bDynamic;
    };
};


bool CTouchController::OnSDLEvent(SDL_Event* event)
{
#define TouchEvent TouchEventProxy

    switch (event->type)
    {
    case SDL_EVENT_FINGER_DOWN:
        TouchEvent(TOUCH::EVENT_DOWN, event->tfinger.fingerID, event->tfinger.x, event->tfinger.y);
        break;

    case SDL_EVENT_FINGER_MOTION:
        TouchEvent(TOUCH::EVENT_MOTION, event->tfinger.fingerID, event->tfinger.x, event->tfinger.y);
        break;

    case SDL_EVENT_FINGER_UP:
        TouchEvent(TOUCH::EVENT_UP, event->tfinger.fingerID, event->tfinger.x, event->tfinger.y);
        break;

    default:
        break;
    };

#undef TouchEvent

    return true;
};


void CTouchController::GetSDLWindowSize(float* w, float* h) const
{
    int32 wi = 0;
    int32 hi = 0;
    SDL_GetWindowSize(CWebSpecific::m_pWindow, &wi, &hi);

    *w = static_cast<float>(wi);
    *h = static_cast<float>(hi);
};


void CTouchController::UpdatePhysicalController(void)
{
    int32 controller = GetTouchController();
    if ((controller < 0) || (controller >= CController::Max()))
        return;

    IPhysicalController& touch = CInputsDevice::Instance().ControllerResource(controller);
    touch.Clear();

    IPhysicalController::INFO& info = touch.Info();

    info.m_eState = CController::STATE_CONNECT;

    uint32 digital = 0;

    for (int32 i = 0; i < COUNT_OF(m_aButton); ++i)
    {
        if (m_aButton[i].bDown)
            digital |= m_aButton[i].digital;
    };

    int32 lx = static_cast<int32>( m_aStick[0].pTouch ? (m_aStick[0].x * static_cast<float>(TYPEDEF::SINT16_MAX)) : 0 );
    int32 ly = static_cast<int32>( m_aStick[0].pTouch ? (m_aStick[0].y * static_cast<float>(TYPEDEF::SINT16_MAX)) : 0 );

    int32 rx = 0;
    int32 ry = 0;

    if (COUNT_OF(m_aStick) > 1)
    {
        rx = static_cast<int32>( m_aStick[1].pTouch ? (m_aStick[1].x * static_cast<float>(TYPEDEF::SINT16_MAX)) : 0 );
        ry = static_cast<int32>( m_aStick[1].pTouch ? (m_aStick[1].y * static_cast<float>(TYPEDEF::SINT16_MAX)) : 0 );
    };

    const int32 DEADZONE = static_cast<int32>(static_cast<float>(TYPEDEF::SINT16_MAX) * 0.10f);

    info.m_aAnalog[CController::ANALOG_LSTICK_X]    = static_cast<int16>(ClampValue(lx, DEADZONE));
    info.m_aAnalog[CController::ANALOG_LSTICK_Y]    = static_cast<int16>(ClampValue(-1 - ly, DEADZONE));
    info.m_aAnalog[CController::ANALOG_RSTICK_X]    = static_cast<int16>(ClampValue(rx, DEADZONE));
    info.m_aAnalog[CController::ANALOG_RSTICK_Y]    = static_cast<int16>(ClampValue(-1 - ry, DEADZONE));
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


int32 CTouchController::ClampValue(int32 value, int32 clamp) const
{
    return ((value < clamp) && (value > -clamp)) ? 0 : value;
};


void CTouchController::ToVirtualScreenPos(float* x, float* y) const
{
    float w = 0.0f;
    float h = 0.0f;
    GetSDLWindowSize(&w, &h);

    *x = CSprite::m_fVirtualScreenX + ((CSprite::m_fVirtualScreenW / w) * (*x));
    *y = CSprite::m_fVirtualScreenY + ((CSprite::m_fVirtualScreenH / h) * (*y));
};


void CTouchController::ToVirtualScreenPos(RwV2d* v2) const
{
    ToVirtualScreenPos(&v2->x, &v2->y);
};


bool CTouchController::IsRectContainPoint(const RwV2d* pos,
                                          const RwV2d* size,
                                          const RwV2d* point,
                                          bool bRealScreen) const
{
    float x = point->x;

    if (m_bLeftHanded) {
        float w = (bRealScreen ? static_cast<float>(CScreen::Width()) :
                                 CSprite::m_fVirtualScreenW);
        x = (w - x);
    };

    bool isInsideX = (x >= pos->x) && (x <= pos->x + size->x);
    bool isInsideY = (point->y >= pos->y) && (point->y <= pos->y + size->y);

    return (isInsideX && isInsideY);
};


bool CTouchController::IsCircleContainPoint(const RwV2d* pos,
                                            float fRadius,
                                            const RwV2d* point,
                                            bool bRealScreen) const
{
    float x = point->x;

    if (m_bLeftHanded) {
        float w = (bRealScreen ? static_cast<float>(CScreen::Width()) :
                                 CSprite::m_fVirtualScreenW);
        x = (w - x);
    };

    float dx = (x - pos->x);
    float dy = (point->y - pos->y);

    float distSQ = (dx * dx + dy * dy);
    float radiusSQ = (fRadius * fRadius);

    return (distSQ <= radiusSQ);
};


bool CTouchController::IsStagePaused(void) const
{
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage)
        return pStage->IsPaused();

    return false;
};


bool CTouchController::IsStageMenuPause(void) const
{
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage)
        return (pStage->GetPauseType() == CGameStage::PAUSETYPE_MENU);

    return false;
};


bool CTouchController::IsStageRetToMenu(void) const
{
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage) {
        return (pStage->GetResult() == CGameStage::RESULT_RET_STAGESEL) ||
               (pStage->GetResult() == CGameStage::RESULT_RET_TITLE);
    };

    return false;
};


bool CTouchController::IsInteractive(void) const
{
    return CGameData::Attribute().IsInteractive();
};


bool CTouchController::IsCostumeAvailable(GAMETYPES::COSTUME costume) const
{
    static const SECRETID::VALUE s_aSamuraiCostumeSecretID[] =
    {
        SECRETID::ID_HOME_SAMURAI_LEO,
        SECRETID::ID_HOME_SAMURAI_RAP,
        SECRETID::ID_HOME_SAMURAI_MIC,
        SECRETID::ID_HOME_SAMURAI_DON,
    };

    static const SECRETID::VALUE s_aNexusCostumeSecretID[] =
    {
        SECRETID::ID_HOME_NEXUS_LEO,
        SECRETID::ID_HOME_NEXUS_RAP,
        SECRETID::ID_HOME_NEXUS_MIC,
        SECRETID::ID_HOME_NEXUS_DON,
    };

    const SECRETID::VALUE* aSecretID = nullptr;
    int32 numSecretID = 0;

    switch (costume)
    {
    case GAMETYPES::COSTUME_SAMURAI:
        aSecretID = s_aSamuraiCostumeSecretID;
        numSecretID = COUNT_OF(s_aSamuraiCostumeSecretID);
        break;

    case GAMETYPES::COSTUME_NEXUS:
        aSecretID = s_aNexusCostumeSecretID;
        numSecretID = COUNT_OF(s_aNexusCostumeSecretID);
        break;

    default:
        ASSERT(false);
        break;
    };

    for (int32 i = 0; i < numSecretID; ++i)
    {
        if (CGameData::Record().Secret().IsUnlockedSecret(aSecretID[i]))
            return true;
    };

    return false;
};

#endif /* defined(TARGET_WEB) */