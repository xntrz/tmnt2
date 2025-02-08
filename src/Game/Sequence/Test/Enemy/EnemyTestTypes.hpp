#pragma once

#ifdef _DEBUG

#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/Component/GameMain/StageID.hpp"


namespace ENEMYTESTTYPES
{
    struct TEST_CTX
    {
        ENEMYID::VALUE  enemyId[12];
        int32           enemyCount;
        STAGEID::VALUE  stageId;
        bool            loadItemBoxFlag;
    };
};

#endif /* _DEBUG */