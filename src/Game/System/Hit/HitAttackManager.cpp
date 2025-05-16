#include "HitAttackManager.hpp"
#include "HitAttackData.hpp"
#include "HitCatchData.hpp"
#include "Intersection.hpp"
#ifdef _DEBUG
#include "HitDebug.hpp"
#endif /* _DEBUG */

#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectType.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Misc/DebugShape.hpp"


#ifdef _DEBUG
#define DATA_POOL_SIZE (128)
#else /* _DEBUG */
#define DATA_POOL_SIZE (128)
#endif /* _DEBUG */


class CHitRecordContainer;


static CHitRecordContainer* s_pHitRecordContainer = nullptr;


static inline CHitRecordContainer& HitRecordContainer(void)
{
    ASSERT(s_pHitRecordContainer);
    return *s_pHitRecordContainer;
};


class CCatchDataRecord : public CListNode<CCatchDataRecord>
{
public:
    CCatchDataRecord(void);
    ~CCatchDataRecord(void);
    void Cleanup(void);
    void SetNewAllocated(bool bSet);
    bool IsNewAllocated(void) const;
    void SetRecord(const CHitCatchData* pHitCatchData);
    bool IsEqual(const CHitCatchData* pHitCatchData) const;
    
private:
    bool m_bIsNewAllocated;
    uint32 m_uObjHandle;
    int32 m_nCatchNo;
};


class CAttackDataRecord : public CListNode<CAttackDataRecord>
{
public:
    CAttackDataRecord(void);
    ~CAttackDataRecord(void);
    void Cleanup(void);
    void SetNewAllocated(bool bSet);
    bool IsNewAllocated(void) const;
    void SetAttackRecord(const CHitAttackData* pHitAttackData);
    void SetCatchRecord(CCatchDataRecord* pCatchDataRecord);
    bool IsEqual(const CHitAttackData* pHitAttackData) const;
    bool IsHit(const CHitCatchData* pHitCatchData) const;
    bool IsTroubleAttack(void) const;

private:
    bool m_bIsNewAllocated;
    uint32 m_uObjHandle;
    char m_szMotionName[16];
    int32 m_nAttackNo;
    bool m_bIsTroubleAttack;
    CList<CCatchDataRecord> m_listCatchDataRecord;
};


class CHitRecordContainer
{
public:
    CHitRecordContainer(void);
    ~CHitRecordContainer(void);
    void Cleanup(CList<CAttackDataRecord>* pList);
    void Period(void);
    void SetCurrentAttackData(const CHitAttackData* pHitAttackData);
    void SetHitRecord(const CHitCatchData* pHitCatchData);
    CCatchDataRecord* GetCatchRecord(void);
    CAttackDataRecord* GetAttackRecord(void);
    void RestoreAttackRecord(CAttackDataRecord* pAttackDataRecord);
    void RestoreCatchRecord(CCatchDataRecord* pCatchDataRecord);
    bool IsHit(const CHitCatchData* pHitCatchData);

private:
    CAttackDataRecord m_aAttackDataRecord[DATA_POOL_SIZE];
    CCatchDataRecord m_aCatchDataRecord[DATA_POOL_SIZE];
    CList<CAttackDataRecord> m_listAttackPool;
    CList<CCatchDataRecord> m_listCatchPool;
    CList<CAttackDataRecord>* m_pListOld;
    CList<CAttackDataRecord>* m_pListNew;
    CList<CAttackDataRecord> m_listPool;
    CList<CAttackDataRecord> m_listAlloc;
    CAttackDataRecord* m_pNowAttackRecord;
    CAttackDataRecord* m_pPrevAttackRecord;
};


class CHitAttackContainer
{
public:
    CHitAttackContainer(void);
    ~CHitAttackContainer(void);
    void Cleanup(void);
    void CleanupAttack(CList<CHitAttackData>* pList);
    void CleanupCatch(CList<CHitCatchData>* pList);
    void Period(void);
    void Dispatch(CHitAttackData* pAttack, CHitCatchData* pCatch);
    void RegistAttack(const CHitAttackData* pHitAttack);
    void RegistCatch(const CHitCatchData* pHitCatch);
    void SetPause(bool bPause);
    bool IsPaused(void) const;
    bool IsHitTarget(const CHitAttackData* pAttack, const CHitCatchData* pCatch) const;

private:
    CHitAttackData m_aHitAttackData[DATA_POOL_SIZE];
    CHitCatchData m_aHitCatchData[DATA_POOL_SIZE];
    CList<CHitAttackData> m_listAttackPool;
    CList<CHitCatchData> m_listCatchPool;
    CList<CHitAttackData> m_listAttackAlloc;
    CList<CHitAttackData> m_listAttackShotAlloc;
    CList<CHitCatchData> m_listCatchAlloc;
    bool m_bPause;
};


