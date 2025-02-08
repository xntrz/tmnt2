#pragma once

#include "MessageWindow.hpp"

#include "Game/System/Misc/Gamepad.hpp"


class CDialog : public CMessageWindow
{
public:
    enum STATUS
    {
        STATUS_YES = 0,
        STATUS_NO,
    };
    
public:
    CDialog(COLOR color = COLOR_NORMAL,
            STATUS defaultStatus = STATUS_YES,
            int32 iController = CController::CONTROLLER_LOCKED_ON_VIRTUAL);
    
    virtual ~CDialog(void);
    virtual void SetSprite(float x, float y, float w, float h) override;
    virtual void DrawInWindow(const Rt2dBBox& bbox) const override;
    virtual void Input(void) override;    
    void MoveCursor(void);
    void RotateCursor(void);
    void GetPositionYesNo(RwV2d* pPos, STATUS status) const;
    void SetStatus(STATUS stauts);
    STATUS GetStatus(void) const;
    void SetController(int32 iController);
    int32 GetController(void) const;
    
protected:
    STATUS m_status;
    float m_fCursorRotation;
    int32 m_iController;
};