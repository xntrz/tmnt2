#include "AreaInfo.hpp"
#include "StageInfo.hpp"
#include "WorldID.hpp"

#include "Game/System/Text/GameText.hpp"
#include "Game/Component/GameData/GameTime.hpp"


struct CAreaInfo::INFO
{
    GAMETEXT            m_AreaDispName;
    GAMETEXT            m_WorldDispName;
    const char*         m_pszEpisodeName;
    const char*         m_pszEpisodeNo;
    int32               m_iWindowNo;
    STAGEID::VALUE      m_StageID;
    int32               m_iStageNum;
    ANTIQUEID::VALUE    m_AntiqueID;
    uint32              m_uClearTimeBest;    // h:m:s
    uint32              m_uClearTimeWorst;   // h:m:s
};


/*static*/ CAreaInfo::INFO CAreaInfo::m_aNormalInfo[] =
{
    { GAMETEXT(0),       GAMETEXT(0),     "",                         "0-0",    0,  STAGEID::ID_NONE,       0,  ANTIQUEID::ID_NONE,     0,                      0                       },
    { GAMETEXT(0x1A),    GAMETEXT(0x60),  "Backyard",                 "0-1",    2,  STAGEID::ID_ST01T,      1,  ANTIQUEID::ID_AT01,     GAMETIME_HMS(0, 10, 0), GAMETIME_HMS(0, 30, 0), },
    { GAMETEXT(0x1B),    GAMETEXT(0x61),  "Rooftop",                  "0-2",    2,  STAGEID::ID_ST02N,      2,  ANTIQUEID::ID_AT02,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 10, 0), },
    { GAMETEXT(0x1C),    GAMETEXT(0x62),  "Home",                     "0-0",    0,  STAGEID::ID_ST03S,      1,  ANTIQUEID::ID_NONE,     0,                      0                       },
    { GAMETEXT(0x1D),    GAMETEXT(0x63),  "The cavern",               "1-1",    0,  STAGEID::ID_ST04N,      2,  ANTIQUEID::ID_AT03,     GAMETIME_HMS(0, 5, 30), GAMETIME_HMS(0, 10, 30),},
    { GAMETEXT(0x1E),    GAMETEXT(0x64),  "Secret hall",              "1-2",    0,  STAGEID::ID_ST05N,      1,  ANTIQUEID::ID_AT04,     GAMETIME_HMS(0, 4, 0),  GAMETIME_HMS(0, 10, 0), },
    { GAMETEXT(0x1F),    GAMETEXT(0x65),  "Ledge above",              "1-3",    0,  STAGEID::ID_ST06R,      1,  ANTIQUEID::ID_AT05,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 3, 0),  },
    { GAMETEXT(0x20),    GAMETEXT(0x66),  "Lobby",                    "2-1",    2,  STAGEID::ID_ST09N,      1,  ANTIQUEID::ID_AT06,     GAMETIME_HMS(0, 4, 0),  GAMETIME_HMS(0, 9, 0),  },
    { GAMETEXT(0x21),    GAMETEXT(0x67),  "Lager lab",                "2-2",    2,  STAGEID::ID_ST10N,      1,  ANTIQUEID::ID_AT07,     GAMETIME_HMS(0, 5, 30), GAMETIME_HMS(0, 5, 0),  },	// ??? in retail game best time is greater than worst
    { GAMETEXT(0x22),    GAMETEXT(0x68),  "Transmat chamber",         "2-3",    2,  STAGEID::ID_ST11J,      1,  ANTIQUEID::ID_AT08,     GAMETIME_HMS(0, 1, 50), GAMETIME_HMS(0, 2, 0),  },
    { GAMETEXT(0x23),    GAMETEXT(0x69),  "Frozen relic",             "3-1",    3,  STAGEID::ID_ST12N,      1,  ANTIQUEID::ID_AT09,     GAMETIME_HMS(0, 4, 30), GAMETIME_HMS(0, 11, 0), },
    { GAMETEXT(0x24),    GAMETEXT(0x6A),  "Glacial valley",           "3-2",    3,  STAGEID::ID_ST13R,      1,  ANTIQUEID::ID_AT10,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 3, 0),  },
    { GAMETEXT(0x25),    GAMETEXT(0x6B),  "Mt. zero",                 "3-3",    3,  STAGEID::ID_ST14N,      1,  ANTIQUEID::ID_AT11,     GAMETIME_HMS(0, 3, 30), GAMETIME_HMS(0, 7, 30), },
    { GAMETEXT(0x26),    GAMETEXT(0x6C),  "Alley way",                "4-1",    3,  STAGEID::ID_ST15N,      1,  ANTIQUEID::ID_AT12,     GAMETIME_HMS(0, 2, 30), GAMETIME_HMS(0, 5, 0),  },
    { GAMETEXT(0x28),    GAMETEXT(0x6E),  "The sewers under peblak",  "4-2",    0,  STAGEID::ID_ST17N,      1,  ANTIQUEID::ID_AT13,     GAMETIME_HMS(0, 2, 0),  GAMETIME_HMS(0, 3, 0),  },
    { GAMETEXT(0x29),    GAMETEXT(0x6F),  "Junction of the sewers",   "4-3",    0,  STAGEID::ID_ST18F,      2,  ANTIQUEID::ID_AT14,     GAMETIME_HMS(0, 4, 0),  GAMETIME_HMS(0, 7, 0),  },
    { GAMETEXT(0x2A),    GAMETEXT(0x70),  "Industrial plant",         "4-4",    0,  STAGEID::ID_ST19N,      1,  ANTIQUEID::ID_AT15,     GAMETIME_HMS(0, 2, 30), GAMETIME_HMS(0, 4, 30), },
    { GAMETEXT(0x2B),    GAMETEXT(0x71),  "Deepest industrial plant", "4-5",    0,  STAGEID::ID_ST20F,      2,  ANTIQUEID::ID_AT16,     GAMETIME_HMS(0, 3, 10), GAMETIME_HMS(0, 7, 0),  },
    { GAMETEXT(0x2C),    GAMETEXT(0x72),  "Spaceport inn",            "4-6",    0,  STAGEID::ID_ST21N,      2,  ANTIQUEID::ID_AT17,     GAMETIME_HMS(0, 4, 30), GAMETIME_HMS(0, 8, 0),  },
    { GAMETEXT(0x2D),    GAMETEXT(0x73),  "Wilderness",               "4-7",    0,  STAGEID::ID_ST22R,      1,  ANTIQUEID::ID_AT18,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 3, 0),  },
    { GAMETEXT(0x2E),    GAMETEXT(0x74),  "Forest",                   "4-8",    0,  STAGEID::ID_ST23N,      1,  ANTIQUEID::ID_AT19,     GAMETIME_HMS(0, 5, 0),  GAMETIME_HMS(0, 8, 0),  },
    { GAMETEXT(0x2F),    GAMETEXT(0x75),  "Forest clearing",          "4-9",    0,  STAGEID::ID_ST24N,      1,  ANTIQUEID::ID_AT20,     GAMETIME_HMS(0, 5, 30), GAMETIME_HMS(0, 10, 30),},
    { GAMETEXT(0x30),    GAMETEXT(0x76),  "Cargo bay",                "5-1",    0,  STAGEID::ID_ST25J,      1,  ANTIQUEID::ID_AT21,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 5, 0),  },
    { GAMETEXT(0x31),    GAMETEXT(0x77),  "Battlefield",              "5-4",    0,  STAGEID::ID_ST26N,      1,  ANTIQUEID::ID_AT22,     GAMETIME_HMS(0, 3, 30), GAMETIME_HMS(0, 5, 30), },
    { GAMETEXT(0x32),    GAMETEXT(0x78),  "Space dock",               "5-5",    0,  STAGEID::ID_ST27N,      1,  ANTIQUEID::ID_AT23,     GAMETIME_HMS(0, 5, 0),  GAMETIME_HMS(0, 8, 0),  },
    { GAMETEXT(0x33),    GAMETEXT(0x79),  "Engine compartment",       "5-2",    0,  STAGEID::ID_ST28N,      1,  ANTIQUEID::ID_AT24,     GAMETIME_HMS(0, 2, 0),  GAMETIME_HMS(0, 5, 30), },
    { GAMETEXT(0x34),    GAMETEXT(0x7A),  "Toricerator prison",       "5-3",    0,  STAGEID::ID_ST29N,      1,  ANTIQUEID::ID_AT25,     GAMETIME_HMS(0, 3, 30), GAMETIME_HMS(0, 8, 30), },
    { GAMETEXT(0x35),    GAMETEXT(0x7B),  "Underneath the arena",     "5-6",    0,  STAGEID::ID_ST30N,      2,  ANTIQUEID::ID_AT26,     GAMETIME_HMS(0, 3, 50), GAMETIME_HMS(0, 7, 0),  },
    { GAMETEXT(0x36),    GAMETEXT(0x7C),  "The tri-sports arena",     "5-7",    0,  STAGEID::ID_ST31NB,     1,  ANTIQUEID::ID_AT27,     GAMETIME_HMS(0, 2, 30), GAMETIME_HMS(0, 15, 0), },
    { GAMETEXT(0x37),    GAMETEXT(0x7D),  "Asteroid field",           "5-8",    0,  STAGEID::ID_ST32R,      1,  ANTIQUEID::ID_AT28,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 3, 0),  },
    { GAMETEXT(0x38),    GAMETEXT(0x7E),  "Shore",                    "6-1",    0,  STAGEID::ID_ST33J,      1,  ANTIQUEID::ID_AT29,     GAMETIME_HMS(0, 2, 50), GAMETIME_HMS(0, 6, 50), },
    { GAMETEXT(0x39),    GAMETEXT(0x7F),  "Foot's lair",              "6-4",    0,  STAGEID::ID_ST34N,      1,  ANTIQUEID::ID_AT30,     GAMETIME_HMS(0, 4, 0),  GAMETIME_HMS(0, 6, 30), },
    { GAMETEXT(0x3A),    GAMETEXT(0x80),  "A hidden small bay",       "6-5",    0,  STAGEID::ID_ST35N,      1,  ANTIQUEID::ID_AT31,     GAMETIME_HMS(0, 5, 30), GAMETIME_HMS(0, 8, 30), },
    { GAMETEXT(0x3B),    GAMETEXT(0x81),  "The burning village",      "6-2",    0,  STAGEID::ID_ST36N,      1,  ANTIQUEID::ID_AT32,     GAMETIME_HMS(0, 3, 50), GAMETIME_HMS(0, 7, 50), },
    { GAMETEXT(0x3C),    GAMETEXT(0x82),  "Bamboo forest",            "6-3",    0,  STAGEID::ID_ST37N,      1,  ANTIQUEID::ID_AT33,     GAMETIME_HMS(0, 4, 30), GAMETIME_HMS(0, 7, 0),  },
    { GAMETEXT(0x3D),    GAMETEXT(0x83),  "The cliff",                "6-6",    0,  STAGEID::ID_ST38NB,     1,  ANTIQUEID::ID_AT34,     GAMETIME_HMS(0, 1, 0),  GAMETIME_HMS(0, 5, 0),  },
    { GAMETEXT(0x3E),    GAMETEXT(0x84),  "Oracle pod chamber",       "6-7",    0,  STAGEID::ID_ST39N,      1,  ANTIQUEID::ID_AT35,     GAMETIME_HMS(0, 2, 30), GAMETIME_HMS(0, 4, 0),  },
    { GAMETEXT(0x3F),    GAMETEXT(0x85),  "Transmat chamber",         "6-8",    0,  STAGEID::ID_ST40OB,     1,  ANTIQUEID::ID_AT36,     GAMETIME_HMS(0, 4, 0),  GAMETIME_HMS(0, 6, 30), },
    { GAMETEXT(0x40),    GAMETEXT(0x86),  "The sewers",               "7-1",    1,  STAGEID::ID_ST41N,      1,  ANTIQUEID::ID_AT37,     GAMETIME_HMS(0, 3, 30), GAMETIME_HMS(0, 8, 0),  },
    { GAMETEXT(0x41),    GAMETEXT(0x87),  "Turtles old lair",         "7-2",    1,  STAGEID::ID_ST42NB,     1,  ANTIQUEID::ID_AT38,     GAMETIME_HMS(0, 3, 30), GAMETIME_HMS(0, 6, 0),  },
    { GAMETEXT(0x42),    GAMETEXT(0x88),  "Park",                     "8-1",    1,  STAGEID::ID_ST43N,      1,  ANTIQUEID::ID_AT39,     GAMETIME_HMS(0, 4, 0),  GAMETIME_HMS(0, 6, 30), },
    { GAMETEXT(0x43),    GAMETEXT(0x89),  "Bridge",                   "8-2",    1,  STAGEID::ID_ST44N,      2,  ANTIQUEID::ID_AT40,     GAMETIME_HMS(0, 3, 30), GAMETIME_HMS(0, 10, 0), },
    { GAMETEXT(0x44),    GAMETEXT(0x8A),  "Mean streets",             "9-1",    1,  STAGEID::ID_ST45N,      1,  ANTIQUEID::ID_AT41,     GAMETIME_HMS(0, 6, 0),  GAMETIME_HMS(0, 10, 0), },
    { GAMETEXT(0x45),    GAMETEXT(0x8B),  "The sewers",               "9-2",    1,  STAGEID::ID_ST46R,      1,  ANTIQUEID::ID_AT42,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 3, 0),  },
    { GAMETEXT(0x46),    GAMETEXT(0x8C),  "The sewers",               "9-3",    1,  STAGEID::ID_ST47O,      2,  ANTIQUEID::ID_AT43,     GAMETIME_HMS(0, 2, 30), GAMETIME_HMS(0, 5, 0),  },
    { GAMETEXT(0x47),    GAMETEXT(0x8D),  "Subway tunnel",            "10-1",   3,  STAGEID::ID_ST48N,      1,  ANTIQUEID::ID_AT44,     GAMETIME_HMS(0, 3, 45), GAMETIME_HMS(0, 6, 45), },
    { GAMETEXT(0x48),    GAMETEXT(0x8E),  "Collapsed area",           "10-2",   3,  STAGEID::ID_ST49N,      1,  ANTIQUEID::ID_AT45,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 6, 0),  },
    { GAMETEXT(0x49),    GAMETEXT(0x8F),  "Air",                      "10-3",   3,  STAGEID::ID_ST50NB,     1,  ANTIQUEID::ID_AT46,     GAMETIME_HMS(0, 1, 50), GAMETIME_HMS(0, 4, 0),  },
    { GAMETEXT(0x4B),    GAMETEXT(0x91),  "Docks",                    "11-1",   0,  STAGEID::ID_ST52F,      2,  ANTIQUEID::ID_AT47,     GAMETIME_HMS(0, 5, 0),  GAMETIME_HMS(0, 9, 0),  },
    { GAMETEXT(0x4C),    GAMETEXT(0x92),  "Storage",                  "11-2",   0,  STAGEID::ID_ST53F,      2,  ANTIQUEID::ID_AT48,     GAMETIME_HMS(0, 5, 0),  GAMETIME_HMS(0, 9, 0),  },
    { GAMETEXT(0x4D),    GAMETEXT(0x93),  "Foot lab",                 "11-3",   0,  STAGEID::ID_ST54F,      2,  ANTIQUEID::ID_AT49,     GAMETIME_HMS(0, 9, 0),  GAMETIME_HMS(0, 8, 0),  },
    { GAMETEXT(0x4E),    GAMETEXT(0x94),  "Engine room",              "11-4",   0,  STAGEID::ID_ST55F,      2,  ANTIQUEID::ID_AT50,     GAMETIME_HMS(0, 4, 40), GAMETIME_HMS(0, 7, 0),  },
    { GAMETEXT(0x4F),    GAMETEXT(0x95),  "Weapons depot",            "11-5",   1,  STAGEID::ID_ST56NB,     1,  ANTIQUEID::ID_AT51,     GAMETIME_HMS(0, 1, 50), GAMETIME_HMS(0, 4, 0),  },
    { GAMETEXT(0x50),    GAMETEXT(0x96),  "The freighter bridge",     "11-6",   1,  STAGEID::ID_ST57NB,     1,  ANTIQUEID::ID_AT52,     GAMETIME_HMS(0, 1, 50), GAMETIME_HMS(0, 4, 0),  },
    { GAMETEXT(0x51),    GAMETEXT(0x97),  "Deck",                     "11-7",   1,  STAGEID::ID_ST58OB1,    2,  ANTIQUEID::ID_AT53,     GAMETIME_HMS(0, 3, 0),  GAMETIME_HMS(0, 7, 0),  },
};


