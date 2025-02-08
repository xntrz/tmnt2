#pragma once


namespace MAPTYPES
{
    enum HITTYPE
    {
        HITTYPE_NONE = 0,
        HITTYPE_MAP,
        HITTYPE_GIMMICK,
    };

    enum GIMMICKTYPE
    {
        GIMMICKTYPE_NORMAL = 0,
        GIMMICKTYPE_SEESAW,
        GIMMICKTYPE_PUSHOBJ,
        GIMMICKTYPE_CONSOLE,
        GIMMICKTYPE_PILLAR,
        GIMMICKTYPE_SWITCH_PILLAR,
        GIMMICKTYPE_THROWOBJ,
        GIMMICKTYPE_BELTCONVEYOR,
    };

    /* TODO: in some places it test/check like flag and some not */
    enum ATTRIBUTE : uint32
    {
        ATTRIBUTE_UNKNOWN   = 0x0,
        ATTRIBUTE_WATER     = 0x1,
        ATTRIBUTE_SNOW      = 0x2,
        ATTRIBUTE_SLIP      = 0x4,
        ATTRIBUTE_MAGMA     = 0x8,
        ATTRIBUTE_DEATH     = 0x10,
        ATTRIBUTE_POISON    = 0x20,
        ATTRIBUTE_GRASS     = 0x40,
        ATTRIBUTE_JUMP      = 0x80,
        ATTRIBUTE_MISSJUMP  = 0x100,
        ATTRIBUTE_DAMAGE    = 0x200,
        ATTRIBUTE_ONEWAY    = 0x400,
        ATTRIBUTE_SAND      = 0x800,
        ATTRIBUTE_MESH      = 0x1000,
        ATTRIBUTE_DOBON     = 0x2000,
    };
};