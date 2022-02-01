#include "EnemyTracer.hpp"
#include "CharacterCompositor.hpp"

#include "Game/System/GameObject/GameObjectManager.hpp"


CEnemyTracer::CEnemyTracer(CCharacter* pCharacter)
: m_hObj(0)
{
    ASSERT(pCharacter);
    ASSERT(pCharacter->GetCharacterType() == CCharacter::TYPE_ENEMY);

    m_hObj = pCharacter->GetHandle();
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
        
        CCharacter* pChr = (CCharacter*)pObj;
        ASSERT(pChr->GetCharacterType() == CCharacter::TYPE_ENEMY);
        
        return pChr->GetDirection();
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
        
        CCharacter* pChr = (CCharacter*)pObj;
        ASSERT(pChr->GetCharacterType() == CCharacter::TYPE_ENEMY);
        
        pChr->GetFootPosition(pvPosition);
    };
};