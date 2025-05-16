#pragma once


class CSphere
{
public:
    CSphere(float fRadius,
            int32 nDivisionVer,
            int32 nDivisionHor,
            int32 nRepetationU,
            int32 nRepetationV,
            const RwRGBA& color,
            bool bRandom = false);
    ~CSphere(void);
    void Run(float dt);
    void Draw(void);
    void SetPosition(const RwV3d* pvPosition);
    void SetTexture(RwTexture* pTexture);
    void SetUV(float u0, float v0, float u1, float v1);
    void SetScale(const RwV3d* pvScaling);
    void Reset(void);
    void ResetUV(void);
    void SetVertex(void);
    void SetVertexRandom(void);
    void RenderStatePush(void);
    void RenderStatePop(void);
    
private:
    RwV3d m_vPosition;
    float m_fRadius;
    int32 m_nDivisionVer;
    int32 m_nDivisionHor;
    RwRGBA m_Color;
    RwMatrix m_matrix;
    RwTexture* m_pTexture;
    int32 m_nRepetationU;
    int32 m_nRepetationV;
    float m_u0;
    float m_v0;
    float m_u1;
    float m_v1;
    bool m_bRandom;
    bool m_bUVAnim;
    RwIm3DVertex* m_pVertex;
    int32 m_nNumVertex;
    int32 m_nNumBodyVertex;
    int32 m_nNumTopvertex;
    int32 m_nNumUnderVertex;
};