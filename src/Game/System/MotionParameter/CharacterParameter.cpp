#include "CharacterParameter.hpp"


CCharacterParameter::CCharacterParameter(INIT_PARAMETER* pParam)
: m_collisionparameter({ 0.0f, 0.0f })
, m_nBody(0)
, m_nCatch(0)
, m_nMotion(0)
, m_pCurrentMotion(nullptr)
, m_nLocus(0)
, m_bOffAtomic(false)
, m_nOffAtomic(0)
, m_paOffAtomic(nullptr)
, m_bReverseAtomicNo(false)
, m_nAtomicNum(0)
{
    if (pParam)
    {
        Initialize(pParam);
    };
};


CCharacterParameter::~CCharacterParameter(void)
{
    ASSERT(m_listPos.empty());
    ASSERT(m_listBody.empty());
    ASSERT(m_listCatch.empty());
    ASSERT(m_listLocus.empty());
    ASSERT(m_listMotion.empty());
};


void CCharacterParameter::Cleanup(void)
{
    m_szName[0] = '\0';
    m_collisionparameter = { 0.0f, 0.0f };

    DestroyHitList(m_listPos);

    m_nBody = 0;
    DestroyHitList(m_listBody);

    m_nCatch = 0;
    DestroyHitList(m_listCatch);

    m_nLocus = 0;
    DestroyLocusList(m_listLocus);

    m_nMotion = 0;
    m_pCurrentMotion = nullptr;
    DestroyMotionList(m_listMotion);

    m_bOffAtomic = false;
    m_bReverseAtomicNo = false;
    m_nAtomicNum = 0;
    if (m_paOffAtomic)
    {
        delete[] m_paOffAtomic;
        m_paOffAtomic = nullptr;
    };
};


void CCharacterParameter::Initialize(INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    std::strcpy(m_szName, pParam->m_szName);
    m_collisionparameter.m_fRadius = pParam->m_fRadius;
    m_collisionparameter.m_fHeight = pParam->m_fHeight;
};


const CMotionParameterController::COLLISIONPARAMETER* CCharacterParameter::GetCollisionParameter(void) const
{
    return &m_collisionparameter;
};


void CCharacterParameter::CreatePosition(CHitSphereParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    CHitSphereParameter* pPosition = new CHitSphereParameter(pParam);
    ASSERT(pPosition);

    m_listPos.push_back(pPosition);
};


CHitSphereParameter* CCharacterParameter::GetPositionParameter(int32 no)
{
    CHitSphereParameter* pPosition = m_listPos.search(no);
    ASSERT(pPosition);

    return pPosition;
};


void CCharacterParameter::CreateHitBody(CHitSphereParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);
    
    CHitSphereParameter* pBody = new CHitSphereParameter(pParam);
    ASSERT(pBody);

    ++m_nBody;
    m_listBody.push_back(pBody);
};


CHitSphereParameter* CCharacterParameter::GetHitBodyParameter(int32 no)
{
    ASSERT(no >= 0);
    ASSERT(no < m_nBody);

    CHitSphereParameter* pBody = m_listBody.search(no);
    ASSERT(pBody);
    
    return pBody;
};


int32 CCharacterParameter::GetHitBodyNum(void) const
{
    return m_nBody;
};


void CCharacterParameter::CreateHitCatch(CHitSphereParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);
    
    CHitSphereParameter* pCatch = new CHitSphereParameter(pParam);
    ASSERT(pCatch);

    ++m_nCatch;
    m_listCatch.push_back(pCatch);
};


CHitSphereParameter* CCharacterParameter::GetHitCatchParameter(int32 no)
{
    ASSERT(no >= 0);
    ASSERT(no < m_nCatch);

    CHitSphereParameter* pCatch = m_listCatch.search(no);
    ASSERT(pCatch);

    return pCatch;
};


int32 CCharacterParameter::GetHitCatchNum(void) const
{
    return m_nCatch;
};


void CCharacterParameter::CreateMotion(CMotionParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    CMotionParameter* pMotion = new CMotionParameter(pParam);
    ASSERT(pMotion);

    ++m_nMotion;
    m_listMotion.push_back(pMotion);
	SetCurrentMotion(pMotion);
};


const char* CCharacterParameter::GetMotionName(int32 no)
{
    ASSERT(no >= 0);
    ASSERT(no < m_nMotion);

    CMotionParameter* pMotion = m_listMotion.search(no);
    ASSERT(pMotion);

    return pMotion->GetName();
};


CMotionParameter* CCharacterParameter::GetMotion(const char* pszMotion)
{
    ASSERT(pszMotion);

    for (CMotionParameter& it : m_listMotion)
    {
        if (!std::strcmp(pszMotion, it.GetName()))
            return &it;
    };

    return nullptr;
};


int32 CCharacterParameter::GetMotionNum(void) const
{
    return m_nMotion;
};