CCatchDataRecord::CCatchDataRecord(void)
: m_bIsNewAllocated(true)
, m_uObjHandle(0)
, m_nCatchNo(0)
{
    Cleanup();
};


CCatchDataRecord::~CCatchDataRecord(void)
{
    Cleanup();
};


void CCatchDataRecord::Cleanup(void)
{
    m_uObjHandle = 0;
    m_nCatchNo = 0;
};


void CCatchDataRecord::SetNewAllocated(bool bSet)
{
    m_bIsNewAllocated = bSet;
};


bool CCatchDataRecord::IsNewAllocated(void) const
{
    return m_bIsNewAllocated;
};


void CCatchDataRecord::SetRecord(const CHitCatchData* pHitCatchData)
{
    m_nCatchNo = pHitCatchData->GetCatchNo();
    m_uObjHandle = pHitCatchData->GetObjectHandle();
};


bool CCatchDataRecord::IsEqual(const CHitCatchData* pHitCatchData) const
{
    return  (pHitCatchData->GetObjectHandle() == m_uObjHandle) &&
            (pHitCatchData->GetCatchNo() == m_nCatchNo);
};


CAttackDataRecord::CAttackDataRecord(void)
: m_bIsNewAllocated(true)
, m_uObjHandle(0)
, m_nAttackNo(0)
, m_bIsTroubleAttack(false)    
{
    m_szMotionName[0] = '\0';

    Cleanup();
};


CAttackDataRecord::~CAttackDataRecord(void)
{
    Cleanup();
};


void CAttackDataRecord::Cleanup(void)
{
    auto it = m_listCatchDataRecord.begin();
    while (it)
    {
        CCatchDataRecord* pCatchDataRecord = &(*it);
        it = m_listCatchDataRecord.erase(it);

        HitRecordContainer().RestoreCatchRecord(pCatchDataRecord);
    };

    m_uObjHandle = 0;
    m_nAttackNo = 0;
    m_bIsTroubleAttack = false;
    m_szMotionName[0] = '\0';;
};


void CAttackDataRecord::SetNewAllocated(bool bSet)
{
    m_bIsNewAllocated = bSet;
};


bool CAttackDataRecord::IsNewAllocated(void) const
{
    return m_bIsNewAllocated;
};


void CAttackDataRecord::SetAttackRecord(const CHitAttackData* pHitAttackData)
{    
    m_uObjHandle = pHitAttackData->GetObjectHandle();
    m_nAttackNo = pHitAttackData->GetAttackNo();
    m_bIsTroubleAttack = pHitAttackData->IsTroubleAttack();
    std::strcpy(m_szMotionName, pHitAttackData->GetMotion());
};


void CAttackDataRecord::SetCatchRecord(CCatchDataRecord* pCatchDataRecord)
{
    m_listCatchDataRecord.push_back(pCatchDataRecord);
};


bool CAttackDataRecord::IsEqual(const CHitAttackData* pHitAttackData) const
{
    return (pHitAttackData->GetObjectHandle() == m_uObjHandle &&
            pHitAttackData->GetAttackNo() == m_nAttackNo &&
            !std::strcmp(pHitAttackData->GetMotion(), m_szMotionName));
};


bool CAttackDataRecord::IsHit(const CHitCatchData* pHitCatchData) const
{
    for (CCatchDataRecord& it : m_listCatchDataRecord)
    {
        if (it.IsEqual(pHitCatchData))
            return true;
    };

    return false;
};


bool CAttackDataRecord::IsTroubleAttack(void) const
{
    return m_bIsTroubleAttack;
};


