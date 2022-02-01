#include "ShotManager.hpp"
#include "Shot.hpp"

#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/Gimmick/Gimmick.hpp"


class CShotList
{
private:
    struct SHOTWORK : public CListNode<SHOTWORK>
    {
        CShot* m_pShot;
    };

public:
    CShotList(void);
    ~CShotList(void);
    void Run(void);
    void Draw(void);
    void GarbageCollection(void);
    uint32 Regist(SHOTID::VALUE idShot, const RwV3d* pvPos, const RwV3d* pvVec, CGameObject* pObject, float fRadian, float fLife);
    bool IsExist(SHOTID::VALUE idShot) const;

private:
    SHOTWORK m_aWork[64];
    CList<SHOTWORK> m_listWorkPool;
    CList<SHOTWORK> m_listWorkAlloc;
};


CShotList::CShotList(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aWork); ++i)
    {
        m_aWork[i].m_pShot = nullptr;
        m_listWorkPool.push_back(&m_aWork[i]);
    };
};


CShotList::~CShotList(void)
{
    ;
};


void CShotList::Run(void)
{
    GarbageCollection();
};


void CShotList::Draw(void)
{
    for (SHOTWORK& it : m_listWorkAlloc)
    {
        CShot* pShot = it.m_pShot;
        ASSERT(pShot);

        pShot->Draw();
    };
};


void CShotList::GarbageCollection(void)
{
    auto it = m_listWorkAlloc.begin();
    while (it)
    {
        SHOTWORK* pWork = &(*it);
        CShot* pShot = pWork->m_pShot;
        ASSERT(pShot);

        if (pShot->IsEnd())
        {
            delete pShot;
            
            pWork->m_pShot = nullptr;
            
            it = m_listWorkAlloc.erase(it);
            m_listWorkPool.push_back(pWork);
        }
        else
        {
            ++it;
        };
    };
};


uint32 CShotList::Regist(SHOTID::VALUE idShot, const RwV3d* pvPos, const RwV3d* pvVec, CGameObject* pObject, float fRadian, float fLife)
{
    ASSERT(!m_listWorkPool.empty());

    SHOTWORK* pWork = m_listWorkPool.front();
    ASSERT(pWork);
    m_listWorkPool.erase(pWork);
    m_listWorkAlloc.push_back(pWork);
    ASSERT(!pWork->m_pShot);

    CShot::PARAMETER param;
    param.m_idShot = idShot;
    param.m_vPosition = *pvPos;
    param.m_vDirection = *pvVec;
    param.m_pObject = pObject;
    param.m_fLifetime = fLife;

    CShot* pShot = CShot::New(&param);
    ASSERT(pShot);
    pWork->m_pShot = pShot;

    return pShot->GetHandle();
};


bool CShotList::IsExist(SHOTID::VALUE idShot) const
{
    ASSERT(idShot > SHOTID::ID_UNKNOWN && idShot < SHOTID::ID_MAX);

    for (SHOTWORK& it : m_listWorkAlloc)
    {
        CShot* pShot = it.m_pShot;
        ASSERT(pShot);
        
        if (pShot->GetID() == idShot)
            return true;
    };

    return false;
};


static CShotList* s_pShotList = nullptr;


static CShotList& ShotList(void)
{
    ASSERT(s_pShotList);
    return *s_pShotList;
};


static inline bool GetObjectPositionForCorrectDirection(CGameObject* pGameObject, RwV3d* pResult)
{
    ASSERT(pGameObject);
    ASSERT(pResult);
    
    bool bResult = false;
    
    switch (pGameObject->GetType())
    {
    case GAMEOBJECTTYPE::ENEMY:
        {
            CEnemy* pEnemy = (CEnemy*)pGameObject;
            pEnemy->GetPosition(pResult);            
            bResult = true;
        }
        break;

    case GAMEOBJECTTYPE::GIMMICK:
        {
            CGimmick* pGimmick = (CGimmick*)pGameObject;
            if (IS_FLAG_SET(pGimmick->GetFeatures(), GIMMICKTYPES::FEATURE_HOMING))
            {
                pGimmick->GetCenterPosition(pResult);                
                bResult = true;
            };
        }
        break;
    };

    return bResult;
};


static inline bool IsRequireCorrectDirection(MAGICID::VALUE idMagic)
{
    return (idMagic == MAGICID::ID_DON_LASER_BEAM);
};


static inline bool IsRequireCorrectDirection(SHOTID::VALUE idShot)
{
    return (idShot == SHOTID::ID_PACK           ||
            idShot == SHOTID::ID_KUNAI_PLAYER   ||
            idShot == SHOTID::ID_KNIFE_PLAYER);
};


