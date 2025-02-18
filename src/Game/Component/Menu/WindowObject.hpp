#pragma once

#include "System/Common/Sprite.hpp"


class CWindowObject
{
protected:
    static const int32 SPRITENUM = 4;
    static const float OPENTIME;
    
    enum PHASE
    {
        PHASE_NONE = 0,
        PHASE_OPEN,
        PHASE_ACTIVE,
        PHASE_ACTION,
        PHASE_CLOSE,
        PHASE_END,
    };

    class CMessageText
    {
    public:
        CMessageText(void);
        virtual ~CMessageText(void);
        void SetText(const wchar* pwszText, float fHeight, const RwRGBA& rColor);
        void SetText(const wchar* pwszText);
        void Draw(const Rt2dBBox& bbox) const;
        const wchar* Text(void) const;
        
    protected:
        wchar* m_pwszText;
        float  m_fHeight;
        RwRGBA m_Color;
    };

    struct SPRITETEXTURE
    {
        const char* m_pszName;
        RwTexture*  m_pTexture;
    };

public:
    static void Load(void);
    
    CWindowObject(void);
    virtual ~CWindowObject(void);
    virtual void SetSprite(float x, float y, float w, float h);
    virtual void DoneInput(void);
    virtual void DrawInWindow(const Rt2dBBox& bbox) const;
    virtual void Input(void);
    virtual void InputAction(void);
    void Period(void);
    void Draw(void) const;
    void Open(void);
    void Close(void);
    void Opening(void);
    void Closing(void);
    void SetPositionOnOpening(void);
    void SetPositionOnClosing(void);
    void SetPosition(Rt2dBBox& bbox);
    void SetPosition(float x, float y, float w, float h);
    void Set(float x, float y, float w, float h);
    void SetTexture(void);
    void SetTitle(const wchar* pwszText, float fHeight, const RwRGBA& rColor);    
    void SetTitle(const wchar* pwszText);
    void SetText(const wchar* pwszText, float fHeight, const RwRGBA& rColor);
    void SetText(const wchar* pwszText);
    void SetOpenAction(bool bSet);
    bool IsOpen(void) const;
    bool IsActive(void) const;

protected:
    static SPRITETEXTURE m_aSpriteTextureTable[SPRITENUM];
    CSprite              m_aSprite[SPRITENUM];
    CMessageText         m_textMsg;
    CMessageText         m_textTitle;
    Rt2dBBox             m_bboxDraw;
    Rt2dBBox             m_bbox;
    PHASE                m_phase;
    float                m_fTime;
    bool                 m_bOpenAction;
};