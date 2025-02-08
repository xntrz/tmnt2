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

    enum FEATURE : uint32
    {
        FEATURE_NONE        = 0,
        FEATURE_HOMING      = (1 << 0), // gimmick can be used for shot direction correction (see ShotManager)
        FEATURE_ABLE_SLEEP  = (1 << 1), // gimmick is able to enter gameobject sleep state (see GamePropertyObj active object update)
    };

    DEFINE_ENUM_FLAG_OPS(FEATURE);

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