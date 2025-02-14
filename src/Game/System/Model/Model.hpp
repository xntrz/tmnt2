#pragma once

#include "ModelManager.hpp"


class CModel
{
public:
    class IAtomicCallbackFunctor
    {
    public:
        virtual RpAtomic* operator()(RpAtomic* pAtomic) = 0;
    };

    class IFrameCallbackFunctor
    {
    public:        
        virtual RwFrame* operator()(RwFrame* pFrame) = 0;
    };

    class IMaterialCallbackFunctor
    {
    public:
        virtual RpMaterial* operator()(RpMaterial* pMaterial) = 0;
    };

public:
    CModel(const char* pszName, RpClump* pClump);
    virtual ~CModel(void);
    virtual void Draw(void);
    virtual CModelManager::MODELTYPE GetType(void) const;
    void CalcCurrentBoundingSphere(RwSphere* pBoundingSphere) const;
    RwMatrix* GetBoneMatrix(int32 index);
    RwMatrix* GetBoneMatrixFromID(int32 id);
    RwV3d* GetBonePositionFromID(int32 id);
    int32 GetPartsNum(void) const;
    int32 GetBoneNum(void) const;
    RpHAnimHierarchy* GetHierarchyOriginal(void) const;
    RpHAnimHierarchy* GetHierarchyCurrent(void) const;
    void SetBoneMatrixInit(bool bSet);
    void SetPartsDrawEnable(int32 index, bool bEnable);
    void SetRpHAnimHierarchy(RpHAnimHierarchy* pHierarchy);
    RpAtomic* GetRootAtomic(void);
    RpAtomic* GetAtomicFromIndex(int32 index);
    RpHAnimHierarchy* GetHierarchyFromClump(void);
    void ReplaceHierarchyForClump(RpHAnimHierarchy* pHierarchy);
    void ReplaceMaterialColor(const RwRGBA& color);
    void SetLightingEnable(bool bEnable);
    void UpdateFrame(void);
    void UpdateFrame(RpClump* pClump);
    RpClump* GetClump(void);
    void InitializeAtomic(void);
    void CalcBoundingSphere(void);
    void ForAllChildrenFrameCallback(IFrameCallbackFunctor* pFunctor);
    void ForAllAtomicCallback(IAtomicCallbackFunctor* pFunctor);
    void ForAllMaterialCallback(IMaterialCallbackFunctor* pFunctor);

    inline void SetPosition(const RwV3d& rPos) { m_vPosition = rPos; };
    inline void SetPosition(const RwV3d* pPos) { m_vPosition = *pPos; };
    inline void GetPosition(RwV3d* pvPosition) const{ *pvPosition = m_vPosition; };
    
    inline void SetRotation(const RwV3d& rRot){ m_vRotation = rRot; };
    inline void SetRotation(const RwV3d* pRot){ m_vRotation = *pRot; };
    inline void GetRotation(RwV3d* pvRotation) const { *pvRotation = m_vRotation; };
    
    inline void SetDrawEnable(bool bEnable){ m_bDrawEnable = bEnable; };
    inline bool IsDrawEnable(void) const { return m_bDrawEnable; };
    
    inline RwRGBA GetColor(void) const  { return m_MaterialColor; };
    inline void SetColor(const RwRGBA& color) { m_MaterialColor = color; };
    
    inline void SetClippingEnable(bool bEnable) { m_bClippingEnable = bEnable; };
    inline bool IsClippingEnable(void) const { return m_bClippingEnable; };
    
    inline void SetBoundingSphereOffset(const RwV3d* pvBSOffset) { m_vBSphereOffset = *pvBSOffset; };
    inline void SetBoundingSphereRadius(float r) { m_BSphere.radius = r; };
    
    inline const char* GetName(void) const { return m_pszName; };



protected:
    static RwFrame* CallbackFrame(RwFrame* frame, void* data);
    static RpMaterial* CallbackMaterial(RpMaterial* material, void* data);
    static RpAtomic* CallbackAtomic(RpAtomic* atomic, void* data);
    static RpAtomic* CallbackAtomicForMaterialCallback(RpAtomic* atomic, void* data);

protected:
    RwV3d m_vPosition;
    RwV3d m_vRotation;
    RwRGBA m_MaterialColor;
    RwSphere m_BSphere;
    RwV3d m_vBSphereOffset;
    const char* m_pszName;
    RpClump* m_pClump;
    RpHAnimHierarchy* m_pHierarchyOriginal;
    RpHAnimHierarchy* m_pHierarchyCurrent;
    RpAtomic* m_pRootAtomic;
    bool m_bInitializedBoneMatrix;
    bool m_bDrawEnable;
    bool m_bClippingEnable;
};