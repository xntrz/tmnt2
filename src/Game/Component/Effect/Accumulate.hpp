#pragma once


class CModel;


class CAccumulate final
{
public:
    CAccumulate(CModel* pModel, int32 nBoneID, const RwV3d* pvTop, const RwV3d* pvBottom);
    ~CAccumulate(void);
    void Draw(void);
    void SetModel(CModel* pModel);
    void SetBoneID(int32 nBoneID);
    void SetLine(const RwV3d* pvTop, const RwV3d* pvBottom);
    void SetTextureSize(float w, float h);
    void SetColor(const RwRGBA& color);
    void SetVertexAll(void);
    void SetVertex(RwV3d* pvPosition, RwV3d* aSummit, RwMatrix* pMatrix);
    
private:
    CModel* m_pModel;
    int32 m_nBoneID;
    RwTexture* m_pTexture;
    RwV3d m_vTop;
    RwV3d m_vBottom;
    RwV2d m_vSize;
    RwRGBA m_Color;
    RwIm3DVertex* m_pVertex;
    int32 m_nVertexNum;
    int32 m_nParticleMax;
};