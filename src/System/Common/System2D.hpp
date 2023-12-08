#pragma once


class CSystem2D
{
public:
    static bool Initialize(void);
    static void Terminate(void);
    static bool BeginScene(void);
    static void EndScene(void);
    static void PushRenderState(void);
    static void PopRenderState(void);
    static bool Reset(void);
};