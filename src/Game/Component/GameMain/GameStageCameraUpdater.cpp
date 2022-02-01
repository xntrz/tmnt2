#include "GameStageCameraUpdater.hpp"
#include "GameTypes.hpp"
#include "GameProperty.hpp"
#include "GamePlayer.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


/*static*/ CDefaultCameraUpdater* CDefaultCameraUpdater::Instance(void)
{
    static CDefaultCameraUpdater s_DefaultCameraUpdt;
    return &s_DefaultCameraUpdt;
};


void CDefaultCameraUpdater::Update(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);
    
    RwV3d avPosition[GAMETYPES::PLAYERS_MAX] = { Math::VECTOR3_ZERO };
    RwV3d vLookat = Math::VECTOR3_ZERO;
    int32 nNumPlayer = CGameProperty::GetPlayerNum();

    for (int32 i = 0; i < nNumPlayer; ++i)
    {
        RwV3d vTmp = Math::VECTOR3_ZERO;
        CGameProperty::Player(i).GetPosition(&vTmp);

        avPosition[i] = vTmp;
        
        vLookat.x += vTmp.x;
        vLookat.y += vTmp.y;
        vLookat.z += vTmp.z;
    };

    if (nNumPlayer > 0)
    {
        vLookat.x *= (1.0f / float(nNumPlayer));
        vLookat.y *= (1.0f / float(nNumPlayer));
        vLookat.z *= (1.0f / float(nNumPlayer));
    };

    float fRequestZoom = pMapCamera->CalcNiceZoon(avPosition, nNumPlayer);
    pMapCamera->Update(&vLookat, fRequestZoom);
};


/*static*/ CBossBattleCameraUpdater* CBossBattleCameraUpdater::Instance(void)
{
    static CBossBattleCameraUpdater s_BossBattleCameraUpdt;
    return &s_BossBattleCameraUpdt;
};


void CBossBattleCameraUpdater::Update(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);

    RwV3d vLookat = Math::VECTOR3_ZERO;
    int32 nNumPlayer = CGameProperty::GetPlayerNum();
    int32 nNumEnemy = 0;

    for (int32 i = 0; i < nNumPlayer; ++i)
    {
        RwV3d vTmp = Math::VECTOR3_ZERO;
        CGameProperty::Player(i).GetPosition(&vTmp);
        
        vLookat.x += vTmp.x;
        vLookat.y += vTmp.y;
        vLookat.z += vTmp.z;
    };    

    CGameObject* pGameObject = CGameObjectManager::GetNext(nullptr);
    while (pGameObject)
    {
        if (pGameObject->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CCharacter* pCharacter = (CCharacter*)pGameObject;

            if (pCharacter->GetAttackCharacterType() == CCharacter::TYPE_ENEMY)
            {
                CCharacterCompositor* pCharacterCompositor = (CCharacterCompositor*)pCharacter;
                if (pCharacterCompositor->GetID() >= ENEMYID::BOSSBEGIN &&
                    pCharacterCompositor->GetID() < ENEMYID::BOSSEND)
                {
                    ++nNumEnemy;

                    RwV3d vTmp = Math::VECTOR3_ZERO;
                    pCharacterCompositor->GetPosition(&vTmp);
                    
                    vLookat.x += vTmp.x;
                    vLookat.y += vTmp.y;
                    vLookat.z += vTmp.z;
                };
            };
        };

        pGameObject = CGameObjectManager::GetNext(pGameObject);
    };

    int32 nNumTotalObj = nNumPlayer + nNumEnemy;

    if(nNumTotalObj > 0)
    {
        vLookat.x *= (1.0f / float(nNumTotalObj));
        vLookat.y *= (1.0f / float(nNumTotalObj));
        vLookat.z *= (1.0f / float(nNumTotalObj));
    };

    pMapCamera->Update(&vLookat);
};