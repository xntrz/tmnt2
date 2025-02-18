#include "MapInfo.hpp"


struct MAPINFO
{
    const char*         m_pszName;
    CMapInfo::CATEGORY  m_category;
};


static const MAPINFO s_aMapInfo[] =
{
    { "NONE",   CMapInfo::CATEGORY_NY,            },
    { "M01T",   CMapInfo::CATEGORY_NY,            },
    { "M02N",   CMapInfo::CATEGORY_NY,            },
    { "M02NB",  CMapInfo::CATEGORY_NY,            },
    { "M03S",   CMapInfo::CATEGORY_NY,            },    
    { "M04N",   CMapInfo::CATEGORY_UNDERGROUND,   },
    { "M04NB",  CMapInfo::CATEGORY_UNDERGROUND,   },
    { "M05N",   CMapInfo::CATEGORY_UNDERGROUND,   },
    { "M06R",   CMapInfo::CATEGORY_UNDERGROUND,   },    
    { "M09N",   CMapInfo::CATEGORY_SPACE,         },
    { "M10N",   CMapInfo::CATEGORY_SPACE,         },
    { "M11J",   CMapInfo::CATEGORY_SPACE,         },
    { "M12N",   CMapInfo::CATEGORY_ZERO,          },
    { "M13R",   CMapInfo::CATEGORY_ZERO,          },
    { "M14N",   CMapInfo::CATEGORY_ZERO,          },
    { "M15N",   CMapInfo::CATEGORY_SPACE,         },
    { "M17N",   CMapInfo::CATEGORY_SPACE,         },
    { "M18F",   CMapInfo::CATEGORY_SPACE,         },
    { "M18FB",  CMapInfo::CATEGORY_SPACE,         },
    { "M19N",   CMapInfo::CATEGORY_SPACE,         },
    { "M20F",   CMapInfo::CATEGORY_SPACE,         },
    { "M20FB",  CMapInfo::CATEGORY_SPACE,         },
    { "M21N",   CMapInfo::CATEGORY_SPACE,         },
    { "M21NB",  CMapInfo::CATEGORY_SPACE,         },
    { "M22R",   CMapInfo::CATEGORY_SPACE,         },
    { "M23N",   CMapInfo::CATEGORY_SPACE,         },
    { "M24N",   CMapInfo::CATEGORY_SPACE,         },
    { "M25J",   CMapInfo::CATEGORY_SPACE,         },
    { "M26N",   CMapInfo::CATEGORY_SPACE,         },
    { "M27N",   CMapInfo::CATEGORY_SPACE,         },
    { "M28N",   CMapInfo::CATEGORY_SPACE,         },
    { "M29N",   CMapInfo::CATEGORY_SPACE,         },
    { "M30N",   CMapInfo::CATEGORY_SPACE,         },
    { "M30NB",  CMapInfo::CATEGORY_SPACE,         },
    { "M31NB",  CMapInfo::CATEGORY_SPACE,         },
    { "M32R",   CMapInfo::CATEGORY_SPACE,         },
    { "M33J",   CMapInfo::CATEGORY_JAPAN,         },
    { "M34N",   CMapInfo::CATEGORY_JAPAN,         },
    { "M35N",   CMapInfo::CATEGORY_JAPAN,         },
    { "M36N",   CMapInfo::CATEGORY_JAPAN,         },
    { "M37N",   CMapInfo::CATEGORY_JAPAN,         },
    { "M38NB",  CMapInfo::CATEGORY_JAPAN,         },
    { "M39N",   CMapInfo::CATEGORY_SPACE,         },
    { "M40OB",  CMapInfo::CATEGORY_SPACE,         },    
    { "M41N",   CMapInfo::CATEGORY_NY,            },
    { "M42N",   CMapInfo::CATEGORY_NY,            },
    { "M42NB",  CMapInfo::CATEGORY_NY,            },
    { "M43N",   CMapInfo::CATEGORY_NY,            },
    { "M44N",   CMapInfo::CATEGORY_NY,            },
    { "M44NB",  CMapInfo::CATEGORY_NY,            },
    { "M45N",   CMapInfo::CATEGORY_NY,            },
    { "M46R",   CMapInfo::CATEGORY_NY,            },
    { "M47O",   CMapInfo::CATEGORY_NY,            },
    { "M47OB",  CMapInfo::CATEGORY_NY,            },
    { "M48N",   CMapInfo::CATEGORY_NY,            },
    { "M49N",   CMapInfo::CATEGORY_NY,            },
    { "M50NB",  CMapInfo::CATEGORY_NY,            },
    { "M52F",   CMapInfo::CATEGORY_NY,            },
    { "M52FB",  CMapInfo::CATEGORY_NY,            },
    { "M53F",   CMapInfo::CATEGORY_NY,            },
    { "M53FB",  CMapInfo::CATEGORY_NY,            },
    { "M54F",   CMapInfo::CATEGORY_NY,            },
    { "M54FB",  CMapInfo::CATEGORY_NY,            },
    { "M55F",   CMapInfo::CATEGORY_NY,            },
    { "M55FB",  CMapInfo::CATEGORY_NY,            },
    { "M56NB",  CMapInfo::CATEGORY_NY,            },
    { "M57NB",  CMapInfo::CATEGORY_NY,            },
    { "M58O",   CMapInfo::CATEGORY_NY,            },
    { "M58OB",  CMapInfo::CATEGORY_NY,            },
    { "M60X",   CMapInfo::CATEGORY_NY,            },
    { "M61X",   CMapInfo::CATEGORY_NY,            },
    { "TEST",   CMapInfo::CATEGORY_NY,            },
};


static_assert(COUNT_OF(s_aMapInfo) == MAPID::ID_MAX, "update table");


static const MAPINFO& MapInfo(MAPID::VALUE idMap)
{    
    ASSERT(idMap >= MAPID::ID_NONE);
    ASSERT(idMap < MAPID::ID_MAX);

    return s_aMapInfo[idMap];
};


/*static*/ const char* CMapInfo::GetName(MAPID::VALUE idMap)
{
    return MapInfo(idMap).m_pszName;
};


/*static*/ CMapInfo::CATEGORY CMapInfo::GetCategory(MAPID::VALUE idMap)
{
    return MapInfo(idMap).m_category;
};

