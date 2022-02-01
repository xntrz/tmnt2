#pragma once


class CDebugShape
{
public:
    static float m_fLabelHeight;
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void FrameBegin(void);
    static void FrameEnd(void);
    static void Draw3D(void);
    static void Draw2D(void);
    static void ShowSphere(const RwSphere* pSphere);
    static void ShowSphere(const RwSphere* pSphere, const RwRGBA& rColor);
    static void ShowLine(const RwLine* pLine);
    static void ShowLine(const RwLine* pLine, const RwRGBA& rColorStart, const RwRGBA& rColorEnd);
    static void ShowPlane(const RwV3d aPoint[4]);
    static void ShowPlane(const RwV3d aPoint[4], const RwRGBA& rColor);
    static void ShowBox(const RwV3d aPoint[8]);
    static void ShowBox(const RwV3d aPoint[8], const RwRGBA& rColor);
    static void ShowLabel(const RwV3d* pvPosition, const char* pszLabel);
    static void ShowLabel(const RwV3d* pvPosition, const char* pszLabel, const RwRGBA& rColor);
};