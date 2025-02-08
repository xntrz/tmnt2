#include "Model.hpp"

#include "Game/System/Misc/RenderStateManager.hpp"
#include "System/Common/Camera.hpp"


class CCalcBoundingSphereFunctor : public CModel::IAtomicCallbackFunctor
{
public:
    virtual RpAtomic* operator()(RpAtomic* pAtomic) override;

    RwSphere m_bsphere;
};


class CGetAtomicFromIndexFunctor : public CModel::IAtomicCallbackFunctor
{
public:
    virtual RpAtomic* operator()(RpAtomic* pAtomic) override;
    
    int32 m_iCurrentIndex;
    int32 m_iTargetIndex;
    RpAtomic* m_pTargetAtomic;
};


class CGetRootAtomicFunctor : public CModel::IAtomicCallbackFunctor
{
public:
    virtual RpAtomic* operator()(RpAtomic* pAtomic) override;
    
    RpAtomic* m_pRootAtomic;
};


class CInitializeAtomicFunctor : public CModel::IAtomicCallbackFunctor
{
public:
    virtual RpAtomic* operator()(RpAtomic* pAtomic) override;
};


class CReplaceHierarchyFunctor : public CModel::IAtomicCallbackFunctor
{
public:
    virtual RpAtomic* operator()(RpAtomic* pAtomic) override;

    RpHAnimHierarchy* m_pHierarchy;
};


class CSetLightingEnableFunctor : public CModel::IAtomicCallbackFunctor
{
public:    
    virtual RpAtomic* operator()(RpAtomic* pAtomic) override;
    
    bool m_bEnable;
};


class CReplaceMaterialColorFunctor : public CModel::IMaterialCallbackFunctor
{
public:
    virtual RpMaterial* operator()(RpMaterial* pMaterial) override;

    RwRGBA m_MaterialColor;
};


class CGetHierarchyFunctor : public CModel::IFrameCallbackFunctor
{
public:
    virtual RwFrame* operator()(RwFrame* pFrame) override;

    RpHAnimHierarchy* m_pHierarchy;
};


//
// *********************************************************************************
//


RpAtomic* CCalcBoundingSphereFunctor::operator()(RpAtomic* pAtomic)
{
    RwSphere* pSphere = RpAtomicGetBoundingSphereMacro(pAtomic);
    if (!pSphere)
        return pAtomic;

    if (m_bsphere.radius <= 0.0f)
    {
        m_bsphere = *pSphere;
        return pAtomic;
    };

    RwSphere shBase;
    RwSphere shChild;

    if (m_bsphere.radius <= pSphere->radius)
    {
        shBase  = *pSphere;
        shChild = m_bsphere;
    }
    else
    {
        shBase  = m_bsphere;
        shChild = *pSphere;
    };

    RwV3d vTemp = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vTemp, &shChild.center, &shBase.center);

    float fToChildRadius = (shChild.radius + Math::Vec3_Length(&vTemp));
    if (fToChildRadius > shBase.radius)
        shBase.radius = fToChildRadius;

    m_bsphere = shBase;

    return pAtomic;
};


//
// *********************************************************************************
//


RpAtomic* CGetAtomicFromIndexFunctor::operator()(RpAtomic* pAtomic)
{
    if (m_iCurrentIndex++ == m_iTargetIndex)
        m_pTargetAtomic = pAtomic;

    return (m_pTargetAtomic ? nullptr : pAtomic);
};


//
// *********************************************************************************
//


RpAtomic* CGetRootAtomicFunctor::operator()(RpAtomic* pAtomic)
{
    RwFrame* pFrame = RpAtomicGetFrameMacro(pAtomic);

    if (!RwFrameGetRoot(pFrame))
        return pAtomic;

    m_pRootAtomic = pAtomic;
    return nullptr;
};


//
// *********************************************************************************
//


RpAtomic* CInitializeAtomicFunctor::operator()(RpAtomic* pAtomic)
{
    RpGeometry* pGeometry = RpAtomicGetGeometry(pAtomic);    
    ASSERT(pGeometry);

    RwUInt32 geometryFlags = RpGeometryGetFlagsMacro(pGeometry);
    geometryFlags |= rpGEOMETRYMODULATEMATERIALCOLOR;
    RpGeometrySetFlagsMacro(pGeometry, geometryFlags);    
    
    return pAtomic;
};


