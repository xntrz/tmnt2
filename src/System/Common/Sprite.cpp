#include "Sprite.hpp"
#include "Screen.hpp"
#include "Camera.hpp"
#include "RenderState.hpp"


static inline float SinFloat(float x)
{
    return std::sin(x);
};


static inline float CosFloat(float x)
{
    return std::cos(x);
};


/*static*/ float CSprite::VIRTUALSCREEN_DEFAULT_X = -(TYPEDEF::DEFAULT_SCREEN_WIDTH  * 0.5f);
/*static*/ float CSprite::VIRTUALSCREEN_DEFAULT_Y = -(TYPEDEF::DEFAULT_SCREEN_HEIGHT * 0.5f);
/*static*/ float CSprite::VIRTUALSCREEN_DEFAULT_W = TYPEDEF::DEFAULT_SCREEN_WIDTH;
/*static*/ float CSprite::VIRTUALSCREEN_DEFAULT_H = TYPEDEF::DEFAULT_SCREEN_HEIGHT;
/*static*/ float CSprite::m_fVirtualScreenX = 0.0f;
/*static*/ float CSprite::m_fVirtualScreenY = 0.0f;
/*static*/ float CSprite::m_fVirtualScreenW = 0.0f;
/*static*/ float CSprite::m_fVirtualScreenH = 0.0f;


/*static*/ void CSprite::SetDefaultVirtualScreen(void)
{
    m_fVirtualScreenX = VIRTUALSCREEN_DEFAULT_X;
    m_fVirtualScreenY = VIRTUALSCREEN_DEFAULT_Y;
    m_fVirtualScreenW = VIRTUALSCREEN_DEFAULT_W;
    m_fVirtualScreenH = VIRTUALSCREEN_DEFAULT_H;
};


/*static*/ void CSprite::GetRealScreenPos(float* x, float* y)
{
    *x = ((*x - m_fVirtualScreenX) / m_fVirtualScreenW) * static_cast<float>(CScreen::Width());
    *y = ((*y - m_fVirtualScreenY) / m_fVirtualScreenH) * static_cast<float>(CScreen::Height());
};


/*static*/ void CSprite::PushRenderStates(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE,  false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE,    false);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE,     rwCULLMODECULLNONE);
};


/*static*/ void CSprite::PopRenderStates(void)
{
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
};


CSprite::CSprite(void)
: m_fX(0.0f)
, m_fY(0.0f)
, m_fZ(RwIm2DGetNearScreenZMacro())
, m_fW(static_cast<float>(CScreen::Width()))
, m_fH(static_cast<float>(CScreen::Height()))
, m_fOffsetX(0.5f)
, m_fOffsetY(0.5f)
#ifdef TMNT2_BUILD_EU
, m_aColor()
#else
, m_Color({ 0xFF, 0xFF, 0xFF, 0xFF })
#endif
, m_fU0(0.0f)
, m_fV0(0.0f)
, m_fU1(1.0f)
, m_fV1(1.0f)
, m_fRotate(0.0f)
, m_pTexture(nullptr)
, m_bCameraCheckFlag(false)
{
#ifdef TMNT2_BUILD_EU
    for (int32 i = 0; i < COUNT_OF(m_aColor); ++i)
        m_aColor[i] = { 0xFF, 0xFF, 0xFF, 0xFF };
#endif /* TMNT2_BUILD_EU */
};


CSprite::~CSprite(void)
{
    ;
};


void CSprite::SetZ(float fZ)
{
    m_fZ = fZ;
};


#ifdef TMNT2_BUILD_EU

void CSprite::SetRGBA(int32 pt, uint8 r, uint8 g, uint8 b, uint8 a)
{
    ASSERT(pt < COUNT_OF(m_aColor));

    m_aColor[pt] = { r, g, b, a };
};

#endif /* TMNT2_BUILD_EU */


void CSprite::SetRGBA(const RwRGBA& rgba)
{
#ifdef TMNT2_BUILD_EU
    for (int32 i = 0; i < COUNT_OF(m_aColor); ++i)
        SetRGBA(i, rgba.red, rgba.green, rgba.blue, rgba.alpha);
#else /* TMNT2_BUILD_EU */
    m_Color = rgba;
#endif /* TMNT2_BUILD_EU */
};


void CSprite::SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
    SetRGBA({ r, g, b, a });
};


