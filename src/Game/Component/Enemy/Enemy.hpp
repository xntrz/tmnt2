#pragma once

#include "EnemyID.hpp"
#include "EnemyTypes.hpp"

#include "Game/System/GameObject/GameObject.hpp"
#include "Game/Component/Effect/EffectID.hpp"
#include "Game/Component/GameMain/StageID.hpp"


class CEnemyCharacter;


class CEnemy : public CGameObject
{
public:
    static int32 m_iRefCount;
    static int32 m_iUniqueCount;
    
    static CEnemy* New(const ENEMYTYPES::CREATEINFO* pCreateInfo);

    /* returns effect at specified index required for enemy, returns ID_UNKNOWN if there is no more effects or no need */
    static EFFECTID::VALUE GetNeededEffect(ENEMYID::VALUE enemyId, int32 no);

    /* some resources loads by stage id (sounds)-  so its returns stage id required for enemy
       if there is no need returns ID_NONE */
    static STAGEID::VALUE GetNeededStage(ENEMYID::VALUE enemyId);

    CEnemy(const char* pszName, ENEMYID::VALUE idEnemy, CEnemyCharacter* pEnemyChr);
    virtual ~CEnemy(void);
    void Kill(void);
    void Start(void);
    void StartAI(void);
    void StopAI(void);
    void GetPosition(RwV3d* pvPosition) const;
    int32 GetHPMax(void) const;
    int32 GetHP(void);
    CEnemyCharacter& Character(void);
    const CEnemyCharacter& Character(void) const;

private:
    CEnemyCharacter* m_pEnemyCharacter;
};