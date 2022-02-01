#pragma once

#include "Game/Component/GameMain/GameStageCameraUpdater.hpp"


class CNexusEnemyIntroductionCamera : public IGameStageCameraUpdater
{
public:
    virtual void Update(CMapCamera* pMapCamera) override;
};