#pragma once


namespace PLAYERID
{
    enum VALUE
    {
        ID_LEO = 0,
        ID_RAPH,
        ID_MIC,
        ID_DON,
        ID_SLASHUUR,
        ID_CASEY,
        ID_KARAI,
        ID_SPLINTER,

        ID_MAX,
        ID_INVALID = ID_MAX,
        ID_START = ID_LEO,
        ID_RANDOM = -1,
    };

    const char* GetName(VALUE idPlayer);    
    bool IsImplemented(VALUE idPlayer);
    bool IsEnabled(VALUE idPlayer);
    RwRGBA GetColor(VALUE idPlayer);
};