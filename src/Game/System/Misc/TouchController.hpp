#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "System/Common/Process/Process.hpp"
#include "System/Common/Sprite.hpp"

#if defined(TARGET_WEB)

#include "Game/System/Misc/Gamepad.hpp"
#include "Game/Sequence/Test/DebugUtils.hpp"

#include "SDL3/SDL.h"


class CTouchController : public CProcess
{
private:
    static const Rt2dBBox VIRTUAL_SCREEN_SIZE;
    static const float STICK_RADIUS;
    static const float BUTTON_RADIUS;
    static const float BUTTON_HIT_RADIUS_SCALE;
    static const RwV2d CONTROL_OFFSET;

    static const int32 STICK_MAX = 1; //CController::STICK_NUM;
    static const int32 BUTTON_MAX = CController::DIGITAL_NUM / 2;
    static const int32 TOUCH_MAX = 4;
    static const int32 TAP_MAX = 2;
    static const uint64 TAP_DURATION = 200; // ms

    class CSpriteEx : public CSprite
    {
    public:
        CSpriteEx(void);
        virtual ~CSpriteEx(void);
        void Move(float x, float y);
        void Resize(float w, float h);
    };

    struct TOUCH
    {
        enum EVENT
        {
            EVENT_DOWN = 0,
            EVENT_MOTION,
            EVENT_UP,
        };

        bool         bInUse;
        bool         bDown;
        bool         bTrigger;
        bool         bStickHit;
        SDL_FingerID id;
        RwV2d        startPosWin;
        RwV2d        startPos;
        RwV2d        currentPos;
        bool         bLock;
        bool         bUp;
    };

    struct BUTTON
    {
        uint32      digital;
        RwV2d       pos;
        float       fRadius;
        RwTexture*  pTexture;
        bool        bEnabled;
        int32       holdDelay;

        bool        bDown;
        int32       delay;
    };

    struct BUTTON_LAYOUT
    {
        uint32  digital;
        RwV2d   pos;
        float   fRadius;
    };

    struct STICK
    {
        Rt2dBBox area;
        RwV2d    pos;
        RwV2d    thumbPos;
        float    fRadius;
        float    x;
        float    y;
        TOUCH*   pTouch;
    };

    struct TAP
    {
        uint64  startTimestamp;
        int32   counter;
        bool    trigger[TAP_MAX];
    };

    enum MENU_BUTTON
    {
        MENU_BUTTON_WEAK_ATTACK = (1 << 0),
        MENU_BUTTON_JUMP        = (1 << 1),
        MENU_BUTTON_GUARD       = (1 << 2),
        MENU_BUTTON_OK          = (1 << 3),
        MENU_BUTTON_CANCEL      = (1 << 4),
        MENU_BUTTON_COSTUME_NEX = (1 << 5),
        MENU_BUTTON_COSTUME_SAM = (1 << 6),
    };

    enum ANCHOR
    {
        ANCHOR_POS_TOP_LEFT = 0,
        ANCHOR_POS_BOTTOM_RIGHT,

        ANCHOR_FLAG_REAL_SCREEN = 0x100,

        ANCHOR_MASK_POS = 0x00FF,
        ANCHOR_MASK_FLAG = 0xFF00,
    };

public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pSender);
    static void Terminate(CProcess* pSender);

    CTouchController(void);
    virtual ~CTouchController(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
    void StickInit(void);
    void StickUpdateArea(void);
    void StickUpdate(void);
    void StickDraw(void) const;
    void ButtonInit(void);
    void ButtonUpdate(void);
    void ButtonUpdateGesture(void);
    void ButtonUpdateNormal(void);
    void ButtonDraw(void) const;
    void ButtonDrawName(void) const;
    void ButtonDrawCenteredText(const char* pszText, float r, float h, float x, float y) const;
    void ButtonDrawSinglelineCenteredText(const char* pszText, float r, float h, float x, float y) const;
    void ButtonDrawMultilineCenteredText(const char* pszText, int32 lineNum, float r, float h, float x, float y) const;
    void ButtonDrawCenteredString(const char* pszText, float r, float h, float x, float y) const;
    void ButtonPosCorrection(float* x, float* y, uint32 anchor) const;
    void ButtonUpdateLayout(void);
    void ButtonSetLayout(const BUTTON_LAYOUT* pLayout, int32 count, const int32* pHoldDelayTable = nullptr);
    void ButtonSetLayoutEmpty(void);
    void ButtonSetLayoutMenu(int32 menuButtonFlag);
    void ButtonSetLayoutNormal(bool bSetPause = true);
    void ButtonSetLayoutRideJump(void);
    void ButtonSetLayoutRideShot(void);
    void TouchInit(void);
    void TouchUpdate(void);
    void TouchPostUpdate(void);
    void TouchDraw(void) const;
    TOUCH* TouchAlloc(void);
    void TouchFree(TOUCH* pTouch);
    void TouchEventProxy(TOUCH::EVENT type, SDL_FingerID id, float x, float y);
    void TouchEvent(TOUCH::EVENT type, SDL_FingerID id, float x, float y);
    TOUCH* TouchFind(SDL_FingerID id);
    void TouchClear(TOUCH* pTouch);
    STICK* TouchCheckHitStickArea(const TOUCH* pTouch);
    void TouchForceFullscreenOnLost(void);
    void TapInit(void);
    void TapUpdate(void);
    void TapOccurred(void);
    uint64 TapGetNowTimestamp(void) const;
    void OptionUpdate(void);
    bool OnSDLEvent(SDL_Event* event);
    void GetSDLWindowSize(float* w, float* h) const;
    void UpdatePhysicalController(void);
    int32 ClampValue(int32 value, int32 clamp) const;
    void ToVirtualScreenPos(float* x, float* y) const;
    void ToVirtualScreenPos(RwV2d* v2) const;
    bool IsRectContainPoint(const RwV2d* pos, const RwV2d* size, const RwV2d* point, bool bRealScreen) const;
    bool IsCircleContainPoint(const RwV2d* pos, float fRadius, const RwV2d* point, bool bRealScreen) const;
    bool IsStagePaused(void) const;
    bool IsStageMenuPause(void) const;
    bool IsStageRetToMenu(void) const;
    bool IsInteractive(void) const;
    bool IsCostumeAvailable(GAMETYPES::COSTUME costume) const;
    void CheckInputActivateFullscreen(void);

private:
    static int32            m_iRefCount;
    static bool             m_bDebugDraw;
    static bool             m_bLeftHanded; // makes touch controller for left hand
    static bool             m_bFloating; // allows stick follows to finger if outside radius
    static bool             m_bDynamic; // allows to trigger stick from any screen place
    static bool             m_bLatchMode; // try to avoid input lost
    float                   m_fAlphaRatio;
    mutable CSpriteEx       m_sprite;
    TOUCH                   m_aTouch[TOUCH_MAX];
    BUTTON                  m_aButton[BUTTON_MAX];
    STICK                   m_aStick[STICK_MAX];
    TAP                     m_tap;
    RwTexture*              m_pTexStick;
    RwTexture*              m_pTexButton;
    bool                    m_bOptionChanged;
    bool                    m_bGestureMode;
#ifdef _DEBUG
    mutable CDebugFontCtrl  m_font;
#endif /* _DEBUG */
};

#endif /* defined(TARGET_WEB) */