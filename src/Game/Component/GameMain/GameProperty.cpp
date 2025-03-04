#include "GameProperty.hpp"
#include "GamePropertyObj.hpp"
#include "GameEvent.hpp"
#include "GamePlayerContainer.hpp"
#include "GameEnemyContainer.hpp"
#include "GameGimmickContainer.hpp"
#include "GamePlayer.hpp"
#include "GameEnemy.hpp"
#include "GameGimmick.hpp"

#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/Screen.hpp"


static CGamePropertyObject* s_pGamePropertyObject = nullptr;


static inline CGamePropertyObject& GamePropertyObject(void)
{
    ASSERT(s_pGamePropertyObject);
    return *s_pGamePropertyObject;
};


/*static*/ void CGameProperty::Initialize(void)
{
    if (!s_pGamePropertyObject)
    {
        s_pGamePropertyObject = new CGamePropertyObject;

        CGameEvent::AttachProperty(s_pGamePropertyObject);
    };
};


/*static*/ void CGameProperty::Terminate(void)
{
    if (s_pGamePropertyObject)
    {
        CGameEvent::DetachProperty();
        
        delete s_pGamePropertyObject;
        s_pGamePropertyObject = nullptr;
    };
};


/*static*/ void CGameProperty::Period(void)
{
    GamePropertyObject().Period();
};


/*static*/ GAMETYPES::DIFFICULTY CGameProperty::GetDifficulty(void)
{
    if (CGameData::Attribute().IsNexusMode())
        return GAMETYPES::DIFFICULTY_NORMAL;
    
    return CGameData::Option().Play().GetDifficulty();
};


/*static*/ float CGameProperty::GetElapsedTime(void)
{
    return GamePropertyObject().GetElapsedTime();
};


/*static*/ float CGameProperty::GetTotalElapsedTime(void)
{
    return GamePropertyObject().GetTotalElapsedTime();
};


/*static*/ float CGameProperty::GetGravity(void)
{
    return GamePropertyObject().GetGravity();
};


/*static*/ void CGameProperty::SetCurrentRwCamera(RwCamera* pRwCamera)
{
    ASSERT(pRwCamera);
    GamePropertyObject().SetRwCamera(pRwCamera);
};


/*static*/ void CGameProperty::SetMapCamera(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);
    GamePropertyObject().SetMapCamera(pMapCamera);
};


/*static*/ CMapCamera* CGameProperty::GetMapCamera(void)
{
    return GamePropertyObject().GetMapCamera();
};


/*static*/ void CGameProperty::GetCameraViewMatrix(RwMatrix* pMatrix)
{
    GamePropertyObject().GetCameraViewMatrix(pMatrix);
};


/*static*/ void CGameProperty::GetCameraFrameMatrix(RwMatrix* pMatrix)
{
    GamePropertyObject().GetCameraFrameMatrix(pMatrix);
};


/*static*/ bool CGameProperty::AdjustWithinMoveBoundary(RwV3d* pvOut, RwV3d* pvIn)
{
    ASSERT(pvOut);
    ASSERT(pvIn);
    
    CMapCamera* pMapCamera = GetMapCamera();
    if ((!pMapCamera) || (GetPlayerNum() <= 1))
    {
        *pvOut = *pvIn;
        return false;
    };

    float fMoveBoundaryRadius = pMapCamera->GetViewAreaRadius();
    if (fMoveBoundaryRadius < 3.0f)
        fMoveBoundaryRadius = 3.0f;
    
    RwV3d vAt = Math::VECTOR3_ZERO;
    pMapCamera->GetLookat(&vAt);

    RwV3d vVec = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vVec, pvIn, &vAt);
    vVec.y = 0.0f;

    float fLen = Math::Vec3_Length(&vVec);
    fLen = std::fabs(fLen);

    if (fMoveBoundaryRadius >= fLen)
    {
        if (pvOut != pvIn)
            *pvOut = *pvIn;
        
        return false;
    };

    Math::Vec3_Normalize(&vVec, &vVec);
    Math::Vec3_Scale(&vVec, &vVec, fMoveBoundaryRadius);
    Math::Vec3_Add(&vVec, &vVec, &vAt);

    pvOut->x = vVec.x;
    pvOut->y = pvIn->y;
    pvOut->z = vVec.z;

    float fMapHeight = CWorldMap::GetMapHeight(pvOut);
    if (fMapHeight > pvOut->y)
        pvOut->y = fMapHeight;

    return true;
};


/*static*/ void CGameProperty::SetCameraVibration(float fPower, float fTime, int32 nFreq)
{
    CMapCamera* pMapCamera = GamePropertyObject().GetMapCamera();
    ASSERT(pMapCamera);

    pMapCamera->SetCameraVibration(fPower, fTime, nFreq);
};


/*static*/ void CGameProperty::StopCameraVibration(void)
{
    CMapCamera* pMapCamera = GamePropertyObject().GetMapCamera();
    ASSERT(pMapCamera);

    pMapCamera->StopCameraVibration();
};


/*static*/ void CGameProperty::AddPlayerCharacter(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    GamePropertyObject().PlayerContainer().AddPlayer(nPlayerNo, idPlayer, costume);
};


/*static*/ IGamePlayer* CGameProperty::Player(int32 no)
{
    return GamePropertyObject().PlayerContainer().GamePlayer(no);
};


/*static*/ IGamePlayer* CGameProperty::GetGamePlayer(int32 no)
{
    IGamePlayer* pGamePlayer = GamePropertyObject().PlayerContainer().GamePlayer(no);
    pGamePlayer->AddRef();
    return pGamePlayer;
};


/*static*/ int32 CGameProperty::GetPlayerNum(void)
{
    return GamePropertyObject().PlayerContainer().GetPlayerNum();
};


/*static*/ CEnemy* CGameProperty::GetEnemy(int32 no)
{
    CGameEnemy& GameEnemy = GamePropertyObject().EnemyContainer().GameEnemy(no);
    return (GameEnemy.IsAlive() ? GameEnemy.GetEnemy() : nullptr);
};


/*static*/ int32 CGameProperty::GetEnemyMax(void)
{
    return GAMETYPES::ENEMY_MAX;
};


/*static*/ CGimmick* CGameProperty::GetGimmick(int32 no)
{
    CGameGimmick& GameGimmick = GamePropertyObject().GimmickContainer().GameGimmick(no);
    return (GameGimmick.IsAlive() ? GameGimmick.GetGimmick() : nullptr);
};


/*static*/ int32 CGameProperty::GetGimmickMax(void)
{
    return GAMETYPES::GIMMICK_MAX;
};


/*static*/ CGameGimmick* CGameProperty::GetGameGimmick(int32 no)
{
    return &GamePropertyObject().GimmickContainer().GameGimmick(no);
};


/*static*/ int32 CGameProperty::GetGameGimmickNum(void)
{
    return GamePropertyObject().GimmickContainer().GameGimmickMax();
};


/*static*/ CGameEnemy* CGameProperty::GetGameEnemy(int32 no)
{
    return &GamePropertyObject().EnemyContainer().GameEnemy(no);
};


/*static*/ int32 CGameProperty::GetGameEnemyNum(void)
{
    return GamePropertyObject().EnemyContainer().GameEnemyMax();
};