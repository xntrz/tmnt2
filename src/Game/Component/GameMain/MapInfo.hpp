#pragma once

#include "MapID.hpp"


class CMapInfo
{
public:
    enum CATEGORY
    {
        CATEGORY_NY = 0,
        CATEGORY_SPACE,
        CATEGORY_JAPAN,
        CATEGORY_UNDERGROUND,
        CATEGORY_ZERO,

        CATEGORY_DEFAULT = CATEGORY_NY,
    };

public:
    static const char* GetName(MAPID::VALUE idMap);
    static CATEGORY GetCategory(MAPID::VALUE idMap);
};