#pragma once


class CWaterSplashManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Run(void);
    static void Draw(void);
    static void StartWaterSplash(void);
    static void StopWaterSplash(void);
    static void SetBasisPosition(const RwV3d* pvPosition);
    static void SetDefaultTexture(void);
};