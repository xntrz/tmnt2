#include "StageCommonLoadInfo.hpp"

#include "System/Common/File/AfsFileID.hpp"


CStageCommonLoadInfo::CStageCommonLoadInfo(GAMETYPES::STAGEMODE stagemode)
: m_stagemode(stagemode)
{
    ;
};


CStageCommonLoadInfo::~CStageCommonLoadInfo(void)
{
    ;
};


int32 CStageCommonLoadInfo::GetFileID(void) const
{
    int32 nFileID = AFSFILEID::ID_STCOMMON;

    switch (m_stagemode)
    {
    case GAMETYPES::STAGEMODE_HOME:
        nFileID = AFSFILEID::ID_STCOMMON_H;
        break;

    case GAMETYPES::STAGEMODE_RIDE:
        nFileID = AFSFILEID::ID_STCOMMON_R;
        break;

    case GAMETYPES::STAGEMODE_NEXUS:
        nFileID = AFSFILEID::ID_STCOMMON_N;
        break;
    };

    return nFileID;
};


bool CStageCommonLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    ASSERT(false);
    
    return false;
};