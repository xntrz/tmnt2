#include "PlayerID.hpp"

#include "Game/Component/GameData/GameData.hpp"


namespace
{
    struct PLAYERINFO
    {
        const char* m_pszName;
        bool m_bImplemented;
        RwRGBA m_Color;
    };


    static const PLAYERINFO s_aPlayerInfoList[] =
    {
        { "leonardo",       true,   { 0x00, 0x98, 0xFF, 0xFF } },
        { "raphael",        true,   { 0xCA, 0x00, 0x10, 0xFF } },
        { "michelangero",   true,   { 0xFF, 0x7C, 0x00, 0xFF } },
        { "donatello",      true,   { 0x85, 0x3D, 0xD7, 0xFF } },
        { "slashuur",       true,   { 0x00, 0x98, 0xFF, 0xFF } },
        { "casey",          true,   { 0xCA, 0x00, 0x10, 0xFF } },
        { "karai",          true,   { 0xFF, 0x7C, 0x00, 0xFF } },
        { "splinter",       true,   { 0x85, 0x3D, 0xD7, 0xFF } },
    };


    static_assert(COUNT_OF(s_aPlayerInfoList) == PLAYERID::ID_MAX, "update me");

    
    const PLAYERINFO& PlayerInfo(PLAYERID::VALUE idPlayer)
    {
        ASSERT(idPlayer >= 0 && idPlayer < COUNT_OF(s_aPlayerInfoList));
        return s_aPlayerInfoList[idPlayer];
    };
};


namespace PLAYERID
{
    const char* GetName(VALUE idPlayer)
    {
        return PlayerInfo(idPlayer).m_pszName;
    };


    bool IsImplemented(VALUE idPlayer)
    {
        return PlayerInfo(idPlayer).m_bImplemented;
    };

    
    bool IsEnabled(VALUE idPlayer)
    {
        bool bResult = false;
        
        switch (idPlayer)
        {
        case ID_LEO:
        case ID_RAP:
        case ID_MIC:
        case ID_DON:
            bResult = true;
            break;

        case ID_SLA:
            bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHARACTER_SLASHUUR);
            break;
            
        case ID_CAS:
            bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHARACTER_CASEY);
            break;
            
        case ID_KAR:
            bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHARACTER_KARAI);
            break;
            
        case ID_SPL:
            bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHARACTER_SPLINTER);
            break;

        default:
            ASSERT(false);
            break;
        };

        if (bResult)
            bResult = IsImplemented(idPlayer);

        return bResult;
    };

    
    RwRGBA GetColor(VALUE idPlayer)
    {
        return PlayerInfo(idPlayer).m_Color;
    };
};
