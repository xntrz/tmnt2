#include "BaseAI6034.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/MotionParameter/MotionParameterController.hpp"
#include "Game/System/Hit/Intersection.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


namespace Utils6034
{
    bool IsDownState(ENEMYTYPES::STATUS status)
    {
        return (status == ENEMYTYPES::STATUS_THROWN_BACK)
            || (status == ENEMYTYPES::STATUS_THROWN_FRONT)
            || (status == ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK)
            || (status == ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT)
            || (status == ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK)
            || (status == ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT)
            || (status == ENEMYTYPES::STATUS_CRASHWALL_BACK)
            || (status == ENEMYTYPES::STATUS_CRASHWALL_FRONT)
            || (status == ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK)
            || (status == ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT)
            || (status == ENEMYTYPES::STATUS_FLYAWAY_BACK)
            || (status == ENEMYTYPES::STATUS_FLYAWAY_FRONT)
            || (status == ENEMYTYPES::STATUS_GETUP);
    };


    bool IsFlyawayState(ENEMYTYPES::STATUS status)
    {
        return CEnemyUtils::IsFlyawayState(status);
    };


    bool IsParallelVector(const RwV3d* pvecA, const RwV3d* pvecB)
    {
        RwV3d vecCross = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vecCross, pvecA, pvecB);

