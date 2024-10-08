#include "Enemy.hpp"
#include "EnemyCharacter.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/System/Misc/DebugShape.hpp"


static CEnemyCharacter* CreateEnemyCharacter(ENEMYID::VALUE id)
{
    if (!ENEMYID::GetImplementProgress(id))
        return nullptr;
    
    return nullptr;
};


/*static*/ int32 CEnemy::m_iRefCount = 0;
/*static*/ int32 CEnemy::m_iUniqueCount = 0;


/*static*/ CEnemy* CEnemy::New(const ENEMYTYPES::CREATEINFO* pCreateInfo)
{
    char szEnemyName[GAMEOBJECTTYPES::NAME_MAX];
    szEnemyName[0] = '\0';
    
    std::sprintf(szEnemyName, "s%03d_%04d", pCreateInfo->m_idEnemy, m_iUniqueCount);

#ifdef _DEBUG
	OUTPUT(
		"Creating \"%s\" (\"%s\") enemy at %03f | %03f | %03f\n",
        ENEMYID::GetName(pCreateInfo->m_idEnemy),
        szEnemyName,
        pCreateInfo->m_vPosition.x,
		pCreateInfo->m_vPosition.y,
		pCreateInfo->m_vPosition.z
	);

	RwSphere sphere = { 0 };
	sphere.center = pCreateInfo->m_vPosition;
	sphere.radius = 0.5f;

	CDebugShape::m_fDuration = 3.0f;
    CDebugShape::ShowSphere(&sphere, { 0xFF, 0xFF, 0x00, 0xFF });
    CDebugShape::ShowLabel(&pCreateInfo->m_vPosition, ENEMYID::GetName(pCreateInfo->m_idEnemy));
    CDebugShape::m_fDuration = 0.0f;
#endif    

    CEnemyCharacter::PreInitialize(pCreateInfo);
    CEnemyCharacter* pEnemyCharacter = CreateEnemyCharacter(pCreateInfo->m_idEnemy);
    if (!pEnemyCharacter)
        return nullptr;

    CEnemy* pEnemy = new CEnemy(szEnemyName, pCreateInfo->m_idEnemy, pEnemyCharacter);
    if (pEnemy)
    {
        pEnemyCharacter->SetOwner(pEnemy->GetHandle());
        
        ++m_iRefCount;
        ++m_iUniqueCount;

        CGameEvent::SetEnemyCreated(pEnemy);
    }
    else
    {
        delete pEnemyCharacter;
        pEnemyCharacter = nullptr;
    };

    return pEnemy;
};


CEnemy::CEnemy(const char* pszName, ENEMYID::VALUE idEnemy, CEnemyCharacter* pEnemyChr)
: CGameObject(pszName, GAMEOBJECTTYPE::ENEMY)
, m_pEnemyCharacter(pEnemyChr)
{
    ASSERT(m_pEnemyCharacter);
};


CEnemy::~CEnemy(void)
{
    CGameEvent::SetEnemyDestroyed(this);

    if (m_pEnemyCharacter)
    {
        delete m_pEnemyCharacter;
        m_pEnemyCharacter = nullptr;
    };

    ASSERT(m_iRefCount > 0);
    if (!--m_iRefCount)
        m_iUniqueCount = 0;
};


CEnemyCharacter& CEnemy::EnemyCharacter(void) const
{
    ASSERT(m_pEnemyCharacter);
    return *m_pEnemyCharacter;
};


void CEnemy::Kill(void)
{
    ASSERT(m_pEnemyCharacter);

    if ((m_pEnemyCharacter->GetStatus() == ENEMYTYPES::STATUS_CAUGHT) ||
        (m_pEnemyCharacter->GetStatus() == ENEMYTYPES::STATUS_LIFTED) ||
        (m_pEnemyCharacter->GetStatus() == ENEMYTYPES::STATUS_LIFTED_WALK))
    {
        return;
    };

    delete this;
};


void CEnemy::Start(void)
{
    ASSERT(m_pEnemyCharacter);
    m_pEnemyCharacter->Start();
};


void CEnemy::StartAI(void)
{
    ASSERT(m_pEnemyCharacter);
    m_pEnemyCharacter->StartAI();
};


void CEnemy::StopAI(void)
{
    ASSERT(m_pEnemyCharacter);
    m_pEnemyCharacter->StopAI();
};


void CEnemy::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(m_pEnemyCharacter);
    m_pEnemyCharacter->GetBodyPosition(pvPosition);
};


int32 CEnemy::GetHPMax(void) const
{
    ASSERT(m_pEnemyCharacter);
    return m_pEnemyCharacter->CharacterParameter().m_feature.m_iHPMax;
};


int32 CEnemy::GetHP(void)
{
    ASSERT(m_pEnemyCharacter);
    return m_pEnemyCharacter->CharacterParameter().m_feature.m_iHP;
};