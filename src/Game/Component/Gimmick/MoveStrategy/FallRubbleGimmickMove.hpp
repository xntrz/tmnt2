#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CFallRubbleGimmickMove : public CGimmickMove
{
public:
    CFallRubbleGimmickMove(float fRefCoe, float fBodyHitRadius);
    virtual ~CFallRubbleGimmickMove(void);
};