void CSprite::SetRGB(uint8 r, uint8 g, uint8 b)
{
#ifdef TMNT2_BUILD_EU
    for (int32 i = 0; i < COUNT_OF(m_aColor); ++i)
        m_aColor[i] = { r, g, b };
#else /* TMNT2_BUILD_EU */
    m_Color = { r, g, b };
#endif /* TMNT2_BUILD_EU */
};


void CSprite::SetAlpha(uint8 a)
{
#ifdef TMNT2_BUILD_EU
    for (int32 i = 0; i < COUNT_OF(m_aColor); ++i)
        m_aColor[i].alpha = a;
#else /* TMNT2_BUILD_EU */
    m_Color.alpha = a;
#endif /* TMNT2_BUILD_EU */
};


void CSprite::SetOffset(float x, float y)
{
    m_fOffsetX = x;
    m_fOffsetY = y;
};


void CSprite::SetRotate(float fRotate)
{
    m_fRotate = fRotate;
};


void CSprite::SetTexture(RwTexture* pTexture)
{
    ASSERT(pTexture);
    m_pTexture = pTexture;
};


void CSprite::SetTextureAndResize(RwTexture* pTexture)
{
    SetTexture(pTexture);
    
    RwRaster* pRwRaster = RwTextureGetRasterMacro(m_pTexture);
    ASSERT(pRwRaster);
    
    m_fW = static_cast<float>(RwRasterGetWidthMacro(pRwRaster));
    m_fH = static_cast<float>(RwRasterGetHeightMacro(pRwRaster));
};


void CSprite::SetTextureEmpty(void)
{
    m_pTexture = nullptr;
};


void CSprite::SetUV(float u0, float v0, float u1, float v1)
{
    m_fU0 = u0;
    m_fV0 = v0;
    m_fU1 = u1;
    m_fV1 = v1;
};


void CSprite::ResetUV(void)
{
    m_fU0 = m_fV0 = 0.0f;
    m_fU1 = m_fV1 = 1.0f;
};


void CSprite::SetPositionAndSize(float x, float y, float w, float h)
{
    ASSERT(m_fVirtualScreenW != 0.0f);
    ASSERT(m_fVirtualScreenH != 0.0f);

    m_fX = (x - m_fVirtualScreenX) / m_fVirtualScreenW * static_cast<float>(CScreen::Width());
    m_fY = (y - m_fVirtualScreenY) / m_fVirtualScreenH * static_cast<float>(CScreen::Height());

    m_fW = static_cast<float>(CScreen::Width()) / m_fVirtualScreenW * w;
    m_fH = static_cast<float>(CScreen::Height()) / m_fVirtualScreenH * h;
};


void CSprite::SetPositionAndSizeRealScreen(float x, float y, float w, float h)
{
    m_fX = x;
    m_fY = y;

    m_fW = w;
    m_fH = h;
};


void CSprite::Resize(float w, float h)
{
    ASSERT(m_fVirtualScreenW != 0.0f);
    ASSERT(m_fVirtualScreenH != 0.0f);

    float scaleW = static_cast<float>(CScreen::Width()) / VIRTUALSCREEN_DEFAULT_W;
    float scaleH = static_cast<float>(CScreen::Height()) / VIRTUALSCREEN_DEFAULT_H;

    m_fW = (w * scaleW);
    m_fH = (h * scaleH);
};


void CSprite::ResizeStrict(float w, float h)
{
    ASSERT(m_fVirtualScreenW != 0.0f);
    ASSERT(m_fVirtualScreenH != 0.0f);

    float scaleW = static_cast<float>(CScreen::Width()) / VIRTUALSCREEN_DEFAULT_W;
    float scaleH = static_cast<float>(CScreen::Height()) / VIRTUALSCREEN_DEFAULT_H;

    float scale = Min(scaleW, scaleH);

    m_fW = (w * scale);
    m_fH = (h * scale);
};


void CSprite::ResizeRealScreen(float w, float h)
{
    m_fW = w;
    m_fH = h;
};


void CSprite::Move(float x, float y)
{
    ASSERT(m_fVirtualScreenW != 0.0f);
    ASSERT(m_fVirtualScreenH != 0.0f);

    m_fX = (x - m_fVirtualScreenX) / m_fVirtualScreenW * static_cast<float>(CScreen::Width());
    m_fY = (y - m_fVirtualScreenY) / m_fVirtualScreenH * static_cast<float>(CScreen::Height());
};


