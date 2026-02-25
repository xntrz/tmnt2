#include "EffectLoadInfo.hpp"


bool CEffectCommonLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    std::sprintf(pszFilenameBuff, "Common/Effect/List/EffectCommon.list");
    return true;
};


//
// *********************************************************************************
//


CEffectLoadInfo::CEffectLoadInfo(EFFECTID::VALUE effectID)
: m_effectID(effectID)
, m_szEffectName()
{
    m_szEffectName[0] = '\0';

    if (!EFFECTID::IsGeneric(effectID))
    {
        const char* pszEffectName = EFFECTID::GetNameFromID(m_effectID);
        std::strcpy(m_szEffectName, pszEffectName);
    };
};


bool CEffectLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    if (std::strlen(m_szEffectName) > 0)
    {
        const char* pszPath = "Common/Effect/List/individual/";
        std::sprintf(pszFilenameBuff, "%s%s_ef.list", pszPath, m_szEffectName);

        return true;
    };

    return false;
};