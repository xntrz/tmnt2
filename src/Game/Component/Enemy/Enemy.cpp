#include "Enemy.hpp"

/*static*/ int32 CEnemy::m_iRefCount = 0;
/*static*/ int32 CEnemy::m_iUniqueCount = 0;


/*static*/ CEnemy* CEnemy::New(const ENEMYTYPES::CREATEINFO* pCreateInfo)
{
    return nullptr;
};


CEnemy::CEnemy(const char* pszName, ENEMYID::VALUE idEnemy, CEnemyCharacter* pEnemyChr)
: CGameObject(pszName, GAMEOBJECTTYPE::ENEMY)
, m_pEnemyCharacter(pEnemyChr)
{
    ASSERT(m_pEnemyCharacter);
};


CEnemy::~CEnemy(void)
{
    ;
};


CEnemyCharacter& CEnemy::EnemyCharacter(void) const
{
    ASSERT(m_pEnemyCharacter);
    return *m_pEnemyCharacter;
};


void CEnemy::Kill(void)
{
    ;
};


void CEnemy::Start(void)
{
    
};


void CEnemy::StartAI(void)
{
    ;
};


void CEnemy::StopAI(void)
{
    ;
};


void CEnemy::GetPosition(RwV3d* pvPosition) const
{
    ;
};


int32 CEnemy::GetHPMax(void) const
{
    return 0;
};


int32 CEnemy::GetHP(void)
{
    return 0;
};