void CCharacterParameter::SetCurrentMotion(CMotionParameter* pMotion)
{
    ASSERT(pMotion);
    m_pCurrentMotion = pMotion;
};


void CCharacterParameter::CreateAttack(CAttackParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(m_pCurrentMotion);

    m_pCurrentMotion->CreateAttackParameter(pParam);
};


void CCharacterParameter::AttachAttack(const char* pszMotionName, int32 no, CAttackParameter::ATTACH_PARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(m_pCurrentMotion);
    ASSERT(pszMotionName);

    CMotionParameter* pMotionParameter = GetMotion(pszMotionName);
	ASSERT(pMotionParameter);
    m_pCurrentMotion = pMotionParameter;
    m_pCurrentMotion->AttachAttackParameter(no, pParam);
};


void CCharacterParameter::CreateChainMotion(CChainMotionParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(m_pCurrentMotion);

    m_pCurrentMotion->CreateChainMotionParameter(pParam);
};


void CCharacterParameter::AttachChainMotion(CChainMotionParameter::ATTACH_PARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(m_pCurrentMotion);

    m_pCurrentMotion->AttachChainMotionParameter(pParam);
};


void CCharacterParameter::CreateTiming(CTimingParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(m_pCurrentMotion);

    m_pCurrentMotion->CreateTimingParameter(pParam);
};


void CCharacterParameter::CreateLocus(CLocusParameter::INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    CLocusParameter* pLocus = new CLocusParameter(pParam);
    ASSERT(pLocus);

    ++m_nLocus;
    m_listLocus.push_back(pLocus);
};


void CCharacterParameter::GetLocusParamter(int32 no, CMotionParameterController::LOCUSPARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(no >= 0);
    ASSERT(no < m_nLocus);

    CLocusParameter* pLocus = m_listLocus.search(no);

    ASSERT(pLocus);
    if (pLocus)
    {
        pParam->m_nBoneID       = pLocus->GetBoneID();
        pParam->m_vPositionHigh = *pLocus->GetPosHigh();
        pParam->m_vPositionLow  = *pLocus->GetPosLow();
        pParam->m_Color         = pLocus->GetColor();
    };
};


int32 CCharacterParameter::GetLocusNum(void) const
{
    return m_nLocus;
};


void CCharacterParameter::SetExistOffAtomic(bool bExist)
{
    m_bOffAtomic = bExist;
};


void CCharacterParameter::SetDefaultOffAtomicNum(int32 nAtomicNum)
{
    ASSERT(m_bOffAtomic);
    ASSERT(m_nOffAtomic == 0);
    ASSERT(!m_paOffAtomic);

    m_nOffAtomic = nAtomicNum;
    m_paOffAtomic = new int32[m_nOffAtomic];
};


void CCharacterParameter::SetDefaultOffAtomicNo(int32 nIndex, int32 nAtomicNo)
{
    ASSERT(m_bOffAtomic);
    ASSERT(nIndex < m_nOffAtomic);
    ASSERT(m_paOffAtomic);

    m_paOffAtomic[nIndex] = nAtomicNo;
};


bool CCharacterParameter::IsDrawAtomic(int32 nAtomicNo) const
{
    for (int32 i = 0; i < m_nOffAtomic; ++i)
    {
        if (m_paOffAtomic[i] == nAtomicNo)
            return false;
    };

    return true;
};


void CCharacterParameter::SetReverseAtomicNo(bool bSet)
{
    m_bReverseAtomicNo = bSet;
};


bool CCharacterParameter::IsReverseAtomicNo(void) const
{
    return m_bReverseAtomicNo;
};


void CCharacterParameter::SetAtomicNum(int32 nAtomicNum)
{
    m_nAtomicNum = nAtomicNum;
};


int32 CCharacterParameter::GetAtomicNum(void) const
{
    return m_nAtomicNum;
};


void CCharacterParameter::DestroyHitList(CList<CHitSphereParameter>& rList)
{
    auto it = rList.begin();
    auto itEnd = rList.end();
    while (it != itEnd)
    {
        CHitSphereParameter* pHit = &(*it);
        it = rList.erase(it);

        delete pHit;
    };
};


void CCharacterParameter::DestroyLocusList(CList<CLocusParameter>& rList)
{
    auto it = rList.begin();
    auto itEnd = rList.end();
    while (it != itEnd)
    {
        CLocusParameter* pLocus = &(*it);
        it = rList.erase(it);

        delete pLocus;
    };
};


void CCharacterParameter::DestroyMotionList(CList<CMotionParameter>& rList)
{
    auto it = rList.begin();
    auto itEnd = rList.end();
    while (it != itEnd)
    {
        CMotionParameter* pMotion = &(*it);
        it = rList.erase(it);

        delete pMotion;
    };
};


const char* CCharacterParameter::GetName(void) const
{
    return m_szName;
};