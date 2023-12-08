#pragma once


class CSprite
{
public:
    static float VIRTUALSCREEN_DEFAULT_X;
    static float VIRTUALSCREEN_DEFAULT_Y;
    static float VIRTUALSCREEN_DEFAULT_W;
    static float VIRTUALSCREEN_DEFAULT_H;
    static float m_fVirtualScreenX;
    static float m_fVirtualScreenY;
    static float m_fVirtualScreenW;
    static float m_fVirtualScreenH;
    
public:
    static void SetDefaultVirtualScreen(void);
    static void GetRealScreenPos(float* x, float* y);
    static void PushRenderStates(void);
    static void PopRenderStates(void);

    CSprite(void);
    virtual ~CSprite(void);
    void SetScreenSize(void);
    void SetZ(float fZ);
    void SetRGBA(const RwRGBA& rgba);
    void SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
    void SetRGB(uint8 r, uint8 g, uint8 b);
    void SetAlpha(uint8 a);
    void SetOffset(float x, float y);
    void SetRotate(float fRotate);
    void SetTexture(RwTexture* pTexture);
    void SetTextureAndResize(RwTexture* pTexture);
    void SetTextureEmpty(void);
    void SetUV(float u0, float v0, float u1, float v1);
    void ResetUV(void);
    void SetPositionAndSize(float x, float y, float w, float h);
    void SetPositionAndSizeRealScreen(float x, float y, float w, float h);
    void Resize(float w, float h);
    void ResizeStrict(float w, float h);
    void ResizeRealScreen(float w, float h);
    void Move(float x, float y);
	void Draw(void) const;
    void DrawRotate(void) const;

private:    
    float m_fX;
    float m_fY;
    float m_fZ;
    float m_fW;
    float m_fH;
    float m_fOffsetX;
    float m_fOffsetY;
    RwRGBA m_Color;
    float m_fU0;
    float m_fV0;
    float m_fU1;
    float m_fV1;
    float m_fRotate;
    RwTexture* m_pTexture;
    bool m_bCameraCheckFlag;
};