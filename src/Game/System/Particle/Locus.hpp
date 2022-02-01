#pragma once


class CLocus
{
public:
    static const int32 POINTSET_MAX = 120;
    
public:
    CLocus(int32 nNumPoint, const RwRGBA& color);
    ~CLocus(void);
    void Draw(void);
    void Reset(void);
    void SetPosition(const RwV3d* pvPositionHight, const RwV3d* pvPositionLow);
    void DeletePair(void);
    void SetColor(const RwRGBA& color);
    void SetAlphaBasis(uint8 uAlphaBasis);
    void SetTexture(RwTexture* pTexture);
    void SetDrawEnable(bool bEnable);
    void RenderStateSet(void);
    void RenderStateReset(void);
    void GetCompletionListHigh(RwV3d* aCompletionList);
    void GetCompletionListLow(RwV3d* aCompletionList);
    void SetVertex(RwIm3DVertex* pVertex, RwV3d* pVector, RwRGBA& color, uint8 alpha, float u, float v);
    void Vec3CatmullRom(RwV3d* pOut, const RwV3d* pV1, const RwV3d* pV2, const RwV3d* pV3, const RwV3d* pV4, float fS);
    void Vec3Hermite(RwV3d* pOut, const RwV3d* pV1, const RwV3d* pT1, const RwV3d* pV2, const RwV3d* pT2, float fS);
    void CalcSpline3DParam(float* pfParamArray, float fS);

private:
    RwV3d* m_pvPositionHigh;
    RwV3d* m_pvPositionLow;
    int32 m_nNumPoint;
    int32 m_nNumPointMax;
    int32 m_nRepetitionNum;
    RwRGBA m_Color;
    RwTexture* m_pTexture;
    uint8 m_uAlphaBasis;
    bool m_bEnableDraw;
    bool m_bCompletion;
};