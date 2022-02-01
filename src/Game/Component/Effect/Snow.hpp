#pragma once


class CMapCamera;


class CSnowManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Run(void);
    static void Draw(void);
    static void StartSnow(void);
    static void StopSnow(void);
    static void SetNormalSnow(void);
    static void SetRideSnow(void);
    static void SetCameraPosition(CMapCamera* pMapCamera);
    static void SetBasisPosition(const RwV3d* pvPosition);
};