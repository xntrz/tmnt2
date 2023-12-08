#pragma once


namespace PLAYERID
{
    enum VALUE
    {
        ID_INVALID = -1,
        
        ID_LEO = 0,
        ID_RAP,
        ID_MIC,
        ID_DON,
        ID_SLA,
        ID_CAS,
        ID_KAR,
        ID_SPL,

        ID_MAX,
        ID_START = ID_LEO,
    };

    const char* GetName(VALUE idPlayer);    
    bool IsImplemented(VALUE idPlayer);
    bool IsEnabled(VALUE idPlayer);
    RwRGBA GetColor(VALUE idPlayer);
};