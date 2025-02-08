#include "EnemyTestCamera.hpp"

#ifdef _DEBUG

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/Gamepad.hpp"


void CEnemyTestCamera::Update(CMapCamera* pMapCamera)
{
    /* copy of CDefaultCameraUpdater from Game/Component/GameMain/GameStageCameraUpdater.cpp except zoom factor */
    
    static const float fZoomFactor = 0.75f;
    
    ASSERT(pMapCamera);

    RwV3d avPosition[GAMETYPES::PLAYERS_MAX] = { Math::VECTOR3_ZERO };
    RwV3d vLookat = Math::VECTOR3_ZERO;

    int32 numPos = 0;
    int32 nNumPlayer = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < nNumPlayer; ++i)
    {
        if (i >= CController::Max())
            continue;

        ++numPos;

        RwV3d vTmp = Math::VECTOR3_ZERO;
        CGameProperty::Player(i)->GetPosition(&vTmp);

        avPosition[i] = vTmp;

        vLookat.x += vTmp.x;
        vLookat.y += vTmp.y;
        vLookat.z += vTmp.z;
    };

    if (numPos > 0)
    {
        vLookat.x *= (1.0f / static_cast<float>(numPos));
        vLookat.y *= (1.0f / static_cast<float>(numPos));
        vLookat.z *= (1.0f / static_cast<float>(numPos));
    };

    float fRequestZoom = pMapCamera->CalcNiceZoom(avPosition, numPos);
    pMapCamera->Update(&vLookat, fRequestZoom * fZoomFactor);
};

#endif /* _DEBUG */