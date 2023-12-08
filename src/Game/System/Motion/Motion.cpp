#include "Motion.hpp"


CMotion::CMotion(const char* pszName, RtAnimAnimation* pHAnimation)
: m_pszName(pszName)
, m_pHAnimation(pHAnimation)
{
    ASSERT(m_pszName);
    ASSERT(m_pHAnimation);
};