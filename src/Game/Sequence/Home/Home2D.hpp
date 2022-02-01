#pragma once


class CHomeCamera;


class CHome2D
{
public:
    static void Initialize(CHomeCamera* pHomeCamera);
    static void Terminate(void);
    static void Period(void);
    static void Draw(void);
    static void ResetTime(void);
};