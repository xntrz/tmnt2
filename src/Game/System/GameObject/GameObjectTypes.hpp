#pragma once


namespace GAMEOBJECTTYPES
{
    const int32 NAME_MAX = 16;

    enum FLAG : uint32
    {
        FLAG_PAUSE = (1 << 0), // NOTE: unused
        FLAG_SLEEP = (1 << 1),
        
        FLAG_DEFAULT = 0,
    };

    DEFINE_ENUM_FLAG_OPS(FLAG);

    enum
    {
        MESSAGEID_SLEEP = 0,
        MESSAGEID_AWAKE,
        MESSAGEID_CATCHATTACK,
        MESSAGEID_ATTACKRESULT,
    };
};