#include "ChainMotionParameter.hpp"


CChainMotionParameter::CChainMotionParameter(INIT_PARAMETER* pParam)
: m_fStart(0.0f)
, m_fEnd(0.0f)
{
    for (int32 i = 0; i < COUNT_OF(m_aszMotionName); ++i)
        m_aszMotionName[i][0] = '\0';

    Initialize(pParam);
};


CChainMotionParameter::~CChainMotionParameter(void)
{
    ;
};


void CChainMotionParameter::Initialize(INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    m_fStart = pParam->m_fStart;
    m_fEnd = pParam->m_fEnd;
};


void CChainMotionParameter::Attach(ATTACH_PARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->m_nButton >= 0 && pParam->m_nButton < CHAIN_MAX);

    std::strcpy(m_aszMotionName[pParam->m_nButton], pParam->m_szMotionName);
};


float CChainMotionParameter::GetNextChainMotionConnectTime(const char* pszNextMotion)
{    
    if (pszNextMotion)
    {
        for (int32 i = 0; i < COUNT_OF(m_aszMotionName); ++i)
        {
            if (!std::strcmp(m_aszMotionName[i], pszNextMotion))
                return m_fEnd;
        };

        return 0.0f;
    };

    return m_fEnd;
};