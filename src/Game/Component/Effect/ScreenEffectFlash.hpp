#pragma once


class CScreenEffectFlash
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Run(void);
    static void Draw(void);
    static void Flash(void);
    static void SetColor(const RwRGBA& color);
};