/*static*/ CAreaInfo::INFO CAreaInfo::m_aNexusInfo[] =
{
    { GAMETEXT(0x55),    GAMETEXT(0x98),  "Kitty open",               "0-0",    0,  STAGEID::ID_ST60X_A01,  10, ANTIQUEID::ID_NONE,     0,                      0,                      },
    { GAMETEXT(0x56),    GAMETEXT(0x98),  "Monster open",             "0-1",    1,  STAGEID::ID_ST60X_B01,  10, ANTIQUEID::ID_NONE,     0,                      0,                      },
    { GAMETEXT(0x57),    GAMETEXT(0x98),  "Foot battle",              "0-2",    1,  STAGEID::ID_ST60X_C01,  10, ANTIQUEID::ID_AT54,     0,                      0,                      },
    { GAMETEXT(0x58),    GAMETEXT(0x98),  "Battle nexus",             "0-3",    1,  STAGEID::ID_ST60X_D01,  10, ANTIQUEID::ID_AT55,     0,                      0,                      },
};


/*static*/ CAreaInfo::INFO& CAreaInfo::Info(AREAID::VALUE idArea)
{
    if ((idArea >= AREAID::NEXUSSTART) && (idArea < AREAID::NEXUSMAX))
    {
        return m_aNexusInfo[idArea - AREAID::NEXUSSTART];
    }
    else if ((idArea >= AREAID::NORMALSTART) && (idArea < AREAID::NORMALMAX))
    {
        return m_aNormalInfo[idArea];
    }
    else
    {
        return m_aNormalInfo[AREAID::ID_NONE];
    };
};


