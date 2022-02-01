#pragma once

#include "MapID.hpp"


class CMapInfo
{
protected:
    struct INFO;

    static const INFO m_aMapInfo[];

    static const INFO& Info(MAPID::VALUE idMap);

public:
    enum CATEGORY
    {
        CATEGORY_NY = 0,
        CATEGORY_SPACE,
        CATEGORY_JAPAN,
        CATEGORY_UNDERGROUND,

        CATEGORY_DEFAULT = CATEGORY_NY,
    };

public:
    static const char* GetName(MAPID::VALUE idMap);
    static CATEGORY GetCategory(MAPID::VALUE idMap);
};