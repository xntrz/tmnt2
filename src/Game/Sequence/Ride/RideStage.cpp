#include "RideStage.hpp"
#include "RideCharacter.hpp"
#include "RideGamePlayer.hpp"
#include "Ride2D.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/RideFlagGimmick.hpp"
#include "Game/Component/Effect/ScreenEffectAccel.hpp"
#include "Game/Component/Effect/Snow.hpp"
#include "Game/Component/Shot/Shot.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/CameraDataMan.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/MessageManager.hpp"


static float s_fMoveLimitZBack      = 0.0f;
static float s_fMoveLimitZFront     = 0.0f;
static float s_fMoveLimitXBack      = -6.0f;
static float s_fMoveLimitXFront     = 6.0f;
static float s_fMoveLimitYBottom    = -15.5f;
static float s_fMoveLimitYTop       = 4.5f;


class CRideStageMessage
{
public:
    CRideStageMessage(void);
    void Period(void);
    
private:
    int32 m_step;
};


class CRideStageContainer
{
public:
    CRideStageContainer(void);
	void OnLoaded(void);
    void Period(void);
    void Draw(void);
    void MoveBasisPoint(void);
    void InitShadowFlag(void);
    void InitBasisPoint(void);
    bool IsEnd(void) const;
    void GetBasisPoint(RwV3d* pvBasisPoint) const;
    void GetBasisVector(RwV3d* pvBasisVector) const;
    bool GetShadowFlag(void) const;
    float GetTime(void) const;

private:
    RwV3d m_vBasisPoint;
    RwV3d m_vBasisVector;
    float m_fTime;
    bool m_bShadowFlag;
    CRideStageMessage m_message;
};


CRideStageMessage::CRideStageMessage(void)
: m_step(0)
{
    ;
};


void CRideStageMessage::Period(void)
{
    switch (m_step)
    {
    case 0:
        {
            if (CRideStage::GetTime() > 1.0f)
            {
                m_step = 1;
                
                if (CRideStage::m_bSpace)
                    CMessageManager::Request(SEGROUPID::VALUE(160), PLAYERID::VALUE(-1));
                else
                    CMessageManager::Request(SEGROUPID::VALUE(159), PLAYERID::VALUE(-1));
            };
        }
        break;

    case 1:
        {
            if (CRideStage::GetTime() > 1.0f)
            {
                m_step = 2;
                CMessageManager::Request(SEGROUPID::VALUE(158), PLAYERID::VALUE(-1));                
            };
        }
        break;
    };
};


CRideStageContainer::CRideStageContainer(void)
: m_vBasisPoint(Math::VECTOR3_ZERO)
, m_vBasisVector(Math::VECTOR3_ZERO)
, m_fTime(0.0f)
, m_bShadowFlag(false)
{
    InitShadowFlag();
};


void CRideStageContainer::OnLoaded(void)
{
    InitBasisPoint();
};


void CRideStageContainer::Period(void)
{
    m_fTime += CGameProperty::GetElapsedTime();
    MoveBasisPoint();
    m_message.Period();

    if (IsEnd())
    {
        CGameStage* pStage = CGameStage::GetCurrent();
        ASSERT(pStage);
        pStage->NotifyGameClear(CGamePlayResult::CLEARSUB_A);
    };
};


void CRideStageContainer::Draw(void)
{
    ;
};


void CRideStageContainer::MoveBasisPoint(void)
{
    float dt = CGameProperty::GetElapsedTime();
    
    m_vBasisVector.z = 14.7222f;

    m_vBasisPoint.x += (m_vBasisVector.x * dt);
    m_vBasisPoint.y += (m_vBasisVector.y * dt);
    m_vBasisPoint.z += (m_vBasisVector.z * dt);
};


void CRideStageContainer::InitShadowFlag(void)
{
    m_bShadowFlag = true;
    
    if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST32R)
        m_bShadowFlag = false;
};


void CRideStageContainer::InitBasisPoint(void)
{
    int32 nPathID = CCameraDataManager::GetPathIDFromName("p0_t");
    ASSERT(nPathID != -1);
    CCameraDataManager::GetSplinePos(&m_vBasisPoint, nPathID, 0.0f);
};


bool CRideStageContainer::IsEnd(void) const
{
    float fExtend = 0.0f;

    if (CGameData::PlayParam().GetArea() == AREAID::ID_AREA38)
        fExtend = 20.0f;

    return (CRideStage::GetMoveLimitZMin() > (fExtend + 2500.0f));
};


void CRideStageContainer::GetBasisPoint(RwV3d* pvBasisPoint) const
{
    ASSERT(pvBasisPoint);
    *pvBasisPoint = m_vBasisPoint;
};


void CRideStageContainer::GetBasisVector(RwV3d* pvBasisVector) const
{
    ASSERT(pvBasisVector);
    *pvBasisVector = m_vBasisVector;
};


bool CRideStageContainer::GetShadowFlag(void) const
{
    return m_bShadowFlag;
};


float CRideStageContainer::GetTime(void) const
{
    return m_fTime;
};


static CRideStageContainer* s_pRideStageContainer = nullptr;


static inline CRideStageContainer& RideStageContainer(void)
{
    ASSERT(s_pRideStageContainer);
    return *s_pRideStageContainer;
};