/*static*/ const wchar* CAreaInfo::GetDispName(AREAID::VALUE idArea)
{
    return CGameText::GetText(Info(idArea).m_AreaDispName);
};


/*static*/ const wchar* CAreaInfo::GetDispWorldName(AREAID::VALUE idArea)
{
    return CGameText::GetText(Info(idArea).m_WorldDispName);
};


/*static*/ const char* CAreaInfo::GetEpisode(AREAID::VALUE idArea)
{
    return Info(idArea).m_pszEpisodeNo;
};


/*static*/ int32 CAreaInfo::GetWindowNo(AREAID::VALUE idArea)
{
    return Info(idArea).m_iWindowNo;
};


/*static*/ STAGEID::VALUE CAreaInfo::GetStageID(AREAID::VALUE idArea, int32 index)
{
    return STAGEID::VALUE(Info(idArea).m_StageID + index);
};


/*static*/ int32 CAreaInfo::IndexOfStage(AREAID::VALUE idArea, STAGEID::VALUE idStage)
{
    int32 iIndex = idStage - Info(idArea).m_StageID;
    
    if ((iIndex < 0) || (iIndex >= Info(idArea).m_iStageNum))
        return -1;
    
    return iIndex;
};


/*static*/ int32 CAreaInfo::GetStageNum(AREAID::VALUE idArea)
{
    return Info(idArea).m_iStageNum;
};


