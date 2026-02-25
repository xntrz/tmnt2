#include "RenderStateManager.hpp"

#include "System/Common/RenderState.hpp"


class IStateBlock
{
public:
    virtual void Push(void) = 0;
    virtual void Pop(void) = 0;
};


class CDrawModelBlock final : public IStateBlock
{
public:
    virtual void Push(void) override;
    virtual void Pop(void) override;

private:
    bool m_bVertexAlpha;
    RwTextureAddressMode m_textureAddressU;
    RwTextureAddressMode m_textureAddressV;
};


class CDrawIm2DBlock final : public IStateBlock
{
public:
    virtual void Push(void) override;
    virtual void Pop(void) override;
};


class CDrawIm3DBlock final : public IStateBlock
{
public:
    virtual void Push(void) override;
    virtual void Pop(void) override;
};


class CDrawWorldBlock final : public IStateBlock
{
public:
    virtual void Push(void) override;
    virtual void Pop(void) override;
};


class CBlendAlphaBlock final : public IStateBlock
{
public:
    virtual void Push(void) override;
    virtual void Pop(void) override;
};


class CBlendAddBlock final : public IStateBlock
{
public:
    virtual void Push(void) override;
    virtual void Pop(void) override;
};


class CBlendSubBlock final : public IStateBlock
{
public:
    virtual void Push(void) override;
    virtual void Pop(void) override;
};


void CDrawModelBlock::Push(void)
{
    RwRenderStateGet(rwRENDERSTATETEXTUREADDRESSU, &m_textureAddressU);
    RwRenderStateGet(rwRENDERSTATETEXTUREADDRESSU, &m_textureAddressV);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &m_bVertexAlpha);
};


void CDrawModelBlock::Pop(void)
{
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)m_textureAddressU);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)m_textureAddressV);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)m_bVertexAlpha);
};


void CDrawIm2DBlock::Push(void)
{
    ;
};


void CDrawIm2DBlock::Pop(void)
{
    ;
};


void CDrawIm3DBlock::Push(void)
{
    ;
};


void CDrawIm3DBlock::Pop(void)
{
    ;
};


void CDrawWorldBlock::Push(void)
{
    ;
};


void CDrawWorldBlock::Pop(void)
{
    ;
};


void CBlendAlphaBlock::Push(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
};


void CBlendAlphaBlock::Pop(void)
{
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
};


void CBlendAddBlock::Push(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
};


void CBlendAddBlock::Pop(void)
{
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
};


void CBlendSubBlock::Push(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDZERO);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
};


void CBlendSubBlock::Pop(void)
{
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
};


struct RENDERSTATEDEFAULT
{
    RwTextureAddressMode m_textureAddressU;
    RwTextureAddressMode m_textureAddressV;
    uint32 m_uBorderColor;
    uint32 m_bTexturePerspective;
    RwTextureFilterMode m_textureFilter;
    uint32 m_bZTest;
    uint32 m_bZWrite;
    RwShadeMode m_shadeMode;
    uint32 m_bVertexAlpha;
    RwBlendFunction m_srcBlendFunc;
    RwBlendFunction m_dstBlendFunc;
    uint32 m_bFog;
    uint32 m_uFogColor;
    RwFogType m_fogType;
    RwCullMode m_cullMode;
    RwAlphaTestFunction m_alphaTestFunc;
    uint32 m_byAlphaTestRef;
};


static IStateBlock* s_pCurrentBlock = nullptr;
static RwRGBA s_FogColor = { 0x00, 0x00, 0x00, 0x00 };
static RENDERSTATEDEFAULT s_RsDefault =
{
    rwTEXTUREADDRESSWRAP,
    rwTEXTUREADDRESSWRAP,
    0,
    uint32(true),
    rwFILTERLINEAR,
    uint32(true),
    uint32(true),
    rwSHADEMODEGOURAUD,
    uint32(true),
    rwBLENDSRCALPHA,
    rwBLENDINVSRCALPHA,
    uint32(false),
    0,
    rwFOGTYPELINEAR,
    rwCULLMODECULLBACK,
    rwALPHATESTFUNCTIONGREATER,
    uint32(false),
};


