#pragma once


class CGaugeManager
{
public:
    enum ALPHAMODE
    {
        ALPHAMODE_ALPHA = 0,
        ALPHAMODE_ADD,
        ALPHAMODE_SUB,
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Draw(void);
    static void SetGaugeAlphaMode(ALPHAMODE mode);
};