        return (Math::Vec3_Dot(&vecCross, &vecCross) <= 0.0f);
    };


    void MakeXByDirection(RwV3d* pvecAxisX, const RwV3d* pvecDireciton, const RwV3d* pvecUp)
    {
        RwV3d vecLeft = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vecLeft, pvecDireciton, pvecUp);
        Math::Vec3_Normalize(pvecAxisX, &vecLeft);
    };


    void MakeYByDirection(RwV3d* pvecAxisY, const RwV3d* pvecDireciton, const RwV3d* pvecUp)
    {
        RwV3d vecUp = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vecUp, pvecDireciton, Math::Vec3_Dot(pvecDireciton, pvecUp));
        Math::Vec3_Sub(&vecUp, &vecUp, pvecUp);
        Math::Vec3_Normalize(pvecAxisY, &vecUp);
    };


    void MakeZByDirection(RwV3d* pvecAxisZ, const RwV3d* pvecDireciton)
    {
        Math::Vec3_Normalize(pvecAxisZ, pvecDireciton);
    };


    void MakeAxisByDirection(RwV3d* pvecAxisX,
                             RwV3d* pvecAxisY,
                             RwV3d* pvecAxisZ,
                             const RwV3d* pvecDirection,
                             const RwV3d* pvecUp)
    {
        MakeZByDirection(pvecAxisZ, pvecDirection);
        MakeYByDirection(pvecAxisY, pvecAxisZ, pvecUp);
        MakeXByDirection(pvecAxisX, pvecAxisZ, pvecAxisY);
    };


    void GetCharacterDirectionVector(const CCharacterCompositor* pChrCompositor, RwV3d* pvecDirection)
    {
        pChrCompositor->RotateVectorByDirection(pvecDirection, &Math::VECTOR3_AXIS_Z);
    };


    RwV3d GetPositionByThinkOrder(CAIThinkOrder* pAIThinkOrder)
    {
        switch (pAIThinkOrder->GetOrder())
        {
        case CAIThinkOrder::ORDER_MOVE:
            {
                if (pAIThinkOrder->OrderMove().m_iPlayerNo == -1)
                    return pAIThinkOrder->OrderMove().m_vAt;

                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(pAIThinkOrder->OrderMove().m_iPlayerNo);
                if (pPlrChr)
                {
                    RwV3d vecResult = Math::VECTOR3_ZERO;
                    pPlrChr->GetFootPosition(&vecResult);

                    return vecResult;
                };
            }
            break;

        case CAIThinkOrder::ORDER_ATTACK:
            {
                if (pAIThinkOrder->OrderAttack().m_iPlayerNo == -1)
                    return pAIThinkOrder->OrderAttack().m_vAt;

                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(pAIThinkOrder->OrderAttack().m_iPlayerNo);
                if (pPlrChr)
                {
                    RwV3d vecResult = Math::VECTOR3_ZERO;
                    pPlrChr->GetFootPosition(&vecResult);

                    return vecResult;
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        return Math::VECTOR3_ZERO;
    };


    void GetWorldBonePosition(RwV3d* pvecWorldPos,
                              const CCharacterCompositor* pChrCompositor,
                              int32 iBoneID,
                              const RwV3d* pvecOffset /*= nullptr*/)
    {
        CEnemyUtils::GetWorldBonePosition(pvecWorldPos, pChrCompositor, iBoneID, pvecOffset);
    };


    float CalcRotationRate(float fRate, float fSpeed)
    {
        fRate *= MATH_PI;

        if (fRate > FLT_EPSILON)
            return (fSpeed / fRate);

        return 0.0f;
    };


    void CalcRotationPoint(RwV3d* pvecRotPt,
                           const RwV3d* pvecPos,
                           const RwV3d* pvecAt,
                           float fMoveSpeed,
                           float fMoveDist,
                           bool bRotRight)
    {
        RwV3d vecDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDist, pvecPos, pvecAt);

        float fDist = Math::Vec3_Length(&vecDist);

        float fRot = CalcRotationRate(fDist, fMoveSpeed);
        if (bRotRight)
            fRot *= -1.0f;

        RwMatrix matRotY;
        RwMatrixSetIdentityMacro(&matRotY);
        Math::Matrix_RotateY(&matRotY, fRot);

        RwV3dTransformVector(&vecDist, &vecDist, &matRotY);

        fDist = Math::Vec3_Length(&vecDist);
        if (fDist < fMoveDist)
        {
            Math::Vec3_Normalize(&vecDist, &vecDist);
            Math::Vec3_Scale(&vecDist, &vecDist, fMoveDist);
        };

        Math::Vec3_Add(pvecRotPt, pvecAt, &vecDist);
    };


    void GetMovePosByWorld(RwV3d* pvecMovePos, const RwV3d* pvecPos, const RwV3d* pvecAt)
    {
        RwV3d vecVel = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecVel, pvecAt, pvecPos);

        RwV3d vecPos = *pvecPos;
        CWorldMap::MOVE_HIT_FLAG hitFlag = CWorldMap::MOVE_HIT_FLAG_NONE;
        CWorldMap::CheckCollisionCharacterMove(&vecPos, pvecMovePos, &vecVel, &hitFlag, 1.0f, 2.0f);
    };


    //
    // *********************************************************************************
    //


    CSmallMemoryManager::CSmallMemoryManager(int32 iNodeSize, int32 iNodeMax)
    : m_iNodeSize(iNodeSize)
    , m_iNodeMax(iNodeMax)
    , m_iNodeAllocated(0)
    , m_pMemory(nullptr)
    , m_listMemBlockFree()
    , m_listMemBlockAlloc()
    , m_pPrev(nullptr)
    {
        ASSERT(iNodeSize > 0);
        ASSERT(iNodeMax > 0);

        int32 iTotalSize = (sizeof(MEMBLOCK) + m_iNodeSize) * m_iNodeMax;

        m_pMemory = new uint8[iTotalSize];
		std::memset(m_pMemory, 0, static_cast<size_t>(iTotalSize));

        MEMBLOCK* pMemBlock = reinterpret_cast<MEMBLOCK*>(m_pMemory);
        do
        {
#pragma push_macro("new")
#undef new
            new (pMemBlock) MEMBLOCK;
#pragma pop_macro("new")
            
#ifdef _DEBUG            
            pMemBlock->check = CHECK;
#endif /* _DEBUG */            
            
            m_listMemBlockFree.push_back(pMemBlock);
            pMemBlock = reinterpret_cast<MEMBLOCK*>(reinterpret_cast<uint8*>(pMemBlock) + sizeof(MEMBLOCK) + iNodeSize);
        } while (--iNodeMax);

        ASSERT(reinterpret_cast<uint8*>(pMemBlock) == (m_pMemory + iTotalSize));
    };

    
    /*virtual*/ CSmallMemoryManager::~CSmallMemoryManager(void)
    {
        if (m_pMemory)
        {
            delete[] m_pMemory;
            m_pMemory = nullptr;
        };
    };


    /*virtual*/ void* CSmallMemoryManager::RepAlloc(size_t size, const char* fname, int fline) /*override*/
    {
        if (m_listMemBlockFree.empty())
        {
            FATAL("No free mem blocks");
            return nullptr;
        };

        MEMBLOCK* pMemBlock = m_listMemBlockFree.front();
        m_listMemBlockFree.erase(pMemBlock);
        m_listMemBlockAlloc.push_back(pMemBlock);

        ++m_iNodeAllocated;

        return pMemBlock->memory;
    };


    /*virtual*/ void CSmallMemoryManager::RepFree(void* mem) /*override*/
    {
        ASSERT(mem != nullptr);

        ASSERT(m_iNodeAllocated > 0);
        --m_iNodeAllocated;

        MEMBLOCK* pMemBlock = reinterpret_cast<MEMBLOCK*>(reinterpret_cast<uint8*>(mem) - sizeof(MEMBLOCK));
#ifdef _DEBUG
        ASSERT(pMemBlock->check == CHECK);
#endif /* _DEBUG */
        m_listMemBlockAlloc.erase(pMemBlock);
        m_listMemBlockFree.push_front(pMemBlock);
    };


    /*virtual*/ void* CSmallMemoryManager::RepRwAlloc(size_t size, uint32 hint) /*override*/
    {
        ASSERT(false);
        FATAL("rw alloc should not be here");
        return nullptr;
    };


    /*virtual*/ void CSmallMemoryManager::RepRwFree(void* mem) /*override*/
    {
        ASSERT(false);
        FATAL("rw alloc should not be here");
    };


    /*virtual*/ void* CSmallMemoryManager::RepRwRealloc(void* mem, size_t size, uint32 hint) /*override*/
    {
        ASSERT(false);
        FATAL("rw alloc should not be here");
        return nullptr;
    };


    /*virtual*/ void* CSmallMemoryManager::RepRwCalloc(size_t objSize, size_t size, uint32 hint) /*override*/
    {
        ASSERT(false);
        FATAL("rw alloc should not be here");
        return nullptr;
    };


    /*virtual*/ size_t CSmallMemoryManager::AllocatedSize(void) /*override*/
    {
        return static_cast<size_t>(m_iNodeAllocated * (sizeof(MEMBLOCK) + m_iNodeSize));
    };


    void CSmallMemoryManager::PushGlobal(void)
    {
        m_pPrev = m_pThis;
        m_pThis = this;
    };


    void CSmallMemoryManager::PopGlobal(void)
    {
        m_pThis = m_pPrev;
        m_pPrev = nullptr;
    };


    //
    // *********************************************************************************
    //


    CFixTimer::CFixTimer(void)
    : m_bPause(false)
    , m_fStartTime(0.0f)
    , m_fElapseTime(0.0f)
    , m_fAlarmTime(0.0f)
    , m_fRemainCounterTime(0.0f)
    {
        ;
    };


    CFixTimer::CFixTimer(float fAlarmTime)
    : CFixTimer()
    {
        SetAlarmTime(fAlarmTime);
    };


    CFixTimer::~CFixTimer(void)
    {
        ;
    };


    void CFixTimer::Update(void)
    {
        if (!IsPaused())
        {
            float fCurrentElapseTime = (GetSystemTime() - m_fStartTime);

            m_fRemainCounterTime -= (fCurrentElapseTime - m_fElapseTime);
            m_fElapseTime = fCurrentElapseTime;
        };
    };


    void CFixTimer::UnPause(void)
    {
        if (IsPaused())
        {
            m_fStartTime = (GetSystemTime() - GetElapseTime());
            m_bPause = false;
        };
    };


    void CFixTimer::Start(void)
    {
        Reset();
        UnPause();
    };


    void CFixTimer::SetElapseTime(float fTime)
    {
        m_fRemainCounterTime = (m_fElapseTime + GetRemainCounterTime()) - fTime;
        m_fElapseTime = fTime;
        m_fStartTime = (GetSystemTime() - m_fElapseTime);
    };


    void CFixTimer::SetCounterTime(float fTime)
    {
        m_fRemainCounterTime = fTime;
    };


    void CFixTimer::AddCounterTime(float fAddTime)
    {
        m_fRemainCounterTime = (fAddTime + GetRemainCounterTime());
    };


    void CFixTimer::SetAlarmTime(float fTime)
    {
        m_fAlarmTime = fTime;
    };


    void CFixTimer::Reset(void)
    {
        m_fStartTime = GetSystemTime();
		m_fElapseTime = 0.0f;
        m_fRemainCounterTime = 0.0f;
    };


    void CFixTimer::Pause(void)
    {
        m_bPause = true;
    };


    bool CFixTimer::IsPaused(void) const
    {
        return m_bPause;
    };


    bool CFixTimer::IsCounterPaused(void) const
    {
        return (m_fRemainCounterTime <= 0.0f);
    };


    bool CFixTimer::IsAlarm(void) const
    {
        if (!IsPaused())
            return (m_fAlarmTime <= GetElapseTime());

        return false;
    };


    float CFixTimer::GetSystemTime(void) const
    {
        return CGameProperty::GetTotalElapsedTime();
    };


    float CFixTimer::GetRemainCounterTime(void) const
    {
        if (m_fRemainCounterTime < 0.0f)
            return 0.0f;

        return m_fRemainCounterTime;
    };


    float CFixTimer::GetElapseTime(void) const
    {
        return m_fElapseTime;
    };


    //
    // *********************************************************************************
    //


    CFlagController::CFlagController(void)
    : m_timer(0.0f)
    , m_iCounter(0)
    {
        Start();
    };


    CFlagController::~CFlagController(void)
    {
        ;
    };


    void CFlagController::Update(void)
    {
        m_timer.Update();
    };


    void CFlagController::Start(void)
    {
        Clear();
        m_timer.Start();
    };


    void CFlagController::SetTime(float fTime)
    {
        m_timer.Update();
        m_timer.SetCounterTime(fTime);
    };


    void CFlagController::SetCounter(int32 iCounter)
    {
        m_iCounter = iCounter;
    };


    bool CFlagController::IsEnableFlag(void) const
    {
        if (m_iCounter <= 0)
            return !m_timer.IsCounterPaused();

        return (m_iCounter > 0);
    };


    void CFlagController::IncCounter(void)
    {
        ++m_iCounter;
    };


    void CFlagController::DecCounter(void)
    {
        --m_iCounter;
    };


    void CFlagController::Clear(void)
    {
        SetTime(0.0f);
        SetCounter(0);
    };


    void CFlagController::AddTime(float fTime)
    {
        m_timer.Update();
        m_timer.AddCounterTime(fTime);
    };


    int32 CFlagController::GetCounter(void) const
    {
        return m_iCounter;
    };

    
    const CFixTimer& CFlagController::GetTimer(void) const
    {
        return m_timer;
    };


    //
    // *********************************************************************************
    //


    CDamageCounter::CDamageCounter(void)
    : m_iOverflowCount(100)
    , m_iOverflowQuantity(10000)
    , m_iCurrentCount(0)
    , m_iCurrentQuantity(0)
    , m_resetTimer(0.0f)
    {
        ;
    };


    CDamageCounter::~CDamageCounter(void)
    {
        ;
    };


    void CDamageCounter::Update(void)
    {
        m_resetTimer.Update();
        if (m_resetTimer.IsAlarm())
            Reset();
    };


    void CDamageCounter::Start(void)
    {
        Reset();
        m_resetTimer.Start();
    };


    void CDamageCounter::SetOverflowData(int32 iOverflowCount, int32 iOverflowQuantity, float fAlaramTime)
    {
        m_iOverflowCount = iOverflowCount;
        m_iOverflowQuantity = iOverflowQuantity;
        m_resetTimer.SetAlarmTime(fAlaramTime);
    };


    void CDamageCounter::Reset(void)
    {
        m_iCurrentCount = 0;
        m_iCurrentQuantity = 0;
        m_resetTimer.Reset();
    };


    bool CDamageCounter::IsOverflowDamage(void) const
    {
        if (m_iCurrentCount < m_iOverflowCount)
            return (m_iCurrentQuantity >= m_iOverflowQuantity);

        return (m_iCurrentCount >= m_iOverflowCount);
    };


    void CDamageCounter::AddDamage(int32 iAmount)
    {
        m_resetTimer.Reset();
        ++m_iCurrentCount;
        m_iCurrentQuantity += iAmount;
    };


    //
    // *********************************************************************************
    //

    
    COrderController::COrderController(void)
    : m_bSendOrder(true)
    , m_bLocked(false)
    , m_pCurrentOrder(nullptr)
    , m_nextOrder()
    {
        m_nextOrder.Clear();    
    };


    COrderController::~COrderController(void)
    {
        ;
    };


    void COrderController::Update(void)
    {
        if (!IsAcceptOrder())
            return;

        if (IsSendOrder())
            ClearOrder(m_pCurrentOrder);
        else
            *m_pCurrentOrder = m_nextOrder;

        m_pCurrentOrder->SetAnswer(CAIThinkOrder::RESULT_WAITING);
        m_bSendOrder = true;
    };


    void COrderController::SetTargetOrder(CAIThinkOrder* pAIThinkOrder)
    {
        ASSERT(m_pCurrentOrder == nullptr);
    
        m_pCurrentOrder = pAIThinkOrder;
        ClearOrder(m_pCurrentOrder);
    };


    void COrderController::ClearOrder(CAIThinkOrder* pAIThinkOrder) const
    {
        pAIThinkOrder->Clear();
        pAIThinkOrder->SetOrder(CAIThinkOrder::ORDER_WAIT);
        pAIThinkOrder->SetAnswer(CAIThinkOrder::RESULT_WAITING);
    };


    bool COrderController::IsSendOrder(void) const
    {
        return m_bSendOrder;
    };


    bool COrderController::IsAcceptOrder(void) const
    {
        ASSERT(m_pCurrentOrder != nullptr);
        
        return (m_pCurrentOrder->GetAnswer() == CAIThinkOrder::RESULT_ACCEPT) ||
               (m_pCurrentOrder->GetAnswer() == CAIThinkOrder::RESULT_REFUSE);
    };


    bool COrderController::IsRefused(void) const
    {
        return (m_pCurrentOrder->GetAnswer() == CAIThinkOrder::RESULT_REFUSE);
    };


    CAIThinkOrder& COrderController::Lock(void)
    {
        ASSERT(!IsLocked());

        m_bLocked = true;
        ClearOrder(&m_nextOrder);

        return m_nextOrder;
    };


    void COrderController::Unlock(void)
    {
        ASSERT(IsLocked());

        m_bLocked = false;
        m_bSendOrder = false;
    };


    bool COrderController::IsLocked(void) const
    {
        return m_bLocked;
    };

    
    //
    // *********************************************************************************
    //


    ITracer::ITracer(void)
    : m_bEnableDirection(false)
    , m_bEnableManualUpdate(false)
    {
        ;
    };

    
    /*virtual*/ ITracer::~ITracer(void)
    {
        ;
    };


    void ITracer::SetEnableDirection(bool bEnable)
    {
        m_bEnableDirection = bEnable;
    };


    bool ITracer::IsEnabledDirection(void) const
    {
        return m_bEnableDirection;
    };


    void ITracer::SetEnableManualUpdate(bool bEnable)
    {
        m_bEnableManualUpdate = bEnable;
    };


    bool ITracer::IsEnabledManualUpdate(void) const
    {
        return m_bEnableManualUpdate;
    };


    //
    // *********************************************************************************
    //

    
    CBoneTracer::CBoneTracer(void)
    : m_pChrCompositor(nullptr)
    , m_iBoneID(-1)
    , m_vecOffset(Math::VECTOR3_ZERO)
    , m_vecDirection(Math::VECTOR3_AXIS_Z)
    , m_vecPosition(Math::VECTOR3_ZERO)
    , m_direction(Math::VECTOR3_ZERO)
    {
        ;
    };


    /*virtual*/ CBoneTracer::~CBoneTracer(void)
    {
        ;
    };


    /*virtual*/ const RwV3d& CBoneTracer::GetPosition(void) const /*override*/
    {
        return m_vecPosition;
    };


    /*virtual*/ const RwV3d& CBoneTracer::GetDirection(void) const /*override*/
    {
        return m_direction;
    };


    /*virtual*/ void CBoneTracer::OnUpdate(void) /*override*/
    {
        /* calc bone pos */
        RwV3d vecDispPos = Math::VECTOR3_ZERO;
        m_pChrCompositor->GetBonePosition(&vecDispPos, m_iBoneID, &m_vecOffset);

        RwV3d vecPosition = Math::VECTOR3_ZERO;
        m_pChrCompositor->GetPosition(&vecPosition);

        Math::Vec3_Add(&m_vecPosition, &vecPosition, &vecDispPos);

        /* calc bone dir */
        RwV3d vecBoneCenter = Math::VECTOR3_ZERO;
        m_pChrCompositor->GetBonePosition(&vecBoneCenter, m_iBoneID, &Math::VECTOR3_ZERO);

        RwV3d vecDirection = Math::VECTOR3_ZERO;
        m_pChrCompositor->GetBonePosition(&vecDirection, m_iBoneID, &m_vecDirection);

        Math::Vec3_Sub(&m_direction, &vecDirection, &vecBoneCenter);
        Math::Vec3_Normalize(&m_direction, &m_direction);
    };


    void CBoneTracer::Initialize(const CCharacterCompositor* pChrCompositor, int32 iBoneID)
    {
        ASSERT(m_pChrCompositor == nullptr);

        m_pChrCompositor = pChrCompositor;
        m_iBoneID = iBoneID;
    };


    void CBoneTracer::SetDirection(const RwV3d* pvecDirection)
    {
        m_vecDirection = *pvecDirection;
    };


    void CBoneTracer::SetOffset(const RwV3d* pvecOffset)
    {
        m_vecOffset = *pvecOffset;
    };


    //
    // *********************************************************************************
    //


    CPositionTracer::CPositionTracer(void)
    : m_vecPosition(Math::VECTOR3_ZERO)
    , m_vecDirection(Math::VECTOR3_AXIS_Z)
    {
        ;
    };


    /*virtual*/ CPositionTracer::~CPositionTracer(void)
    {
        ;
    };

    
    /*virtual*/ const RwV3d& CPositionTracer::GetPosition(void) const /*override*/
    {
        return m_vecPosition;
    };


    /*virtual*/ const RwV3d& CPositionTracer::GetDirection(void) const /*override*/
    {
        return m_vecDirection;
    };


    /*virtual*/ void CPositionTracer::OnUpdate(void) /*override*/
    {
        ;
    };


    void CPositionTracer::SetPosition(const RwV3d* pvecPosition)
    {
        m_vecPosition = *pvecPosition;
    };


    //
    // *********************************************************************************
    //


    CPlayerFootTracer::CPlayerFootTracer(void)
    : m_iPlayerID(-1)
    , m_vecPosition(Math::VECTOR3_ZERO)
    {
        ;
    };


    /*virtual*/ CPlayerFootTracer::~CPlayerFootTracer(void)
    {
        ;
    };


    /*virtual*/ const RwV3d& CPlayerFootTracer::GetPosition(void) const /*override*/
    {
        return m_vecPosition;
    };


    /*virtual*/ const RwV3d& CPlayerFootTracer::GetDirection(void) const /*override*/
    {
        return Math::VECTOR3_AXIS_Z;
    };


    /*virtual*/ void CPlayerFootTracer::OnUpdate(void) /*override*/
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_iPlayerID);
        if (pPlrChr)
            pPlrChr->GetFootPosition(&m_vecPosition);
    };


    void CPlayerFootTracer::Initialize(int32 iPlayerID)
    {
        ASSERT(m_iPlayerID == -1);

        m_iPlayerID = iPlayerID;
    };


    //
    // *********************************************************************************
    //


    ITraceEffect::ITraceEffect(void)
    : m_pTracer(nullptr)
    , m_effectTimer(0.0f)
    , m_fScale(1.0f)
    {
        m_effectTimer.SetAlarmTime(9999.9f);
    };


    /*virtual*/ ITraceEffect::~ITraceEffect(void)
    {
        ;
    };


    void ITraceEffect::Initialize(ITracer* pTracer)
    {
        ASSERT(m_pTracer == nullptr);
        m_pTracer = pTracer;
    };


    void ITraceEffect::SetScale(float fScale)
    {
        m_fScale = fScale;
    };


    void ITraceEffect::SetResetTime(float fTime)
    {
        m_effectTimer.SetAlarmTime(fTime);
    };


    //
    // *********************************************************************************
    //


    CTraceEffect::CTraceEffect(void)
    : m_hEffect(0)
    , m_effectId(EFFECTID::ID_UNKNOWN)
    {
        ;
    };


    CTraceEffect::CTraceEffect(EFFECTID::VALUE effectId, ITracer* pTracer)
    : m_hEffect(0)
    , m_effectId(effectId)
    {
        Initialize(effectId, pTracer);
    };


    /*virtual*/ CTraceEffect::~CTraceEffect(void)
    {
        ;
    };


    /*virtual*/ void CTraceEffect::Start(void) /*override*/
    {
        if (m_hEffect)
            return;
        
        m_hEffect = CEffectManager::Play(m_effectId, &m_pTracer->GetPosition());
        CEffectManager::SetScale(m_hEffect, m_fScale);

        m_effectTimer.Start();

        Update();
    };


    /*virtual*/ void CTraceEffect::Stop(void) /*override*/
    {
        if (m_hEffect)
        {
            CEffectManager::End(m_hEffect);
            m_hEffect = 0;
        };

        m_effectTimer.Reset();
    };


    /*virtual*/ void CTraceEffect::Update(void) /*override*/
    {
        if (!m_hEffect)
            return;

        m_effectTimer.Update();

        if (m_effectTimer.IsAlarm() || CEffectManager::IsEnd(m_hEffect))
            Restart();        

        if (!m_pTracer->IsEnabledManualUpdate())
            m_pTracer->OnUpdate();

        CEffectManager::SetPosition(m_hEffect, &m_pTracer->GetPosition());

        if (m_pTracer->IsEnabledDirection())
            CEffectManager::SetDirection(m_hEffect, &m_pTracer->GetDirection());
    };


    void CTraceEffect::Initialize(EFFECTID::VALUE effectId, ITracer* pTracer)
    {
        m_effectId = effectId;
        ITraceEffect::Initialize(pTracer);
    };


    void CTraceEffect::Restart(void)
    {
        if (CEffectManager::IsEnd(m_hEffect))
            m_hEffect = 0;
        else
            Stop();

        Start();
    };


    //
    // *********************************************************************************
    //


    CTraceMagic::CTraceMagic(void)
    : m_hMagic(0)
    , m_magicId(MAGICID::ID_UNKNOWN)
    {
        ;
    };


    CTraceMagic::CTraceMagic(MAGICID::VALUE magicId, ITracer* pTracer)
    : m_hMagic(0)
    , m_magicId(magicId)
    {
        Initialize(magicId, pTracer);
    };


    /*virtual*/ CTraceMagic::~CTraceMagic(void)
    {
        ;
    };


    /*virtual*/ void CTraceMagic::Start(void) /*override*/
    {
        if (m_hMagic)
            return;

        if (!m_pTracer->IsEnabledManualUpdate())
            m_pTracer->OnUpdate();

        const RwV3d* pvecDirection = nullptr;
        if (m_pTracer->IsEnabledDirection())
            pvecDirection = &m_pTracer->GetDirection();

        m_hMagic = CMagicManager::Play(MAGICID::GetNameFromID(m_magicId), &m_pTracer->GetPosition(), pvecDirection);
        CMagicManager::SetScale(m_hMagic, m_fScale);

        m_effectTimer.Start();

        Update();
    };


    /*virtual*/ void CTraceMagic::Stop(void) /*override*/
    {
        if (m_hMagic)
        {
            CMagicManager::End(m_hMagic);
            m_hMagic = 0;
        };

        m_effectTimer.Reset();
    };


    /*virtual*/ void CTraceMagic::Update(void) /*override*/
    {
        if (!m_hMagic)
            return;

        m_effectTimer.Update();

        if (m_effectTimer.IsAlarm())
            Restart();

        if (!m_pTracer->IsEnabledManualUpdate())
            m_pTracer->OnUpdate();

        CMagicManager::SetPosition(m_hMagic, &m_pTracer->GetPosition());

        if (m_pTracer->IsEnabledDirection())
            CMagicManager::SetDirection(m_hMagic, &m_pTracer->GetDirection());
    };


    void CTraceMagic::Initialize(MAGICID::VALUE magicId, ITracer* pTracer)
    {
        m_magicId = magicId;
        ITraceEffect::Initialize(pTracer);
    };


    void CTraceMagic::Restart(void)
    {
        Stop();
        Start();
    };

    
    //
    // *********************************************************************************
    //


    CBodyElecEffectManager::CBodyElecEffectManager(void)
    : m_aTraceEffect()
    , m_aBoneTracer()
    , m_abSetFlag()
    , m_fStepTime(0.0f)
    , m_fStartTime(0.0f)    
    , m_fCurrentTime(0.0f)
    , m_bToOn(false)
    , m_bFootToHead(false)
    {
        for (int32 i = 0; i < COUNT_OF(m_aTraceEffect); ++i)
        {
            m_aTraceEffect[i].Initialize(EFFECTID::ID_SHRD_ELEC, &m_aBoneTracer[i]);
            m_aTraceEffect[i].SetScale(1.2f);
        };
    };


    CBodyElecEffectManager::~CBodyElecEffectManager(void)
    {
        ;
    };


    void CBodyElecEffectManager::Update(void)
    {
        if (m_bFootToHead)
            UpdateFootToHead();
        else
            UpdateHeadToFoot();

        for (int32 i = 0; i < COUNT_OF(m_aTraceEffect); ++i)
            m_aTraceEffect[i].Update();
    };


    void CBodyElecEffectManager::UpdateHeadToFoot(void)
    {
        float fTotalElapsedTime = CGameProperty::GetTotalElapsedTime();

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 1.0f)))
        {
            SetEffect(9);
            SetEffect(8);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 2.0f)))
        {
            SetEffect(7);
            SetEffect(6);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 3.0f)))
        {
            SetEffect(5);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 4.0f)))
        {
            SetEffect(4);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 5.0f)))
        {
            SetEffect(3);
            SetEffect(2);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 6.0f)))
        {
            SetEffect(1);
            SetEffect(0);
        };
    };


    void CBodyElecEffectManager::UpdateFootToHead(void)
    {
        float fTotalElapsedTime = CGameProperty::GetTotalElapsedTime();

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 1.0f)))
        {
            SetEffect(0);
            SetEffect(1);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 2.0f)))
        {
            SetEffect(2);
            SetEffect(3);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 3.0f)))
        {
            SetEffect(4);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 4.0f)))
        {
            SetEffect(5);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 5.0f)))
        {
            SetEffect(6);
            SetEffect(7);
        };

        if (fTotalElapsedTime >= (m_fStartTime + (m_fStepTime * 6.0f)))
        {
            SetEffect(8);
            SetEffect(9);
        };
    };


    void CBodyElecEffectManager::StartUpdate(void)
    {
        m_fStartTime = CGameProperty::GetTotalElapsedTime();

        for (int32 i = 0; i < COUNT_OF(m_abSetFlag); ++i)
            m_abSetFlag[i] = false;

		SwitchAllEffectOn(m_bToOn == false);
    };


    void CBodyElecEffectManager::SwitchAllEffectOn(bool bOn)
    {
        for (int32 i = 0; i < COUNT_OF(m_aTraceEffect); ++i)
        {
            if (bOn)
                m_aTraceEffect[i].Start();
            else
                m_aTraceEffect[i].Stop();
        };
    };


    void CBodyElecEffectManager::SetUpdateStepTime(float fStepTime)
    {
        m_fStepTime = fStepTime;
    };


    void CBodyElecEffectManager::SetDispType(bool bToOn, bool bFootToHead)
    {
        m_bToOn = bToOn;
        m_bFootToHead = bFootToHead;
    };


    void CBodyElecEffectManager::SetCharacter(const CCharacterCompositor* pChrCompositor)
    {
        m_aBoneTracer[0].Initialize(pChrCompositor, 6);
        m_aBoneTracer[1].Initialize(pChrCompositor, 5);
        m_aBoneTracer[2].Initialize(pChrCompositor, 13);
        m_aBoneTracer[3].Initialize(pChrCompositor, 14);
        m_aBoneTracer[4].Initialize(pChrCompositor, 2);
        m_aBoneTracer[5].Initialize(pChrCompositor, 9);
        m_aBoneTracer[6].Initialize(pChrCompositor, 15);
        m_aBoneTracer[7].Initialize(pChrCompositor, 18);
        m_aBoneTracer[8].Initialize(pChrCompositor, 4);
        m_aBoneTracer[9].Initialize(pChrCompositor, 3);
    };


    void CBodyElecEffectManager::SetEffect(int32 iEffectIndex)
    {
        ASSERT(iEffectIndex >= 0);
        ASSERT(iEffectIndex < COUNT_OF(m_abSetFlag));

        if (!m_abSetFlag[iEffectIndex])
        {
            m_abSetFlag[iEffectIndex] = true;

            if (m_bToOn)
                m_aTraceEffect[iEffectIndex].Start();
            else
                m_aTraceEffect[iEffectIndex].Stop();
        };
    };


    //
    // *********************************************************************************
    //


    CElecBallManager::CElecBallManager(void)
    : m_apPlrChr()
    , m_aEffectCheckPos()
    {
        for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
        {
            m_apPlrChr[i] = nullptr;
            m_aEffectCheckPos[i] = Math::VECTOR3_ZERO;
        };
    };


    CElecBallManager::~CElecBallManager(void)
    {
        ;
    };


    void CElecBallManager::InitEffectCheckPos(const CCharacterCompositor* pChrCompositor)
    {
        RwV3d vecFront = Math::VECTOR3_ZERO;
        pChrCompositor->RotateVectorByDirection(&vecFront, &Math::VECTOR3_AXIS_Z);

        RwV3d vecLeft = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vecLeft, &Math::VECTOR3_AXIS_Y, &vecFront);

        RwV3d vecRight = Math::VECTOR3_ZERO;
        Math::Vec3_Negate(&vecRight, &vecLeft);

        Math::Vec3_Add(&m_aEffectCheckPos[0], &vecFront, &vecLeft); // left front
        Math::Vec3_Sub(&m_aEffectCheckPos[1], &vecLeft, &vecFront); // left back
        Math::Vec3_Add(&m_aEffectCheckPos[2], &vecFront, &vecRight); // right front
        Math::Vec3_Sub(&m_aEffectCheckPos[3], &vecRight, &vecFront); // right back

        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        pChrCompositor->GetPosition(&vecMyPos);

        for (int32 i = 0; i < COUNT_OF(m_aEffectCheckPos); ++i)
            Math::Vec3_Add(&m_aEffectCheckPos[i], &m_aEffectCheckPos[i], &vecMyPos);
    };


    void CElecBallManager::ClearTargetPlayerList(void)
    {
        for (int32 i = 0; i < COUNT_OF(m_apPlrChr); ++i)
            m_apPlrChr[i] = nullptr;
    };


    void CElecBallManager::SetTargetPlayerList(const RwV3d* pvecCurrentPos)
    {
        CAIUtils::MakeNearerPlayerData(pvecCurrentPos);

        int32 numNearerPlayers = CAIUtils::GetNearerPlayerDataNum();
        for (int32 i = 0; i < numNearerPlayers; ++i)
        {
            const CAIUtils::NEARER_PLAYERDATA* pNearerPlrData = CAIUtils::GetNearerPlayerData(i);
            ASSERT(pNearerPlrData != nullptr);

            CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(pNearerPlrData->no);
            if (pPlrChr)
                SetTargetPlayer(pPlrChr);
        };

        if ((numNearerPlayers == 1) ||
            (numNearerPlayers == 3))
        {
            const CAIUtils::NEARER_PLAYERDATA* pNearerPlrData = CAIUtils::GetNearerPlayerData(numNearerPlayers - 1);
            ASSERT(pNearerPlrData != nullptr);

            CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(pNearerPlrData->no);
            if (pPlrChr)
                SetTargetPlayer(pPlrChr);
        };
    };


    void CElecBallManager::SetTargetPlayer(const CPlayerCharacter* pTarget)
    {
        float fNearestDist = 10000.0f;
        int32 iNearestEffectIndex = -1;

        for (int32 i = 0; i < COUNT_OF(m_aEffectCheckPos); ++i)
        {
            if (m_apPlrChr[i])
                continue;

            RwV3d vecTargetPos = Math::VECTOR3_ZERO;
            pTarget->GetPosition(&vecTargetPos);

            RwV3d vecDist = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecDist, &vecTargetPos, &m_aEffectCheckPos[i]);

            float fDist = Math::Vec3_Length(&vecDist);
            if (fDist < fNearestDist)
            {
                fNearestDist = fDist;
                iNearestEffectIndex = i;
            };
        };

        ASSERT(iNearestEffectIndex != -1);
        m_apPlrChr[iNearestEffectIndex] = pTarget;
    };


    void CElecBallManager::ShotMagic(const CCharacterCompositor* pChrCompositor,
                                     int32 iBoneHandL,
                                     int32 iBoneHandR)
    {
        RwV3d vecCompositorPos = Math::VECTOR3_ZERO;
        pChrCompositor->GetPosition(&vecCompositorPos);

        /*
         *  Left shots
         */
        RwV3d vecHandDir = Math::VECTOR3_ZERO;
        pChrCompositor->GetBonePosition(&vecHandDir, iBoneHandL, &Math::VECTOR3_ZERO);

        RwV3d vecHandPos = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecHandPos, &vecCompositorPos, &vecHandDir);
        vecHandDir.y = 0.0f;

        RwV3d vecMagicDir = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vecMagicDir, &vecHandDir, &Math::VECTOR3_AXIS_Y);
        Math::Vec3_Add(&vecMagicDir, &vecMagicDir, &vecHandDir);
        
        PlayMagic(&vecHandPos, &vecMagicDir, m_apPlrChr[0]);

        Math::Vec3_Cross(&vecMagicDir, &Math::VECTOR3_AXIS_Y, &vecHandDir);
        Math::Vec3_Add(&vecMagicDir, &vecMagicDir, &vecHandDir);

        PlayMagic(&vecHandPos, &vecMagicDir, m_apPlrChr[1]);

        /*
         *  Right shots
         */
        vecHandDir = Math::VECTOR3_ZERO;
        pChrCompositor->GetBonePosition(&vecHandDir, iBoneHandR, &Math::VECTOR3_ZERO);

        vecHandPos = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecHandPos, &vecCompositorPos, &vecHandDir);
        vecHandDir.y = 0.0f;

        vecMagicDir = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vecMagicDir, &Math::VECTOR3_AXIS_Y, &vecHandDir);
        Math::Vec3_Add(&vecMagicDir, &vecMagicDir, &vecHandDir);

        PlayMagic(&vecHandPos, &vecMagicDir, m_apPlrChr[2]);

        Math::Vec3_Cross(&vecMagicDir, &vecHandDir, &Math::VECTOR3_AXIS_Y);
        Math::Vec3_Add(&vecMagicDir, &vecMagicDir, &vecHandDir);

        PlayMagic(&vecHandPos, &vecMagicDir, m_apPlrChr[3]);
    };


    void CElecBallManager::PlayMagic(const RwV3d* pvecPosition,
                                     const RwV3d* pvecDirection,
                                     const CPlayerCharacter* pTarget)
    {
        CPlayerTargetTracer* pTargetTracer = nullptr;
        if (pTarget)
            pTargetTracer = new CPlayerTargetTracer(pTarget);

        uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_SHRD_ELBALL),
                                            pvecPosition,
                                            pvecDirection,
                                            nullptr,
                                            pTargetTracer,
                                            true);

        CMagicManager::SetSpeed(hMagic, 4.0f);
        CMagicManager::SetLive(hMagic, 3.5f);
        CMagicManager::SetDamage(hMagic, 5);
        CMagicManager::SetChangeStatus(hMagic, CHitAttackData::STATUS_KNOCK);
    };


    //
    // *********************************************************************************
    //

    
    CProbabilityTable::CProbabilityList::CProbabilityList(void)
    : m_iRecordNum(0)
    , m_paRecordArray(nullptr)
    {
        ;
    };


    CProbabilityTable::CProbabilityList::~CProbabilityList(void)
    {
        Cleanup();
    };


    int8 CProbabilityTable::CProbabilityList::CalcProbability(int32 iRecordIndex)
    {            
        ASSERT((iRecordIndex >= 0) &&
               (iRecordIndex < GAMETYPES::DIFFICULTY_NUM));

        float fRand = static_cast<float>(Math::Rand());
        fRand += 0.5f;
        fRand *= (1.0f / static_cast<float>(TYPEDEF::SINT32_MAX));
        fRand *= 100.0f;

        int8 iProbability = static_cast<uint8>(fRand);
        for (int32 i = 0; i < m_iRecordNum; ++i)
        {
            CProbabilityRecord* pRecord = &m_paRecordArray[i];
            
            int8 iRecordProbability = pRecord->aProbability[iRecordIndex];
            if (iRecordProbability > iProbability)
                return pRecord->iActionType;

			iProbability -= iRecordProbability;
        };

        return m_paRecordArray[m_iRecordNum - 1].iActionType;
    };


    void CProbabilityTable::CProbabilityList::CopyListDataArray(CProbabilityRecord* aRecord, int32 iRecordNum)
    {
        Cleanup();

        m_iRecordNum = iRecordNum;
        m_paRecordArray = new CProbabilityRecord[iRecordNum];

        for (int32 i = 0; i < m_iRecordNum; ++i)
            m_paRecordArray[i] = aRecord[i];
    };


    void CProbabilityTable::CProbabilityList::Cleanup(void)
    {
        if (m_paRecordArray)
        {
            delete[] m_paRecordArray;
            m_paRecordArray = nullptr;

            m_iRecordNum = 0;
        };
    };

    
    //
    // *********************************************************************************
    //


    CProbabilityTable::CProbabilityTable(int32 iListNum)
    : m_iListNum(iListNum)
    , m_paListArray(new CProbabilityList[iListNum])
    {
        ;
    };


    CProbabilityTable::~CProbabilityTable(void)
    {
        if (m_paListArray)
        {
            delete[] m_paListArray;
            m_paListArray = nullptr;

            m_iListNum = 0;
        };
    };


    int8 CProbabilityTable::CalcProbability(int32 iListIndex, int32 iRecordIndex)
    {
        CProbabilityList* pList = FindList(iListIndex);
        if (pList)
            return pList->CalcProbability(iRecordIndex);

        return 0;
    };


    void CProbabilityTable::CopyListDataArray(CListData** apListData, int32 iListNum)
    {
        for (int32 i = 0; i < iListNum; ++i)
        {
            CProbabilityList* pList = FindList(apListData[i]->iListIndex);
            if (pList)
                pList->CopyListDataArray(apListData[i]->aRecords, apListData[i]->iRecordsNum);
        };
    };


    CProbabilityTable::CProbabilityList*
    CProbabilityTable::FindList(int32 iListIndex) const
    {
        if (!m_paListArray)
            return nullptr;

        if ((iListIndex < 0) || (iListIndex >= m_iListNum))
            return nullptr;

        return &m_paListArray[iListIndex];
    };


    //
    // *********************************************************************************
    //


    ICharacterController::ICharacterController(void)
    : m_pEnemyChr(nullptr)
    , m_pBaseAIModerator(nullptr)
    , m_orderController()
    , m_damageCounter()
    , m_flagInvincibility()
    , m_flagNoReaction()
    , m_bFlagStateInvinsibility(false)
    , m_bFlagStateNoReaction(false)
    {
        ;
    };


    /*virtual*/ ICharacterController::~ICharacterController(void)
    {
        ;
    };


    /*virtual*/ bool ICharacterController::IsEnableChangeAction(void)
    {
        return m_orderController.IsAcceptOrder();
    };


    /*virtual*/ void ICharacterController::DispatchAttack(int32 iAttackType, int32 iTargetNo)
    {
        SetAttackOrder(iAttackType, iTargetNo);
    };


    /*virtual*/ void ICharacterController::DispatchAttack(int32 iAttackType, const RwV3d* pvecTargetPos)
    {
        SetAttackOrder(iAttackType, pvecTargetPos);
    };


    /*virtual*/ void ICharacterController::DispatchMove(int32 iMoveType, int32 iTargetNo)
    {
        SetMoveOrder(iMoveType, iTargetNo);
    };

    
    /*virtual*/ void ICharacterController::DispatchMove(int32 iMoveType, const RwV3d* pvecTargetPos)
    {
        SetMoveOrder(iMoveType, pvecTargetPos);
    };


    /*virtual*/ void ICharacterController::DispatchTurnPos(const RwV3d* pvecTargetPos)
    {
        SetMoveOrderTurn(MOVETYPE_POSITION, 0, pvecTargetPos);
    };


    /*virtual*/ void ICharacterController::DispatchTurnDirection(const RwV3d* pvecTargetDirection)
    {
        SetMoveOrderTurn(MOVETYPE_DIRECTION, 0, pvecTargetDirection);
    };


    /*virtual*/ void ICharacterController::DispatchTurnTarget(int32 iTargetNo)
    {
        SetMoveOrderTurn(MOVETYPE_TARGET, iTargetNo, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchWalk(void)
    {
        SetMoveOrderWalk(MOVETYPE_NORMAL, 0, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchWalkPos(const RwV3d* pvecTargetPos)
    {
        SetMoveOrderWalk(MOVETYPE_POSITION, 0, pvecTargetPos);
    };


    /*virtual*/ void ICharacterController::DispatchWalkDirection(const RwV3d* pvecTargetDirection)
    {
        SetMoveOrderWalk(MOVETYPE_DIRECTION, 0, pvecTargetDirection);
    };


    /*virtual*/ void ICharacterController::DispatchWalkTarget(int32 iTargetNo)
    {
        SetMoveOrderWalk(MOVETYPE_TARGET, iTargetNo, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchRun(void)
    {
        SetMoveOrderRun(MOVETYPE_NORMAL, 0, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchRunPos(const RwV3d* pvecTargetPos)
    {
        SetMoveOrderRun(MOVETYPE_POSITION, 0, pvecTargetPos);
    };


    /*virtual*/ void ICharacterController::DispatchRunDirection(const RwV3d* pvecTargetDirection)
    {
        SetMoveOrderRun(MOVETYPE_DIRECTION, 0, pvecTargetDirection);
    };


    /*virtual*/ void ICharacterController::DispatchRunTarget(int32 iTargetNo)
    {
        SetMoveOrderRun(MOVETYPE_TARGET, iTargetNo, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchJump(void)
    {
        SetMoveOrderJump(MOVETYPE_NORMAL, 0, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchJumpPos(const RwV3d* pvecTargetPos)
    {
        SetMoveOrderJump(MOVETYPE_POSITION, 0, pvecTargetPos);
    };


    /*virtual*/ void ICharacterController::DispatchJumpDirection(const RwV3d* pvecTargetDirection)
    {
        SetMoveOrderJump(MOVETYPE_DIRECTION, 0, pvecTargetDirection);
    };


    /*virtual*/ void ICharacterController::DispatchJumpTarget(int32 iTargetNo)
    {
        SetMoveOrderJump(MOVETYPE_TARGET, iTargetNo, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchDash(void)
    {
        SetMoveOrderDash(MOVETYPE_NORMAL, 0, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchDashPos(const RwV3d* pvecTargetPos)
    {
        SetMoveOrderDash(MOVETYPE_POSITION, 0, pvecTargetPos);
    };


    /*virtual*/ void ICharacterController::DispatchDashDirection(const RwV3d* pvecTargetDirection)
    {
        SetMoveOrderDash(MOVETYPE_DIRECTION, 0, pvecTargetDirection);
    };


    /*virtual*/ void ICharacterController::DispatchDashTarget(int32 iTargetNo)
    {
        SetMoveOrderDash(MOVETYPE_TARGET, iTargetNo, &Math::VECTOR3_ZERO);
    };


    /*virtual*/ void ICharacterController::DispatchGuard(void)
    {
        SetGuardOrder();
    };


    /*virtual*/ void ICharacterController::OnUpdate(void)
    {
        ;
    };


    void ICharacterController::Update(void)
    {
        m_orderController.Update();
        m_damageCounter.Update();

        OnUpdate();
    };


    void ICharacterController::Initialize(CEnemyCharacter* pEnemyChr,
                                          CAIThinkOrder* pAIThinkOrder,
                                          IBaseAIModerator* pBaseAIModerator)
    {
        ASSERT(pEnemyChr != nullptr);
        ASSERT(pAIThinkOrder != nullptr);
        ASSERT(pBaseAIModerator != nullptr);
        ASSERT(m_pEnemyChr == nullptr);
        ASSERT(m_pBaseAIModerator == nullptr);

        m_pEnemyChr = pEnemyChr;
        m_pBaseAIModerator = pBaseAIModerator;

        m_orderController.SetTargetOrder(pAIThinkOrder);
        m_damageCounter.Start();
        m_flagInvincibility.Start();
        m_flagNoReaction.Start();
    };


    void ICharacterController::StoreEnemyFlag(void)
    {
        m_bFlagStateInvinsibility = m_pEnemyChr->TestFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
        m_bFlagStateNoReaction = m_pEnemyChr->TestFlag(ENEMYTYPES::FLAG_NOREACTION);
    };


    void ICharacterController::SetEnemyFlag(void)
    {
        m_flagInvincibility.Update();

        if (m_flagInvincibility.IsEnableFlag())
            m_pEnemyChr->SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

        m_flagNoReaction.Update();

        if (m_flagNoReaction.IsEnableFlag())
            m_pEnemyChr->SetFlag(ENEMYTYPES::FLAG_NOREACTION);
    };


    void ICharacterController::RestoreEnemyFlag(void)
    {
        if (m_bFlagStateInvinsibility)
            m_pEnemyChr->SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
        else
            m_pEnemyChr->ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

        if (m_bFlagStateNoReaction)
            m_pEnemyChr->SetFlag(ENEMYTYPES::FLAG_NOREACTION);
        else
            m_pEnemyChr->ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    };


    void ICharacterController::SetOverflowData(int32 iOverflowCount, int32 iOverflowQuantity, float fAlarmTime)
    {
        m_damageCounter.SetOverflowData(iOverflowCount, iOverflowQuantity, fAlarmTime);
    };


    void ICharacterController::Death(void)
    {
        m_pEnemyChr->SetStatus(ENEMYTYPES::STATUS_QUIT);
    };


    void ICharacterController::SetMoveOrderTurn(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos /*= nullptr*/)
    {
        if (!IsValidMoveType(eMoveType))
            return;

        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_WAIT);
        order.OrderWait().m_iPlayerNo = -1;
        order.OrderWait().m_vAt = Math::VECTOR3_ZERO;

        switch (eMoveType)
        {
        case MOVETYPE_POSITION:
            order.OrderWait().m_iWaitType = Utils6034::ORDERTYPE_WAIT_TURN_POS;
            order.OrderWait().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_DIRECTION:
            order.OrderWait().m_iWaitType = Utils6034::ORDERTYPE_WAIT_TURN_POS;
            order.OrderWait().m_vAt = GetAtForDirection(pvecTargetPos);
            break;

        case MOVETYPE_TARGET:
            order.OrderWait().m_iWaitType = Utils6034::ORDERTYPE_WAIT_TURN_TAR;
            order.OrderWait().m_iPlayerNo = iTargetNo;
            break;

        default:
            break;
        };

        m_orderController.Unlock();
    };


    void ICharacterController::SetMoveOrderWalk(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos /*= nullptr*/)
    {
        if (!IsValidMoveType(eMoveType))
            return;

        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_MOVE);
        order.OrderMove().m_iPlayerNo = -1;
        order.OrderMove().m_vAt = Math::VECTOR3_ZERO;

        switch (eMoveType)
        {
        case MOVETYPE_NORMAL:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_WALK_POS;
            order.OrderMove().m_vAt = GetAtForNormal();
            break;

        case MOVETYPE_POSITION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_WALK_POS;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_DIRECTION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_WALK_POS;
            order.OrderMove().m_vAt = GetAtForDirection(pvecTargetPos);
            break;

        case MOVETYPE_TARGET:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_WALK_TAR;
            order.OrderMove().m_iPlayerNo = iTargetNo;
            break;

        default:
            break;
        };

        m_orderController.Unlock();
    };


    void ICharacterController::SetMoveOrderRun(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos /*= nullptr*/)
    {
        if (!IsValidMoveType(eMoveType))
            return;

        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_MOVE);
        order.OrderMove().m_iPlayerNo = -1;
        order.OrderMove().m_vAt = Math::VECTOR3_ZERO;

        switch (eMoveType)
        {
        case MOVETYPE_NORMAL:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_RUN_POS;
            order.OrderMove().m_vAt = GetAtForNormal();
            break;

        case MOVETYPE_POSITION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_RUN_POS;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_DIRECTION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_RUN_POS;
            order.OrderMove().m_vAt = GetAtForDirection(pvecTargetPos);
            break;

        case MOVETYPE_TARGET:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_RUN_TAR;
            order.OrderMove().m_iPlayerNo = iTargetNo;
            break;

        default:
            break;
        };

        m_orderController.Unlock();
    };


    void ICharacterController::SetMoveOrderJump(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos /*= nullptr*/)
    {
        if (!IsValidMoveType(eMoveType))
            return;

        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_MOVE);
        order.OrderMove().m_iPlayerNo = -1;
        order.OrderMove().m_vAt = Math::VECTOR3_ZERO;

        switch (eMoveType)
        {
        case MOVETYPE_NORMAL:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_JUMP;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_POSITION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_JUMP_POS;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_DIRECTION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_JUMP_DIR;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_TARGET:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_JUMP_TAR;
            order.OrderMove().m_iPlayerNo = iTargetNo;
            break;

        default:
            break;
        };

        m_orderController.Unlock();
    };


    void ICharacterController::SetMoveOrderDash(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos /*= nullptr*/)
    {
        if (!IsValidMoveType(eMoveType))
            return;

        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_MOVE);
        order.OrderMove().m_iPlayerNo = -1;
        order.OrderMove().m_vAt = Math::VECTOR3_ZERO;

        switch (eMoveType)
        {
        case MOVETYPE_NORMAL:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_DASH;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_POSITION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_DASH_POS;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_DIRECTION:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_DASH_DIR;
            order.OrderMove().m_vAt = *pvecTargetPos;
            break;

        case MOVETYPE_TARGET:
            order.OrderMove().m_iMoveType = Utils6034::ORDERTYPE_MOVE_DASH_TAR;
            order.OrderMove().m_iPlayerNo = iTargetNo;
            break;

        default:
            break;
        };

        m_orderController.Unlock();
    };


    void ICharacterController::SetGuardOrder(void)
    {
        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_DEFENCE);

        m_orderController.Unlock();
    };


    void ICharacterController::SetAttackOrder(int32 iAttackType, int32 iTargetNo)
    {
        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_ATTACK);
        order.OrderAttack().m_iAttackType = iAttackType;
        order.OrderAttack().m_iPlayerNo = iTargetNo;
        order.OrderAttack().m_vAt = Math::VECTOR3_ZERO;

        m_orderController.Unlock();
    };


    void ICharacterController::SetAttackOrder(int32 iAttackType, const RwV3d* pvecTargetPos)
    {
        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_ATTACK);
        order.OrderAttack().m_iAttackType = iAttackType;
        order.OrderAttack().m_iPlayerNo = -1;
        order.OrderAttack().m_vAt = *pvecTargetPos;

        m_orderController.Unlock();
    };


    void ICharacterController::SetMoveOrder(int32 iMoveType, int32 iTargetNo)
    {
        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_MOVE);
        order.OrderMove().m_iMoveType = iMoveType;
        order.OrderMove().m_iPlayerNo = iTargetNo;
        order.OrderMove().m_vAt = Math::VECTOR3_ZERO;

        m_orderController.Unlock();
    };


    void ICharacterController::SetMoveOrder(int32 iMoveType, const RwV3d* pvecTargetPos /*= nullptr*/)
    {
        CAIThinkOrder& order = m_orderController.Lock();

        order.SetOrder(CAIThinkOrder::ORDER_MOVE);
        order.OrderMove().m_iMoveType = iMoveType;
        order.OrderMove().m_iPlayerNo = -1;
        order.OrderMove().m_vAt = (pvecTargetPos ? *pvecTargetPos : Math::VECTOR3_ZERO);

        m_orderController.Unlock();
    };


    void ICharacterController::SendAIMessage(CHARACTERTYPES::ATTACKRESULTTYPE eAttackResultType, int32 iDamage /*= 0*/)
    {
        ASSERT(m_pBaseAIModerator != nullptr);

        if (eAttackResultType == CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE)
            return;

        if (eAttackResultType == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD)
        {
            m_pBaseAIModerator->NotifyGuard();            
        }
        else
        {
            if (eAttackResultType != CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_NOREACTION)
                m_pBaseAIModerator->NotifyDamage(iDamage);

            m_damageCounter.AddDamage(iDamage);
        };
    };


    bool ICharacterController::IsValidMoveType(MOVETYPE eMoveType) const
    {
        return (eMoveType > MOVETYPE_MIN)
            && (eMoveType < MOVETYPE_MAX);
    };


    bool ICharacterController::IsResultKnockDamaged(CHARACTERTYPES::ATTACKRESULTTYPE eAttackResultType) const
    {
        return (eAttackResultType != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD)
            && (eAttackResultType != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT)
            && (eAttackResultType != CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE)
            && (eAttackResultType != CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_NOREACTION);
    };


    bool ICharacterController::IsFlyawayState(void) const
    {
        return Utils6034::IsFlyawayState(m_pEnemyChr->GetStatus());
    };


    bool ICharacterController::IsDownState(void) const
    {
        return Utils6034::IsDownState(m_pEnemyChr->GetStatus());
    };


    RwV3d ICharacterController::GetAtForDirection(const RwV3d* pvecTargetPos) const
    {
        RwV3d vecPos = Math::VECTOR3_ZERO;
        m_pEnemyChr->Compositor().GetPosition(&vecPos);

        Math::Vec3_Add(&vecPos, &vecPos, pvecTargetPos);

        return vecPos;
    };


    RwV3d ICharacterController::GetAtForNormal(void) const
    {
        RwV3d vecDirection = Math::VECTOR3_AXIS_Z;
        m_pEnemyChr->Compositor().RotateVectorByDirection(&vecDirection, &vecDirection);

        return GetAtForDirection(&vecDirection);
    };


    //
    // *********************************************************************************
    //


    ITargetManager::ITargetManager(const CEnemyCharacter* pEnemyChr)
    : m_timer(0.0f)
    , m_pEnemyChr(pEnemyChr)
    , m_fMyHitRadius(0.0f)
    , m_iTargetNo(0)
    , m_fTargetDistance(0.0f)
    , m_fTargetHitRadius(1.0f)
    , m_fTargetMaxDistForAttack(4.0f)
    , m_aDistanceData()
    , m_playerStateWatcher()
    {
        for (int32 i = 0; i < COUNT_OF(m_aDistanceData); ++i)
        {
            m_aDistanceData[i].iPlayerNo = -1;
            m_aDistanceData[i].fDistance = 0.0f;
        };

        const RwSphere* pHitBodySphere = m_pEnemyChr->Compositor().GetMotionParameterController()->GetHitBodySphere(0);
        m_fMyHitRadius = pHitBodySphere->radius;
    };


    /*virtual*/ ITargetManager::~ITargetManager(void)
    {
        ;
    };


    void ITargetManager::Update(void)
    {
        m_timer.Update();
        m_playerStateWatcher.Update();

        OnUpdate();
    };


    void ITargetManager::Start(void)
    {
        m_timer.Start();

        OnStart();
    };


    void ITargetManager::SortPlayerByDistance(void)
    {
        int32 numPlayers = CAIUtils::GetPlayerNum();
        for (int32 i = 0; i < numPlayers; ++i)
        {
            CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
            if (!pPlrChr)
                continue;

            RwV3d vecPlrPos = Math::VECTOR3_ZERO;
            pPlrChr->GetPosition(&vecPlrPos);

            RwV3d vecMyPos = Math::VECTOR3_ZERO;
            m_pEnemyChr->Compositor().GetPosition(&vecMyPos);

            float fDist = CEnemyUtils::GetDistance(&vecMyPos, &vecPlrPos);

            m_aDistanceData[i].fDistance = fDist;
            m_aDistanceData[i].iPlayerNo = i;
        };

        DISTANCE_DATA* itBegin = std::begin(m_aDistanceData);
        DISTANCE_DATA* itEnd = std::begin(m_aDistanceData) + numPlayers;

        std::sort(itBegin, itEnd, [](DISTANCE_DATA& rDataA, DISTANCE_DATA& rDataB) {
            return (rDataA.fDistance < rDataB.fDistance);
        });
    };


    bool ITargetManager::IsTargetStun(void) const
    {
        return PlayerStateWatcher().IsStunState(GetTargetNo());
    };


    bool ITargetManager::IsTargetFlyaway(void) const
    {
        return PlayerStateWatcher().IsDamagedFlyawayState(GetTargetNo());
    };


    bool ITargetManager::IsTargetAttack(void) const
    {
        if (GetTargetDistance() >= m_fTargetMaxDistForAttack)
            return false;

        return PlayerStateWatcher().IsAttack(GetTargetNo());
    };


    bool ITargetManager::IsTargetAttackTrigger(void) const
    {
        if (GetTargetDistance() >= m_fTargetMaxDistForAttack)
            return false;

        return PlayerStateWatcher().IsAttackTrigger(GetTargetNo());
    };


    bool ITargetManager::IsTargetSeeMe(void) const
    {
        CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
        if (pMapCamera)
        {
            RwV3d vecMyPos = GetMyPos();
            RwV3d vecTargetPos = GetTargetPos();

            if (pMapCamera->IsPosVisible(&vecMyPos) &&
                pMapCamera->IsPosVisible(&vecTargetPos))
                return true;
        };

        return false;
    };


    bool ITargetManager::GetNearestActivePlayer(int32* piPlayerNo, float* pfDist) const
    {
        int32 numPlayers = CAIUtils::GetPlayerNum();
        for (int32 i = 0; i < numPlayers; ++i)
        {
            if (!PlayerStateWatcher().IsDownState(m_aDistanceData[i].iPlayerNo))
            {
                *piPlayerNo = m_aDistanceData[i].iPlayerNo;
                *pfDist     = (m_aDistanceData[i].fDistance - m_fTargetHitRadius - m_fMyHitRadius);

                return true;
            };
        };

        return false;
    };

    
    RwV3d ITargetManager::GetMyPos(void) const
    {
        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        m_pEnemyChr->Compositor().GetPosition(&vecMyPos);

        return vecMyPos;
    };


    CPlayerCharacter* ITargetManager::GetTarget(void) const
    {
        return CAIUtils::GetActivePlayer(GetTargetNo());
    };


    RwV3d ITargetManager::GetTargetPos(void) const
    {
        CPlayerCharacter* pPlrChr = GetTarget();
        if (!pPlrChr)
            return Math::VECTOR3_ZERO;
        
        RwV3d vecPlrPos = Math::VECTOR3_ZERO;
        pPlrChr->GetPosition(&vecPlrPos);

        return vecPlrPos;
    };


    int32 ITargetManager::GetTargetNo(void) const
    {
#ifdef _DEBUG
        int32 numPlayers = CAIUtils::GetPlayerNum();
        if ((m_iTargetNo < 0) || (m_iTargetNo > numPlayers))
            ASSERT(false);
#endif /* _DEBUG */

        return m_iTargetNo;
    };


    float ITargetManager::GetTargetDistance(void) const
    {
        if (m_fTargetDistance < 0.0f)
            return 0.0f;
        
        return m_fTargetDistance;
    };


    float ITargetManager::GetTargetDistanceSQ(void) const
    {
        float fTargetDistance = GetTargetDistance();

        return (fTargetDistance * fTargetDistance);
    };


    float ITargetManager::GetCurrentTargetDistance(void) const
    {
        RwV3d vecTargetPos = GetTargetPos();
        RwV3d vecMyPos = GetMyPos();
        float fDist = CEnemyUtils::GetDistance(&vecMyPos, &vecTargetPos);

        return (fDist - m_fTargetHitRadius - m_fMyHitRadius);
    };


    const CPlayerStateWatcher& ITargetManager::PlayerStateWatcher(void) const
    {
        return m_playerStateWatcher;
    };


    //
    // *********************************************************************************
    //


    CNearestTargetSelector::CNearestTargetSelector(const CEnemyCharacter* pEnemyChr)
    : ITargetManager(pEnemyChr)
    {
        ;
    };


    /*virtual*/ CNearestTargetSelector::~CNearestTargetSelector(void)
    {
        ;
    };


    /*virtual*/ void CNearestTargetSelector::OnStart(void) /*override*/
    {
        ;
    };


    /*virtual*/ void CNearestTargetSelector::OnUpdate(void) /*override*/
    {
        if (!m_timer.IsAlarm())
        {
            m_fTargetDistance = GetCurrentTargetDistance();
            return;
        };

        m_timer.Reset();
        SortPlayerByDistance();

        if (!GetNearestActivePlayer(&m_iTargetNo, &m_fTargetDistance))
            m_fTargetDistance = GetCurrentTargetDistance();
    };


    //
    // *********************************************************************************
    //


    IAIState::IAIState(int32 iHandle)
    : m_iHandle(iHandle)
    {
        ASSERT(m_iHandle > INVALID_STATE_HANDLE);
    };


    /*virtual*/ IAIState::~IAIState(void)
    {
        ;
    };

    int32 IAIState::GetHandle(void) const
    {
        return m_iHandle;
    };
    

    //
    // *********************************************************************************
    //


    CAIStateContext::CAIStateContext(int32 iStateMax)
    : m_iCurrentStateNum(0)
    , m_iStateMax(iStateMax)
    , m_apAIState(nullptr)
    , m_iCurrentStateIndex(0)
    , m_iDefaultStateHandle(IAIState::INVALID_STATE_HANDLE)
    , m_iForceStateHandle(-2)
    , m_iStateResult(IAIState::RUNRESULT_CONTINUE)
    {
        ASSERT(m_iStateMax >= 0);

        m_apAIState = new IAIState*[m_iStateMax];

        for (int32 i = 0; i < m_iStateMax; ++i)
            m_apAIState[i] = nullptr;
    };


    /*virtual*/ CAIStateContext::~CAIStateContext(void)
    {
        if (m_apAIState)
        {
            for (int32 i = 0; i < m_iStateMax; ++i)
            {
                if (m_apAIState[i])
                {
                    delete m_apAIState[i];
                    m_apAIState[i] = nullptr;
                };
            };

            delete[] m_apAIState;
            m_apAIState = nullptr;

            m_iStateMax = 0;
        };
    };


    /*virtual*/ int32 CAIStateContext::OnGetNextState(int32 iEndResult)
    {
		if (iEndResult == IAIState::INVALID_STATE_HANDLE)
			return GetDefaultState();

		return iEndResult;
    };


    void CAIStateContext::RegistAIState(IAIState* pAIState)
    {
        ASSERT(m_iCurrentStateNum < m_iStateMax);
        m_apAIState[m_iCurrentStateNum++] = pAIState;
    };


    void CAIStateContext::SetDefaultState(int32 iStateHandle)
    {
        m_iDefaultStateHandle = iStateHandle;
    };


    void CAIStateContext::ChangeState(int32 iNextStateHandle)
    {
        if (iNextStateHandle != GetCurrentStateHandle())
            ChangeStateForce(iNextStateHandle);
    };


    void CAIStateContext::ChangeStateForce(int32 iNextStateHandle)
    {
        if ((iNextStateHandle == IAIState::INVALID_STATE_HANDLE) ||
            IsContainState(iNextStateHandle))
        {
            m_iForceStateHandle = iNextStateHandle;
            m_iStateResult = IAIState::RUNRESULT_END;
        };
    };


    IAIState::RUNRESULT CAIStateContext::RunState(void)
    {
        if (m_iStateResult == IAIState::RUNRESULT_END)
        {
            int32 iNextStateHandle = EndState();
            StartState(iNextStateHandle);
        };

        IAIState* pAIState = GetCurrentState();
        m_iStateResult = pAIState->OnRun();

        return static_cast<IAIState::RUNRESULT>(m_iStateResult);
    };


    void CAIStateContext::StartState(int32 iStateHandle)
    {
        while (!StartState_Sub(iStateHandle))
            iStateHandle = EndState();
    };


    bool CAIStateContext::StartState_Sub(int32 iStateHandle)
    {
        int32 iStateIndex = FindStateIndex(iStateHandle);
        if (iStateIndex == -1)
            iStateIndex = 0;

        m_iCurrentStateIndex = iStateIndex;
        m_iForceStateHandle = -2;
        m_iStateResult = IAIState::RUNRESULT_CONTINUE;

        IAIState* pAIState = GetCurrentState();
        IAIState::STARTRESULT startResult = pAIState->OnStart();

        return (startResult == IAIState::STARTRESULT_END);
    };


    void CAIStateContext::StartDefaultState(void)
    {
        StartState(m_iDefaultStateHandle);
    };


    int32 CAIStateContext::EndState(void)
    {
        IAIState* pAIState = GetCurrentState();

        int32 iNextStateHandle = pAIState->OnEnd();
        if (m_iForceStateHandle != -2)
        {
            iNextStateHandle = m_iForceStateHandle;
            m_iForceStateHandle = -2;
        };

        return OnGetNextState(iNextStateHandle);
    };


    bool CAIStateContext::IsContainState(int32 iStateHandle) const
    {
        int32 iIndex = FindStateIndex(iStateHandle);
        return (iIndex != -1);
    };


    int32 CAIStateContext::GetDefaultState(void) const
    {
        return m_iDefaultStateHandle;
    };


    int32 CAIStateContext::GetCurrentStateHandle(void) const
    {
        IAIState* pAIState = GetCurrentState();
        return pAIState->GetHandle();
    };


    int32 CAIStateContext::GetForceChangeStateHandle(void) const
    {
        return m_iForceStateHandle;
    };


    IAIState* CAIStateContext::GetCurrentState(void) const
    {
        return m_apAIState[m_iCurrentStateIndex];
    };


    int32 CAIStateContext::FindStateIndex(int32 iStateHandle) const
    {
        for (int32 i = 0; i < m_iCurrentStateNum; ++i)
        {
            if (m_apAIState[i]->GetHandle() == iStateHandle)
                return i;
        };

        return -1;
    };


    //
    // *********************************************************************************
    //


    IMessageStackAIStateContext::IMessageStackAIStateContext(int32 iStateMax)
    : CAIStateContext(iStateMax)
    , m_memoryManager(sizeof(MEM_NODE), iStateMax)
    , m_messageStack()
    , m_msgSenderWait()
    , m_msgSenderNormalAttack()
    {
        ;
    };


    /*virtual*/ IMessageStackAIStateContext::~IMessageStackAIStateContext(void)
    {
        Cleanup();
    };


    /*virtual*/ int32 IMessageStackAIStateContext::OnGetNextState(int32 iEndResult) /*override*/
    {
        if (iEndResult == IAIState::INVALID_STATE_HANDLE)
        {
            if (m_messageStack.IsEmpty())
                return GetDefaultState();

            STACK_DATA data = m_messageStack.Peek();
            m_messageStack.Pop();

            iEndResult = data.iStateHandle;

            if (data.pMessage)
            {
                data.pMessage->SendMessage();

                m_memoryManager.PushGlobal();
                delete data.pMessage;
                m_memoryManager.PopGlobal();
            };
        };

        return iEndResult;
    };


    void IMessageStackAIStateContext::PushMessage(int32 iStateHandle, IPolymorphMessage* pMessage /*= nullptr*/)
    {
        m_messageStack.Push({ iStateHandle, pMessage });
    };

    
    void IMessageStackAIStateContext::PushMessage(int32 iStateHandle, const CNormalAttackData& data)
    {
        using CNormalAttackMessage = CPolymorphCapsel<CNormalAttackData>;

        m_memoryManager.PushGlobal();
        CNormalAttackMessage* pMessage = new CNormalAttackMessage(m_msgSenderNormalAttack, data);
        m_memoryManager.PopGlobal();

        PushMessage(iStateHandle, pMessage);
    };


    void IMessageStackAIStateContext::PushMessage(int32 iStateHandle, const CWaitData& data)
    {
        using CWaitMessage = CPolymorphCapsel<CWaitData>;

        m_memoryManager.PushGlobal();
        CWaitMessage* pMessage = new CWaitMessage(m_msgSenderWait, data);
        m_memoryManager.PopGlobal();

        PushMessage(iStateHandle, pMessage);
    };


    void IMessageStackAIStateContext::Cleanup(void)
    {
        while (!m_messageStack.IsEmpty())
        {
            STACK_DATA data = m_messageStack.Peek();
            m_messageStack.Pop();

            if (data.pMessage)
            {
                m_memoryManager.PushGlobal();
                delete data.pMessage;
                m_memoryManager.PopGlobal();
            };
        };
    };


    void IMessageStackAIStateContext::SetReceiver(IMessageReceiver<CWaitData>* pReceiver)
    {
        m_msgSenderWait.SetReceiver(pReceiver);
    };


    void IMessageStackAIStateContext::SetReceiver(IMessageReceiver<CNormalAttackData>* pReceiver)
    {
        m_msgSenderNormalAttack.SetReceiver(pReceiver);
    };


    //
    // *********************************************************************************
    //

    
    IActionDispatcher::IActionDispatcher(int32 iProbabilityListNum)
    : m_probabilityTable(iProbabilityListNum)
    , m_pAIStateContext(nullptr)
    , m_pCharacterController(nullptr)
    {
        ;
    };


    /*virtual*/ IActionDispatcher::~IActionDispatcher(void)
    {
        ;
    };


    /*virtual*/ bool IActionDispatcher::DispatchNormalAttack(int32 iTargetNo, float fTargetDistanceSQ)
    {
        return true;
    };


    /*virtual*/ bool IActionDispatcher::DispatchAerialCounterAttack(int32 iTargetNo)
    {
        return true;
    };


    /*virtual*/ bool IActionDispatcher::DispatchCrowdCounterAttack(int32 iTargetNo)
    {
        return true;
    };


    /*virtual*/ bool IActionDispatcher::DispatchDamagedAction(int32 iTargetNo, float fTargetDistanceSQ)
    {
        return true;
    };


    /*virtual*/ bool IActionDispatcher::DispatchDefence(void)
    {
        return true;
    };


    void IActionDispatcher::Initialize(IMessageStackAIStateContext* pAIStateContext,
                                       ICharacterController* pCharacterController)
    {
        ASSERT(m_pAIStateContext == nullptr);
        ASSERT(m_pCharacterController == nullptr);

        m_pAIStateContext = pAIStateContext;
        m_pCharacterController = pCharacterController;

        OnInitialize();
    };


    //
    // *********************************************************************************
    //

    
    IBaseAIModerator::IBaseAIModerator(CEnemyCharacter* pEnemyChr, IActionDispatcher* pActionDispatcher, int32 iAIStateMax)
    : CAIModerator(pEnemyChr)
    , m_aiStateContext(iAIStateMax)
    , m_targetManager(pEnemyChr)
    , m_pActionDispatcher(pActionDispatcher)
    , m_pCharacterController(nullptr)
    {
        ;
    };


    /*virtual*/ IBaseAIModerator::~IBaseAIModerator(void)
    {
        ;
    };


    /*virtual*/ void IBaseAIModerator::Draw(void) /*override*/
    {
        CAIModerator::Draw();
#ifdef _DEBUG
        if (!CEnemyDebug::SHOW_AI_INFO)
            return;

        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        CharacterController().EnemyChr().Compositor().GetFootPosition(&vecMyPos);

        float fHeight = CharacterController().EnemyChr().Compositor().GetCollisionParameter().m_fHeight;
        vecMyPos.y += (fHeight * 1.5f);

        enum
        {
            INFOFLAG_ORDER              = (1 << 0),
            INFOFLAG_MOTION             = (1 << 1),
            INFOFLAG_STATE_NOW          = (1 << 2),
            INFOFLAG_STATE_FORCE        = (1 << 3),
            INFOFLAG_TARGET             = (1 << 4),
            INFOFLAG_FLAG_NOREACTION    = (1 << 5),
            INFOFLAG_FLAG_INVINCIBILITY = (1 << 6),
        };

        uint32 infoFlag = 0;

        char szInfoOrder[128];
        szInfoOrder[0] = '\0';

        char szMotion[128];
        szMotion[0] = '\0';

        char szStateNow[64];
        szStateNow[0] = '\0';

        char szStateForce[64];
        szStateForce[0] = '\0';

        char szTargetInfo[128];
        szTargetInfo[0] = '\0';

        char szFlagInfoNoReaction[128];
        szFlagInfoNoReaction[0] = '\0';

        char szFlagInfoInvincibility[128];
        szFlagInfoInvincibility[0] = '\0';

        /* get order info */
        static const char* s_apszOrderName[] =
        {
            "NOTHING",
            "WAIT",
            "MOVE",
            "RUN",
            "ATTACK",
            "DEFENCE",
        };

        static_assert(COUNT_OF(s_apszOrderName) == CAIThinkOrder::ORDER_MAX, "update table");

        std::sprintf(szInfoOrder, "ORDER: %s", s_apszOrderName[CharacterController().EnemyChr().AIThinkOrder().GetOrder()]);
        infoFlag |= INFOFLAG_ORDER;

        /* get motion info */
        const char* pszMotion = CharacterController().EnemyChr().Compositor().GetMotionName();
        if (pszMotion)
        {
            std::sprintf(szMotion, "MOTION: %s", pszMotion);
            infoFlag |= INFOFLAG_MOTION;
        }
        else
        {
            std::sprintf(szMotion, "NO MOTION");
            infoFlag |= INFOFLAG_MOTION;
        };

        /* get now state */
        std::sprintf(szStateNow, "STATE: %" PRId32 " (force: %" PRId32 ")",
                     MessageStackAIStateContext().GetCurrentStateHandle(),
                     MessageStackAIStateContext().GetForceChangeStateHandle());

        infoFlag |= INFOFLAG_STATE_NOW;

        /* get target */
        std::sprintf(szTargetInfo, "TARGET: %" PRId32 " (%.02f -- %.02f)",
                     TargetManager().GetTargetNo(),
                     TargetManager().GetTargetDistance(),
                     TargetManager().GetTargetDistanceSQ());

        infoFlag |= INFOFLAG_TARGET;

        /* get no reaction flag */
        std::sprintf(szFlagInfoNoReaction, "NO REACTION: %s (%" PRId32 " -- %.02f)",
                     CharacterController().FlagNoReaction().IsEnableFlag() ? "true" : "false",
                     CharacterController().FlagNoReaction().GetCounter(),
                     CharacterController().FlagNoReaction().GetTimer().GetRemainCounterTime());

        infoFlag |= INFOFLAG_FLAG_NOREACTION;

        /* get invincibility flag */
        std::sprintf(szFlagInfoInvincibility, "INVINCIBILITY: %s (%" PRId32 " -- %.02f)",
                     CharacterController().FlagInvincibility().IsEnableFlag() ? "true" : "false",
                     CharacterController().FlagInvincibility().GetCounter(),
                     CharacterController().FlagInvincibility().GetTimer().GetRemainCounterTime());

        infoFlag |= INFOFLAG_FLAG_INVINCIBILITY;

        /* draw by info flag */
        RwRGBA color = { 0xFF, 0xFF, 0x7F, 0xFF }; // yellow

        CDebugShape::m_fLabelHeight = 14.0f;
        CDebugShape::m_fLabelOffsetY = 0.0f;

        if (infoFlag & INFOFLAG_ORDER)
        {
            CDebugShape::ShowLabel(&vecMyPos, szInfoOrder, color);
            CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
        };

        if (infoFlag & INFOFLAG_MOTION)
        {
            CDebugShape::ShowLabel(&vecMyPos, szMotion, color);
            CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
        };

        if (infoFlag & INFOFLAG_TARGET)
        {
            CDebugShape::ShowLabel(&vecMyPos, szTargetInfo, color);
            CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
        };

        if (infoFlag & INFOFLAG_STATE_NOW)
        {
            CDebugShape::ShowLabel(&vecMyPos, szStateNow, color);
            CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
        };

        if (infoFlag & INFOFLAG_FLAG_NOREACTION)
        {
            CDebugShape::ShowLabel(&vecMyPos, szFlagInfoNoReaction, color);
            CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
        };

        if (infoFlag & INFOFLAG_FLAG_INVINCIBILITY)
        {
            CDebugShape::ShowLabel(&vecMyPos, szFlagInfoInvincibility, color);
            CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
        };

        CDebugShape::m_fLabelOffsetY = 0.0f;
#endif /* _DEBUG */        
    };


    /*virtual*/ void IBaseAIModerator::NotifyDamage(int32 iAmount)
    {
        ;
    };


    /*virtual*/ void IBaseAIModerator::NotifyGuard(void)
    {
        ;
    };


    void IBaseAIModerator::Initialize(ICharacterController* pCharacterController)
    {
        ASSERT(m_pActionDispatcher != nullptr);
        m_pActionDispatcher->Initialize(&m_aiStateContext, pCharacterController);

        m_pCharacterController = pCharacterController;
        
        IBaseAIState::SetInitParam(this);

        OnInitialize();
    };

    
    //
    // *********************************************************************************
    //


    /*static*/ IBaseAIModerator* IBaseAIState::m_pInitBaseAIModerator = nullptr;


    /*static*/ void IBaseAIState::SetInitParam(IBaseAIModerator* pBaseAIModerator)
    {
        m_pInitBaseAIModerator = pBaseAIModerator;
    };

    
    //
    // *********************************************************************************
    //

    
    CWaitState::CWaitState(int32 iHandle)
    : IBaseAIState(iHandle)
    , m_waitTimer(0.0f)
    {
        ;
    };


    /*virtual*/ CWaitState::STARTRESULT
    CWaitState::OnStart(void) /*override*/
    {
        m_waitTimer.Reset();
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CWaitState::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CWaitState::RUNRESULT
    CWaitState::OnRun(void) /*override*/
    {
        m_waitTimer.Update();
        
        if (m_waitTimer.IsAlarm())
            return RUNRESULT_END;

        TargetManager().Update();

        int32 iTargetNo = TargetManager().GetTargetNo();
        CharacterController().DispatchTurnTarget(iTargetNo);

        return RUNRESULT_CONTINUE;
    };


    /*virtual*/ void CWaitState::ReceiveMessage(const CWaitData& data) /*override*/
    {
        m_waitTimer.SetAlarmTime(data.fWaitTime);
    };


    //
    // *********************************************************************************
    //


    CNormalAttackState::CNormalAttackState(int32 iHandle)
    : IBaseAIState(iHandle)
    , m_iAttackType(-1)
    , m_iTargetNo(-1)
    {
        ;
    };


    /*virtual*/ CNormalAttackState::STARTRESULT
    CNormalAttackState::OnStart(void) /*override*/
    {
        CharacterController().DispatchAttack(m_iAttackType, m_iTargetNo);
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CNormalAttackState::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CNormalAttackState::RUNRESULT
    CNormalAttackState::OnRun(void) /*override*/
    {
        return RUNRESULT_END;
    };


    /*virtual*/ void CNormalAttackState::ReceiveMessage(const CNormalAttackData& data) /*override*/
    {
        m_iAttackType = data.iAttackType;
        m_iTargetNo = data.iTargetNo;
    };


    //
    // *********************************************************************************
    //


    CBaseThinkingState::CBaseThinkingState(int32 iHandle, int32 iCtxStateMax)
    : IBaseAIState(iHandle)
    , m_moveStateCtx(iCtxStateMax)
    , m_bUpdateTargetMgr(true)
    {
        ;
    };


    /*virtual*/ CBaseThinkingState::STARTRESULT
    CBaseThinkingState::OnStart(void) /*override*/
    {
        int32 iHandle = ActionDispatcher().CalcMoveProbability();

        ASSERT(m_moveStateCtx.IsContainState(iHandle));
        m_moveStateCtx.ChangeStateForce(iHandle);

        TargetManager().Start();

        return STARTRESULT_END;
    };


    /*virtual*/ int32 CBaseThinkingState::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CBaseThinkingState::RUNRESULT
    CBaseThinkingState::OnRun(void) /*override*/
    {
        if (m_bUpdateTargetMgr)
            TargetManager().Update();

        if (TargetManager().PlayerStateWatcher().IsAllPlayerDownState() && !OnAllTargetDown())
            return RUNRESULT_END;

        int32 iTargetNo = TargetManager().GetTargetNo();
        if (TargetManager().IsTargetAttackTrigger() && !OnTargetAttack(iTargetNo))
            return RUNRESULT_END;
        
        RUNRESULT result = m_moveStateCtx.RunState();        
        if ((result == RUNRESULT_END) && !OnEndMove())
            return RUNRESULT_END;
            
        return RUNRESULT_CONTINUE;
    };


    /*virtual*/ bool CBaseThinkingState::OnAllTargetDown(void)
    {
        return true;
    };


    /*virtual*/ bool CBaseThinkingState::OnTargetAttack(int32 iTargetNo)
    {
        return true;
    };


    /*virtual*/ bool CBaseThinkingState::OnEndMove(void)
    {
        return true;
    };

    
    //
    // *********************************************************************************
    //
    

    CBaseGuardState::CBaseGuardState(int32 iHandle)
    : CBaseGuardState(iHandle, 0.1f, 5.0f)
    {
        ;
    };

    
    CBaseGuardState::CBaseGuardState(int32 iHandle, float fGuardTime, float fGuardTimeMax)
    : IBaseAIState(iHandle)
    , m_step(-1)
    , m_timerGuard(0.0f)
    , m_timerGuardTotal(0.0f)
    {
        m_timerGuard.SetAlarmTime(fGuardTime);
        m_timerGuardTotal.SetAlarmTime(fGuardTimeMax);
    };


    /*virtual*/ CBaseGuardState::STARTRESULT
    CBaseGuardState::OnStart(void) /*override*/
    {
        m_step = 0;
        m_timerGuard.Start();
        m_timerGuardTotal.Start();

        CharacterController().DispatchGuard();

        return STARTRESULT_END;
    };


    /*virtual*/ int32 CBaseGuardState::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CBaseGuardState::RUNRESULT
    CBaseGuardState::OnRun(void) /*override*/
    {
        CharacterController().DispatchGuard();
        TargetManager().Update();

        switch (m_step)
        {
        case 0:
            {
                if (!TargetManager().IsTargetAttack())
                {
                    m_timerGuard.Start();
                    ++m_step;
                };
            }
            break;

        case 1:
            {
                if (TargetManager().IsTargetAttack())
                {
                    m_step = 0;
                    break;
                };

                m_timerGuard.Update();

                if (m_timerGuard.IsAlarm())
                    return RUNRESULT_END;
            }
            break;

        default:
            break;
        };

        m_timerGuardTotal.Update();

        if (m_timerGuardTotal.IsAlarm())
            return RUNRESULT_END;

        return RUNRESULT_CONTINUE;
    };


    //
    // *********************************************************************************
    //


    CBaseDownState::CBaseDownState(int32 iHandle, float fNoReactionAddTime)
    : IBaseAIState(iHandle)
    , m_fNoReactionAddTime(fNoReactionAddTime)
    {
        ;
    };


    /*virtual*/ CBaseDownState::STARTRESULT
    CBaseDownState::OnStart(void) /*override*/
    {
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CBaseDownState::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CBaseDownState::RUNRESULT
    CBaseDownState::OnRun(void) /*override*/
    {
        ENEMYTYPES::STATUS statusNow = AIModerator().EnemyCharacter().GetStatus();

        if (Utils6034::IsDownState(statusNow))
            return RUNRESULT_CONTINUE;

        CharacterController().FlagNoReaction().AddTime(m_fNoReactionAddTime);
        return RUNRESULT_END;
    };


    //
    // *********************************************************************************
    //

    
    CBaseDamagedCounterState::CBaseDamagedCounterState(int32 iHandle)
    : IBaseAIState(iHandle)
    {
        ;
    };


    /*virtual*/ CBaseDamagedCounterState::STARTRESULT
    CBaseDamagedCounterState::OnStart(void) /*override*/
    {
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CBaseDamagedCounterState::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CBaseDamagedCounterState::RUNRESULT
    CBaseDamagedCounterState::OnRun(void) /*override*/
    {
        MessageStackAIStateContext().Cleanup();
        TargetManager().Update();

        float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
        int32 iTargetNo = TargetManager().GetTargetNo();
        ActionDispatcher().DispatchDamagedAction(iTargetNo, fTargetDistSQ);

        return RUNRESULT_END;
    };


    //
    // *********************************************************************************
    //


    CBaseCounterDownState::CBaseCounterDownState(int32 iHandle)
    : CBaseCounterDownState(iHandle, 0.7f, 4.0f)
    {
        ;
    };


    CBaseCounterDownState::CBaseCounterDownState(int32 iHandle, float fCounterMaxTargetDistSQ)
    : CBaseCounterDownState(iHandle, 0.7f, fCounterMaxTargetDistSQ)
    {
        ;
    };


    CBaseCounterDownState::CBaseCounterDownState(int32 iHandle, float fNoReactionAddTime, float fCounterMaxTargetDistSQ)
    : CBaseDownState(iHandle, fNoReactionAddTime)
    , m_fCounterMaxTargetDistSQ(fCounterMaxTargetDistSQ)
    {
        ;
    };


    /*virtual*/ CBaseCounterDownState::RUNRESULT
    CBaseCounterDownState::OnRun(void) /*override*/
    {
        if (Utils6034::IsDownState(AIModerator().EnemyCharacter().GetStatus()))
            return RUNRESULT_CONTINUE;

        TargetManager().Update();

        bool bIsTargetAttack = TargetManager().IsTargetAttack();
        bool bIsTargetInRange = (TargetManager().GetTargetDistanceSQ() < m_fCounterMaxTargetDistSQ);

        if (bIsTargetAttack || bIsTargetInRange)
        {
            float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
            int32 iTargetNo = TargetManager().GetTargetNo();
            ActionDispatcher().DispatchDamagedAction(iTargetNo, fTargetDistSQ);
        };

        return RUNRESULT_END;
    };

    
    //
    // *********************************************************************************
    //


    IBaseMoveState::IBaseMoveState(int32 iHandle)
    : IBaseAIState(iHandle)
    , m_moveTimer(2.0f)
    , m_fMinTargetDistSQ(0.64f)
    , m_bPauseTimer(false)
    {
        ;
    };


    //
    // *********************************************************************************
    //


    CWalkDispatcher::CWalkDispatcher(int32 iHandle)
    : IBaseMoveState(iHandle)
    {
        ;
    };

    
    /*virtual*/ CWalkDispatcher::STARTRESULT
    CWalkDispatcher::OnStart(void) /*override*/
    {
        m_moveTimer.Start();
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CWalkDispatcher::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CWalkDispatcher::RUNRESULT
    CWalkDispatcher::OnRun(void) /*override*/
    {
        TargetManager().Update();

        if (!m_bPauseTimer)
            m_moveTimer.Update();

        m_bPauseTimer = false;
		
		// TODO it will always order to walk even if we close enough to target
        int32 iTargetNo = TargetManager().GetTargetNo();
        CharacterController().DispatchWalkTarget(iTargetNo);

        if (m_moveTimer.IsAlarm())
            return RUNRESULT_END;

        float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
        if (fTargetDistSQ <= m_fMinTargetDistSQ)
            return RUNRESULT_END;

        return RUNRESULT_CONTINUE;
    };

    
    //
    // *********************************************************************************
    //

    
    CRunDispatcher::CRunDispatcher(int32 iHandle)
    : IBaseMoveState(iHandle)
    {
        ;
    };


    /*virtual*/ CRunDispatcher::STARTRESULT
    CRunDispatcher::OnStart(void) /*override*/
    {
        m_moveTimer.Start();
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CRunDispatcher::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CRunDispatcher::RUNRESULT 
    CRunDispatcher::OnRun(void) /*override*/
    {
        TargetManager().Update();

        if (!m_bPauseTimer)
            m_moveTimer.Update();

        m_bPauseTimer = false;

        int32 iTargetNo = TargetManager().GetTargetNo();
        CharacterController().DispatchRunTarget(iTargetNo);

        if (m_moveTimer.IsAlarm())
            return RUNRESULT_END;

        float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
        if (fTargetDistSQ <= m_fMinTargetDistSQ)
            return RUNRESULT_END;

        return RUNRESULT_CONTINUE;
    };


    //
    // *********************************************************************************
    //


    CCircleRunDispatcher::CCircleRunDispatcher(int32 iHandle)
    : CCircleRunDispatcher(iHandle, 2.5f, 1.0f, 0.8f)
    {
        ;
    };


    CCircleRunDispatcher::CCircleRunDispatcher(int32 iHandle,
                                               float fMoveTime,
                                               float fMinTargetDist,
                                               float fInRangeTime)
    : IBaseMoveState(iHandle)
    , m_bFlagDirection(false)
    , m_timerInRange(fInRangeTime)
    {
        m_moveTimer.SetAlarmTime(fMoveTime);

        m_fMinTargetDistSQ = (fMinTargetDist * fMinTargetDist);
    };


    /*virtual*/ CCircleRunDispatcher::STARTRESULT
    CCircleRunDispatcher::OnStart(void) /*override*/
    {
        m_moveTimer.Start();
        m_bFlagDirection = ((Math::Rand() & 1) ? true : false);
        m_timerInRange.Reset();
        m_timerInRange.Pause();

        return STARTRESULT_END;
    };


    /*virtual*/ int32 CCircleRunDispatcher::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CCircleRunDispatcher::RUNRESULT
    CCircleRunDispatcher::OnRun(void) /*override*/
    {
        TargetManager().Update();

        if (!m_bPauseTimer)
            m_moveTimer.Update();

        m_bPauseTimer = false;

        OrderRunPos();

        m_timerInRange.Update();

        if (m_moveTimer.IsAlarm() || m_timerInRange.IsAlarm())
            return RUNRESULT_END;

        float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
        if (fTargetDistSQ <= m_fMinTargetDistSQ)
            m_timerInRange.UnPause();

        return RUNRESULT_CONTINUE;
    };


    void CCircleRunDispatcher::OrderRunPos(void)
    {
        float fRunSpeed = AIModerator().EnemyCharacter().Feature().m_fRunMoveSpeed;

        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        AIModerator().EnemyCharacter().Compositor().GetPosition(&vecMyPos);

        RwV3d vecRunPos = Math::VECTOR3_ZERO;
        CalcRunPos(&vecRunPos, &vecMyPos, fRunSpeed);

        const float fOfsY = 0.65f;

        vecMyPos.y += fOfsY;
        vecRunPos.y += fOfsY;

        if (CWorldMap::CheckCollisionLine(&vecMyPos, &vecRunPos))
            m_bFlagDirection = !m_bFlagDirection; /* TODO: like in retail there is problem at the joint of 2 walls */

        vecRunPos.y -= fOfsY;

        CharacterController().DispatchRunPos(&vecRunPos);
    };


    void CCircleRunDispatcher::CalcRunPos(RwV3d* pvecRunPos,
                                          const RwV3d* pvecCurrentPos,
                                          float fRunMoveSpeed) const
    {
        float fRunDist = 0.0f;

        float fTargetDist = TargetManager().GetTargetDistance();
        if (fTargetDist < 5.0f)
            fRunDist = 5.0f;

        RwV3d vecTargetPos = TargetManager().GetTargetPos();

        Utils6034::CalcRotationPoint(pvecRunPos,
                                     pvecCurrentPos,
                                     &vecTargetPos,
                                     fRunMoveSpeed,
                                     fRunDist,
                                     m_bFlagDirection);
    };


    //
    // *********************************************************************************
    //


    CDashDispatcher::CDashDispatcher(int32 iHandle)
    : IBaseMoveState(iHandle)
    {
        m_moveTimer.SetAlarmTime(0.6f);
        m_fMinTargetDistSQ = 2.25f;
    };


    /*virtual*/ CDashDispatcher::STARTRESULT
    CDashDispatcher::OnStart(void) /*override*/
    {
        m_moveTimer.Start();
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CDashDispatcher::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CDashDispatcher::RUNRESULT
    CDashDispatcher::OnRun(void) /*override*/
    {
        TargetManager().Update();

        if (!m_bPauseTimer)
            m_moveTimer.Update();

        m_bPauseTimer = false;

        if (m_moveTimer.IsAlarm())
            return RUNRESULT_END;

        float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
        if (fTargetDistSQ <= m_fMinTargetDistSQ)
            return RUNRESULT_END;

        int32 iTargetNo = TargetManager().GetTargetNo();
        CharacterController().DispatchDashTarget(iTargetNo);

        return RUNRESULT_CONTINUE;
    };


    //
    // *********************************************************************************
    //

    
    CCircleDashDispatcher::CCircleDashDispatcher(int32 iHandle)
    : IBaseMoveState(iHandle)
    , m_bFlagDirection(false)
    {
        m_moveTimer.SetAlarmTime(0.6f);
        m_fMinTargetDistSQ = 2.25f;
    };


    /*virtual*/ CCircleDashDispatcher::STARTRESULT
    CCircleDashDispatcher::OnStart(void) /*override*/
    {
        m_moveTimer.Start();
        m_bFlagDirection = ((Math::Rand() & 1) ? true : false);
        return STARTRESULT_END;
    };


    /*virtual*/ int32 CCircleDashDispatcher::OnEnd(void) /*override*/
    {
        return FROMSTACK_STATE_HANDLE;
    };


    /*virtual*/ CCircleDashDispatcher::RUNRESULT
    CCircleDashDispatcher::OnRun(void) /*override*/
    {
        TargetManager().Update();

        if (!m_bPauseTimer)
            m_moveTimer.Update();

        m_bPauseTimer = false;

        if (m_moveTimer.IsAlarm())
            return RUNRESULT_END;

        float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
        if (fTargetDistSQ <= m_fMinTargetDistSQ)
            return RUNRESULT_END;

        if (OrderDash())
            return RUNRESULT_END;

        return RUNRESULT_CONTINUE;
    };


    bool CCircleDashDispatcher::OrderDash(void)
    {
        float fDashSpeed = AIModerator().EnemyCharacter().Feature().m_fDashMoveSpeed;

        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        AIModerator().EnemyCharacter().Compositor().GetPosition(&vecMyPos);

        RwV3d vecDashPos = Math::VECTOR3_ZERO;
        CalcDashPos(&vecDashPos, &vecMyPos, fDashSpeed);

        if (!CWorldMap::CheckCollisionLine(&vecMyPos, &vecDashPos))
        {
            CharacterController().DispatchDashPos(&vecDashPos);
            return false;
        };

        int32 iTargetNo = TargetManager().GetTargetNo();
        CharacterController().DispatchDashTarget(iTargetNo);

        return true;
    };


    void CCircleDashDispatcher::CalcDashPos(RwV3d* pvecDashPos,
                                            const RwV3d* pvecCurrentPos,
                                            float fDashMoveSpeed) const
    {
        float fDashDist = 0.0f;

        float fTargetDist = TargetManager().GetTargetDistance();
        if (fTargetDist < 2.0f)
            fDashDist = 2.0f;

        RwV3d vecTargetPos = TargetManager().GetTargetPos();

        Utils6034::CalcRotationPoint(pvecDashPos,
                                     pvecCurrentPos,
                                     &vecTargetPos,
                                     fDashMoveSpeed,
                                     fDashDist,
                                     m_bFlagDirection);
    };


    //
    // *********************************************************************************
    //

    
    /*virtual*/ void CBaseHideObserver::OnStart(void) /*override*/
    {
        ;
    };


    /*virtual*/ CBaseHideObserver::RESULT CBaseHideObserver::Observing(void) /*override*/
    {
        return RESULT_END;
    };


    /*virtual*/ ENEMYTYPES::STATUS CBaseHideObserver::OnEnd(void) /*override*/
    {
        return ENEMYTYPES::STATUS_IDLE;
    };


    //
    // *********************************************************************************
    //

    
    /*virtual*/ void CBaseDeathObserver::OnStart(void) /*override*/
    {
        m_bIsNexusMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X);

        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
        EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
        EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);
        EnemyChr().Compositor().StopMotion();

        EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    };


    /*virtual*/ CBaseDeathObserver::RESULT CBaseDeathObserver::Observing(void) /*override*/
    {
        if (!m_bIsNexusMap)
            return RESULT_CONTINUE;

        CEnemyUtils::EntryNexusDeathEffect(&EnemyChr());
        return RESULT_END;
    };


    /*virtual*/ ENEMYTYPES::STATUS CBaseDeathObserver::OnEnd(void) /*override*/
    {
        return ENEMYTYPES::STATUS_QUIT;
    };


    //
    // *********************************************************************************
    //


    /*virtual*/ void CBaseThinkingObserver::OnStart(void) /*override*/
    {
        m_nextStatus = ENEMYTYPES::STATUS_IDLE;
    };


    /*virtual*/ CBaseThinkingObserver::RESULT CBaseThinkingObserver::Observing(void) /*override*/
    {
        RESULT result = RESULT_END;

        CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
        switch (order)
        {
        case CAIThinkOrder::ORDER_WAIT:
            {
                int32 waitType = EnemyChr().AIThinkOrder().OrderWait().m_iWaitType;
                switch (waitType)
                {
                case Utils6034::ORDERTYPE_WAIT_TURN_POS:
                case Utils6034::ORDERTYPE_WAIT_TURN_TAR:
                    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
                    break;

                default:
                    m_nextStatus = static_cast<ENEMYTYPES::STATUS>(OnGetOriginalWaitStatus(waitType));
                    break;
                };
            }
            break;

        case CAIThinkOrder::ORDER_MOVE:
            {
                int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
                switch (moveType)
                {
                case Utils6034::ORDERTYPE_MOVE_WALK_POS:
                case Utils6034::ORDERTYPE_MOVE_WALK_TAR:
                    m_nextStatus = ENEMYTYPES::STATUS_WALK;
                    break;

                case Utils6034::ORDERTYPE_MOVE_RUN_POS:
                case Utils6034::ORDERTYPE_MOVE_RUN_TAR:
                    m_nextStatus = ENEMYTYPES::STATUS_RUN;
                    break;

                case Utils6034::ORDERTYPE_MOVE_DASH:
                case Utils6034::ORDERTYPE_MOVE_DASH_POS:
                case Utils6034::ORDERTYPE_MOVE_DASH_TAR:
                case Utils6034::ORDERTYPE_MOVE_DASH_DIR:
                    m_nextStatus = ENEMYTYPES::STATUS_DASH;
                    break;

                case Utils6034::ORDERTYPE_MOVE_JUMP:
                case Utils6034::ORDERTYPE_MOVE_JUMP_POS:
                case Utils6034::ORDERTYPE_MOVE_JUMP_TAR:
                case Utils6034::ORDERTYPE_MOVE_JUMP_DIR:
                    m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
                    break;

                default:
                    m_nextStatus = static_cast<ENEMYTYPES::STATUS>(OnGetOriginalMoveStatus(moveType));
                    break;
                };
            }
            break;

        case CAIThinkOrder::ORDER_ATTACK:
            {
                int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(OnGetOriginalAttackStatus(attackType));
            }
            break;

        case CAIThinkOrder::ORDER_DEFENCE:
            {
                m_nextStatus = ENEMYTYPES::STATUS_GUARD;
            }
            break;

        default:
            break;
        };

        return result;
    };


    /*virtual*/ ENEMYTYPES::STATUS CBaseThinkingObserver::OnEnd(void) /*override*/
    {
        return m_nextStatus;
    };


    /*virtual*/ int32 CBaseThinkingObserver::OnGetOriginalWaitStatus(int32 iWaitType)
    {
        return ENEMYTYPES::STATUS_IDLE;
    };


    /*virtual*/ int32 CBaseThinkingObserver::OnGetOriginalMoveStatus(int32 iMoveType)
    {
        return ENEMYTYPES::STATUS_IDLE;
    };


    /*virtual*/ int32 CBaseThinkingObserver::OnGetOriginalAttackStatus(int32 iAttackType)
    {
        return ENEMYTYPES::STATUS_IDLE;
    };


    //
    // *********************************************************************************
    //

    
    /*virtual*/ void CBaseJumpReadyObserver::OnStart(void) /*override*/
    {
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JUMP_READY, true);
    };


    /*virtual*/ CBaseJumpReadyObserver::RESULT CBaseJumpReadyObserver::Observing(void) /*override*/
    {
        if (EnemyChr().Compositor().IsMotionEnd())
            return RESULT_END;

        return RESULT_CONTINUE;
    };


    /*virtual*/ ENEMYTYPES::STATUS CBaseJumpReadyObserver::OnEnd(void) /*override*/
    {

        RwV3d vecVelocity = Math::VECTOR3_ZERO;

        switch (EnemyChr().AIThinkOrder().OrderMove().m_iMoveType)
        {
        case ORDERTYPE_MOVE_JUMP:
            {
                vecVelocity = { 0.0f, 0.0f, EnemyChr().Feature().m_fAerialMoveSpeed };
                EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
            }
            break;
            
        case ORDERTYPE_MOVE_JUMP_POS:
            {
                RwV3d vecOrderPos = EnemyChr().AIThinkOrder().OrderMove().m_vAt;

                RwV3d vecMyPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetPosition(&vecMyPos);

                Math::Vec3_Sub(&vecVelocity, &vecOrderPos, &vecMyPos);
                Math::Vec3_Normalize(&vecVelocity, &vecVelocity);
                Math::Vec3_Scale(&vecVelocity, &vecVelocity, EnemyChr().Feature().m_fAerialMoveSpeed);
            }
            break;
            
        case ORDERTYPE_MOVE_JUMP_TAR:
            {
                int32 iOrderTarget = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(iOrderTarget);
                if (pPlrChr)
                {
                    RwV3d vecPlrPos = Math::VECTOR3_ZERO;
                    pPlrChr->GetPosition(&vecPlrPos);

                    RwV3d vecMyPos = Math::VECTOR3_ZERO;
                    EnemyChr().Compositor().GetPosition(&vecMyPos);

                    Math::Vec3_Sub(&vecVelocity, &vecPlrPos, &vecMyPos);
                    Math::Vec3_Normalize(&vecVelocity, &vecVelocity);
                    Math::Vec3_Scale(&vecVelocity, &vecVelocity, EnemyChr().Feature().m_fAerialMoveSpeed);
                }
                else
                {
                    vecVelocity = { 0.0f, 0.0f, EnemyChr().Feature().m_fAerialMoveSpeed };
                    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
                };
            }
            break;
            
        case ORDERTYPE_MOVE_JUMP_DIR:
            {
                RwV3d vecOrderDir = EnemyChr().AIThinkOrder().OrderMove().m_vAt;

                Math::Vec3_Normalize(&vecVelocity, &vecOrderDir);
                Math::Vec3_Scale(&vecVelocity, &vecVelocity, EnemyChr().Feature().m_fAerialMoveSpeed);
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        vecVelocity.y += EnemyChr().Feature().m_fJumpInitializeSpeed;

        EnemyChr().Compositor().SetVelocity(&vecVelocity);

        return ENEMYTYPES::STATUS_AERIAL;
    };


    //
    // *********************************************************************************
    //

    
    CBaseThrowObserver::CBaseThrowObserver(ICharacterController* pCharacterController)
    : CBaseThrowObserver(INVALID_SE, pCharacterController)
    {
        ;
    };


    CBaseThrowObserver::CBaseThrowObserver(int32 seID, ICharacterController* pCharacterController)
    : m_orderTargetNo(-1)
    , m_seID(seID)
    , m_pCharacterController(pCharacterController)
    , m_bThrowing(false)
    {
        ;
    };


    /*virtual*/ void CBaseThrowObserver::OnStart(void) /*override*/
    {
        m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        m_bThrowing = false;

        CCommonEnemyObserver::CBaseThrow::OnStart();
    };


    /*virtual*/ ENEMYTYPES::STATUS CBaseThrowObserver::OnEnd(void) /*override*/
    {
        EnemyChr().Compositor().SetMotionCtrlTime(1.0f);

        if (IsThrowSuccess())
        {
            float fDir = EnemyChr().Compositor().GetDirection();
            EnemyChr().Compositor().SetDirection(fDir - MATH_PI);

            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);            
        };

        if (m_bThrowing)
            m_pCharacterController->FlagNoReaction().DecCounter();

        return CCommonEnemyObserver::CBaseThrow::OnEnd();
    };


    /*virtual*/ void CBaseThrowObserver::OnThrowStandby(void) /*override*/
    {
        int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTargetNo);
        if (pPlrChr)
        {
            RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
            pPlrChr->GetFootPosition(&vecFootPosPlr);

            CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(),
                                          &vecFootPosPlr,
                                          EnemyChr().Feature().m_fRotateRate,
                                          0.0f);

        };
    };


    /*virtual*/ void CBaseThrowObserver::OnLift(void) /*override*/
    {
        CCommonEnemyObserver::CBaseThrow::OnLift();
    };


    /*virtual*/ void CBaseThrowObserver::OnThrowHit(void) /*override*/
    {
        if (m_seID != INVALID_SE)
            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), m_seID);

        m_pCharacterController->FlagNoReaction().IncCounter();
        m_bThrowing = true;
    };


    void CBaseThrowObserver::SetSE(int32 seID)
    {
        m_seID = seID;
    };

    
    //
    // *********************************************************************************
    //


    /*virtual*/ void CGuardObserver::OnStart(void) /*override*/
    {
        m_step = 0;

        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    };


    /*virtual*/ CGuardObserver::RESULT CGuardObserver::Observing(void) /*override*/
    {
        switch (m_step)
        {
        case 0:
            {
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_1, true);
                ++m_step;
            }
            break;

        case 1:
            {
                if (EnemyChr().Compositor().IsMotionEnd())
                {
                    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_2, true);
                    ++m_step;
                };
            }
            break;

        case 2:
            {
                if (EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_DEFENCE)
                    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                else
                    ++m_step;
            }
            break;

        case 3:
            {
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_3, true);
                ++m_step;
            }
            break;

        case 4:
            {
                if (EnemyChr().Compositor().IsMotionEnd())
                    return RESULT_END;                
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        
        return RESULT_CONTINUE;
    };


    /*virtual*/ ENEMYTYPES::STATUS CGuardObserver::OnEnd(void) /*override*/
    {
        return ENEMYTYPES::STATUS_THINKING;
    };


    //
    // *********************************************************************************
    //


    CBaseOneShotAttack::CBaseOneShotAttack(const char* pszMotion)
    : CCommonEnemyObserver::COneShotMotion(pszMotion)
    {
        ;
    };


    /*virtual*/ CBaseOneShotAttack::RESULT CBaseOneShotAttack::Observing(void) /*override*/
    {
        RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());

        CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(),
                                      &vecOrderPos,
                                      EnemyChr().Feature().m_fRotateRate,
                                      MATH_DEG2RAD(3.0f));

        return CCommonEnemyObserver::COneShotMotion::Observing();
    };


    //
    // *********************************************************************************
    //


    CBaseOneShotTurnAttack::CBaseOneShotTurnAttack(const char* pszMotion)
    : CBaseOneShotAttack(pszMotion)
    {
        ;
    };

    
    /*virtual*/ void CBaseOneShotTurnAttack::OnStart(void) /*override*/
    {
        CBaseOneShotAttack::OnStart();

        RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());
        float fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecOrderPos);
        EnemyChr().Compositor().SetDirection(fDir);
    };


    //
    // *********************************************************************************
    //


    CElecAttackObserver::CElecAttackObserver(const CCharacterCompositor* pChrCompositor,
                                             const char* pszMotionName,
                                             ICharacterController* pChrController)
    : m_stepTiming(-1)
    , m_stepBodyEffect(-1)
    , m_elecEffectMgr()
    , m_aHandTraceEffect()
    , m_aHandBoneTracer()
    , m_elecBallMgr()
    , m_timerRotation(0.0f)
    , m_timerBodyEffect(0.0f)
    , m_timerHandEffect(0.0f)
    , m_pszMotionName(pszMotionName)
    , m_pChrController(pChrController)
    , m_bFlagNoReactionChanged(false)
    {
        const RwV3d vecHandElecOffset = { 0.0f, 0.3f, 0.0f };

        m_aHandBoneTracer[0].Initialize(pChrCompositor, 4);
        m_aHandBoneTracer[0].SetOffset(&vecHandElecOffset);

        m_aHandBoneTracer[1].Initialize(pChrCompositor, 3);
        m_aHandBoneTracer[1].SetOffset(&vecHandElecOffset);

        m_aHandTraceEffect[0].Initialize(EFFECTID::ID_SHRD_ELEND, &m_aHandBoneTracer[0]);
        m_aHandTraceEffect[0].SetScale(1.2f);
        m_aHandTraceEffect[0].SetResetTime(0.1f);
        
        m_aHandTraceEffect[1].Initialize(EFFECTID::ID_SHRD_ELEND, &m_aHandBoneTracer[1]);
        m_aHandTraceEffect[1].SetScale(1.2f);
        m_aHandTraceEffect[1].SetResetTime(0.1f);

        m_elecEffectMgr.SetCharacter(pChrCompositor);
    };


    /*virtual*/ void CElecAttackObserver::OnStart(void) /*override*/
    {
        m_stepTiming = 0;
        m_stepBodyEffect = 0;
        m_bFlagNoReactionChanged = false;

        m_elecEffectMgr.SetUpdateStepTime(0.04f);
        m_elecEffectMgr.SetDispType(true, true);
        m_elecEffectMgr.StartUpdate();

        m_timerRotation.SetAlarmTime(0.8f);
        m_timerRotation.Start();

        m_timerBodyEffect.SetAlarmTime(0.39f);
        m_timerBodyEffect.Start();

        m_timerHandEffect.SetAlarmTime(0.8f);
        m_timerHandEffect.Start();

        EnemyChr().Compositor().ChangeMotion(m_pszMotionName, true);
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

        if (Math::RandRange(0, 100) >= 14)
            CMessageManager::Request(SEGROUPID::VALUE(89));
        else
            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8468));

        CGameSound::PlayAttackSE(&EnemyChr().Compositor());
    };


    /*virtual*/ CElecAttackObserver::RESULT
    CElecAttackObserver::Observing(void) /*override*/
    {
        UpdateTiming();
        UpdateRotation();
        UpdateHandEffect();
        UpdateBodyEffectMgr();

        if (EnemyChr().Compositor().IsMotionEnd())
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            return RESULT_END;
        };

        return RESULT_CONTINUE;
    };


    /*virtual*/ ENEMYTYPES::STATUS
    CElecAttackObserver::OnEnd(void) /*override*/
    {
        for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
            m_aHandTraceEffect[i].Stop();

        m_elecEffectMgr.SwitchAllEffectOn(false);

        if (m_bFlagNoReactionChanged)
            m_pChrController->FlagNoReaction().DecCounter();

        return ENEMYTYPES::STATUS_THINKING;
    };


    void CElecAttackObserver::ShotElball(void) const
    {
        RwV3d vecMagicDirection = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().RotateVectorByDirection(&vecMagicDirection, &Math::VECTOR3_AXIS_Z);;
        vecMagicDirection.y = 0.0f;

        RwV3d vecBonePosHandLeft = Math::VECTOR3_ZERO;
        CEnemyUtils::GetWorldBonePosition(&vecBonePosHandLeft, &EnemyChr().Compositor(), 4);
        ShotElballMagic(&vecBonePosHandLeft, &vecMagicDirection);

        RwV3d vecBonePosHandRight = Math::VECTOR3_ZERO;
        CEnemyUtils::GetWorldBonePosition(&vecBonePosHandRight, &EnemyChr().Compositor(), 3);
        ShotElballMagic(&vecBonePosHandRight, &vecMagicDirection);
    };


    void CElecAttackObserver::ShotElballMagic(const RwV3d* pvecPosition, const RwV3d* pvecDirection) const
    {
        uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_SHRD_ELBALL),
                                            pvecPosition,
                                            pvecDirection);

        CMagicManager::SetScale(hMagic, 4.0f);
        CMagicManager::SetSpeed(hMagic, 23.0f);
        CMagicManager::SetDamage(hMagic, 12);
    };


    void CElecAttackObserver::UpdateTiming(void)
    {
        if (!EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
            return;

        switch (m_stepTiming)
        {
        case 0:
            {
                m_pChrController->FlagNoReaction().IncCounter();
                m_bFlagNoReactionChanged = true;

                RwV3d vecMyPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetPosition(&vecMyPos);

                m_elecBallMgr.InitEffectCheckPos(&EnemyChr().Compositor());
                m_elecBallMgr.ClearTargetPlayerList();
                m_elecBallMgr.SetTargetPlayerList(&vecMyPos);
                m_elecBallMgr.ShotMagic(&EnemyChr().Compositor(), 4, 3);

                ++m_stepTiming;
            }
            break;

        case 1:
            {
                RwV3d vecMyPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetPosition(&vecMyPos);

                m_elecBallMgr.InitEffectCheckPos(&EnemyChr().Compositor());
                m_elecBallMgr.ClearTargetPlayerList();
                m_elecBallMgr.SetTargetPlayerList(&vecMyPos);
                m_elecBallMgr.ShotMagic(&EnemyChr().Compositor(), 4, 3);

                ++m_stepTiming;
            }
            break;

        case 2:
            {
                ShotElball();

                m_elecEffectMgr.SetUpdateStepTime(0.045f);
                m_elecEffectMgr.SetDispType(true, false);
                m_elecEffectMgr.StartUpdate();

                for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
                    m_aHandTraceEffect[i].Stop();
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };


    void CElecAttackObserver::UpdateRotation(void)
    {
        m_timerRotation.Update();

        if (m_timerRotation.IsAlarm())
            return;

        int32 iTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(iTargetNo);
        if (pPlrChr)
        {
            RwV3d vecPlrPos = Math::VECTOR3_ZERO;
            pPlrChr->GetPosition(&vecPlrPos);

            float fRotRate = EnemyChr().Feature().m_fRotateRate;
            CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vecPlrPos, fRotRate, 0.0f);
        };
    };


    void CElecAttackObserver::UpdateHandEffect(void)
    {
        m_timerHandEffect.Update();

        if (m_timerHandEffect.IsAlarm())
        {
            for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
                m_aHandTraceEffect[i].Start();

            m_timerHandEffect.Pause();
        };

        for (int32 i = 0; i < COUNT_OF(m_aHandTraceEffect); ++i)
            m_aHandTraceEffect[i].Update();
    };


    void CElecAttackObserver::UpdateBodyEffectMgr(void)
    {
        m_timerBodyEffect.Update();

        if (m_timerBodyEffect.IsAlarm())
        {
            switch (m_stepBodyEffect)
            {
            case 0:
                {
                    m_elecEffectMgr.SetUpdateStepTime(0.248f);
                    m_elecEffectMgr.SetDispType(false, true);
                    m_elecEffectMgr.StartUpdate();

                    m_timerBodyEffect.SetAlarmTime(1.748f);

                    ++m_stepBodyEffect;
                }
                break;

            case 1:
                {
                    m_elecEffectMgr.SetUpdateStepTime(0.04f);
                    m_elecEffectMgr.SetDispType(false, false);
                    m_elecEffectMgr.StartUpdate();

                    m_timerBodyEffect.Pause();

                    ++m_stepBodyEffect;
                }
                break;

            default:
                break;
            };
        };

        m_elecEffectMgr.Update();
    };


    //
    // *********************************************************************************
    //


    CTenguAttackObserver::CTenguAttackObserver(void)
    : m_step(-1)
    {
        ;
    };


    /*virtual*/ void
    CTenguAttackObserver::OnStart(void) /*override*/
    {
        m_step = 0;

        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    };


    /*virtual*/ CTenguAttackObserver::RESULT
    CTenguAttackObserver::Observing(void) /*override*/
    {
        switch (m_step)
        {
        case 0:
            {
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_A, true);
                CGameSound::PlayAttackSE(&EnemyChr().Compositor());

                ++m_step;
            }
            break;

        case 1:
            {
                float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
                float fMotionNextT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
                if (fMotionNowT >= fMotionNextT)
                {
                    ++m_step;
                }
                else
                {
                    int32 iTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
                    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(iTargetNo);
                    if (pPlrChr)
                    {
                        RwV3d vecPlrPos = Math::VECTOR3_ZERO;
                        pPlrChr->GetPosition(&vecPlrPos);

                        float fRotRate = EnemyChr().Feature().m_fRotateRate;
                        CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vecPlrPos, fRotRate, 0.0f);
                    };
                };
            }
            break;

        case 2:
            {
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_AA, true);
                CGameSound::PlayAttackSE(&EnemyChr().Compositor());

                ++m_step;
            }
            break;

        case 3:
            {
                if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
                    ShotTenguSword();

                if (EnemyChr().Compositor().IsMotionEnd())
                {
                    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                    return RESULT_END;
                };
            }
            break;

        default:
            break;
        };
        
        return RESULT_CONTINUE;
    };


    /*virtual*/ ENEMYTYPES::STATUS
    CTenguAttackObserver::OnEnd(void) /*override*/
    {
        return ENEMYTYPES::STATUS_THINKING;
    };


    void CTenguAttackObserver::ShotTenguSword(void) const
    {
        /* get sword world pos */
        RwV3d vecSwordPos = Math::VECTOR3_ZERO;
        CEnemyUtils::GetWorldBonePosition(&vecSwordPos, &EnemyChr().Compositor(), 7);

        /* calc direction offset step */
        RwV3d vecFront = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().RotateVectorByDirection(&vecFront, &Math::VECTOR3_AXIS_Z);
        vecFront.y = 0.0f;

        Math::Vec3_Normalize(&vecFront, &vecFront);

        RwV3d vecLeft = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vecLeft, &Math::VECTOR3_AXIS_Y, &vecFront);

        RwV3d vecDirOffset = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vecDirOffset, &vecLeft, 0.25f);

        /* calc magic direction */
        CPlayerCharacter* pPlrChr = CAIUtils::FindFirstPlayer();

        RwV3d vecPlrPos = Math::VECTOR3_ZERO;
        pPlrChr->GetPosition(&vecPlrPos);
        vecPlrPos.y = CWorldMap::GetMapHeight(&vecPlrPos) + 0.3f;

        RwV3d vecMagicDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecMagicDir, &vecPlrPos, &vecSwordPos);
        Math::Vec3_Normalize(&vecMagicDir, &vecMagicDir);
        vecMagicDir.x = vecFront.x;
        vecMagicDir.z = vecFront.z;

        /* check for dist */
        vecPlrPos = Math::VECTOR3_ZERO;
        pPlrChr->GetPosition(&vecPlrPos);

        static const float s_fTenguSwordRangeSQ = 3.24f;

        /* TODO squared dist? */
        float fDist = CEnemyUtils::GetDistance(&EnemyChr().Compositor(), &vecPlrPos);
        if (fDist >= s_fTenguSwordRangeSQ)
        {
            /* shot center */
            RwV3d vecMagicDirCenter = vecMagicDir;
            ShotTenguSwordMagic(&vecSwordPos, &vecMagicDirCenter);

            /* shot left */
            RwV3d vecMagicDirLeft = Math::VECTOR3_ZERO;
            Math::Vec3_Add(&vecMagicDirLeft, &vecMagicDir, &vecDirOffset);

            ShotTenguSwordMagic(&vecSwordPos, &vecMagicDirLeft);

            /* shot right */
            RwV3d vecMagicDirRight = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecMagicDirRight, &vecMagicDir, &vecDirOffset);

            ShotTenguSwordMagic(&vecSwordPos, &vecMagicDirRight);

            /* play SE */
            RwV3d vecPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetPosition(&vecPos);
            Math::Vec3_Add(&vecPos, &vecPos, &vecSwordPos);

            CGameSound::PlayPositionSE(&vecPos, SDCODE_SE(4360));
        };
    };


    void CTenguAttackObserver::ShotTenguSwordMagic(const RwV3d* pvecPosition, const RwV3d* pvecDirection) const
    {
        uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_TENGSWORD), pvecPosition, pvecDirection);
        CMagicManager::SetSpeed(hMagic, 25.0f);
    };


    //
    // *********************************************************************************
    //


    CChargeAttackObserver::CChargeAttackObserver(ICharacterController* pChrController)
    : m_step(-1)
    , m_pChrController(pChrController)
    {
        ;
    };


    /*virtual*/ void
    CChargeAttackObserver::OnStart(void) /*override*/
    {
        m_step = 0;
        m_fMoveTimer = 0.0f;
        m_vecVelocity = Math::VECTOR3_ZERO;
        m_pChrController->FlagNoReaction().IncCounter();

        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    };


    /*virtual*/ CChargeAttackObserver::RESULT
    CChargeAttackObserver::Observing(void) /*override*/
    {
        switch (m_step)
        {
        case 0:
            {
                EnemyChr().Compositor().ChangeMotion("B1", true);
                ++m_step;
            }
            break;

        case 1:
            {                
                if (EnemyChr().Compositor().IsMotionEnd())
                    ++m_step;

                RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());
                float fRotRate = EnemyChr().Feature().m_fRotateRate;
                CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vecOrderPos, fRotRate, 0.0f);
            }
            break;

        case 2:
            {
                EnemyChr().Compositor().ChangeMotion("B2", true);

                m_vecVelocity = { 0.0f, 0.0f, 15.0f };
                EnemyChr().Compositor().RotateVectorByDirection(&m_vecVelocity, &m_vecVelocity);

                m_fMoveTimer = 0.0f;
                ++m_step;
            }
            break;

        case 3:
            {
                if (!EnemyChr().Compositor().IsMotionEnd())
                {
                    if (m_fMoveTimer > 0.08f)
                        EnemyChr().Compositor().SetVelocity(&m_vecVelocity);

                    m_fMoveTimer += CGameProperty::GetElapsedTime();

                    break;
                };

                m_vecVelocity = { 0.0f, 0.0f, 20.0f };
                EnemyChr().Compositor().RotateVectorByDirection(&m_vecVelocity, &m_vecVelocity);

                ++m_step;
            }
            break;

        case 4:
            {
                EnemyChr().Compositor().ChangeMotion("B3", true);
                ++m_step;
            }
            break;

        case 5:
            {
                if (!EnemyChr().Compositor().IsMotionEnd())
                {
                    EnemyChr().Compositor().SetVelocity(&m_vecVelocity);
                    break;
                };

                ++m_step;
            }
            break;

        case 6:
            {
                EnemyChr().Compositor().ChangeMotion("B4", true);
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

                ++m_step;
            }
            break;

        case 7:
            {
                if (EnemyChr().Compositor().IsMotionEnd())
                {
                    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                    return RESULT_END;
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        return RESULT_CONTINUE;
    };


    /*virtual*/ ENEMYTYPES::STATUS
    CChargeAttackObserver::OnEnd(void) /*override*/
    {
        m_pChrController->FlagNoReaction().DecCounter();
        return ENEMYTYPES::STATUS_THINKING;
    };

    
    //
    // *********************************************************************************
    //


    /*virtual*/ void CDashStatus::OnStart(void) /*override*/
    {
        m_step = 0;
        m_fDashTime = EnemyChr().Feature().m_fDashMoveTime;
    };


    /*virtual*/ CDashStatus::RESULT
    CDashStatus::Observing(void) /*override*/
    {
        switch (m_step)
        {
        case 0:
            {
                EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DASH1, true);

                int32 iMoveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
                switch (iMoveType)
                {
                case ORDERTYPE_MOVE_DASH_POS:                    
                    {
                        RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
                        EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

                        RwV3d vecOrderPos = EnemyChr().AIThinkOrder().OrderMove().m_vAt;

                        float fDir = std::atan2(vecOrderPos.x - vecMyFootPos.x,
                                                vecOrderPos.z - vecMyFootPos.z);

                        EnemyChr().Compositor().SetDirection(fDir);
                    }
                    break;

                case ORDERTYPE_MOVE_DASH_TAR:
                    {
                        int32 iTargetNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
                        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(iTargetNo);
                        if (pPlrChr)
                        {
                            RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
                            pPlrChr->GetFootPosition(&vecPlrFootPos);

                            RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
                            EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

                            float fDir = std::atan2(vecPlrFootPos.x - vecMyFootPos.x,
                                                    vecPlrFootPos.z - vecMyFootPos.z);

                            EnemyChr().Compositor().SetDirection(fDir);
                        };
                    }
                    break;

                case ORDERTYPE_MOVE_DASH_DIR:
                    {
                        RwV3d vecOrderDir = EnemyChr().AIThinkOrder().OrderMove().m_vAt;
                        float fDir = std::atan2(vecOrderDir.x, vecOrderDir.z);

                        EnemyChr().Compositor().SetDirection(fDir);
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                };

                RwV3d vecVelocity = { 0.0f, 0.0f, EnemyChr().Feature().m_fDashMoveSpeed };
                EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
                EnemyChr().Compositor().SetVelocity(&vecVelocity);

                float fDir = EnemyChr().Compositor().GetDirection();

                CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH1, new CEnemyTracer(&EnemyChr()), &Math::VECTOR3_ZERO, fDir);
                CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH2, new CEnemyTracer(&EnemyChr()), &Math::VECTOR3_ZERO, fDir);

                ++m_step;
            }
            break;

        case 1:
            {
                if (EnemyChr().Compositor().IsMotionEnd())
                    ++m_step;
            }
            break;

        case 2:
            {
                if (m_fDashTime <= 0.0f)
                {
                    ++m_step;   
                    break;
                };

                m_fDashTime -= CGameProperty::GetElapsedTime();
            }
            break;

        case 3:
            {
                EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DASH2, true);
                ++m_step;
            }
            break;

        case 4:
            {
                if (EnemyChr().Compositor().IsMotionEnd())
                {
                    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

                    return RESULT_END;
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        return RESULT_CONTINUE;
    };


    /*virtual*/ ENEMYTYPES::STATUS
    CDashStatus::OnEnd(void) /*override*/
    {
        EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
        return ENEMYTYPES::STATUS_THINKING;
    };
};