CHitRecordContainer::CHitRecordContainer(void)
: m_pListOld(&m_listPool)
, m_pListNew(&m_listAlloc)
, m_pNowAttackRecord(nullptr)
, m_pPrevAttackRecord(nullptr)
{

    for (int32 i = 0; i < COUNT_OF(m_aAttackDataRecord); ++i)
    {
        m_aAttackDataRecord[i].SetNewAllocated(false);
        m_listAttackPool.push_back(&m_aAttackDataRecord[i]);
    };

    for (int32 i = 0; i < COUNT_OF(m_aCatchDataRecord); ++i)
    {
        m_aCatchDataRecord[i].SetNewAllocated(false);
        m_listCatchPool.push_back(&m_aCatchDataRecord[i]);
    };
};


CHitRecordContainer::~CHitRecordContainer(void)
{
    Cleanup(&m_listPool);
    Cleanup(&m_listAlloc);
};


void CHitRecordContainer::Cleanup(CList<CAttackDataRecord>* pList)
{
    ASSERT(pList);
    
    auto it = pList->begin();
    auto itEnd = pList->end();
    while (it != itEnd)
    {
        CAttackDataRecord* pAttack = &(*it);
        ASSERT(pAttack);
        
        it = pList->erase(it);

        RestoreAttackRecord(pAttack);
    };
};


void CHitRecordContainer::Period(void)
{
    Cleanup(m_pListOld);

    if (m_pListOld == &m_listPool)
    {
        m_pListOld = m_pListNew;
        m_pListNew = &m_listPool;
    }
    else
    {
        m_pListOld = m_pListNew;
        m_pListNew = &m_listAlloc;
    };

    m_pNowAttackRecord = nullptr;
    m_pPrevAttackRecord = nullptr;
};


void CHitRecordContainer::SetCurrentAttackData(const CHitAttackData* pHitAttackData)
{
    if (m_pPrevAttackRecord && !m_pPrevAttackRecord->IsEqual(pHitAttackData))
    {
        m_pPrevAttackRecord->unlink();
        RestoreAttackRecord(m_pPrevAttackRecord);
        m_pPrevAttackRecord = nullptr;
    };

    if (!m_pPrevAttackRecord)
    {
        ASSERT(m_pListOld);
        
        for (CAttackDataRecord& it : *m_pListOld)
        {
            if (it.IsEqual(pHitAttackData))
            {
                m_pPrevAttackRecord = &it;
                break;
            };
        };
    };

    if (!m_pNowAttackRecord || !m_pNowAttackRecord->IsEqual(pHitAttackData))
    {
        if (m_pPrevAttackRecord && m_pPrevAttackRecord->IsTroubleAttack())
        {
            m_pPrevAttackRecord->unlink();
            m_pNowAttackRecord = m_pPrevAttackRecord;
            m_pPrevAttackRecord = nullptr;
        }
        else
        {
            CAttackDataRecord* pAttackDataRecord = GetAttackRecord();
            ASSERT(pAttackDataRecord);

            m_pNowAttackRecord = pAttackDataRecord;
            m_pNowAttackRecord->SetAttackRecord(pHitAttackData);
        };

        ASSERT(m_pNowAttackRecord);
        
        m_pListNew->push_back(m_pNowAttackRecord);
    };
};


void CHitRecordContainer::SetHitRecord(const CHitCatchData* pHitCatchData)
{
    ASSERT(m_pNowAttackRecord);
    
    CCatchDataRecord* pCatchDataRecord = GetCatchRecord();
    ASSERT(pCatchDataRecord);

    pCatchDataRecord->SetRecord(pHitCatchData);
    m_pNowAttackRecord->SetCatchRecord(pCatchDataRecord);
};


CCatchDataRecord* CHitRecordContainer::GetCatchRecord(void)
{
    CCatchDataRecord* pRet = nullptr;

    if (!m_listCatchPool.empty())
    {
        CCatchDataRecord* pNode = m_listCatchPool.front();
        m_listCatchPool.erase(pNode);

        pRet = pNode;
    }
    else
    {
        pRet = new CCatchDataRecord;
    };

    return pRet;
};


CAttackDataRecord* CHitRecordContainer::GetAttackRecord(void)
{
    CAttackDataRecord* pRet = nullptr;

    if (!m_listAttackPool.empty())
    {
        CAttackDataRecord* pNode = m_listAttackPool.front();
        m_listAttackPool.erase(pNode);
        
        pRet = pNode;
    }
    else
    {
        pRet = new CAttackDataRecord;
    };
    
    return pRet;
};


