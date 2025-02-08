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

    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        RwV3d vTmp = Math::VECTOR3_ZERO;
        CGameProperty::Player(i)->GetPosition(&vTmp);

        avPosition[i] = vTmp;
        
        vLookat.x += vTmp.x;
        vLookat.y += vTmp.y;
        vLookat.z += vTmp.z;
    };

    if (playerNum > 0)
    {
        vLookat.x *= (1.0f / static_cast<float>(playerNum));
        vLookat.y *= (1.0f / static_cast<float>(playerNum));
        vLookat.z *= (1.0f / static_cast<float>(playerNum));
    };

    float fZoom = pMapCamera->CalcNiceZoom(avPosition, playerNum);
    pMapCamera->Update(&vLookat, fZoom);
};


//
// *********************************************************************************
//


/*static*/ CBossBattleCameraUpdater* CBossBattleCameraUpdater::Instance(void)
{
    static CBossBattleCameraUpdater s_BossBattleCameraUpdt;
    return &s_BossBattleCameraUpdt;
};


void CBossBattleCameraUpdater::Update(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);

    RwV3d vLookat = Math::VECTOR3_ZERO;
    int32 enemyNum = 0;

    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        RwV3d vTmp = Math::VECTOR3_ZERO;
        CGameProperty::Player(i)->GetPosition(&vTmp);
        
        vLookat.x += vTmp.x;
        vLookat.y += vTmp.y;
        vLookat.z += vTmp.z;
    };    

    CGameObject* pGameObject = CGameObjectManager::GetNext(GAMEOBJECTTYPE::CHARACTER);
    while (pGameObject)
    {
        ASSERT(pGameObject->GetType() == GAMEOBJECTTYPE::CHARACTER);
        CCharacter* pChr = static_cast<CCharacter*>(pGameObject);

        if (pChr->GetAttackCharacterType() == CCharacter::TYPE_ENEMY)
        {
            CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pChr);

            if ((pChrCompositor->GetID() >= ENEMYID::BOSSBEGIN) &&
                (pChrCompositor->GetID() <  ENEMYID::BOSSEND))
            {
                ++enemyNum;

                RwV3d vTmp = Math::VECTOR3_ZERO;
                pChrCompositor->GetPosition(&vTmp);

                vLookat.x += vTmp.x;
                vLookat.y += vTmp.y;
                vLookat.z += vTmp.z;
            };
        };

        pGameObject = CGameObjectManager::GetNext(GAMEOBJECTTYPE::CHARACTER, pGameObject);
    };

    int32 totalNum = (playerNum + enemyNum);
    if (totalNum > 0)
    {
        vLookat.x *= (1.0f / static_cast<float>(totalNum));
        vLookat.y *= (1.0f / static_cast<float>(totalNum));
        vLookat.z *= (1.0f / static_cast<float>(totalNum));
    };

    pMapCamera->Update(&vLookat);
};