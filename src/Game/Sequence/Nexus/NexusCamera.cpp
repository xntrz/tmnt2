#include "NexusCamera.hpp"

#include "Game/System/Map/MapCamera.hpp"


void CNexusEnemyIntroductionCamera::Update(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);

    RwV3d vAt = { 0.0f, 2.0f, -2.0f };
    pMapCamera->Update(&vAt, 8.0f);
};