void CHitRecordContainer::RestoreAttackRecord(CAttackDataRecord* pAttackDataRecord)
{
    ASSERT(pAttackDataRecord);
    
    if (pAttackDataRecord->IsNewAllocated())
    {
        delete pAttackDataRecord;
    }
    else
    {
        pAttackDataRecord->Cleanup();
        m_listAttackPool.push_back(pAttackDataRecord);
    };
};


void CHitRecordContainer::RestoreCatchRecord(CCatchDataRecord* pCatchDataRecord)
{
    ASSERT(pCatchDataRecord);
    
    if (pCatchDataRecord->IsNewAllocated())
    {
        delete pCatchDataRecord;
    }
    else
    {
        pCatchDataRecord->Cleanup();
        m_listCatchPool.push_back(pCatchDataRecord);
    };
};


bool CHitRecordContainer::IsHit(const CHitCatchData* pHitCatchData)
{
    ASSERT(m_pNowAttackRecord);

    if (m_pNowAttackRecord->IsHit(pHitCatchData))
        return true;

    SetHitRecord(pHitCatchData);

    if (m_pPrevAttackRecord && m_pPrevAttackRecord->IsHit(pHitCatchData))
        return true;

    return false;
};


CHitAttackContainer::CHitAttackContainer(void)
: m_bPause(false)
{
    for (int32 i = 0; i < COUNT_OF(m_aHitAttackData); ++i)
    {
        m_aHitAttackData[i].SetFlagAllocated(false);
        m_listAttackPool.push_back(&m_aHitAttackData[i]);
    };

    for (int32 i = 0; i < COUNT_OF(m_aHitCatchData); ++i)
    {
        m_aHitCatchData[i].SetFlagAllocated(false);
        m_listCatchPool.push_back(&m_aHitCatchData[i]);
    };
};


CHitAttackContainer::~CHitAttackContainer(void)
{
    Cleanup();
};


void CHitAttackContainer::Cleanup(void)
{
    CleanupCatch(&m_listCatchAlloc);
    CleanupAttack(&m_listAttackShotAlloc);
    CleanupAttack(&m_listAttackAlloc);
};


void CHitAttackContainer::CleanupAttack(CList<CHitAttackData>* pList)
{
    ASSERT(pList);
    
    auto it = pList->begin();
    auto itEnd = pList->end();
    while (it != itEnd)
    {
        CHitAttackData* pHitAttackData = &(*it);

        it = pList->erase(it);

        if (pHitAttackData->IsFlagAllocated())
        {
            delete pHitAttackData;
        }
        else
        {
            pHitAttackData->Cleanup();
            m_listAttackPool.push_back(pHitAttackData);
        };
    };
};


void CHitAttackContainer::CleanupCatch(CList<CHitCatchData>* pList)
{
    ASSERT(pList);

    auto it = pList->begin();
    auto itEnd = pList->end();
    while (it != itEnd)
    {
        CHitCatchData* pHitCatchData = &(*it);
        
        it = pList->erase(it);

        if (pHitCatchData->IsFlagAllocated())
        {
            delete pHitCatchData;
        }
        else
        {
            pHitCatchData->Cleanup();
            m_listCatchPool.push_back(pHitCatchData);
        };
    };
};