/*static*/ ANTIQUEID::VALUE CAreaInfo::GetAntiqueID(AREAID::VALUE idArea)
{
    return Info(idArea).m_AntiqueID;
};


/*static*/ AREAID::VALUE CAreaInfo::FindAntiqueArea(ANTIQUEID::VALUE idAntique)
{
    for (int32 i = 0; i < AREAID::SELECTABLEMAX; ++i)
    {
        if (Info(AREAID::VALUE(i)).m_AntiqueID == idAntique)
            return AREAID::VALUE(i);
    };

    for (int32 i = AREAID::NEXUSSTART; i < AREAID::NEXUSMAX; ++i)
    {
        if (Info(AREAID::VALUE(i)).m_AntiqueID == idAntique)
            return AREAID::VALUE(i);
    };

    return AREAID::ID_NONE;
};


/*static*/ MOVIEID::VALUE CAreaInfo::GetAreaOutMovieID(AREAID::VALUE idArea)
{
    switch (idArea)
    {
    case AREAID::ID_AREA02:
        return MOVIEID::VALUE(5);

    case AREAID::ID_AREA06:
        return MOVIEID::VALUE(9);

    case AREAID::ID_AREA11:
        return MOVIEID::VALUE(12);

    case AREAID::ID_AREA14:
        return MOVIEID::VALUE(14);

    case AREAID::ID_AREA21:
        return MOVIEID::VALUE(23);

    case AREAID::ID_AREA24:
        return MOVIEID::VALUE(25);

    case AREAID::ID_AREA27:
    case AREAID::ID_AREA28:
        return MOVIEID::VALUE(27);

    case AREAID::ID_AREA29:
        return MOVIEID::VALUE(29);

    case AREAID::ID_AREA30:
        return MOVIEID::VALUE(31);

    case AREAID::ID_AREA31:
        return MOVIEID::VALUE(33);

    case AREAID::ID_AREA32:
        return MOVIEID::VALUE(34);

    case AREAID::ID_AREA38:
        return MOVIEID::VALUE(37);

    case AREAID::ID_AREA40:
        return MOVIEID::VALUE(4);

    case AREAID::ID_AREA42:
        return MOVIEID::VALUE(43);

    case AREAID::ID_AREA44:
        return MOVIEID::VALUE(46);

    case AREAID::ID_AREA45:
        return MOVIEID::VALUE(48);

    case AREAID::ID_AREA46:
        return MOVIEID::VALUE(49);

    case AREAID::ID_AREA47:
        return MOVIEID::VALUE(51);

    case AREAID::ID_AREA50:
        return MOVIEID::VALUE(55);

    case AREAID::ID_AREA57:
        return MOVIEID::VALUE(67);

    default:
        return MOVIEID::ID_MAX;
    };
};


