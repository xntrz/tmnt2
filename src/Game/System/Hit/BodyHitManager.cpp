#include "BodyHitManager.hpp"
#include "BodyHitData.hpp"
#include "Intersection.hpp"
#ifdef _DEBUG
#include "HitDebug.hpp"
#endif /* _DEBUG */

#include "Game/System/Misc/DebugShape.hpp"


class CBodyHitContainer
{
public:
    CBodyHitContainer(void);
    ~CBodyHitContainer(void);
    bool Check(CBodyHitData* pBodyHitData, const RwV3d* pMoveVelocity, RwV3d* pResult);
    CBodyHitData* Alloc(void);
    void Free(CBodyHitData* pBodyHitData);
    void ScratchVector(RwV3d* pOutVector, const RwV3d* pVector, const RwV3d* pAxis) const;    

private:
    CBodyHitData m_aHitPool[256];
    CList<CBodyHitData> m_listHitAlloc;
    CList<CBodyHitData> m_listHitFree;
};


CBodyHitContainer::CBodyHitContainer(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aHitPool); ++i)
        m_listHitFree.push_back(&m_aHitPool[i]);
};


CBodyHitContainer::~CBodyHitContainer(void)
{
    ASSERT(m_listHitAlloc.empty());
};


bool CBodyHitContainer::Check(CBodyHitData* pBodyHitData, const RwV3d* pMoveVelocity, RwV3d* pResult)
{
    ASSERT(pBodyHitData);
    ASSERT(pMoveVelocity);
    ASSERT(pResult);

#ifdef _DEBUG
    if (CHitDebug::SHOW_HIT_BODY)
    {
        RwSphere sphere;
        sphere.center = *pBodyHitData->GetCurrentPos();
        sphere.radius = pBodyHitData->GetHitRadius();
        
        CDebugShape::ShowSphere(&sphere);
    };
#endif /* _DEBUG */

    if (!pBodyHitData->IsEnableState(CBodyHitData::STATE_ENABLE))
        return false;
    
    RwV3d vMoveBuff = *pMoveVelocity;
    RwV3d vMinVelocity = vMoveBuff;
    float fMoveLengthSQ = Math::Vec3_Dot(pMoveVelocity, pMoveVelocity);
    float fMinMoveLengthSQ = fMoveLengthSQ;
    
    for (CBodyHitData& it : m_listHitAlloc)
    {
        bool bIsSame = (&it == pBodyHitData);
        bool bIsValidID = (pBodyHitData->GetHitID() != CBodyHitData::INVALID_HIT_ID);
        bool bIsSameID = (it.GetHitID() == pBodyHitData->GetHitID());
        bool bIsEnabled = it.IsEnableState(CBodyHitData::STATE_ENABLE);

        bool bIsStateValid = (!pBodyHitData->IsEnableState(CBodyHitData::STATE_SELF_TO_OTHER) && !it.IsEnableState(CBodyHitData::STATE_SELF_TO_OTHER))
                          || (!pBodyHitData->IsEnableState(CBodyHitData::STATE_TODO_0x2) && !it.IsEnableState(CBodyHitData::STATE_TODO_0x2))
                          || (pBodyHitData->IsEnableState(CBodyHitData::STATE_SELF_TO_OTHER) && it.IsEnableState(CBodyHitData::STATE_TODO_0x2));

        if (!bIsSame && (!bIsValidID || !bIsSameID) && bIsEnabled && bIsStateValid)
        {
            RwV3d vHit = Math::VECTOR3_ZERO;
            bool bIntersect = Intersection::SphereAndSphereGetAwayVelocity(pBodyHitData->GetCurrentPos(),
                                                                           pBodyHitData->GetHitRadius(),
                                                                           it.GetCurrentPos(),
                                                                           it.GetHitRadius(),
                                                                           &vHit);
            if (bIntersect)
            {
                Math::Vec3_Scale(&vHit, &vHit, 1.5f);

                if (fMinMoveLengthSQ > FLT_EPSILON)
                    vMinVelocity = vHit;
                else
                    Math::Vec3_Add(&vMinVelocity, &vMinVelocity, &vHit);

                Math::Vec3_Scale(&vHit, &vHit, -1.0f);
                ScratchVector(&vMoveBuff, &vMoveBuff, &vHit);
                fMinMoveLengthSQ = 0.0f;
            }
            else if (fMinMoveLengthSQ > 0.0f)
            {
                RwV3d vZero = Math::VECTOR3_ZERO;
                bIntersect = Intersection::MoveSphereAndMoveSphereVelocity(pBodyHitData->GetCurrentPos(),
                                                                           &vMinVelocity,
                                                                           pBodyHitData->GetHitRadius(),
                                                                           it.GetCurrentPos(),
                                                                           &vZero,
                                                                           it.GetHitRadius(),
                                                                           &vHit);
                if (bIntersect)
                {
                    float fHitMoveLengthSQ = Math::Vec3_Dot(&vHit, &vHit);
                    if (fHitMoveLengthSQ < fMinMoveLengthSQ)
                    {
                        fMinMoveLengthSQ = fHitMoveLengthSQ;
                        vMinVelocity = vHit;
                    };
                };
            };
        };        
    };

    if (fMinMoveLengthSQ >= fMoveLengthSQ)
        return false;

    if (fMinMoveLengthSQ <= FLT_EPSILON)
        Math::Vec3_Add(&vMinVelocity, &vMinVelocity, &vMoveBuff);

    *pResult = vMinVelocity;
    
    return true;
};


