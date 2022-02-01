#pragma once


class CCloth
{
private:
    struct SPRING
    {
        int32 m_nPointIndex1;
        int32 m_nPointIndex2;
        float m_fSpringConstant;
        float m_fDecayCoefficient;
        float m_fLength;
    };
    
    struct CLOTHPOINT
    {
        float m_fMass;
        float m_fInvMass;
        RwV3d m_vPosition;
        RwV3d m_vVelocity;
        RwV3d m_vAcceleration;
        RwV3d m_vForces;
        bool m_bLock;
    };

    struct CLOTHPARAMETER
    {
        int32 m_nDivisionWidth;
        int32 m_nDivisionHeight;
        float m_fWidth;
        float m_fHeight;
        float m_fClothMass;
        float m_fSpringTensionConstant;
        float m_fSpringShearConstant;
        float m_fSpringDampingConstant;
        float m_fSpringLengthMax;
        float m_fDragCoefficient;
        int32 m_nWindFactor;
        RwRGBA m_Color;
        RwV3d m_vWind;
        float m_fGravity;
        RwTexture* m_pTexture;
    };

public:
    CCloth(void);
    ~CCloth(void);
    void Create(void);
    void Update(float dt);
    void Draw(void);
    void SetBaseParameter(int32 nDivisionWidth, int32 nDivisionHeight, float w, float h);
    void SetColor(const RwRGBA& color);
    void SetTension(float fTension);
    void SetShear(float fShear);
    void SetLength(float fLen);
    void SetDrag(float fDrag);
    void SetWeight(float fWeight);
    void SetWind(const RwV3d* pvWind);
    void SetGravity(float fGravity);
    void SetJointPosition(const RwV3d* pvJoint);
    void ResetCloth(const RwV3d* pvPosition);
    void CreateCloth(void);
    void CalcForces(void);
    void StepSimulation(void);
    void Integration(CLOTHPOINT* pClothPoint, float dt);
    void CheckCollision(CLOTHPOINT* pClothPoint);
    void SetVertex(RwIm3DVertex* pVertex, RwV3d* pPos, float u, float v);
    void SetDeltaTime(float dt);
    void RenderStatePush(void);
    void RenderStatePop(void);

private:
    CLOTHPOINT* m_pClothPointList;
    SPRING* m_pSpringList;
    int32 m_nDivisionWidth;
    int32 m_nDivisionHeight;
    int32 m_nRows;
    int32 m_nColumns;
    float m_fWidth;
    float m_fHeight;
    float m_fClothMass;
    float m_fSpringTensionConstant;
    float m_fSpringShearConstant;
    float m_fSpringDampingConstant;
    float m_fSpringLength;
    float m_fDragCoefficient;
    int32 m_nWindFactor;
    float m_fGravity;
    RwRGBA m_Color;
    RwV3d m_vWind;
    RwTexture* m_pTexture;
    float m_fDt;
};