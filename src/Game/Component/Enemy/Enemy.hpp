#pragma once

#include "EnemyID.hpp"
#include "EnemyTypes.hpp"

#include "Game/System/GameObject/GameObject.hpp"


class CEnemyCharacter;


class CEnemy : public CGameObject
{
public:
    static CEnemy* New(const ENEMYTYPES::CREATEINFO* pCreateInfo);
    
    CEnemy(const char* pszName, ENEMYID::VALUE idEnemy, CEnemyCharacter* pEnemyChr);
    virtual ~CEnemy(void);
    virtual CEnemyCharacter& EnemyCharacter(void) const;
    void Kill(void);
    void Start(void);
    void StartAI(void);
    void StopAI(void);
    void GetPosition(RwV3d* pvPosition) const;
    int32 GetHPMax(void) const;
    int32 GetHP(void);

private:
    static int32 m_iRefCount;
    static int32 m_iUniqueCount;
    CEnemyCharacter* m_pEnemyCharacter;
};