/*static*/ void CRenderStateManager::BeginBlock(BLOCK block)
{
    static CDrawModelBlock s_DrawModelBlock;
    static CDrawIm2DBlock s_DrawIm2DBlock;
    static CDrawIm3DBlock s_DrawIm3DBlock;
    static CDrawWorldBlock s_DrawWorldBlock;
    static CBlendAlphaBlock s_BlendAlphaBlock;
    static CBlendAddBlock s_BlendAddBlock;
    static CBlendSubBlock s_BlendSubBlock;
    
    ASSERT(s_pCurrentBlock == nullptr);
    
    switch (block)
    {
    case BLOCK_MODEL:
        s_pCurrentBlock = &s_DrawModelBlock;
        break;

    case BLOCK_IM2D:
        s_pCurrentBlock = &s_DrawIm2DBlock;
        break;

    case BLOCK_IM3D:
        s_pCurrentBlock = &s_DrawIm3DBlock;
        break;

    case BLOCK_WORLD:
        s_pCurrentBlock = &s_DrawWorldBlock;
        break;

    case BLOCK_BLEND_ALPHA:
        s_pCurrentBlock = &s_BlendAlphaBlock;
        break;

    case BLOCK_BLEND_ADD:
        s_pCurrentBlock = &s_BlendAddBlock;
        break;

    case BLOCK_BLEND_SUB:
        s_pCurrentBlock = &s_BlendSubBlock;
        break;

    case BLOCK_NONE:
        s_pCurrentBlock = nullptr;
        break;

    default:
        ASSERT(false);
        break;
    };

    if (s_pCurrentBlock)
        s_pCurrentBlock->Push();
};


/*static*/ void CRenderStateManager::EndBlock(void)
{
    ASSERT(s_pCurrentBlock != nullptr);
    
    s_pCurrentBlock->Pop();
    s_pCurrentBlock = nullptr;
};


/*static*/ void CRenderStateManager::SetDefault(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSU, s_RsDefault.m_textureAddressU);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSV, s_RsDefault.m_textureAddressV);
    RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, s_RsDefault.m_uBorderColor);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, s_RsDefault.m_bTexturePerspective);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, s_RsDefault.m_textureFilter);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, s_RsDefault.m_bZTest);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, s_RsDefault.m_bZWrite);
    RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, s_RsDefault.m_shadeMode);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, s_RsDefault.m_bVertexAlpha);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, s_RsDefault.m_srcBlendFunc);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, s_RsDefault.m_dstBlendFunc);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, s_RsDefault.m_bFog);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGCOLOR, RWRGBALONG(s_FogColor.red, s_FogColor.green, s_FogColor.blue, s_FogColor.alpha));
    RENDERSTATE_PUSH(rwRENDERSTATEFOGTYPE, s_RsDefault.m_fogType);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, s_RsDefault.m_cullMode);
    RENDERSTATE_PUSH(rwRENDERSTATEALPHATESTFUNCTION, s_RsDefault.m_alphaTestFunc);
    RENDERSTATE_PUSH(rwRENDERSTATEALPHATESTFUNCTIONREF, s_RsDefault.m_byAlphaTestRef);
};


/*static*/ void CRenderStateManager::SetForDrawBeginning(void)
{
    ;
};


/*static*/ void CRenderStateManager::ClearFogParam(void)
{
    s_FogColor = { 0x00, 0x00, 0x00, 0x00 };
    s_RsDefault.m_bFog = false;
};


/*static*/ void CRenderStateManager::SetFogParam(bool bSet, const RwRGBA& color)
{
    if (bSet)
    {
        s_FogColor = color;
        s_RsDefault.m_bFog = true;
        s_RsDefault.m_uFogColor = RWRGBALONG(s_FogColor.red,
                                             s_FogColor.green,
                                             s_FogColor.blue,
                                             s_FogColor.alpha);
    }
    else
    {
        ClearFogParam();
    };
};

