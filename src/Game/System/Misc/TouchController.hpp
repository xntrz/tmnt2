#pragma once

#include "System/Common/Process/Process.hpp"
#include "System/Common/Sprite.hpp"

#if defined(TARGET_WEB)

#include "System/Web/WebPhysicalController.hpp"
#include "Game/Sequence/Test/DebugUtils.hpp"

#include "SDL3/SDL.h"


#ifdef _DEBUG
#define TOUCH_EDITOR
#endif /* _DEBUG */


class CTouchControllerProcess : public CProcess
{
private:
    static const float STICK_RADIUS;
    static const float STICK_THUMB_RADIUS_RATIO;
    static const RwV2d STICK_AREA_ANCHOR;
    static const RwV2d STICK_AREA_SIZE;

    static const RwV2d BUTTON_AREA_ANCHOR;
    static const float BUTTON_RADIUS;

    struct TOUCH_DEVICE
    {
        const char*         pszName;
        SDL_TouchDeviceType type;
        SDL_TouchID         id;
    };

    enum TOUCH_EVENT
    {
        TOUCH_EVENT_DOWN = 0,
        TOUCH_EVENT_MOTION,
        TOUCH_EVENT_UP,
    };

    struct TOUCH
    {        
        bool         bDown;
        bool         bTrigger;
        SDL_FingerID id;
        float        fWinStartPosY;
        float        fWinStartPosX;
        float        fStartPosX;
        float        fStartPosY;
        float        fCurrentPosX;
        float        fCurrentPosY;
    };

    struct BUTTON
    {        
        RwV2d       vecPos;
        RwV2d       vecOffset;
        RwTexture*  pTexture;
        bool        bIsHold;
        uint32      digital;
    };

    struct STICK
    {
        RwV2d  vecPos;
        RwV2d  vecThumbPos;
        float  fRadius;        
        TOUCH* pTouch;
        float  lx;
        float  ly;
    };

public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pSender);
    static void Terminate(CProcess* pSender);

    CTouchControllerProcess(void);
    virtual ~CTouchControllerProcess(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
    void UpdateTouchStartPos(void);
    void UpdateButtonHit(void);
    void UpdateStickHit(void);
    void DrawButtonOverlay(void) const;
    void DrawStickOverlay(void) const;
    void DrawFingerInfo(void) const;
    void UpdatePhysicalController(void);
    bool OnEvent(SDL_Event* event);
    void OnTouchEvent(TOUCH_EVENT event, SDL_FingerID id, float x, float y);
    bool IsTouchExist(SDL_FingerID id) const;
    void WinPosToVirtualScreenPos(float* x, float* y) const;
    bool IsStickTouch(const TOUCH* pTouch) const;

private:
    uint8           m_overlayAlpha;
    mutable CSprite m_sprite;
    int32           m_numTouchDevices;
    SDL_TouchID*    m_aTouchDevices;
    TOUCH           m_aTouch[4]; // up to 4 simultaneous touches maximum
    BUTTON          m_aButton[BITSOF(CController::DIGITAL)];
    STICK           m_stick;
#ifdef _DEBUG
    mutable CDebugFontCtrl m_font;
#endif /* _DEBUG */
};

#endif /* defined(TARGET_WEB) */