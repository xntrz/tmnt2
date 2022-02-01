#pragma once


class CRenderStateManager
{
public:
    enum BLOCK
    {
        BLOCK_NONE = 0,
        BLOCK_MODEL,
        BLOCK_IM2D,
        BLOCK_IM3D,
        BLOCK_WORLD,
        BLOCK_BLEND_ALPHA,
        BLOCK_BLEND_ADD,
        BLOCK_BLEND_SUB,

        BLOCKNUM,
    };

public:
    static void BeginBlock(BLOCK block);
    static void EndBlock(void);
    static void SetDefault(void);
    static void SetForDrawBeginning(void);
    static void ClearFogParam(void);
    static void SetFogParam(bool bSet, const RwRGBA& color);
};