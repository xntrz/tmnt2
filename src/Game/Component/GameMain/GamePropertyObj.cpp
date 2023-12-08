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

    ASSERT(m_pPlayerContainer);
    ASSERT(m_pEnemyContainer);
    ASSERT(m_pGimmickContainer);
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
    for (int32 i = 0; i < m_pEnemyContainer->GameEnemyMax(); ++i)
    {
        CGameEnemy& GameEnemy = m_pEnemyContainer->GameEnemy(i);

        if (!GameEnemy.IsAlive())
            continue;

        RwV3d vPosition = Math::VECTOR3_ZERO;
        GameEnemy.GetPosition(&vPosition);

        if (vPosition.y <= m_fDeathHeight)
        {
            if (CGimmickUtils::CalcNearestPlayerDistanceXZ(&vPosition) <= m_fSleepDistance)
            {
                if (!GameEnemy.IsActive())
                    GameEnemy.Activate();
            }
            else
            {
                if(GameEnemy.IsActive())
                    GameEnemy.Inactivate();
            };
        }
        else
        {
            GameEnemy.InvokeDeathFloor();
        };
    };

    //
    //  Update game gimmicks
    //
    for (int32 i = 0; i < m_pGimmickContainer->GameGimmickMax(); ++i)
    {
        CGameGimmick& GameGimmick = m_pGimmickContainer->GameGimmick(i);
		if (!GameGimmick.IsAlive())
			continue;

		if (!GameGimmick.IsAbleToSleep())
			continue;

		RwV3d vPosition = Math::VECTOR3_ZERO;
		GameGimmick.GetPosition(&vPosition);

		if ((m_pMapCamera->GetCameraMode() == CMapCamera::MODE_INTRODUCTION) ||
			(CGimmickUtils::CalcNearestPlayerDistanceXZ(&vPosition) <= m_fSleepDistance))
		{
			if (GameGimmick.IsSleep())
				GameGimmick.Resume();
		}
		else
		{
			if (!GameGimmick.IsSleep())
				GameGimmick.Sleep();
		};
    };
};


void CGamePropertyObject::watchPlayerFallen(void)
{
    for (int32 i = 0; i < m_pPlayerContainer->GetPlayerNum(); ++i)
    {
        IGamePlayer* pGamePlayer = PlayerContainer().GamePlayer(i);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        pGamePlayer->GetPosition(&vPosition);

		if (vPosition.y <= m_fDeathHeight)
			pGamePlayer->InvokeDeathFloor();
    };
};
