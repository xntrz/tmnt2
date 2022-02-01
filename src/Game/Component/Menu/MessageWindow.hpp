#pragma once

#include "WindowObject.hpp"


class CMessageWindow : public CWindowObject
{
public:
    enum PRIORITY
    {
        PRIORITY_NORMAL = 0,
        PRIORITY_TOP,
    };

    enum COLOR
    {
        COLOR_NORMAL = 0,
        COLOR_UNLOCK,
        COLOR_ERROR,
        COLOR_TUTORIAL,
    };

private:
    static const int32 MESSAGEWINDOW_MAX = 5;
    
    struct WINDOWINFO
    {
        CMessageWindow* m_pWindow;
        PRIORITY m_priority;
    };

public:
    static void Initialize(void);
    static void Terminate(void);
    static void DrawNormal(void);
    static void DrawFront(void);
    static void Move(void);

    CMessageWindow(COLOR color);
    virtual ~CMessageWindow(void);
    void SetColor(COLOR color);
    void SetPriority(PRIORITY priority);

protected:
    static int32 m_iWindowNum;    
    static WINDOWINFO m_aWindowTable [];
    static RwRGBA m_aColorTable [];
    int32 m_iWindowIndex;
};