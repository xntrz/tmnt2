#pragma once


class CEffectLightManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Run(void);
    static void Draw(void);
    static uint32 Regist(const RwV3d* pvPosition, float fRadius, const RwRGBA& color);
    static void Remove(uint32 hLight);
    static void SetPosition(uint32 hLight, const RwV3d* pvPosition);
    static void SetColor(uint32 hLight, const RwRGBA& color);
    static void SetRadius(uint32 hLight, float fRadius);
};