#include "AttackParameter.hpp"

#include "Game/System/Hit/HitAttackData.hpp"


CAttackParameter::CAttackParameter(INIT_PARAMETER* pParam)
: m_no(0)
, m_nBoneID(0)
, m_fStart(0.0f)
, m_fEnd(0.0f)
, m_nPower(0)
, m_nStatus(0)
, m_fStatusVal1(0.0f)
, m_fStatusVal2(0.0f)
, m_bConfusion(false)
, m_bGuardImpact(false)
, m_bSlice(false)
, m_bReflectShot(false)
, m_nAntiguard(0)
, m_uTarget(0)
{
    Initialize(pParam);
};


CAttackParameter::~CAttackParameter(void)
{
    ;
};


void CAttackParameter::Initialize(INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    m_no        = pParam->m_no;
    m_nBoneID   = pParam->m_nBoneID;
    m_fStart    = pParam->m_fStart;
    m_fEnd      = pParam->m_fEnd;
    m_sphere    = pParam->m_sphere;
};


void CAttackParameter::Attach(ATTACH_PARAMETER* pParam)
{
    ASSERT(pParam);

    m_no            = pParam->m_nNo;
    m_nPower        = pParam->m_nPower;
    m_nStatus       = pParam->m_nStatus;
    m_fStatusVal1   = pParam->m_fStatusVal1;
    m_fStatusVal2   = pParam->m_fStatusVal2;
    m_bConfusion    = pParam->m_bConfusion;
    m_bGuardImpact  = pParam->m_bGuardImpact;
    m_bSlice        = pParam->m_bSlice;
    m_bReflectShot  = pParam->m_bReflectShot;
    m_nAntiguard    = pParam->m_nAntiguard;
    m_uTarget       = pParam->m_uTarget;
};


bool CAttackParameter::IsAttackEnableTime(float fNow) const
{
    const float PRECISION = 1.0f / 8192.0f;
    
    return (fNow >= (m_fStart - PRECISION)) &&
           (fNow <= (m_fEnd   + PRECISION));
};


void CAttackParameter::GetParameter(CHitAttackData* pHitAttackData) const
{
    ASSERT(pHitAttackData);
    
    pHitAttackData->SetAttackNo(m_no);
    pHitAttackData->SetShape(CHitAttackData::SHAPE_SPHERE);
    pHitAttackData->SetSphere(&m_sphere);
    pHitAttackData->SetPower(m_nPower);
    pHitAttackData->SetStatus(CHitAttackData::STATUS(m_nStatus));
    pHitAttackData->SetStatusParameter(m_fStatusVal1, m_fStatusVal2);
    pHitAttackData->SetFlagConfusion(m_bConfusion);
    pHitAttackData->SetFlagGuardImpact(m_bGuardImpact);
    pHitAttackData->SetFlagSlice(m_bSlice);
    pHitAttackData->SetFlagReflectShot(m_bReflectShot);
    pHitAttackData->SetAntiguard(CHitAttackData::ANTIGUARD(m_nAntiguard));
    pHitAttackData->SetTarget(CHitAttackData::TARGET(m_uTarget));

    if (pHitAttackData->GetStatus() == CHitAttackData::STATUS_STUN)
    {
        pHitAttackData->SetEffect(CHitAttackData::EFFECT_STUN);
    }
    else if (pHitAttackData->GetStatus() == CHitAttackData::STATUS_FLYAWAY)
    {
        pHitAttackData->SetEffect(CHitAttackData::EFFECT_LARGE);
    }
    else
    {
        pHitAttackData->SetEffect(CHitAttackData::EFFECT_SMALL);
    };
};


const RwSphere* CAttackParameter::GetSphere(void) const
{
    return &m_sphere;
};


int32 CAttackParameter::GetBoneID(void) const
{
    return m_nBoneID;
};