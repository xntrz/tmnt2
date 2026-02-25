#pragma once


class CRenderState
{
public:
    union _Value
    {
        RwAlphaTestFunction m_AlphaTestFunction;
        RwBlendFunction m_BlendFunction;
        RwStencilFunction m_StencilFunction;
        RwStencilOperation m_StencilOperation;
        RwTextureAddressMode m_TextureAddressMode;
        RwTextureFilterMode m_TextureFilterMode;
        RwShadeMode m_ShadeMode;
        RwCullMode m_CullMode;
        RwFogType m_FogType;
        RwRaster* m_pRaster;
        void* m_pVoid;
        RwReal m_Real;
        int64 m_Long;
        uint32 m_UInt32;
        int32 m_Int32;
        uint8 m_UInt8;
        bool m_Bool;
    };

    CHECK_SIZE(CRenderState::_Value, 0x8);

public:
    static void Initialize(void);

public:
    static _Value m_apValue[BITSOF(RwRenderState)];
};


#define RENDERSTATE_PUSH(rs, value)				       					                    \
    (RwRenderStateGet((rs), reinterpret_cast<void*>(&CRenderState::m_apValue[(rs)])),       \
     RwRenderStateSet((rs), reinterpret_cast<void*>(value)))


#define RENDERSTATE_POP(rs)                                                                 \
    (RwRenderStateSet((rs), reinterpret_cast<void*>(CRenderState::m_apValue[(rs)].m_pVoid)))
