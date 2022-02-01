#pragma once

#include "PCTypedefs.hpp"


class CPCFramework;


class CPCSystem
{
public:
    static LRESULT CALLBACK WndProcRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    CPCSystem(CPCFramework* pFramework);
    ~CPCSystem(void);
    bool Initialize(void);
    void Terminate(void);
    void Run(void);
    void WindowShow(bool bFullscreen);
    void WindowHide(bool bFullscreen);
    void CursorShow(bool bState);
    bool IsCursorHidden(void) const;
    
private:
    bool CheckOS(void);
    bool WindowCreate(void);
    void WindowDestroy(void);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnActivate(HWND hWnd, uint32 state, HWND hwndActDeact, BOOL fMinimized);
    void OnSize(HWND hWnd, uint32 state, int32 w, int32 h);
    void OnSizing(HWND hWnd, RECT* pRect);
    void OnClose(HWND hWnd);
    void OnDestroy(HWND hWnd);
    bool OnSetCursor(HWND hWnd, HWND hWndCursor, uint32 codeHitTest, uint32 ms);
    
private:
    CPCFramework* m_pFramework;
    char m_szOsName[64];
    STICKYKEYS m_StickyKeys;
    TOGGLEKEYS m_ToggleKeys;
    FILTERKEYS m_FilterKeys;
    int32 m_iCursorRef;
    BOOL m_bScreenSavingEnabled;
    bool m_bIsForeground;
    bool m_bCursorState;
};