#include "GameLoader.hpp"

#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"

#include "LoadInfo/PlayerLoadInfo.hpp"
#include "LoadInfo/RidePlayerLoadInfo.hpp"
#include "LoadInfo/StageLoadInfo.hpp"
#include "LoadInfo/EnbuLoadInfo.hpp"
#include "LoadInfo/EnemyLoadInfo.hpp"
#include "LoadInfo/EffectLoadInfo.hpp"
#include "LoadInfo/MapLoadInfo.hpp"


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
        loadObjectRecursive(rInfo.GetDependObjInfo(i), nDepth + 1);

    FNAME fileID = rInfo.GetFileID();
    
    if (!FNAME_EQUAL(fileID, FNAME_INVALID))
        CDataLoader::Regist(fileID);
};


/*static*/ void CGameLoader::SetMode(MODE mode)
{
    Instance().m_mode = mode;
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


/*static*/ void CGameLoader::LoadEnemy(ENEMYID::VALUE idEnemy)
{
    int32 i = 0;
    EFFECTID::VALUE idEffect = CEnemy::GetNeededEffect(idEnemy, i);
    while (idEffect != EFFECTID::ID_UNKNOWN)
    {
        LoadEffect(idEffect);
        idEffect = CEnemy::GetNeededEffect(idEnemy, ++i);
    };

    CEnemyLoadInfo info(idEnemy);
    Instance().loadObjectRecursive(info);
};


/*static*/ void CGameLoader::LoadEffect(EFFECTID::VALUE idEffect)
{
    CEffectLoadInfo info(idEffect);
    Instance().loadObjectRecursive(info);
};


/*static*/ void CGameLoader::LoadMap(MAPID::VALUE idMap)
{
    CMapLoadInfo info(idMap);
    Instance().loadObjectRecursive(info);
};