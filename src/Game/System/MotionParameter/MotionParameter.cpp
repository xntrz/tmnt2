#include "MotionParameter.hpp"


#define TIMING(value) (((value) + (1.0f / 8192.0f)) * 256.0f)


CMotionParameter::CMotionParameter(INIT_PARAMETER* pParam)
: m_fBlendTime(0.0f)
, m_playmode(0)
, m_nAttack(0)
, m_pChainMotionParameter(nullptr)
, m_nLocusTiming(0)
, m_nAtomicTiming(0)
, m_nAttackTiming(0)
, m_nInvincibleTiming(0)
, m_nAtomicOffset(0)
, m_nLocusOffset(0)
{
    Initialize(pParam);
};


CMotionParameter::~CMotionParameter(void)
{
    if (m_pChainMotionParameter)
    {
        delete m_pChainMotionParameter;
        m_pChainMotionParameter = nullptr;
    };

    DestroyListAttack(m_listAttackParameter);

    ASSERT(m_listAttackParameter.empty());
    
    DestroyListTiming(m_listAttackTimingParameter);
    DestroyListTiming(m_listLocusTimingParameter);
    DestroyListTiming(m_listAtomicTimingParameter);
    DestroyListTiming(m_listInvincibleTimingParameter);

    ASSERT(m_listAttackTimingParameter.empty());
    ASSERT(m_listLocusTimingParameter.empty());
    ASSERT(m_listAtomicTimingParameter.empty());
    ASSERT(m_listInvincibleTimingParameter.empty());
};


void CMotionParameter::Initialize(INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    std::strcpy(m_szName, pParam->m_szName);
    m_fBlendTime = pParam->m_fBlendTime;
    m_playmode = pParam->m_playmode;
};


void CMotionParameter::CreateAttackParameter(CAttackParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    CAttackParameter* pAttack = new CAttackParameter(pParam);
    ASSERT(pAttack);

    ++m_nAttack;
    m_listAttackParameter.push_back(pAttack);
};


void CMotionParameter::AttachAttackParameter(int32 no, CAttackParameter::ATTACH_PARAMETER* pParam)
{
    CAttackParameter* pAttack = GetAttackParameter(no);
    
    ASSERT(pAttack);
    ASSERT(pParam);
    
    pAttack->Attach(pParam);
};


CAttackParameter* CMotionParameter::GetAttackParameter(int32 no)
{
    ASSERT(no >= 0 && no < m_nAttack);
	return m_listAttackParameter.search(no);
};


int32 CMotionParameter::GetNumAttackParameter(void) const
{
    return m_nAttack;
};


void CMotionParameter::CreateChainMotionParameter(CChainMotionParameter::INIT_PARAMETER* pParam)
{
    ASSERT(!m_pChainMotionParameter);
    m_pChainMotionParameter = new CChainMotionParameter(pParam);
    ASSERT(m_pChainMotionParameter);
};


void CMotionParameter::AttachChainMotionParameter(CChainMotionParameter::ATTACH_PARAMETER* pParam)
{
    ASSERT(m_pChainMotionParameter);
    m_pChainMotionParameter->Attach(pParam);
};


float CMotionParameter::GetNextChainMotionConnectTime(const char* pszNextMotion)
{
    if (m_pChainMotionParameter)
        return m_pChainMotionParameter->GetNextChainMotionConnectTime(pszNextMotion);
    else
        return 0.0f;
};


