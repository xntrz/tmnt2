#pragma once

#include "GimmickID.hpp"


class CGimmick;


class CGimmickFactory
{
public:
    static CGimmick* Create(GIMMICKID::VALUE idGimmick, int32 subid, void* pParam);
};