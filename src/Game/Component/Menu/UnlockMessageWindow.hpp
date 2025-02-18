#pragma once

#include "MessageWindow.hpp"

#include "Game/Component/GameMain/SecretID.hpp"
#include "System/Common/Sprite.hpp"


class CUnlockMessage : public CMessageWindow
{
private:
    static const int32 PASSWORD_SPRITE_NUM = 7;
    
public:
    static void SetTexture(void);
    
    CUnlockMessage(void);
    virtual ~CUnlockMessage(void);
    virtual void SetSprite(float x, float y, float w, float h) override;
    virtual void DrawInWindow(const Rt2dBBox& bbox) const override;
    virtual void Input(void) override;
    void SetMessage(SECRETID::VALUE idSecret);
    void SetPassword(const char* pszPassword);
    void SetPasswordDraw(bool bSet);
    void SetSizeFromID(SECRETID::VALUE idSecret);    

private:
    static SPRITETEXTURE m_aPasswordTextureTable[];
    CSprite              m_aSpritePassword[PASSWORD_SPRITE_NUM];
    CSprite              m_cursorSprite;
    CMessageText         m_cursorText;
    float                m_fCursorRotation;
    bool                 m_bPasswordDraw;
};