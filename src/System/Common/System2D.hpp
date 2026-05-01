#pragma once


class CCamera;


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
    static void SetLayerDepth(float fDepth);

private:
    static CCamera* m_pDefaultCamera;
    static CCamera* m_pCurrentCamera;
    static uint32 m_uReferenceCount;
};