CBodyHitData* CBodyHitContainer::Alloc(void)
{
    ASSERT(!m_listHitFree.empty());

    CBodyHitData* pNode = m_listHitFree.front();
    m_listHitFree.erase(pNode);
    m_listHitAlloc.push_back(pNode);

    return pNode;
};


void CBodyHitContainer::Free(CBodyHitData* pBodyHitData)
{
    ASSERT(pBodyHitData);
    ASSERT(!m_listHitAlloc.empty());

    m_listHitAlloc.erase(pBodyHitData);
    m_listHitFree.push_front(pBodyHitData);
};


void CBodyHitContainer::ScratchVector(RwV3d* pOutVector, const RwV3d* pVector, const RwV3d* pAxis) const
{
    ASSERT(pOutVector);
    ASSERT(pVector);
    ASSERT(pAxis);

    RwV3d vNAxis = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vNAxis, pAxis);

    float fD = Math::Vec3_Dot(pVector, &vNAxis);
    if (fD >= 0.0f)
    {
        Math::Vec3_Scale(&vNAxis, &vNAxis, fD * 0.4f);
        Math::Vec3_Sub(pOutVector, pVector, &vNAxis);
    };
};


static CBodyHitContainer* s_pBodyHitContainer = nullptr;


static inline CBodyHitContainer& BodyHitContainer(void)
{
    ASSERT(s_pBodyHitContainer);
    return *s_pBodyHitContainer;
};


/*static*/ void CBodyHitManager::Initialize(void)
{
    if (!s_pBodyHitContainer)
        s_pBodyHitContainer = new CBodyHitContainer;
};


/*static*/ void CBodyHitManager::Terminate(void)
{
    if (s_pBodyHitContainer)
    {
        delete s_pBodyHitContainer;
        s_pBodyHitContainer = nullptr;
    };
};


/*static*/ CBodyHitData* CBodyHitManager::AllocData(void)
{
    return BodyHitContainer().Alloc();
};


/*static*/ void CBodyHitManager::FreeData(CBodyHitData* pBodyHitData)
{
    BodyHitContainer().Free(pBodyHitData);
};


/*static*/ bool CBodyHitManager::CheckHit(CBodyHitData* pBodyHitData, const RwV3d* pMoveVelocity, RwV3d* pResult)
{
    return BodyHitContainer().Check(pBodyHitData, pMoveVelocity, pResult);
};