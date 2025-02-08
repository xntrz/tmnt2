#include "EnemyTracer.hpp"
#include "EnemyCharacter.hpp"
#include "CharacterCompositor.hpp"

#include "Game/System/GameObject/GameObjectManager.hpp"


CEnemyTracer::CEnemyTracer(CCharacterCompositor* pChrCompositor)
: m_hObj(0)
{
    ASSERT(pChrCompositor->GetCharacterType() == CCharacter::TYPE_ENEMY);
    m_hObj = pChrCompositor->GetHandle();
};


CEnemyTracer::CEnemyTracer(CEnemyCharacter* pEnemyChr)
: CEnemyTracer(&pEnemyChr->Compositor())
{
    ;
};


CEnemyTracer::~CEnemyTracer(void)
{
    ;
};


float CEnemyTracer::GetDirection(void)
{
    CGameObject* pObj = CGameObjectManager::GetObject(m_hObj);
    if (pObj)
    {
        ASSERT(pObj->GetType() == GAMEOBJECTTYPE::CHARACTER);
        
        CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pObj);
        ASSERT(pChrCompositor->GetCharacterType() == CCharacter::TYPE_ENEMY);
        
        return pChrCompositor->GetDirection();
    };

    return 0.0f;
};


void CEnemyTracer::GetPosition(RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    CGameObject* pObj = CGameObjectManager::GetObject(m_hObj);
    if (pObj)
    {
        ASSERT(pObj->GetType() == GAMEOBJECTTYPE::CHARACTER);
        
        CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pObj);
        ASSERT(pChrCompositor->GetCharacterType() == CCharacter::TYPE_ENEMY);
        
        pChrCompositor->GetFootPosition(pvPosition);
    };
};