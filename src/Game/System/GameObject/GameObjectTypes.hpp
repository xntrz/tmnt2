#pragma once


namespace GAMEOBJECTTYPES
{
    const int32 NAME_MAX = 16;

    enum FLAG : uint32
    {
        FLAG_NONE = 0x0,
        FLAG_PAUSE = 0x1,
        FLAG_SLEEP = 0x2,
        
        FLAG_DEFAULT = FLAG_NONE,
    };

    enum
    {
        MESSAGEID_SLEEP = 0,
        MESSAGEID_AWAKE,
        MESSAGEID_CATCHATTACK,
        MESSAGEID_ATTACKRESULT,
    };
};