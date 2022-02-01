#include "HitSphereParameter.hpp"


CHitSphereParameter::CHitSphereParameter(INIT_PARAMETER* pParam)
: m_nBoneID(0)
, m_sphere({ { Math::VECTOR3_ZERO }, 0.0f })
{
    Initialize(pParam);
};


CHitSphereParameter::~CHitSphereParameter(void)
{
    ;
};


void CHitSphereParameter::Initialize(INIT_PARAMETER* pParam)
{
    m_nBoneID = pParam->m_nBoneID;
    m_sphere = pParam->m_sphere;
};


int32 CHitSphereParameter::GetBoneID(void) const
{
    return m_nBoneID;
};


const RwSphere* CHitSphereParameter::GetSphere(void) const
{
    return &m_sphere;
};