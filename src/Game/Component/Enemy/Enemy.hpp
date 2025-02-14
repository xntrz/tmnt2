#pragma once

#include "EnemyID.hpp"
#include "EnemyTypes.hpp"

#include "Game/System/GameObject/GameObject.hpp"
#include "Game/Component/Effect/EffectID.hpp"


class CEnemyCharacter;


class CEnemy : public CGameObject
{
public:
    static int32 m_iRefCount;
    static int32 m_iUniqueCount;
    
    static CEnemy* New(const ENEMYTYPES::CREATEINFO* pCreateInfo);
    static EFFECTID::VALUE GetNeededEffect(ENEMYID::VALUE enemyId, int32 no); // returns ID_UNKNOWN if there is no more effects
    
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