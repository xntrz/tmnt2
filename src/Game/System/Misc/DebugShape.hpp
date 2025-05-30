#pragma once


class CDebugShape
{
public:
    static float m_fLabelHeight;
    static float m_fLabelOffsetY;
    static float m_fDuration;
    static float m_fLineThickness;
    static float m_fSphereScaleY;

public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(float dt);
    static void FrameBegin(void);
    static void FrameEnd(void);
    static void Draw3D(void);
    static void Draw2D(void);
    static void ShowSphere(const RwSphere* pSphere);
    static void ShowSphere(const RwSphere* pSphere, const RwRGBA& rColor);
    static void ShowSphere(const RwV3d* pvecPos, float fRadius = 0.5f, const RwRGBA& color = { 255, 0, 0, 255 });
    static void ShowSphere(const RwV3d& rvecPos, float fRadius = 0.5f, const RwRGBA& color = { 255, 0, 0, 255 });
    static void ShowSphere(const RwV3d* pvecPos, const RwRGBA& color);
    static void ShowSphere(const RwV3d& rvecPos, const RwRGBA& color);
    static void ShowLine(const RwLine* pLine);
    static void ShowLine(const RwLine* pLine, const RwRGBA& rColorStart, const RwRGBA& rColorEnd);
    static void ShowPlane(const RwV3d aPoint[4]);
    static void ShowPlane(const RwV3d aPoint[4], const RwRGBA& rColor);
    static void ShowBox(const RwV3d aPoint[8]);
    static void ShowBox(const RwV3d aPoint[8], const RwRGBA& rColor);
    static void ShowLabel(const RwV3d* pvPosition, const char* pszLabel);
    static void ShowLabel(const RwV3d* pvPosition, const char* pszLabel, const RwRGBA& rColor);
    static void DurationPush(float t);
    static void DurationPop(void);
};