/*static*/ bool CRideStage::m_bSpace = false;


/*static*/ void CRideStage::Initialize(void)
{
    if (!s_pRideStageContainer)
    {
        s_pRideStageContainer = new CRideStageContainer;
    };

	SetMoveLimit();
    CRideFlagGimmick::Initialize();
};


/*static*/ void CRideStage::Terminate(void)
{
    if (s_pRideStageContainer)
    {
        delete s_pRideStageContainer;
        s_pRideStageContainer = nullptr;
    };
};


/*static*/ void CRideStage::Period(void)
{
    SetMoveLimit();
    RideStageContainer().Period();

    const CWorldMap::MAPINFO* pMapInfo = CWorldMap::GetMapInfo();
    ASSERT(pMapInfo);

    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    ASSERT(pMapCamera);

    if (pMapInfo->m_weather == CWorldMap::WEATHER_SNOW)
        CSnowManager::SetCameraPosition(pMapCamera);
    else
        CScreenEffectAccel::SetCenter(pMapCamera);
};


/*static*/ void CRideStage::Draw(void)
{
    RideStageContainer().Draw();
};


/*static*/ void CRideStage::OnLoaded(void)
{
	RideStageContainer().OnLoaded();

    const CWorldMap::MAPINFO* pMapInfo = CWorldMap::GetMapInfo();
    ASSERT(pMapInfo);

    if (pMapInfo->m_weather == CWorldMap::WEATHER_SNOW)
        CSnowManager::SetRideSnow();
    else
        CScreenEffectAccel::StartAccel();
};


/*static*/ void CRideStage::GetBasisPoint(RwV3d* pvBasisPoint)
{
    RideStageContainer().GetBasisPoint(pvBasisPoint);
};


/*static*/ void CRideStage::GetBasisVector(RwV3d* pvBasisVector)
{
    RideStageContainer().GetBasisVector(pvBasisVector);
};


/*static*/ int32 CRideStage::GetScore(RIDETYPES::SCOREKIND scorekind)
{
    int32 nScore = 0;

    for (int32 i = 0; i < CGameProperty::GetPlayerNum(); ++i)
    {
        IGamePlayer& GamePlayer = CGameProperty::Player(i);
        nScore += GamePlayer.GetScore(scorekind);
    };

    return nScore;
};


/*static*/ void CRideStage::AddScore(RIDETYPES::SCOREKIND scorekind, CHitCatchData* pHitCatchData)
{
    CRide2D::AddMedalAnimation(scorekind);
    
    CGameObject* pObject = pHitCatchData->GetObject();
    ASSERT(pObject);
    CGameObjectManager::SendMessage(pObject, RIDETYPES::MESSAGEID_SCORE_INC, (void*)scorekind);
};


/*static*/ void CRideStage::NotifyGimmickDestroyed(CHitAttackData* pHitAttackData)
{
    CGameObject* pObject = pHitAttackData->GetObject();
    if (pObject->GetType() == GAMEOBJECTTYPE::SHOT)
    {
        CShot* pShot = (CShot*)pObject;
        
        CGameObject* pParent = CGameObjectManager::GetObject(pShot->GetParentHandle());
        if (pParent)
            CGameObjectManager::SendMessage(pParent, RIDETYPES::MESSAGEID_SHOT_DESTROY);        
    };
};


/*static*/ float CRideStage::GetTime(void)
{
    return RideStageContainer().GetTime();
};


/*static*/ bool CRideStage::GetShadowFlag(void)
{
    return RideStageContainer().GetShadowFlag();
};


/*static*/ void CRideStage::UpdateResultData(void)
{
    for (int32 i = 0; i < CGameProperty::GetPlayerNum(); ++i)
    {
        IGamePlayer& GamePlayer = CGameProperty::Player(i);

        for (int32 i = 0; i < GamePlayer.GetCharacterNum(); ++i)
        {
            CRideCharacter* pRideChr = (CRideCharacter*)GamePlayer.GetCharacter(i);
            CGameData::PlayResult().SetRideCharaPlaySecond(pRideChr->GetID(), pRideChr->GetPlaytime());
        };
    };
};


/*static*/ void CRideStage::SetMoveLimit(void)
{
    RwV3d vBasisPoint = Math::VECTOR3_ZERO;
    GetBasisPoint(&vBasisPoint);

    s_fMoveLimitZFront = vBasisPoint.z - 2.0f;
    s_fMoveLimitZBack = vBasisPoint.z - 8.0f;
};


/*static*/ float CRideStage::GetMoveLimitZMax(void)
{
    return s_fMoveLimitZFront;
};


/*static*/ float CRideStage::GetMoveLimitZMin(void)
{
    return s_fMoveLimitZBack;
};


/*static*/ float CRideStage::GetMoveLimitXMax(void)
{
    return s_fMoveLimitXFront;
};


/*static*/ float CRideStage::GetMoveLimitXMin(void)
{
    return s_fMoveLimitXBack;
};


/*static*/ float CRideStage::GetMoveLimitYMax(void)
{
    return s_fMoveLimitYTop;
};


/*static*/ float CRideStage::GetMoveLimitYMin(void)
{
    return s_fMoveLimitYBottom;
};