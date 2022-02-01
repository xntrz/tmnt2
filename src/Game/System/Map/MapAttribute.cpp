#include "MapAttribute.hpp"


/*static*/ MAPTYPES::ATTRIBUTE CMapAttribute::CheckAttribute(const char* pszAttribute)
{
    ASSERT(pszAttribute);

    struct MAPATTRIBUTE_INFO
    {
        const char* m_pszName;
        MAPTYPES::ATTRIBUTE m_attribute;
    };

    static const MAPATTRIBUTE_INFO s_aMapAttributeInfo[] =
    {    
        { "water",      MAPTYPES::ATTRIBUTE_WATER,      },
        { "snow",       MAPTYPES::ATTRIBUTE_SNOW,       },
        { "slip",       MAPTYPES::ATTRIBUTE_SLIP,       },
        { "magma",      MAPTYPES::ATTRIBUTE_MAGMA,      },
        { "death",      MAPTYPES::ATTRIBUTE_DEATH,      },
        { "poison",     MAPTYPES::ATTRIBUTE_POISON,     },
        { "grass",      MAPTYPES::ATTRIBUTE_GRASS,      },
        { "jump",       MAPTYPES::ATTRIBUTE_JUMP,       },
        { "missjump",   MAPTYPES::ATTRIBUTE_MISSJUMP,   },
        { "damage",     MAPTYPES::ATTRIBUTE_DAMAGE,     },
        { "oneway",     MAPTYPES::ATTRIBUTE_ONEWAY,     },
        { "sand",       MAPTYPES::ATTRIBUTE_SAND,       },
        { "mesh",       MAPTYPES::ATTRIBUTE_MESH,       },
        { "dobon",      MAPTYPES::ATTRIBUTE_DOBON,      },
    };

    for (int32 i = 0; i < COUNT_OF(s_aMapAttributeInfo); ++i)
    {
        if (!std::strcmp(s_aMapAttributeInfo[i].m_pszName, pszAttribute))
            return s_aMapAttributeInfo[i].m_attribute;        
    };

    return MAPTYPES::ATTRIBUTE_UNKNOWN;
};