void CMotionParameter::CreateTimingParameter(CTimingParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    CTimingParameter* pTiming = new CTimingParameter(pParam);
    ASSERT(pTiming);

    switch (pTiming->GetKind())
    {
    case CTimingParameter::TIMING_ATOMIC_ON:
    case CTimingParameter::TIMING_ATOMIC_OFF:
        ++m_nAtomicTiming;
        InsertTimingByTime(m_listAtomicTimingParameter, pTiming);
        break;

    case CTimingParameter::TIMING_LOCUS_ON:
    case CTimingParameter::TIMING_LOCUS_OFF:
        ++m_nLocusTiming;
        InsertTimingByTime(m_listLocusTimingParameter, pTiming);
        break;

    case CTimingParameter::TIMING_ATTACK:
        ++m_nAttackTiming;
        InsertTimingByTime(m_listAttackTimingParameter, pTiming);
        break;

    case CTimingParameter::TIMING_INVINCIBLE_ON:
    case CTimingParameter::TIMING_INVINCIBLE_OFF:
        ++m_nInvincibleTiming;
        InsertTimingByTime(m_listInvincibleTimingParameter, pTiming);
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CMotionParameter::InsertTimingByTime(CList<CTimingParameter>& rList, CTimingParameter* pTiming)
{
    if (rList.empty())
    {
        rList.push_back(pTiming);
    }
    else
    {
        auto it = rList.begin();
        while (it)
        {
            if (pTiming->GetTime() < (*it).GetTime())
                break;
            
            ++it;
        };

        if (it)
        {
            rList.insert(it, pTiming);
        }
        else
        {
            rList.push_back(pTiming);
        };
    };
};


int32 CMotionParameter::GetNumDrawAtomicData(float fPrevTime, float fNowTime)
{
    ASSERT(fPrevTime >= 0.0f);
    ASSERT(fNowTime >= 0.0f);
    ASSERT(fPrevTime <= fNowTime);

    int32 iResult = 0;

    if (Math::FEqual(fPrevTime, 0.0f))
        fPrevTime = -1.0f;

    m_nAtomicOffset = 0;

    for (CTimingParameter& it : m_listAtomicTimingParameter)
    {
        ASSERT(it.IsAtomic());

        if (TIMING(fPrevTime) <= TIMING(it.GetTime()))
        {
            if (TIMING(fNowTime) < TIMING(it.GetTime()))
                break;

            ++iResult;
        }
        else
        {
            ++m_nAtomicOffset;
        };
    };

    return iResult;
};


void CMotionParameter::GetDrawAtomicData(int32 nIndex, CMotionParameterController::DRAW_ATOMIC* pData)
{
    nIndex = m_nAtomicOffset;

    ASSERT(pData);
    ASSERT(nIndex >= 0 && nIndex < m_nAtomicTiming);

    CTimingParameter* pTiming = m_listAtomicTimingParameter.search(nIndex);

    ASSERT(pTiming);
    ASSERT(pTiming->IsAtomic());

    switch (pTiming->GetKind())
    {
    case CTimingParameter::TIMING_ATOMIC_ON:
        pData->m_bDraw = true;
        pData->m_nAtomic = pTiming->GetParam();
        break;

    case CTimingParameter::TIMING_ATOMIC_OFF:
        pData->m_bDraw = false;
        pData->m_nAtomic = pTiming->GetParam();
        break;

    default:
        ASSERT(false);
        break;
    };
};


int32 CMotionParameter::GetNumDrawLocusData(float fPrevTime, float fNowTime)
{
    ASSERT(fPrevTime >= 0.0f);
    ASSERT(fNowTime >= 0.0f);
    ASSERT(fPrevTime <= fNowTime);

    int32 iResult = 0;
    
    if (Math::FEqual(fPrevTime, 0.0f))
        fPrevTime = -1.0f;

    m_nLocusOffset = 0;

    for (CTimingParameter& it : m_listLocusTimingParameter)
    {
        ASSERT(it.IsLocus());

        if (TIMING(fPrevTime) <= TIMING(it.GetTime()))
        {
            if (TIMING(fNowTime) < TIMING(it.GetTime()))
                break;

            ++iResult;
        }
        else
        {
            ++m_nLocusOffset;
        };
    };

    return iResult;
};


void CMotionParameter::GetDrawLocusData(int32 nIndex, CMotionParameterController::DRAW_LOCUS* pData)
{
    nIndex = m_nLocusOffset;
    
    ASSERT(pData);
    ASSERT(nIndex >= 0 && nIndex < m_nLocusTiming);

    CTimingParameter* pTiming = m_listLocusTimingParameter.search(nIndex);

    ASSERT(pTiming);
    ASSERT(pTiming->IsLocus());

    switch (pTiming->GetKind())
    {
    case CTimingParameter::TIMING_LOCUS_ON:
        pData->m_bDraw = true;
        pData->m_nLocus = pTiming->GetParam();
        break;

    case CTimingParameter::TIMING_LOCUS_OFF:
        pData->m_bDraw = false;
        pData->m_nLocus = pTiming->GetParam();
        break;

    default:
        ASSERT(false);
        break;
    };
};


bool CMotionParameter::IsAttackTiming(float fPrevTime, float fNowTime)
{
    ASSERT(fPrevTime >= 0.0f);
    ASSERT(fNowTime >= 0.0f);
    ASSERT(fPrevTime <= fNowTime);

    if (Math::FEqual(fPrevTime, 0.0f))
        fPrevTime = -1.0f;

    for (CTimingParameter& it : m_listAttackTimingParameter)
    {
        ASSERT(it.IsAttack());

        if (TIMING(fPrevTime) < TIMING(it.GetTime()))
            return TIMING(fNowTime) >= TIMING(it.GetTime());
    };

    return false;
};


bool CMotionParameter::IsInvincibleTime(float fNowTime)
{
    ASSERT(fNowTime >= 0.0f);

    bool bResult = false;

    for (CTimingParameter& it : m_listInvincibleTimingParameter)
    {
        ASSERT(it.IsInvincible());
        
        if (TIMING(fNowTime) < TIMING(it.GetTime()))
            break;

        switch (it.GetKind())
        {
        case CTimingParameter::TIMING_INVINCIBLE_ON:
            bResult = true;
            break;

        case CTimingParameter::TIMING_INVINCIBLE_OFF:
            bResult = false;
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    return bResult;
};


void CMotionParameter::DestroyListTiming(CList<CTimingParameter>& rList)
{
    auto it = rList.begin();
    while (it)
    {
        CTimingParameter* pTiming = &(*it);
        it = rList.erase(it);

        delete pTiming;
    };
};


void CMotionParameter::DestroyListAttack(CList<CAttackParameter>& rList)
{
    auto it = rList.begin();
    while (it)
    {
        CAttackParameter* pAttack = &(*it);
        it = rList.erase(it);

        delete pAttack;
    };
};


const char* CMotionParameter::GetName(void) const
{
    return m_szName;
};


float CMotionParameter::GetBlendTime(void) const
{
    return m_fBlendTime;
};


int32 CMotionParameter::GetPlaymode(void) const
{
    return m_playmode;
};