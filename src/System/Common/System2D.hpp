#pragma once


class CSystem2D
{
public:
    static bool Initialize(void);
    static void Terminate(void);
    static bool BeginScene(void);
    static void EndScene(void);
    static bool ScreenChanged(void);
    static void PushRenderState(void);
    static void PopRenderState(void);
    static float VirtualScreenWidth(void);
    static float VirtualScreenHeight(void);
    static float VirtualScreenX(void);
    static float VirtualScreenY(void);
};