//
// *********************************************************************************
//


RpAtomic* CReplaceHierarchyFunctor::operator()(RpAtomic* pAtomic)
{
    if (RpSkinAtomicGetSkin(pAtomic))
        RpSkinAtomicSetHAnimHierarchy(pAtomic, m_pHierarchy);

    return pAtomic;
};


//
// *********************************************************************************
//


RpAtomic* CSetLightingEnableFunctor::operator()(RpAtomic* pAtomic)
{
    RpGeometry* pGeometry = RpAtomicGetGeometryMacro(pAtomic);
    ASSERT(pGeometry);
    
    RwUInt32 geometryFlags = RpGeometryGetFlagsMacro(pGeometry);
	if (m_bEnable)
    {
        geometryFlags &= (~rpGEOMETRYPRELIT);
        geometryFlags |= rpGEOMETRYLIGHT;
	}
	else
    {
        geometryFlags &= (~rpGEOMETRYLIGHT);
        geometryFlags |= rpGEOMETRYPRELIT;
    };
    RpGeometrySetFlagsMacro(pGeometry, geometryFlags);

    return pAtomic;
};


//
// *********************************************************************************
//


RpMaterial* CReplaceMaterialColorFunctor::operator()(RpMaterial* pMaterial)
{
    RpMaterialSetColorMacro(pMaterial, &m_MaterialColor);

    return pMaterial;
};


//
// *********************************************************************************
//


RwFrame* CGetHierarchyFunctor::operator()(RwFrame* pFrame)
{
    m_pHierarchy = RpHAnimFrameGetHierarchy(pFrame);

    return (m_pHierarchy ? nullptr : pFrame);
};


//
// *********************************************************************************
//


