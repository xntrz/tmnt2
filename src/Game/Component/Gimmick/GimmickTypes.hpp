#pragma once


namespace GIMMICKTYPES
{
    enum EVENTTYPE
    {
        EVENTTYPE_NONE = 0,
        EVENTTYPE_ACTIVATE,
        EVENTTYPE_INACTIVATE,
        EVENTTYPE_ENEMYKILL,
    };

    enum QUERY
    {
        QUERY_GEN_ENEMY_MAX = 0,
        QUERY_GEN_DEAD_ENEMY_NUM,
        QUERY_SYS_SWITCH,
        QUERY_EVE_BROKEN,
    };

    enum FEATURE
    {
        FEATURE_NONE    = 0x0,
        FEATURE_HOMING  = 0x1,
        FEATURE_SLEEP   = 0x2,
    };

    enum
    {
        MESSAGEID_RECVEVENT = 275,
    };

    struct EVENT_ARGS
    {
        char m_szSender[16];
        EVENTTYPE m_type;
    };
};