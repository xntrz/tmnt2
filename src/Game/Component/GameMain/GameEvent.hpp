#pragma once

#include "GameTypes.hpp"
#include "ItemID.hpp"

#include "Game/Component/GameData/GamePlayResult.hpp"


class CGamePropertyObject;
class CGimmick;
class CEnemy;


class CGameEvent
{
public:
    static void AttachProperty(CGamePropertyObject* pPropObj);
    static void DetachProperty(void);
    static void SetGameCleared(CGamePlayResult::CLEARSUB clearsub);
    static void SetGameOvered(void);
    static void SetPlayerFallen(int32 nPlayerNo);
    static void SetPlayerDamaged(int32 nPlayerNo);
    static void SetPlayerDead(int32 nPlayerNo);
    static void SetPlayerTakenItem(int32 nPlayerNo, ITEMID::VALUE idItem);
    static void SetPlayerTechnicalAction(int32 nPlayerNo, GAMETYPES::TECACT tecact);
    static void SetPlayerRideAction(int32 nPlayerNo, GAMETYPES::RIDEACT rideact);
    static void SetEnemyCreated(CEnemy* pEnemy);
    static void SetEnemyDestroyed(CEnemy* pEnemy);
    static void SetEnemyDamaged(CEnemy* pEnemy, int32 nAmount);
    static void SetGimmickCreated(CGimmick* pGimmick);
    static void SetGimmickDestroyed(CGimmick* pGimmick);
    static void SetItemBoxPut(void);
    static void SetTutorialOpened(int32 nTutorialNo);

private:
    static CGamePropertyObject& Property(void);
    
    static CGamePropertyObject* m_pProperty;
};