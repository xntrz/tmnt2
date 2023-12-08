#include "GameEvent.hpp"
#include "GameStage.hpp"
#include "GamePropertyObj.hpp"
#include "GameGimmickContainer.hpp"
#include "GamePlayerContainer.hpp"
#include "GameEnemyContainer.hpp"
#include "GameProperty.hpp"
#include "GamePlayer.hpp"

#include "Game/Component/Player/Player.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ CGamePropertyObject* CGameEvent::m_pProperty = nullptr;


/*static*/ CGamePropertyObject& CGameEvent::Property(void)
{
    ASSERT(m_pProperty);
    return *m_pProperty;
};


/*static*/ /*static*/ void CGameEvent::AttachProperty(CGamePropertyObject* pPropObj)
{
    ASSERT(!m_pProperty);
    
    m_pProperty = pPropObj;
};


/*static*/ void CGameEvent::DetachProperty(void)
{
    ASSERT(m_pProperty);
    
    m_pProperty = nullptr;
};


/*static*/ void CGameEvent::SetGameCleared(CGamePlayResult::CLEARSUB clearsub)
{
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage)
    {
        pStage->NotifyGameClear(clearsub);
    };
};


/*static*/ void CGameEvent::SetGameOvered(void)
{
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage)
    {
        pStage->NotifyGameOver();
    };
};


/*static*/ void CGameEvent::SetPlayerFallen(int32 nPlayerNo)
{
	IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(nPlayerNo);

	PLAYERID::VALUE idChr = pGamePlayer->GetCurrentCharacterID();
    CGameData::PlayResult().AddTechnicalAction(idChr, GAMETYPES::TECACT_FALLEN);

	if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_ABYSS))
		pGamePlayer->AddHP(-pGamePlayer->GetHPMax());
	else
		pGamePlayer->AddHP(-GAMETYPES::ATTACKPOWER_MAX);
  
    RwV3d vPosition = Math::VECTOR3_ZERO;
	pGamePlayer->GetLastPosition(&vPosition);
    CGimmickManager::ReplacePlayer(nPlayerNo, &vPosition, true);

	pGamePlayer->Release();
};


/*static*/ void CGameEvent::SetPlayerDamaged(int32 nPlayerNo)
{
	IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(nPlayerNo);

    CGameData::PlayResult().AddTechnicalAction(pGamePlayer->GetCurrentCharacterID(), GAMETYPES::TECACT_DAMAGED);

	if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_FATALBLOW))
		pGamePlayer->AddHP(-pGamePlayer->GetHPMax());
    
	pGamePlayer->Release();
};


/*static*/ void CGameEvent::SetPlayerDead(int32 nPlayerNo)
{
    CGameStage* pStage = CGameStage::GetCurrent();
    if (!pStage)
        return;

    pStage->NotifyPlayerDead(nPlayerNo);

    switch (CGameProperty::Player(nPlayerNo)->GetCurrentCharacterID())
    {
    case PLAYERID::ID_LEO:
    case PLAYERID::ID_RAP:
    case PLAYERID::ID_MIC:
    case PLAYERID::ID_DON:
        CGameSound::PlayObjectSE(CGameProperty::Player(nPlayerNo)->GetPlayer(), 0);
        break;
    };
};


/*static*/ void CGameEvent::SetPlayerTakenItem(int32 nPlayerNo, ITEMID::VALUE idItem)
{
	IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(nPlayerNo);

    switch (idItem)
    {
    case ITEMID::ID_HEAL_SMALL:
        {
            int32 iHP = CGameProperty::GetPlayerNum() * 100;
            if (iHP < 100)
                iHP = 100;
            
			pGamePlayer->AddHP(iHP);
        }
        break;

    case ITEMID::ID_HEAL_FULL:
        {
			pGamePlayer->AddHP(pGamePlayer->GetHPMax());
        }
        break;

    case ITEMID::ID_MISSILE:
    case ITEMID::ID_DON_LASER:
        {
			int32 nPlayerNum = CGameProperty::GetPlayerNum();
            for (int32 i = 0; i < nPlayerNum; ++i)
                CGameProperty::Player(i)->AddShurikenNum(10);
        }
        break;
    };

    CGameData::PlayResult().AddTakenItemCount(idItem);
    CGameData::Record().Item().SetItemTaken(idItem);

	pGamePlayer->GetPlayer()->GenerateItemEffect(idItem);
	pGamePlayer->Release();
};


/*static*/ void CGameEvent::SetPlayerTechnicalAction(int32 nPlayerNo, GAMETYPES::TECACT tecact)
{
	if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_HOME)
		return;

    CGameData::PlayResult().AddTechnicalAction(CGameProperty::Player(nPlayerNo)->GetCurrentCharacterID(), tecact);
};


/*static*/ void CGameEvent::SetPlayerRideAction(int32 nPlayerNo, GAMETYPES::RIDEACT rideact)
{
    CGameData::PlayResult().AddRideAction(CGameProperty::Player(nPlayerNo)->GetCurrentCharacterID(), rideact);
};


/*static*/ void CGameEvent::SetEnemyCreated(CEnemy* pEnemy)
{
    ASSERT(pEnemy);
    Property().EnemyContainer().Regist(pEnemy);
};


/*static*/ void CGameEvent::SetEnemyDestroyed(CEnemy* pEnemy)
{
    ASSERT(pEnemy);
    
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage)
        pStage->NotifyEnemyDead(pEnemy);
    
    Property().EnemyContainer().Remove(pEnemy);
};


/*static*/ void CGameEvent::SetEnemyDamaged(CEnemy* pEnemy, int32 nRemainHP)
{
    ASSERT(pEnemy);
    
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage)
        pStage->NotifyEnemyDamaged(pEnemy, nRemainHP);
};


/*static*/ void CGameEvent::SetGimmickCreated(CGimmick* pGimmick)
{
    ASSERT(pGimmick);
    Property().GimmickContainer().Regist(pGimmick);
};


/*static*/ void CGameEvent::SetGimmickDestroyed(CGimmick* pGimmick)
{
    ASSERT(pGimmick);
    Property().GimmickContainer().Remove(pGimmick);
};


/*static*/ void CGameEvent::SetItemBoxPut(void)
{
    CGameData::PlayResult().AddTotalItemCount();
};


/*static*/ void CGameEvent::SetTutorialOpened(int32 nTutorialNo)
{
    CGameStage* pStage = CGameStage::GetCurrent();
    if (pStage)
        pStage->StartPause(CGameStage::PAUSETYPE_TUTORIAL, (void*)nTutorialNo);
};

