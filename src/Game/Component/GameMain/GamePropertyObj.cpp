#include "GamePropertyObj.hpp"
#include "GamePlayerContainer.hpp"
#include "GameEnemyContainer.hpp"
#include "GameGimmickContainer.hpp"
#include "GameRadar.hpp"
#include "GameEnemy.hpp"
#include "GameGimmick.hpp"
#include "GamePlayer.hpp"
#include "StageInfo.hpp"

#include "Game/Component/Gimmick/GimmickUtils.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Sequence/Ride/RideGamePlayerContainer.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "System/Common/Screen.hpp"


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
    UpdateTime();
    UpdateActiveObject();
    WatchPlayerFallen();
    CGameRadar::Update(m_pEnemyContainer, m_pMapCamera);
};


void CGamePropertyObject::UpdateTime(void)
{
    m_fTotalElapsedTime += CScreen::TimerStride();
};


void CGamePropertyObject::UpdateActiveObject(void)
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
        if (!GameGimmick.IsAlive() || !GameGimmick.IsAbleToSleep())
            continue;

        switch (m_pMapCamera->GetCameraMode())
        {
        case CMapCamera::MODE_INTRODUCTION:
            {
                RwV3d vPosition = Math::VECTOR3_ZERO;
                GameGimmick.GetPosition(&vPosition);

                if (CGimmickUtils::IsPositionVisible(&vPosition))
                {
                    if (GameGimmick.IsSleep())
                        GameGimmick.Resume();
                }
                else
                {
                    if (!GameGimmick.IsSleep())
                        GameGimmick.Sleep();
                };
            }
            break;

        default:
            {
                RwV3d vPosition = Math::VECTOR3_ZERO;
                GameGimmick.GetPosition(&vPosition);

                if (CGimmickUtils::CalcNearestPlayerDistanceXZ(&vPosition) <= m_fSleepDistance)
                {
                    if (GameGimmick.IsSleep())
                        GameGimmick.Resume();
                }
                else
                {
                    if (!GameGimmick.IsSleep())
                        GameGimmick.Sleep();
                };
            }
            break;
        };
    };
};


void CGamePropertyObject::WatchPlayerFallen(void)
{
    for (int32 i = 0; i < m_pPlayerContainer->GetPlayerNum(); ++i)
    {
        IGamePlayer& GamePlayer = PlayerContainer().GamePlayer(i);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        GamePlayer.GetPosition(&vPosition);

        if (vPosition.y <= m_fDeathHeight)
            GamePlayer.InvokeDeathFloor();
    };
};


IGamePlayerContainer& CGamePropertyObject::PlayerContainer(void) const
{
    ASSERT(m_pPlayerContainer);
    return *m_pPlayerContainer;
};


CGameEnemyContainer& CGamePropertyObject::EnemyContainer(void) const
{
    ASSERT(m_pEnemyContainer);
    return *m_pEnemyContainer;
};


CGameGimmickContainer& CGamePropertyObject::GimmickContainer(void) const
{
    ASSERT(m_pGimmickContainer);
    return *m_pGimmickContainer;
};


float CGamePropertyObject::GetTotalElapsedTime(void) const
{
    return m_fTotalElapsedTime;
};


float CGamePropertyObject::GetElapsedTime(void) const
{
    return CScreen::TimerStride();
};


float CGamePropertyObject::GetCleartime(void) const
{
    return m_fCleartime;
};


float CGamePropertyObject::GetSleepDistance(void) const
{
    return m_fSleepDistance;
};


float CGamePropertyObject::GetDeathHeight(void) const
{
    return m_fDeathHeight;
};


float CGamePropertyObject::GetGravity(void) const
{
    return m_fGravity;
};


RwCamera* CGamePropertyObject::GetRwCamera(void) const
{
    ASSERT(m_pRwCamera);
    return m_pRwCamera;
};


CMapCamera* CGamePropertyObject::GetMapCamera(void) const
{
    ASSERT(m_pMapCamera);
    return m_pMapCamera;
};


void CGamePropertyObject::SetRwCamera(RwCamera* pRwCamera)
{
    ASSERT(pRwCamera);
    m_pRwCamera = pRwCamera;
};


void CGamePropertyObject::SetMapCamera(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);
    m_pMapCamera = pMapCamera;
};


void CGamePropertyObject::GetCameraViewMatrix(RwMatrix* pMatrix)
{
    //
    //  Returns camera view matrix
    //
    
    ASSERT(pMatrix);
    ASSERT(m_pRwCamera);

    std::memcpy(
        pMatrix,
        RwCameraGetViewMatrixMacro(m_pRwCamera),
        sizeof(RwMatrix)
    );
};


void CGamePropertyObject::GetCameraFrameMatrix(RwMatrix* pMatrix)
{
    //
    //  Returns camera modeling matrix
    //
    
    ASSERT(pMatrix);
    ASSERT(m_pRwCamera);

    std::memcpy(
        pMatrix,
        RwFrameGetMatrixMacro(RwCameraGetFrameMacro(m_pRwCamera)),
        sizeof(RwMatrix)
    );
};