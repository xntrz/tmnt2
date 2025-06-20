#include "MotionParameterController.hpp"
#include "MotionParameterManager.hpp"
#include "MotionParameter.hpp"
#include "CharacterParameter.hpp"

#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"


CMotionParameterController::CMotionParameterController(const char* pszModelName, int32 nPartsNum)
: m_pCharacterParameter(nullptr)
, m_pCurrentMotionParameter(nullptr)
{
    m_pCharacterParameter = CMotionParameterManager::GetCharacterParameter(pszModelName, nPartsNum);
    ASSERT(m_pCharacterParameter);
};


CMotionParameterController::~CMotionParameterController(void)
{
    ;
};


int32 CMotionParameterController::GetMotionNum(void) const
{
    return CharacterParam().GetMotionNum();
};


const char* CMotionParameterController::GetMotionName(int32 no)
{
    return CharacterParam().GetMotionName(no);
};


void CMotionParameterController::SetCurrentMotion(const char* pszMotionName)
{
    CMotionParameter* pMotion = CharacterParam().GetMotion(pszMotionName);
    
    ASSERT(pMotion);
    
    CharacterParam().SetCurrentMotion(pMotion);
    m_pCurrentMotionParameter = pMotion;
};


float CMotionParameterController::GetBlendTime(void) const
{
    return CurrentMotionParam().GetBlendTime();
};


CMotionParameterController::PLAYMODE CMotionParameterController::GetPlaymode(void) const
{
    PLAYMODE playmode = static_cast<PLAYMODE>(CurrentMotionParam().GetPlaymode());
    
    ASSERT(playmode >= 0);
    ASSERT(playmode < PLAYMODEMAX);

    return playmode;
};


float CMotionParameterController::GetNextChainMotionConnectTime(const char* pszNextMotion)
{
    return CurrentMotionParam().GetNextChainMotionConnectTime(pszNextMotion);
};


const CMotionParameterController::COLLISIONPARAMETER* CMotionParameterController::GetCollisionParameter(void) const
{
    return CharacterParam().GetCollisionParameter();
};


int32 CMotionParameterController::GetNumHitBodyData(void) const
{
    return CharacterParam().GetHitBodyNum();
};


int32 CMotionParameterController::GetHitBodyBoneID(int32 no)
{
    return CharacterParam().GetHitBodyParameter(no)->GetBoneID();
};


const RwSphere* CMotionParameterController::GetHitBodySphere(int32 no)
{
    return CharacterParam().GetHitBodyParameter(no)->GetSphere();
};


int32 CMotionParameterController::GetNumHitCatchData(void) const
{
    return CharacterParam().GetHitCatchNum();
};


int32 CMotionParameterController::GetHitCatchBoneID(int32 no)
{
    return CharacterParam().GetHitCatchParameter(no)->GetBoneID();
};


const RwSphere* CMotionParameterController::GetHitCatchSphere(int32 no)
{
    return CharacterParam().GetHitCatchParameter(no)->GetSphere();
};


void CMotionParameterController::GetHitCatchData(int32 no, CHitCatchData* pHitCatchData)
{
    ASSERT(pHitCatchData);
    
    pHitCatchData->SetCatchNo(no);
};


int32 CMotionParameterController::GetNumHitAttackData(void) const
{
    return CurrentMotionParam().GetNumAttackParameter();
};


bool CMotionParameterController::IsHitAttackEnableTime(int32 no, float fNow)
{
    return CurrentMotionParam().GetAttackParameter(no)->IsAttackEnableTime(fNow);
};


int32 CMotionParameterController::GetHitAttackBoneID(int32 no)
{
    return CurrentMotionParam().GetAttackParameter(no)->GetBoneID();
};


const RwSphere* CMotionParameterController::GetHitAttackSphere(int32 no)
{
    return CurrentMotionParam().GetAttackParameter(no)->GetSphere();
};


void CMotionParameterController::GetHitAttackData(int32 no, CHitAttackData* pHitAttackData)
{
    pHitAttackData->SetMotion(CurrentMotionParam().GetName());
    CurrentMotionParam().GetAttackParameter(no)->GetParameter(pHitAttackData);
};


int32 CMotionParameterController::GetNumLocus(void) const
{
    return CharacterParam().GetLocusNum();
};


void CMotionParameterController::GetLocusParameter(int32 no, LOCUSPARAMETER* pParam)
{
    CharacterParam().GetLocusParamter(no, pParam);
};


int32 CMotionParameterController::GetNumDrawAtomicData(float fPrevTime, float fNowTime)
{
    return CurrentMotionParam().GetNumDrawAtomicData(fPrevTime, fNowTime);
};


void CMotionParameterController::GetDrawAtomicData(int32 no, DRAW_ATOMIC* pData)
{
    CurrentMotionParam().GetDrawAtomicData(no, pData);

    if (CharacterParam().IsReverseAtomicNo())
    {
        int32 nAtomicNum = CharacterParam().GetAtomicNum();
        int32 nAtomic = (nAtomicNum - pData->m_nAtomic - 1);

        pData->m_nAtomic = nAtomic;

        ASSERT(pData->m_nAtomic >= 0);
    };
};


int32 CMotionParameterController::GetNumDrawLocusData(float fPrevTime, float fNowTime)
{
    return CurrentMotionParam().GetNumDrawLocusData(fPrevTime, fNowTime);
};


void CMotionParameterController::GetDrawLocusData(int32 no, DRAW_LOCUS* pData)
{
    CurrentMotionParam().GetDrawLocusData(no, pData);
};


bool CMotionParameterController::IsAttackTiming(float fPrevTime, float fNowTime)
{
    return CurrentMotionParam().IsAttackTiming(fPrevTime, fNowTime);
};


bool CMotionParameterController::IsInvincibleTime(float fNowTime)
{
    return CurrentMotionParam().IsInvincibleTime(fNowTime);
};


bool CMotionParameterController::IsDrawAtomic(int32 nAtomicNo)
{
    if (CharacterParam().IsReverseAtomicNo())
        nAtomicNo = (CharacterParam().GetAtomicNum() - 1 - nAtomicNo);

	ASSERT(nAtomicNo >= 0);

    return CharacterParam().IsDrawAtomic(nAtomicNo);
};


void CMotionParameterController::GetOffsetPosition(int32 no, int32& rnBoneNo, RwV3d& rvOffset)
{
    CHitSphereParameter* pParam = CharacterParam().GetPositionParameter(no);
    ASSERT(pParam);

    rnBoneNo = pParam->GetBoneID();
    rvOffset = pParam->GetSphere()->center;
};


CMotionParameter* CMotionParameterController::GetCurrentMotionParameter(void) const
{
    return m_pCurrentMotionParameter;
};


CCharacterParameter& CMotionParameterController::CharacterParam(void)
{
    ASSERT(m_pCharacterParameter);

    return *m_pCharacterParameter;
};


CMotionParameter& CMotionParameterController::CurrentMotionParam(void)
{
    ASSERT(m_pCurrentMotionParameter);
    
    return *m_pCurrentMotionParameter;
};


CCharacterParameter& CMotionParameterController::CharacterParam(void) const
{
    ASSERT(m_pCharacterParameter);

    return *m_pCharacterParameter;
};


CMotionParameter& CMotionParameterController::CurrentMotionParam(void) const
{
    ASSERT(m_pCurrentMotionParameter);

    return *m_pCurrentMotionParameter;
};