/*static*/ bool CAreaInfo::IsPlayMovieOfAreaOut(AREAID::VALUE idArea)
{
    return (GetAreaOutMovieID(idArea) != MOVIEID::ID_MAX);
};


/*static*/ GAMETYPES::NEXUSID CAreaInfo::GetNexusID(AREAID::VALUE idArea)
{
    if ((idArea >= AREAID::NEXUSSTART) && (idArea < AREAID::NEXUSMAX))
        return GAMETYPES::NEXUSID(idArea - AREAID::NEXUSSTART);
    
    return GAMETYPES::NEXUSID_NUM;
};


/*static*/ uint32 CAreaInfo::GetBestRankClearTime(AREAID::VALUE idArea)
{
    return Info(idArea).m_uClearTimeBest;
};


/*static*/ uint32 CAreaInfo::GetWorstRankClearTime(AREAID::VALUE idArea)
{
    return Info(idArea).m_uClearTimeWorst;
};


/*static*/ WORLDID::VALUE CAreaInfo::GetWorldNo(AREAID::VALUE idArea)
{
    static const WORLDID::VALUE AreaToWorldMap[] =
    {
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_JPN,
        WORLDID::ID_JPN,
        WORLDID::ID_JPN,
        WORLDID::ID_JPN,
        WORLDID::ID_JPN,
        WORLDID::ID_JPN,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_FNY,
        WORLDID::ID_FNY,
        WORLDID::ID_FNY,
        WORLDID::ID_KUR,
        WORLDID::ID_KUR,
        WORLDID::ID_KUR,
        WORLDID::ID_KUR,
        WORLDID::ID_KUR,
        WORLDID::ID_KUR,
        WORLDID::ID_KUR,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_DHO,
        WORLDID::ID_FNY,
        WORLDID::ID_KUR,
        WORLDID::ID_MNY,
        WORLDID::ID_DHO,
        WORLDID::ID_TRI,
        WORLDID::ID_JPN,
        WORLDID::ID_FNY,
        WORLDID::ID_KUR,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_MNY,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_DHO,
        WORLDID::ID_TRI,
        WORLDID::ID_TRI,
        WORLDID::ID_JPN,
        WORLDID::ID_JPN,
        WORLDID::ID_FNY,
        WORLDID::ID_KUR,
    };

    static_assert(COUNT_OF(AreaToWorldMap) == AREAID::SELECTABLEMAX, "update me");

    if ((idArea >= 0) && (idArea < AREAID::SELECTABLEMAX))
        return AreaToWorldMap[idArea];
    
    return WORLDID::ID_MAX;
};


/*static*/ int32 CAreaInfo::GetDiscNo(AREAID::VALUE idArea)
{
    return 1;
};