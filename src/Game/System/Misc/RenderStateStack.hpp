#pragma once


class CRenderStateStack
{
private:
    static const int32 RSVALUE_NUM = BITSOF(RwRenderState) - 1;
    
    union RSVALUE
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
    
public:
    CRenderStateStack(void);
    virtual ~CRenderStateStack(void);
    void Push(RwRenderState rs);

    template<class T>
    void Set(RwRenderState rs, T value);

private:    
    void set(RwRenderState rs, void* pValue);

private:
    RSVALUE m_aValue[RSVALUE_NUM];
    uint32 m_flag;
};


template<class T>
void CRenderStateStack::Set(RwRenderState rs, T value)
{
	Push(rs);
    set(rs, (void*)value);
};