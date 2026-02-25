#include "EnemyLoadInfo.hpp"


bool CEnemyCommonLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    std::sprintf(pszFilenameBuff, "Common/Enemy/EnemyCommon.list");
    return true;
};


//
// *********************************************************************************
//


CEnemyLoadInfo::CEnemyLoadInfo(ENEMYID::VALUE enemyID)
: m_enemyID(enemyID)
{
    ;
};


bool CEnemyLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    if (ENEMYID::GetImplementProgress(m_enemyID))
    {
        const char* pszPath = "Common/Enemy/";
        const char* pszExtName = ENEMYID::GetExtName(m_enemyID);

        std::sprintf(pszFilenameBuff, "%s%s.list", pszPath, pszExtName);

         return true;
    };

    return false;
};