CModel::CModel(const char* pszName, RpClump* pClump)
: m_vPosition(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_MaterialColor({ 0xFF, 0xFF,0xFF,0xFF })
, m_BSphere({ { Math::VECTOR3_ZERO }, 0.0f })
, m_vBSphereOffset(Math::VECTOR3_ZERO)
, m_pszName(pszName)
, m_pClump(pClump)
, m_pHierarchyOriginal(nullptr)
, m_pHierarchyCurrent(nullptr)
, m_pRootAtomic(nullptr)
, m_bInitializedBoneMatrix(false)
, m_bDrawEnable(true)
, m_bClippingEnable(true)
{
    ASSERT(m_pszName);
    ASSERT(m_pClump);

    RwFrame* pFrame = RpClumpGetFrameMacro(m_pClump);
    RwFrameUpdateObjects(pFrame);
    
    InitializeAtomic();
    CalcBoundingSphere();

    m_pHierarchyOriginal = GetHierarchyFromClump();
    if (m_pHierarchyOriginal)
    {
        RwInt32 hierarchyFlags = RpHAnimHierarchyGetFlagsMacro(m_pHierarchyOriginal);
        hierarchyFlags |= rpHANIMHIERARCHYUPDATEMODELLINGMATRICES;
        hierarchyFlags |= rpHANIMHIERARCHYUPDATELTMS;
        RpHAnimHierarchySetFlagsMacro(m_pHierarchyOriginal, hierarchyFlags);

        ReplaceHierarchyForClump(m_pHierarchyOriginal);
    };

    m_pRootAtomic = GetRootAtomic();
    m_pHierarchyCurrent = m_pHierarchyOriginal;    
};


CModel::~CModel(void)
{
    if (m_pHierarchyOriginal)
    {
        RpHAnimHierarchyDestroy(m_pHierarchyOriginal);
        m_pHierarchyOriginal = nullptr;
    };

    if (m_pClump)
    {
        RpClumpDestroy(m_pClump);
        m_pClump = nullptr;
    };
};


void CModel::Draw(void)
{
    if (!m_bDrawEnable)
        return;

    if (m_bClippingEnable)
    {
        RwSphere BoundingSphere;
        CalcCurrentBoundingSphere(&BoundingSphere);

        if (!RwCameraFrustumTestSphere(CCamera::CameraCurrent(), &BoundingSphere))
            return;
    };

    CRenderStateManager::BeginBlock(CRenderStateManager::BLOCK_MODEL);

    UpdateFrame();
    ReplaceMaterialColor(m_MaterialColor);

    RpClumpRender(m_pClump);

    CRenderStateManager::EndBlock();
};


CModelManager::MODELTYPE CModel::GetType(void) const
{
    return CModelManager::MODELTYPE_NORMAL;
};


void CModel::CalcCurrentBoundingSphere(RwSphere* pBoundingSphere) const
{
    if (!pBoundingSphere)
        return;
    
    *pBoundingSphere = m_BSphere;

    Math::Vec3_Add(&pBoundingSphere->center, &pBoundingSphere->center, &m_vPosition);
    Math::Vec3_Add(&pBoundingSphere->center, &pBoundingSphere->center, &m_vBSphereOffset);
};


RwMatrix* CModel::GetBoneMatrix(int32 index)
{
    RwMatrix* pResult = nullptr;
    
    if (m_bInitializedBoneMatrix)
    {
        if (m_pHierarchyCurrent)
        {
            RwMatrix* pMatrixArray = RpHAnimHierarchyGetMatrixArray(m_pHierarchyCurrent);
            
            ASSERT(index >= 0);
            ASSERT(index < GetBoneNum());
            ASSERT(pMatrixArray);
            
            pResult = &pMatrixArray[index];
        };
    }
    else
    {
        RwFrame* pFrame = RpClumpGetFrameMacro(m_pClump);
        ASSERT(pFrame);

        pResult = RwFrameGetMatrixMacro(pFrame);
        ASSERT(pResult);
    };

    return pResult;
};


RwMatrix* CModel::GetBoneMatrixFromID(int32 id)
{
    RwMatrix* pResult = nullptr;

    if (m_bInitializedBoneMatrix)
    {
        if (m_pHierarchyCurrent)
        {
            int32 index = RpHAnimIDGetIndex(m_pHierarchyCurrent, id);
            ASSERT(index >= 0);
	
            pResult = GetBoneMatrix(index);            
        };
    }
    else
    {
        RwFrame* pFrame = RpClumpGetFrameMacro(m_pClump);
        ASSERT(pFrame);

        pResult = RwFrameGetMatrixMacro(pFrame);
        ASSERT(pResult);
    };

    return pResult;
};


RwV3d* CModel::GetBonePositionFromID(int32 id)
{
    RwMatrix* pMatrix = GetBoneMatrixFromID(id);
    if (pMatrix)
        return &pMatrix->pos;
    
    return nullptr;
};


int32 CModel::GetPartsNum(void) const
{
    ASSERT(m_pClump);
    
    return RpClumpGetNumAtomics(m_pClump);
};


int32 CModel::GetBoneNum(void) const
{
    return RpSkinGetNumBones(RpSkinAtomicGetSkin(m_pRootAtomic));    
};


RpHAnimHierarchy* CModel::GetHierarchyOriginal(void) const
{
    return m_pHierarchyOriginal;
};


RpHAnimHierarchy* CModel::GetHierarchyCurrent(void) const
{
    return m_pHierarchyCurrent;
};


void CModel::SetBoneMatrixInit(bool bSet)
{
    m_bInitializedBoneMatrix = bSet;
};


void CModel::SetPartsDrawEnable(int32 index, bool bEnable)
{
    RpAtomic* pAtomic = GetAtomicFromIndex(index);    
    ASSERT(pAtomic);

    RwUInt8 atomicFlags = RpAtomicGetFlagsMacro(pAtomic);
    if (bEnable)
        atomicFlags |= rpATOMICRENDER;
    else
        atomicFlags &= (~rpATOMICRENDER);
    RpAtomicSetFlagsMacro(pAtomic, atomicFlags);
};


void CModel::SetRpHAnimHierarchy(RpHAnimHierarchy* pHierarchy)
{
    if (m_pHierarchyCurrent != pHierarchy)
    {
        ReplaceHierarchyForClump(pHierarchy);
        m_pHierarchyCurrent = pHierarchy;
        
        RpHAnimHierarchyAttach(pHierarchy);
    };
};


RpAtomic* CModel::GetRootAtomic(void)
{
    CGetRootAtomicFunctor functor;
    functor.m_pRootAtomic = nullptr;

    ForAllAtomicCallback(&functor);
    
    return functor.m_pRootAtomic;
};


RpAtomic* CModel::GetAtomicFromIndex(int32 index)
{
    CGetAtomicFromIndexFunctor functor;
    functor.m_iTargetIndex  = index;
    functor.m_iCurrentIndex = 0;
    functor.m_pTargetAtomic = nullptr;

    ForAllAtomicCallback(&functor);

    return functor.m_pTargetAtomic;    
};


RpHAnimHierarchy* CModel::GetHierarchyFromClump(void)
{
    CGetHierarchyFunctor functor;
    functor.m_pHierarchy = nullptr;
    
    ForAllChildrenFrameCallback(&functor);
    
    return functor.m_pHierarchy;
};


void CModel::ReplaceHierarchyForClump(RpHAnimHierarchy* pHierarchy)
{
    CReplaceHierarchyFunctor functor;
    functor.m_pHierarchy = pHierarchy;
    
    ForAllAtomicCallback(&functor);
};


void CModel::ReplaceMaterialColor(const RwRGBA& color)
{
    CReplaceMaterialColorFunctor functor;
    functor.m_MaterialColor = color;
    
    ForAllMaterialCallback(&functor);
};


void CModel::SetLightingEnable(bool bEnable)
{
    CSetLightingEnableFunctor functor;
    functor.m_bEnable = bEnable;
    
    ForAllAtomicCallback(&functor);
};


void CModel::UpdateFrame(void)
{
    UpdateFrame(m_pClump);
};


void CModel::UpdateFrame(RpClump* pClump)
{
    ASSERT(pClump);

    RwMatrix matrix;    
    Math::Matrix_AffineTransformation(&matrix, nullptr, &m_vRotation, &m_vPosition);

    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    ASSERT(pFrame);

    RwFrameTransform(pFrame, &matrix, rwCOMBINEREPLACE);
};


RpClump* CModel::GetClump(void)
{
    return m_pClump;
};


void CModel::InitializeAtomic(void)
{
    CInitializeAtomicFunctor functor;
    
    ForAllAtomicCallback(&functor);
};


void CModel::CalcBoundingSphere(void)
{
    CCalcBoundingSphereFunctor functor;
    functor.m_bsphere = { { Math::VECTOR3_ZERO }, 0.0f };

    ForAllAtomicCallback(&functor);

    m_BSphere = functor.m_bsphere;
};


void CModel::ForAllChildrenFrameCallback(IFrameCallbackFunctor* pFunctor)
{
    ASSERT(pFunctor);
    ASSERT(m_pClump);

    RwFrame* pFrame = RpClumpGetFrameMacro(m_pClump);
    ASSERT(pFrame);
    
    RwFrameForAllChildren(pFrame, CallbackFrame, pFunctor);
};


void CModel::ForAllAtomicCallback(IAtomicCallbackFunctor* pFunctor)
{
    ASSERT(pFunctor);
    ASSERT(m_pClump);
    
    RpClumpForAllAtomics(m_pClump, CallbackAtomic, pFunctor);
};


void CModel::ForAllMaterialCallback(IMaterialCallbackFunctor* pFunctor)
{
    ASSERT(pFunctor);
    ASSERT(m_pClump);
    
    RpClumpForAllAtomics(m_pClump, CallbackAtomicForMaterialCallback, pFunctor);
};


/*static*/ RwFrame* CModel::CallbackFrame(RwFrame* frame, void* data)
{
    IFrameCallbackFunctor* pFunctor = static_cast<IFrameCallbackFunctor*>(data);
    ASSERT(pFunctor);

    return (*pFunctor)(frame);
};


/*static*/ RpMaterial* CModel::CallbackMaterial(RpMaterial* material, void* data)
{
    IMaterialCallbackFunctor* pFunctor = static_cast<IMaterialCallbackFunctor*>(data);
    ASSERT(pFunctor);

    return (*pFunctor)(material);
};


/*static*/ RpAtomic* CModel::CallbackAtomic(RpAtomic* atomic, void* data)
{
    IAtomicCallbackFunctor* pFunctor = static_cast<IAtomicCallbackFunctor*>(data);
    ASSERT(pFunctor);

    return (*pFunctor)(atomic);
};


/*static*/ RpAtomic* CModel::CallbackAtomicForMaterialCallback(RpAtomic* atomic, void* data)
{
    RpGeometry* pGeometry = RpAtomicGetGeometryMacro(atomic);
    
    if (pGeometry)
        RpGeometryForAllMaterials(pGeometry, CallbackMaterial, data);
    
    return atomic;
};