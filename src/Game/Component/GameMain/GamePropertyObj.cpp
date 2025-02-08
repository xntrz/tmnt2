#include "GamePropertyObj.hpp"
#include "GamePlayerContainer.hpp"
#include "GameEnemyContainer.hpp"
#include "GameGimmickContainer.hpp"
#include "GameRadar.hpp"
#include "GameEnemy.hpp"
#include "GameGimmick.hpp"
#include "GamePlayer.hpp"
#include "StageInfo.hpp"

#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Sequence/Ride/RideGamePlayerContainer.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Map/WorldMap.hpp"


/*static*/ const float CGamePropertyObject::DEFAULT_GRIVITY = -19.6f;


CGamePropertyObject::CGamePropertyObject(void)
: m_fTotalElapsedTime(0.0f)
, m_fCleartime(0.0f)
, m_fSleepDistance(0.0f)
, m_fDeathHeight(0.0f)
, m_fGravity(DEFAULT_GRIVITY)
, m_pRwCamera(nullptr)
, m_pMapCamera(nullptr)
, m_pPlayerContainer(nullptr)
, m_pEnemyContainer(nullptr)
, m_pGimmickContainer(nullptr)
{
    m_fSleepDistance = CStageInfo::GetObjectSleepDistance(CGameData::PlayParam().GetStage());
    m_fDeathHeight = CWorldMap::GetCharacterDeathHeight();
    
    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
        m_pPlayerContainer = new CRideGamePlayerContainer;
    else
        m_pPlayerContainer = new CGamePlayerContainer;

    m_pEnemyContainer = new CGameEnemyContainer;
    m_pGimmickContainer = new CGameGimmickContainer;
};


CGamePropertyObject::~CGamePropertyObject(void)
{
    if (m_pGimmickContainer)
    {
        delete m_pGimmickContainer;
        m_pGimmickContainer = nullptr;
    };

    if (m_pEnemyContainer)
    {
        delete m_pEnemyContainer;
        m_pEnemyContainer = nullptr;
    };

    if (m_pPlayerContainer)
    {
        delete m_pPlayerContainer;
        m_pPlayerContainer = nullptr;
    };
};


void CGamePropertyObject::Period(void)
{
    updateTime();
    updateActiveObject();
    watchPlayerFallen();
};


void CGamePropertyObject::updateTime(void)
{
    m_fTotalElapsedTime += CScreen::TimerStride();
};


void CGamePropertyObject::updateActiveObject(void)
{
    //
    //  Update game enemies
    //
    int32 enemyMax = m_pEnemyContainer->GameEnemyMax();
    for (int32 i = 0; i < enemyMax; ++i)
    {
        CGameEnemy& gameEnemy = m_pEnemyContainer->GameEnemy(i);

        if (!gameEnemy.IsAlive())
            continue;

        RwV3d vPosition = Math::VECTOR3_ZERO;
        gameEnemy.GetPosition(&vPosition);

        if (vPosition.y >= m_fDeathHeight)
        {
            if (CGimmickUtils::CalcNearestPlayerDistanceXZ(&vPosition) <= m_fSleepDistance)
            {
                if (!gameEnemy.IsActive())
                    gameEnemy.Activate();
            }
            else
            {
				if (gameEnemy.IsActive())
					gameEnemy.Inactivate();
            };
        }
        else
        {
            gameEnemy.InvokeDeathFloor();
        };
    };

    //
    //  Update game gimmicks
    //
    int32 gimmickMax = m_pGimmickContainer->GameGimmickMax();
    for (int32 i = 0; i < gimmickMax; ++i)
    {
        CGameGimmick& gameGimmick = m_pGimmickContainer->GameGimmick(i);
		if (!gameGimmick.IsAlive())
			continue;

		if (!gameGimmick.IsAbleToSleep())
			continue;

		RwV3d vPosition = Math::VECTOR3_ZERO;
		gameGimmick.GetPosition(&vPosition);

		if ((m_pMapCamera->GetCameraMode() == CMapCamera::MODE_INTRODUCTION) ||
			(CGimmickUtils::CalcNearestPlayerDistanceXZ(&vPosition) <= m_fSleepDistance))
		{
			if (gameGimmick.IsSleep())
				gameGimmick.Resume();
		}
		else
		{
			if (!gameGimmick.IsSleep())
				gameGimmick.Sleep();
		};
    };
};


void CGamePropertyObject::watchPlayerFallen(void)
{
    int32 playerNum = m_pPlayerContainer->GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        IGamePlayer* pGamePlayer = PlayerContainer().GamePlayer(i);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        pGamePlayer->GetPosition(&vPosition);

		if (vPosition.y <= m_fDeathHeight)
			pGamePlayer->InvokeDeathFloor();
    };
};
