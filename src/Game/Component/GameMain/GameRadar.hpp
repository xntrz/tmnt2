#pragma once

class CGameRadar
{
public:
    static void Initialize(float fDistance);
    static void Terminate(void);
    static void Update(void);
    static void Draw(void);
    static void SetEnable(bool bState);
    static bool IsEnabled(void);
};