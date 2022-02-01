#include "StageLoadInfo.hpp"

#include "Game/Component/GameMain/StageInfo.hpp"


CStageLoadInfo::CStageLoadInfo(STAGEID::VALUE idStage)
: m_idStage(idStage)
{
    ;
};


CStageLoadInfo::~CStageLoadInfo(void)
{
    ;
};


int32 CStageLoadInfo::GetFileID(void) const
{
    return CStageInfo::GetFileID(m_idStage);
};


bool CStageLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    ASSERT(false);

    return false;
};