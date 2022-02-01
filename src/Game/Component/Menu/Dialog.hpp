#pragma once

#include "MessageWindow.hpp"


class CDialog : public CMessageWindow
{
public:
    enum STATUS
    {
        STATUS_YES = 0,
        STATUS_NO,
    };
    
public:
    CDialog(COLOR color, STATUS defaultStatus, int32 iController);
    virtual ~CDialog(void);
    virtual void SetSprite(float x, float y, float w, float h) override;
    virtual void DrawInWindow(const Rt2dBBox& bbox) const override;
    virtual void Input(void) override;    
    void MoveCursor(void);
    void RotateCursor(void);
    void GetPositionYesNo(RwV2d* pPos, STATUS status) const;
    void SetStatus(STATUS stauts);
    STATUS GetStatus(void) const;
    int32 GetController(void) const;
    
protected:
    STATUS m_status;
    float m_fCursorRotation;
    int32 m_iController;
};