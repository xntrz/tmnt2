#pragma once


class CIceBlockManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Run(void);
    static void Draw(void);
    static uint32 Play(const RwV3d* pvPosition);
    static void Finish(uint32 hIceBlock);
    static void SetPosition(uint32 hIceBlock, const RwV3d* pvPosition);
    static void SetScale(uint32 hIceBlock, const RwV3d* pvScaling);
};