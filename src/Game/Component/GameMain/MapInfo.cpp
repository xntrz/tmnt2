#include "MapInfo.hpp"


struct CMapInfo::INFO
{
    const char* m_pszName;
    CATEGORY m_category;
};


/*static*/ const CMapInfo::INFO CMapInfo::m_aMapInfo[] =
{
    { "NONE",   CATEGORY_NY,            },
    { "M01T",   CATEGORY_NY,            },
    { "M02N",   CATEGORY_NY,            },
    { "M02NB",  CATEGORY_NY,            },
    { "M03S",   CATEGORY_NY,            },    
    { "M04N",   CATEGORY_UNDERGROUND,   },
    { "M04NB",  CATEGORY_UNDERGROUND,   },
    { "M05N",   CATEGORY_UNDERGROUND,   },
    { "M06R",   CATEGORY_UNDERGROUND,   },    
    { "M09N",   CATEGORY_SPACE,         },
    { "M10N",   CATEGORY_SPACE,         },
    { "M11J",   CATEGORY_SPACE,         },
    { "M12N",   CATEGORY_ZERO,          },
    { "M13R",   CATEGORY_ZERO,          },
    { "M14N",   CATEGORY_ZERO,          },
    { "M15N",   CATEGORY_SPACE,         },
    { "M17N",   CATEGORY_SPACE,         },
    { "M18F",   CATEGORY_SPACE,         },
    { "M18FB",  CATEGORY_SPACE,         },
    { "M19N",   CATEGORY_SPACE,         },
    { "M20F",   CATEGORY_SPACE,         },
    { "M20FB",  CATEGORY_SPACE,         },
    { "M21N",   CATEGORY_SPACE,         },
    { "M21NB",  CATEGORY_SPACE,         },
    { "M22R",   CATEGORY_SPACE,         },
    { "M23N",   CATEGORY_SPACE,         },
    { "M24N",   CATEGORY_SPACE,         },
    { "M25J",   CATEGORY_SPACE,         },
    { "M26N",   CATEGORY_SPACE,         },
    { "M27N",   CATEGORY_SPACE,         },
    { "M28N",   CATEGORY_SPACE,         },
    { "M29N",   CATEGORY_SPACE,         },
    { "M30N",   CATEGORY_SPACE,         },
    { "M30NB",  CATEGORY_SPACE,         },
    { "M31NB",  CATEGORY_SPACE,         },
    { "M32R",   CATEGORY_SPACE,         },
    { "M33J",   CATEGORY_JAPAN,         },
    { "M34N",   CATEGORY_JAPAN,         },
    { "M35N",   CATEGORY_JAPAN,         },
    { "M36N",   CATEGORY_JAPAN,         },
    { "M37N",   CATEGORY_JAPAN,         },
    { "M38NB",  CATEGORY_JAPAN,         },
    { "M39N",   CATEGORY_SPACE,         },
    { "M40OB",  CATEGORY_SPACE,         },    
    { "M41N",   CATEGORY_NY,            },
    { "M42N",   CATEGORY_NY,            },
    { "M42NB",  CATEGORY_NY,            },
    { "M43N",   CATEGORY_NY,            },
    { "M44N",   CATEGORY_NY,            },
    { "M44NB",  CATEGORY_NY,            },
    { "M45N",   CATEGORY_NY,            },
    { "M46R",   CATEGORY_NY,            },
    { "M47O",   CATEGORY_NY,            },
    { "M47OB",  CATEGORY_NY,            },
    { "M48N",   CATEGORY_NY,            },
    { "M49N",   CATEGORY_NY,            },
    { "M50NB",  CATEGORY_NY,            },
    { "M52F",   CATEGORY_NY,            },
    { "M52FB",  CATEGORY_NY,            },
    { "M53F",   CATEGORY_NY,            },
    { "M53FB",  CATEGORY_NY,            },
    { "M54F",   CATEGORY_NY,            },
    { "M54FB",  CATEGORY_NY,            },
    { "M55F",   CATEGORY_NY,            },
    { "M55FB",  CATEGORY_NY,            },
    { "M56NB",  CATEGORY_NY,            },
    { "M57NB",  CATEGORY_NY,            },
    { "M58O",   CATEGORY_NY,            },
    { "M58OB",  CATEGORY_NY,            },
    { "M60X",   CATEGORY_NY,            },
    { "M61X",   CATEGORY_NY,            },
    { "TEST",   CATEGORY_NY,            },
};


/*static*/ const CMapInfo::INFO& CMapInfo::Info(MAPID::VALUE idMap)
{
    static_assert(COUNT_OF(m_aMapInfo) == MAPID::ID_MAX, "update me");
    
    ASSERT(idMap >= MAPID::ID_NONE);
    ASSERT(idMap < MAPID::ID_MAX);

    return m_aMapInfo[idMap];
};


/*static*/ const char* CMapInfo::GetName(MAPID::VALUE idMap)
{
    return Info(idMap).m_pszName;
};


/*static*/ CMapInfo::CATEGORY CMapInfo::GetCategory(MAPID::VALUE idMap)
{
    return Info(idMap).m_category;
};

