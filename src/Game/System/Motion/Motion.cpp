#include "Motion.hpp"


CMotion::CMotion(const char* pszName, RtAnimAnimation* pHAnimation)
: m_pszName(pszName)
, m_pHAnimation(pHAnimation)
{
    ASSERT(m_pszName);
    ASSERT(m_pHAnimation);
};


CMotion::~CMotion(void)
{
    ;
};


const char* CMotion::GetName(void) const
{
    return m_pszName;
};


RtAnimAnimation* CMotion::GetAnimation(void) const
{
    return m_pHAnimation;
};


float CMotion::GetEndTime(void) const
{
    return m_pHAnimation->duration;
};

