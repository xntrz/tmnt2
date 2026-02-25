#include "StageLoadInfo.hpp"

#include "Game/Component/GameMain/StageInfo.hpp"


CStageCommonLoadInfo::CStageCommonLoadInfo(GAMETYPES::STAGEMODE stagemode)
: m_stagemode(stagemode)
{
    ;
};


CStageCommonLoadInfo::~CStageCommonLoadInfo(void)
{
    ;
};


FNAME CStageCommonLoadInfo::GetFileID(void) const
{
    FNAME fileID = FPATH("common/stage/stcommon.lpac");

    switch (m_stagemode)
    {
    case GAMETYPES::STAGEMODE_HOME:
        fileID = FPATH("common/stage/stcommon_h.lpac");
        break;

    case GAMETYPES::STAGEMODE_RIDE:
        fileID = FPATH("common/stage/stcommon_r.lpac");
        break;

    case GAMETYPES::STAGEMODE_NEXUS:
        fileID = FPATH("common/stage/stcommon_n.lpac");
        break;
    };

    return fileID;
};


bool CStageCommonLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    ASSERT(false);
    return false;
};


//
// *********************************************************************************
//


CStageGimmickSetDataLoadInfo::CStageGimmickSetDataLoadInfo(STAGEID::VALUE idStage)
: m_idStage(idStage)
{
    ;
};


bool CStageGimmickSetDataLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    const char* pszPathMGD = "common/stage/mgd/";
    const char* pszPathPList = "common/stage/plist/";

    return false;
};


//
// *********************************************************************************
//


CStageLoadInfo::CStageLoadInfo(STAGEID::VALUE idStage)
: m_idStage(idStage)
{
    ;
};


CStageLoadInfo::~CStageLoadInfo(void)
{
    ;
};


FNAME CStageLoadInfo::GetFileID(void) const
{
    return CStageInfo::GetFileID(m_idStage);
};


bool CStageLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    ASSERT(false);
    return false;
};