void CHitAttackContainer::Period(void)
{
    if (m_bPause)    
    {
        Cleanup();
        return;
    };
    
    for (CHitAttackData& itAttack : m_listAttackAlloc)
    {
        if (!itAttack.GetObject() || !itAttack.IsFlagAlive())
            continue;

        HitRecordContainer().SetCurrentAttackData(&itAttack);

        for (CHitCatchData& itCatch : m_listCatchAlloc)
        {       
            if (!itCatch.GetObject() || !itCatch.IsFlagAlive())
                continue;
            
            if (!IsHitTarget(&itAttack, &itCatch))
                continue;

            switch (itAttack.GetShape())
            {
            case CHitAttackData::SHAPE_SPHERE:
                {
                    RwSphere sphereP = *itAttack.GetSphere();
                    RwSphere sphereQ = *itCatch.GetSphere();
                    
                    bool bIntersection = Intersection::SphereAndSphere(&sphereP.center,
                                                                       sphereP.radius,
                                                                       &sphereQ.center,
                                                                       sphereQ.radius);
                    if (bIntersection)
                    {
						if(!HitRecordContainer().IsHit(&itCatch))
							Dispatch(&itAttack, &itCatch);
                    };
                }
                break;

            case CHitAttackData::SHAPE_LINE:
                {
                    RwSphere sphere = *itCatch.GetSphere();
                    RwLine line = *itAttack.GetLine();
                    float fHitDist = 0.0f;
                        
                    bool bIntersection = Intersection::LineAndSphere(&line, &sphere, &fHitDist);
                    
                    if (bIntersection)
                    {
                        itAttack.SetHitDistance(fHitDist);
                        if (!HitRecordContainer().IsHit(&itCatch))
                            Dispatch(&itAttack, &itCatch);
                    };
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };

    Cleanup();
};


void CHitAttackContainer::Dispatch(CHitAttackData* pAttack, CHitCatchData* pCatch)
{
    ASSERT(pAttack);
    ASSERT(pCatch);

    CHitAttackData hitAttack;
    pAttack->CopyData(&hitAttack);

    CHitCatchData hitCatch;
    pCatch->CopyData(&hitCatch);
    
    hitAttack.SetCatch(&hitCatch);
    
    hitCatch.SetAttack(&hitAttack);
    hitCatch.SetFlagHitPosCalculated(false);

    CGameObjectManager::SendMessage(hitCatch.GetObject(),
                                    GAMEOBJECTTYPES::MESSAGEID_CATCHATTACK,
                                    &hitAttack);

    CGameObjectManager::SendMessage(hitAttack.GetObject(),
                                    GAMEOBJECTTYPES::MESSAGEID_ATTACKRESULT,
                                    &hitCatch);

    if (pCatch->GetObjectType() == GAMEOBJECTTYPE::SHOT)
    {
        for (CHitAttackData& it : m_listAttackAlloc)
        {
            if (it.GetObjectHandle() == pCatch->GetObjectHandle())
                it.SetFlagAlive(false);
        };

        pCatch->SetFlagAlive(false);
    };
};


void CHitAttackContainer::RegistAttack(const CHitAttackData* pHitAttack)
{
    ASSERT(pHitAttack);
    ASSERT(!m_listAttackPool.empty());

    if (m_listAttackPool.empty())
        return;
    
    CHitAttackData* pNode = m_listAttackPool.front();
    m_listAttackPool.erase(pNode);
    m_listAttackAlloc.push_back(pNode);

    pHitAttack->CopyData(pNode);
};


void CHitAttackContainer::RegistCatch(const CHitCatchData* pHitCatch)
{
    ASSERT(pHitCatch);
    ASSERT(!m_listCatchPool.empty());

    if (m_listCatchPool.empty())
        return;

    CHitCatchData* pNode = m_listCatchPool.front();
    m_listCatchPool.erase(pNode);
    m_listCatchAlloc.push_back(pNode);

    pHitCatch->CopyData(pNode);
};


void CHitAttackContainer::SetPause(bool bPause)
{
    m_bPause = bPause;
};


bool CHitAttackContainer::IsPaused(void) const
{
    return m_bPause;
};


bool CHitAttackContainer::IsHitTarget(const CHitAttackData* pAttack, const CHitCatchData* pCatch) const
{
    ASSERT(pAttack);
    ASSERT(pCatch);
    ASSERT(pAttack->GetObject());
    ASSERT(pCatch->GetObject());

    if (pAttack->GetObjectHandle() == pCatch->GetObjectHandle())
        return ((pAttack->GetTarget() & CHitAttackData::TARGET_SELF) != 0);

    switch (pCatch->GetObjectType())
    {
    case GAMEOBJECTTYPE::GIMMICK:
        return ((pAttack->GetTarget() & CHitAttackData::TARGET_GIMMICK) != 0);
        
    case GAMEOBJECTTYPE::CHARACTER:
        {
            CCharacter* pCharacter = (CCharacter*)pCatch->GetObject();

            switch (pCharacter->GetCharacterType())
            {
            case CCharacter::TYPE_ENEMY:
                return ((pAttack->GetTarget() & CHitAttackData::TARGET_ENEMY) != 0);

            case CCharacter::TYPE_PLAYER:
                return ((pAttack->GetTarget() & CHitAttackData::TARGET_PLAYER) != 0);

            default:
                ASSERT(false);
                break;
            };
        }
        return false;

    case GAMEOBJECTTYPE::EFFECT:
        return ((pAttack->GetTarget() & CHitAttackData::TARGET_ALL_EXCEPT_SELF) != 0);

    case GAMEOBJECTTYPE::SHOT:
        return ((pAttack->GetTarget() & CHitAttackData::TARGET_SHOT) != 0);

    default:
        ASSERT(false);
        break;
    };

    return false;
};


static CHitAttackContainer* s_pHitAttackContainer = nullptr;


static inline CHitAttackContainer& HitAttackContainer(void)
{
    ASSERT(s_pHitAttackContainer);
    return *s_pHitAttackContainer;
};


/*static*/ void CHitAttackManager::Initialize(void)
{
    if (!s_pHitAttackContainer)
    {
        s_pHitAttackContainer = new CHitAttackContainer;

        ASSERT(!s_pHitRecordContainer);
        s_pHitRecordContainer = new CHitRecordContainer;
    };
};


/*static*/ void CHitAttackManager::Terminate(void)
{
    if (s_pHitAttackContainer)
    {
        ASSERT(s_pHitRecordContainer);
        delete s_pHitRecordContainer;
        s_pHitRecordContainer = nullptr;
        
        delete s_pHitAttackContainer;
        s_pHitAttackContainer = nullptr;
    };
};


/*static*/ void CHitAttackManager::Period(void)
{
    HitAttackContainer().Period();
    
    if (!HitAttackContainer().IsPaused())
        HitRecordContainer().Period();
};


/*static*/ void CHitAttackManager::Pause(void)
{
    HitAttackContainer().SetPause(true);
};


/*static*/ void CHitAttackManager::Resume(void)
{
    HitAttackContainer().SetPause(false);
};


/*static*/ void CHitAttackManager::RegistAttack(const CHitAttackData* pHitAttack)
{
    ASSERT(pHitAttack);
    
#ifdef _DEBUG
    if (CHitDebug::SHOW_HIT_ATTACK)
    {
        RwSphere sphere = { 0 };
        
        switch (pHitAttack->GetShape())
        {
        case CHitAttackData::SHAPE_LINE:
            sphere.center = pHitAttack->GetLine()->start;
            break;

        case CHitAttackData::SHAPE_SPHERE:
            sphere = *pHitAttack->GetSphere();
            break;

        default:
            ASSERT(false);
            break;
        };

        CDebugShape::ShowSphere(&sphere, { 0xFF, 0xFF, 0x00, 0xFF });
    };
#endif /* _DEBUG */
    
    ASSERT(pHitAttack->GetObject());
    HitAttackContainer().RegistAttack(pHitAttack);
};


/*static*/ void CHitAttackManager::RegistCatch(const CHitCatchData* pHitCatch)
{
    ASSERT(pHitCatch);
    
#ifdef _DEBUG
    if (CHitDebug::SHOW_HIT_CATCH)
    {
        RwSphere sphere = { 0 };

        switch (pHitCatch->GetShape())
        {
        case CHitCatchData::SHAPE_SPHERE:
            sphere = *pHitCatch->GetSphere();
            break;

        default:
            ASSERT(false);
            break;
        };

        CDebugShape::ShowSphere(&sphere, { 0xFF, 0x00, 0xFF, 0xFF });

        if (CHitDebug::SHOW_HIT_CATCH_NO)
        {
            char szTmpBuff[16];
            szTmpBuff[0] = '\0';

            std::sprintf(szTmpBuff, "%" PRId32, pHitCatch->GetCatchNo());

            sphere.center.y += (sphere.radius * 1.0f);

            CDebugShape::m_fDuration = 0.0f;
            CDebugShape::m_fLabelHeight = 17.7f;
            CDebugShape::ShowLabel(&sphere.center, szTmpBuff, { 0xFF, 0xFF, 0xFF, 0xFF });
        };
    };
#endif /* _DEBUG */
    
    ASSERT(pHitCatch->GetObject());
    HitAttackContainer().RegistCatch(pHitCatch);
};