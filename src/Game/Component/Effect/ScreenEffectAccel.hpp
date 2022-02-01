#pragma once


class CMapCamera;


class CScreenEffectAccel
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Run(void);
    static void Draw(void);
    static void SetCenter(CMapCamera* pMapCamera);
    static void StartAccel(void);
    static void StopAccel(void);
};