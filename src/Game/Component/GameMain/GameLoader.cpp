#include "GameLoader.hpp"
#include "LoadInfo/PlayerLoadInfo.hpp"
#include "LoadInfo/RidePlayerLoadInfo.hpp"
#include "LoadInfo/StageCommonLoadInfo.hpp"
#include "LoadInfo/StageLoadInfo.hpp"
#include "LoadInfo/EnbuLoadInfo.hpp"

#include "System/Common/File/AfsFileID.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"


/*static*/ CGameLoader& CGameLoader::Instance(void)
{
    static CGameLoader s_TheGameloader;
    return s_TheGameloader;
};


CGameLoader::CGameLoader(void)
: m_mode(MODE_NORMAL)
, m_bInTheBlock(false)
{
    ;
};


CGameLoader::~CGameLoader(void)
{
    ;
};


void CGameLoader::loadObjectRecursive(CGameObjLoadInfo& rInfo, int32 nDepth)
{
    ASSERT(nDepth < LOAD_DEPTH_MAX);

    for (int32 i = 0; i < rInfo.GetDependObjInfoNum(); ++i)
        loadObjectRecursive(*rInfo.GetDependObjInfo(i), nDepth + 1);

    int32 nFileID = rInfo.GetFileID();

    ASSERT(nFileID != AFSFILEID::ID_INVALID);
    ASSERT(nFileID >= 0 && nFileID < AFSFILEID::ID_MAX);
    
    if (nFileID != AFSFILEID::ID_INVALID)
        CDataLoader::Regist(nFileID);
};


/*static*/ void CGameLoader::LoadStageCommonData(GAMETYPES::STAGEMODE stagemode)
{
    CStageCommonLoadInfo info(stagemode);
    Instance().loadObjectRecursive(info);
};


/*static*/ void CGameLoader::LoadStage(STAGEID::VALUE idStage)
{
    CStageLoadInfo info(idStage);
    Instance().loadObjectRecursive(info);
};


/*static*/ void CGameLoader::LoadPlayer(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    CPlayerLoadInfo info(idPlayer, costume);
    Instance().loadObjectRecursive(info);
};


/*static*/ void CGameLoader::LoadRidePlayer(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    CRidePlayerLoadInfo info(idPlayer, costume);
    Instance().loadObjectRecursive(info);
};


/*static*/ void CGameLoader::LoadEnbu(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    CEnbuLoadInfo info(idPlayer, costume);
    Instance().loadObjectRecursive(info);
};