static void CorrectDirection(RwV3d* pvPosition, RwV3d* pvVector, float fRadian)
{
    ASSERT(pvPosition);
    ASSERT(pvVector);
    
    RwV3d vInitDirection = *pvVector;
    
    CGameObject* pTargetObject = nullptr;
    float fTargetDistance = fRadian;
    RwV3d vTargetDirection = Math::VECTOR3_ZERO;
    
    CGameObject* pGameObject = CGameObjectManager::GetNext(nullptr);
    while (pGameObject)
    {
        RwV3d vObjectPosition = Math::VECTOR3_ZERO;
        if (GetObjectPositionForCorrectDirection(pGameObject, &vObjectPosition))
        {
            RwV3d vDistance = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vDistance, &vObjectPosition, pvPosition);

            if (Math::Vec3_Length(&vDistance) < 20.0f)
            {
                RwV3d vDirection = Math::VECTOR3_ZERO;
                Math::Vec3_Normalize(&vDirection, &vDistance);

                float fDistance = (1.0f - Math::Vec3_Dot(&vInitDirection, &vDirection)) * Math::PI05;
                if (fDistance < fTargetDistance)
                {
                    fTargetDistance = fDistance;
                    vTargetDirection = vDirection;
                    pTargetObject = pGameObject;
                };
            };
        };
        
        pGameObject = CGameObjectManager::GetNext(pGameObject);
    };

    if (pTargetObject)
        *pvVector = vTargetDirection;
};


/*static*/ void CShotManager::Initialize(void)
{
    if (!s_pShotList)
    {
        s_pShotList = new CShotList;
        ASSERT(s_pShotList);
    };
};


/*static*/ void CShotManager::Terminate(void)
{
    if (s_pShotList)
    {
        delete s_pShotList;
        s_pShotList = nullptr;
    };
};


/*static*/ void CShotManager::Draw(void)
{
    ShotList().Draw();
};


/*static*/ void CShotManager::Period(void)
{
    ShotList().Run();
};


/*static*/ uint32 CShotManager::Shot(SHOTID::VALUE idShot, const RwV3d* pvPos, float fDirection, CGameObject* pObject, float fRadian, float fLife)
{
    ASSERT(pvPos);
    ASSERT(pObject);

    RwMatrix matRotY;
    RwMatrixSetIdentityMacro(&matRotY);
    Math::Matrix_RotateY(&matRotY, fDirection);
    
    RwV3d vDirection = Math::VECTOR3_AXIS_Z;
    RwV3dTransformPoint(&vDirection, &vDirection, &matRotY);

    return Shot(idShot, pvPos, &vDirection, pObject, fRadian, fLife);
};


/*static*/ uint32 CShotManager::Shot(SHOTID::VALUE idShot, const RwV3d* pvPos, const RwV3d* pvVec, CGameObject* pObject, float fRadian, float fLife)
{
    ASSERT(pvPos);
    ASSERT(pvVec);
    ASSERT(pObject);

    RwV3d vPos = *pvPos;
    RwV3d vVec = *pvVec;
    
    if (IsRequireCorrectDirection(idShot))
        CorrectDirection(&vPos, &vVec, fRadian);

    uint32 hShot = ShotList().Regist(idShot, &vPos, &vVec, pObject, fRadian, fLife);
    ASSERT(hShot);
    
    return hShot;
};


/*static*/ uint32 CShotManager::Shot(MAGICID::VALUE idMagic, const RwV3d* pvPos, const RwV3d* pvVec, CGameObject* pObject, float fRadian)
{
    ASSERT(pvPos);
    ASSERT(pvVec);
    ASSERT(pObject);
    
    RwV3d vPos = *pvPos;
    RwV3d vVec = *pvVec;
    
    if (IsRequireCorrectDirection(idMagic))
        CorrectDirection(&vPos, &vVec, fRadian);

    CMagicManager::CParameter param;
    param.SetObject(pObject);
    param.SetPositon(&vPos);
    param.SetDirection(&vVec);

    uint32 hMagic = CMagicManager::Play(idMagic, &param);
    ASSERT(hMagic);
    
    return hMagic;
};


/*static*/ bool CShotManager::IsExist(SHOTID::VALUE idShot)
{
    return ShotList().IsExist(idShot);
};


/*static*/ void CShotManager::Finish(uint32 hShot)
{
    CGameObject* pGameObject = CGameObjectManager::GetObject(hShot);
    if(pGameObject)
    {
        switch (pGameObject->GetType())
        {
        case GAMEOBJECTTYPE::EFFECT:
            {
                CMagicManager::Finish(hShot);
            }
            break;

        case GAMEOBJECTTYPE::SHOT:
            {
                CShot* pShot = (CShot*)pGameObject;
                pShot->Finish();
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};