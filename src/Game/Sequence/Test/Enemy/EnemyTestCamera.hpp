#pragma once

#include "Game/Component/GameMain/GameStageCameraUpdater.hpp"

#ifdef _DEBUG

class CEnemyTestCamera : public IGameStageCameraUpdater
{
public:
    virtual void Update(CMapCamera* pMapCamera) override;
};

#endif /* _DEBUG */