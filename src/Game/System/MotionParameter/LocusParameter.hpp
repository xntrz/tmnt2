#pragma once


class CLocusParameter : public CListNode<CLocusParameter>
{
public:
    struct INIT_PARAMETER
    {
        int32 m_nBoneID;
        RwV3d m_vPositionHigh;
        RwV3d m_vPositionLow;
        RwRGBA m_Color;
    };
    
public:
    CLocusParameter(INIT_PARAMETER* pParam);
    virtual ~CLocusParameter(void);
    void Initialize(INIT_PARAMETER* pParam);
    int32 GetBoneID(void) const;
    const RwV3d* GetPosHigh(void) const;
    const RwV3d* GetPosLow(void) const;
    RwRGBA GetColor(void) const;

private:
    int32 m_nBoneID;
    RwV3d m_vPositionHigh;
    RwV3d m_vPositionLow;
    RwRGBA m_Color;
};