void CSprite::Draw(void) const
{
    if (!CCamera::CameraCurrent())
        return;

    RwIm2DVertex aVertices[4];

    float x0 = m_fX - (m_fOffsetX * m_fW);
    float y0 = m_fY - (m_fOffsetY * m_fH);
    float x1 = x0 + m_fW;
    float y1 = y0 + m_fH;

    RwV2d xy_vertex[4] =
    {
        { x0, y1, },
        { x0, y0, },
        { x1, y1, },
        { x1, y0, },
    };

    RwV2d uv_vertex[4] =
    {
        { m_fU0, m_fV1, },
        { m_fU0, m_fV0, },
        { m_fU1, m_fV1, },
        { m_fU1, m_fV0, },
    };

    float z = RwIm2DGetNearScreenZMacro();
    float rhw = 1.0f / RwCameraGetNearClipPlaneMacro(CCamera::CameraCurrent());

    for (int32 i = 0; i < COUNT_OF(aVertices); ++i)
    {
        aVertices[i].x = xy_vertex[i].x;
        aVertices[i].y = xy_vertex[i].y;
        aVertices[i].z = m_fZ;
        aVertices[i].u = uv_vertex[i].x;
        aVertices[i].v = uv_vertex[i].y;
        aVertices[i].rhw = rhw;

#ifdef TMNT2_BUILD_EU

        static_assert(COUNT_OF(aVertices) == COUNT_OF(m_aColor),
                      "count of colors should equals to count of vertices");

        aVertices[i].emissiveColor = RWRGBALONGEX(m_aColor[i]);

#else /* TMNT2_BUILD_EU */

        aVertices[i].emissiveColor = RWRGBALONGEX(m_Color);

#endif /* TMNT2_BUILD_EU */
    };

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRasterMacro(m_pTexture));

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, aVertices, COUNT_OF(aVertices));
};


void CSprite::DrawRotate(void) const
{
    if (!CCamera::CameraCurrent())
        return;

    RwIm2DVertex aVertices[4];

    float x0 = m_fX - (m_fOffsetX * m_fW);
    float y0 = m_fY - (m_fOffsetY * m_fH);
    float x1 = x0 + m_fW;
    float y1 = y0 + m_fH;

    RwV2d xy_vertex[4] =
    {
        { x0, y1, },
        { x0, y0, },
        { x1, y1, },
        { x1, y0, },
    };

    RwV2d uv_vertex[4] =
    {
        { m_fU0, m_fV1, },
        { m_fU0, m_fV0, },
        { m_fU1, m_fV1, },
        { m_fU1, m_fV0, },
    };

    float z = RwIm2DGetNearScreenZMacro();
    float rhw = 1.0f / RwCameraGetNearClipPlaneMacro(CCamera::CameraCurrent());

    for (int32 i = 0; i < COUNT_OF(aVertices); ++i)
    {
        float tmp_x = xy_vertex[i].x - (x0 + (m_fW * 0.5f));
        float tmp_y = xy_vertex[i].y - (y0 + (m_fH * 0.5f));

        float x = CosFloat(m_fRotate) * (tmp_x) - SinFloat(m_fRotate) * (tmp_y) + (x0 + (m_fW * 0.5f));
        float y = SinFloat(m_fRotate) * (tmp_x) + CosFloat(m_fRotate) * (tmp_y) + (y0 + (m_fH * 0.5f));

        aVertices[i].x = x;
        aVertices[i].y = y;
        aVertices[i].z = m_fZ;
        aVertices[i].u = uv_vertex[i].x;
        aVertices[i].v = uv_vertex[i].y;
        aVertices[i].rhw = rhw;

#ifdef TMNT2_BUILD_EU

        static_assert(COUNT_OF(aVertices) == COUNT_OF(m_aColor),
                      "count of colors should equals to count of vertices");
                      
        aVertices[i].emissiveColor = RWRGBALONGEX(m_aColor[i]);

#else /* TMNT2_BUILD_EU */
        
        aVertices[i].emissiveColor = RWRGBALONGEX(m_Color);

#endif /* TMNT2_BUILD_EU */
    };

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRasterMacro(m_pTexture));

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, aVertices, COUNT_OF(aVertices));
};

