#include "AreaInfo.hpp"
#include "StageInfo.hpp"
#include "WorldID.hpp"

#include "Game/System/Text/GameText.hpp"
#include "Game/Component/GameData/GameTime.hpp"


struct AREAINFO
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


static const AREAINFO s_aNormalAreaInfo[] =
{
    /*  0 -- ID_NONE            */  { GAMETEXT_EMPTY,         GAMETEXT_EMPTY,             "",                         "0-0",    0,  STAGEID::ID_NONE,       0,  ANTIQUEID::ID_NONE,     0,                        0                           },
    /*  1 -- ID_AREA01  0-1     */  { GAMETEXT_AREA01_NAME,   GAMETEXT_AREA01_WORLD,      "Backyard",                 "0-1",    2,  STAGEID::ID_ST01T,      1,  ANTIQUEID::ID_AT01,     GAMETIME_HMS(0, 10,  0),  GAMETIME_HMS(0, 30, 0),     },
    /*  2 -- ID_AREA02  0-2     */  { GAMETEXT_AREA02_NAME,   GAMETEXT_AREA02_WORLD,      "Rooftop",                  "0-2",    2,  STAGEID::ID_ST02N,      2,  ANTIQUEID::ID_AT02,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0, 10, 0),     },
    /*  3 -- ID_AREA03  0-0     */  { GAMETEXT_AREA03_NAME,   GAMETEXT_AREA03_WORLD,      "Home",                     "0-0",    0,  STAGEID::ID_ST03S,      1,  ANTIQUEID::ID_NONE,     0,                        0                           },
    /*  4 -- ID_AREA04  1-1     */  { GAMETEXT_AREA04_NAME,   GAMETEXT_AREA04_WORLD,      "The cavern",               "1-1",    0,  STAGEID::ID_ST04N,      2,  ANTIQUEID::ID_AT03,     GAMETIME_HMS(0,  5, 30),  GAMETIME_HMS(0, 10, 30),    },
    /*  5 -- ID_AREA05  1-2     */  { GAMETEXT_AREA05_NAME,   GAMETEXT_AREA05_WORLD,      "Secret hall",              "1-2",    0,  STAGEID::ID_ST05N,      1,  ANTIQUEID::ID_AT04,     GAMETIME_HMS(0,  4,  0),  GAMETIME_HMS(0, 10,  0),    },
    /*  6 -- ID_AREA06  1-3     */  { GAMETEXT_AREA06_NAME,   GAMETEXT_AREA06_WORLD,      "Ledge above",              "1-3",    0,  STAGEID::ID_ST06R,      1,  ANTIQUEID::ID_AT05,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  3,  0),    },
    /*  7 -- ID_AREA09  2-1     */  { GAMETEXT_AREA09_NAME,   GAMETEXT_AREA09_WORLD,      "Lobby",                    "2-1",    2,  STAGEID::ID_ST09N,      1,  ANTIQUEID::ID_AT06,     GAMETIME_HMS(0,  4,  0),  GAMETIME_HMS(0,  9,  0),    },
    /*  8 -- ID_AREA10  2-2     */  { GAMETEXT_AREA10_NAME,   GAMETEXT_AREA10_WORLD,      "Lager lab",                "2-2",    2,  STAGEID::ID_ST10N,      1,  ANTIQUEID::ID_AT07,     GAMETIME_HMS(0,  5, 30),  GAMETIME_HMS(0,  5,  0),    },	// ??? in retail game best time is greater than worst
    /*  9 -- ID_AREA11  2-3     */  { GAMETEXT_AREA11_NAME,   GAMETEXT_AREA11_WORLD,      "Transmat chamber",         "2-3",    2,  STAGEID::ID_ST11J,      1,  ANTIQUEID::ID_AT08,     GAMETIME_HMS(0,  1, 50),  GAMETIME_HMS(0,  2,  0),    },
    /* 10 -- ID_AREA12  3-1     */  { GAMETEXT_AREA12_NAME,   GAMETEXT_AREA12_WORLD,      "Frozen relic",             "3-1",    3,  STAGEID::ID_ST12N,      1,  ANTIQUEID::ID_AT09,     GAMETIME_HMS(0,  4, 30),  GAMETIME_HMS(0, 11,  0),    },
    /* 11 -- ID_AREA13  3-2     */  { GAMETEXT_AREA13_NAME,   GAMETEXT_AREA13_WORLD,      "Glacial valley",           "3-2",    3,  STAGEID::ID_ST13R,      1,  ANTIQUEID::ID_AT10,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  3,  0),    },
    /* 12 -- ID_AREA14  3-3     */  { GAMETEXT_AREA14_NAME,   GAMETEXT_AREA14_WORLD,      "Mt. zero",                 "3-3",    3,  STAGEID::ID_ST14N,      1,  ANTIQUEID::ID_AT11,     GAMETIME_HMS(0,  3, 30),  GAMETIME_HMS(0,  7, 30),    },
    /* 13 -- ID_AREA15  4-1     */  { GAMETEXT_AREA15_NAME_1, GAMETEXT_AREA15_WORLD_1,    "Alley way",                "4-1",    3,  STAGEID::ID_ST15N,      1,  ANTIQUEID::ID_AT12,     GAMETIME_HMS(0,  2, 30),  GAMETIME_HMS(0,  5,  0),    },
    /* 14 -- ID_AREA17  4-2     */  { GAMETEXT_AREA17_NAME,   GAMETEXT_AREA17_WORLD,      "The sewers under peblak",  "4-2",    0,  STAGEID::ID_ST17N,      1,  ANTIQUEID::ID_AT13,     GAMETIME_HMS(0,  2,  0),  GAMETIME_HMS(0,  3,  0),    },
    /* 15 -- ID_AREA18  4-3     */  { GAMETEXT_AREA18_NAME,   GAMETEXT_AREA18_WORLD,      "Junction of the sewers",   "4-3",    0,  STAGEID::ID_ST18F,      2,  ANTIQUEID::ID_AT14,     GAMETIME_HMS(0,  4,  0),  GAMETIME_HMS(0,  7,  0),    },
    /* 16 -- ID_AREA19  4-4     */  { GAMETEXT_AREA19_NAME,   GAMETEXT_AREA19_WORLD,      "Industrial plant",         "4-4",    0,  STAGEID::ID_ST19N,      1,  ANTIQUEID::ID_AT15,     GAMETIME_HMS(0,  2, 30),  GAMETIME_HMS(0,  4, 30),    },
    /* 17 -- ID_AREA20  4-5     */  { GAMETEXT_AREA20_NAME,   GAMETEXT_AREA20_WORLD,      "Deepest industrial plant", "4-5",    0,  STAGEID::ID_ST20F,      2,  ANTIQUEID::ID_AT16,     GAMETIME_HMS(0,  3, 10),  GAMETIME_HMS(0,  7,  0),    },
    /* 18 -- ID_AREA21  4-6     */  { GAMETEXT_AREA21_NAME,   GAMETEXT_AREA21_WORLD,      "Spaceport inn",            "4-6",    0,  STAGEID::ID_ST21N,      2,  ANTIQUEID::ID_AT17,     GAMETIME_HMS(0,  4, 30),  GAMETIME_HMS(0,  8,  0),    },
    /* 19 -- ID_AREA22  4-7     */  { GAMETEXT_AREA22_NAME,   GAMETEXT_AREA22_WORLD,      "Wilderness",               "4-7",    0,  STAGEID::ID_ST22R,      1,  ANTIQUEID::ID_AT18,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  3,  0),    },
    /* 20 -- ID_AREA23  4-8     */  { GAMETEXT_AREA23_NAME,   GAMETEXT_AREA23_WORLD,      "Forest",                   "4-8",    0,  STAGEID::ID_ST23N,      1,  ANTIQUEID::ID_AT19,     GAMETIME_HMS(0,  5,  0),  GAMETIME_HMS(0,  8,  0),    },
    /* 21 -- ID_AREA24  4-9     */  { GAMETEXT_AREA24_NAME,   GAMETEXT_AREA24_WORLD,      "Forest clearing",          "4-9",    0,  STAGEID::ID_ST24N,      1,  ANTIQUEID::ID_AT20,     GAMETIME_HMS(0,  5, 30),  GAMETIME_HMS(0, 10, 30),    },
    /* 22 -- ID_AREA25  5-1     */  { GAMETEXT_AREA25_NAME,   GAMETEXT_AREA25_WORLD,      "Cargo bay",                "5-1",    0,  STAGEID::ID_ST25J,      1,  ANTIQUEID::ID_AT21,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  5,  0),    },
    /* 23 -- ID_AREA26  5-4     */  { GAMETEXT_AREA26_NAME,   GAMETEXT_AREA26_WORLD,      "Battlefield",              "5-4",    0,  STAGEID::ID_ST26N,      1,  ANTIQUEID::ID_AT22,     GAMETIME_HMS(0,  3, 30),  GAMETIME_HMS(0,  5, 30),    },
    /* 24 -- ID_AREA27  5-5     */  { GAMETEXT_AREA27_NAME,   GAMETEXT_AREA27_WORLD,      "Space dock",               "5-5",    0,  STAGEID::ID_ST27N,      1,  ANTIQUEID::ID_AT23,     GAMETIME_HMS(0,  5,  0),  GAMETIME_HMS(0,  8,  0),    },
    /* 25 -- ID_AREA28  5-2     */  { GAMETEXT_AREA28_NAME,   GAMETEXT_AREA28_WORLD,      "Engine compartment",       "5-2",    0,  STAGEID::ID_ST28N,      1,  ANTIQUEID::ID_AT24,     GAMETIME_HMS(0,  2,  0),  GAMETIME_HMS(0,  5, 30),    },
    /* 26 -- ID_AREA29  5-3     */  { GAMETEXT_AREA29_NAME,   GAMETEXT_AREA29_WORLD,      "Toricerator prison",       "5-3",    0,  STAGEID::ID_ST29N,      1,  ANTIQUEID::ID_AT25,     GAMETIME_HMS(0,  3, 30),  GAMETIME_HMS(0,  8, 30),    },
    /* 27 -- ID_AREA30  5-6     */  { GAMETEXT_AREA30_NAME,   GAMETEXT_AREA30_WORLD,      "Underneath the arena",     "5-6",    0,  STAGEID::ID_ST30N,      2,  ANTIQUEID::ID_AT26,     GAMETIME_HMS(0,  3, 50),  GAMETIME_HMS(0,  7,  0),    },
    /* 28 -- ID_AREA31  5-7     */  { GAMETEXT_AREA31_NAME,   GAMETEXT_AREA31_WORLD,      "The tri-sports arena",     "5-7",    0,  STAGEID::ID_ST31NB,     1,  ANTIQUEID::ID_AT27,     GAMETIME_HMS(0,  2, 30),  GAMETIME_HMS(0, 15,  0),    },
    /* 29 -- ID_AREA32  5-8     */  { GAMETEXT_AREA32_NAME,   GAMETEXT_AREA32_WORLD,      "Asteroid field",           "5-8",    0,  STAGEID::ID_ST32R,      1,  ANTIQUEID::ID_AT28,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  3,  0),    },
    /* 30 -- ID_AREA33  6-1     */  { GAMETEXT_AREA33_NAME,   GAMETEXT_AREA33_WORLD,      "Shore",                    "6-1",    0,  STAGEID::ID_ST33J,      1,  ANTIQUEID::ID_AT29,     GAMETIME_HMS(0,  2, 50),  GAMETIME_HMS(0,  6, 50),    },
    /* 31 -- ID_AREA34  6-4     */  { GAMETEXT_AREA34_NAME,   GAMETEXT_AREA34_WORLD,      "Foot's lair",              "6-4",    0,  STAGEID::ID_ST34N,      1,  ANTIQUEID::ID_AT30,     GAMETIME_HMS(0,  4,  0),  GAMETIME_HMS(0,  6, 30),    },
    /* 32 -- ID_AREA35  6-5     */  { GAMETEXT_AREA35_NAME,   GAMETEXT_AREA35_WORLD,      "A hidden small bay",       "6-5",    0,  STAGEID::ID_ST35N,      1,  ANTIQUEID::ID_AT31,     GAMETIME_HMS(0,  5, 30),  GAMETIME_HMS(0,  8, 30),    },
    /* 33 -- ID_AREA36  6-2     */  { GAMETEXT_AREA36_NAME,   GAMETEXT_AREA36_WORLD,      "The burning village",      "6-2",    0,  STAGEID::ID_ST36N,      1,  ANTIQUEID::ID_AT32,     GAMETIME_HMS(0,  3, 50),  GAMETIME_HMS(0,  7, 50),    },
    /* 34 -- ID_AREA37  6-3     */  { GAMETEXT_AREA37_NAME,   GAMETEXT_AREA37_WORLD,      "Bamboo forest",            "6-3",    0,  STAGEID::ID_ST37N,      1,  ANTIQUEID::ID_AT33,     GAMETIME_HMS(0,  4, 30),  GAMETIME_HMS(0,  7,  0),    },
    /* 35 -- ID_AREA38  6-6     */  { GAMETEXT_AREA38_NAME,   GAMETEXT_AREA38_WORLD,      "The cliff",                "6-6",    0,  STAGEID::ID_ST38NB,     1,  ANTIQUEID::ID_AT34,     GAMETIME_HMS(0,  1,  0),  GAMETIME_HMS(0,  5,  0),    },
    /* 36 -- ID_AREA39  6-7     */  { GAMETEXT_AREA39_NAME,   GAMETEXT_AREA39_WORLD,      "Oracle pod chamber",       "6-7",    0,  STAGEID::ID_ST39N,      1,  ANTIQUEID::ID_AT35,     GAMETIME_HMS(0,  2, 30),  GAMETIME_HMS(0,  4,  0),    },
    /* 37 -- ID_AREA40  6-8     */  { GAMETEXT_AREA40_NAME,   GAMETEXT_AREA40_WORLD,      "Transmat chamber",         "6-8",    0,  STAGEID::ID_ST40OB,     1,  ANTIQUEID::ID_AT36,     GAMETIME_HMS(0,  4,  0),  GAMETIME_HMS(0,  6, 30),    },
    /* 38 -- ID_AREA41  7-1     */  { GAMETEXT_AREA41_NAME,   GAMETEXT_AREA41_WORLD,      "The sewers",               "7-1",    1,  STAGEID::ID_ST41N,      1,  ANTIQUEID::ID_AT37,     GAMETIME_HMS(0,  3, 30),  GAMETIME_HMS(0,  8,  0),    },
    /* 39 -- ID_AREA42  7-2     */  { GAMETEXT_AREA42_NAME,   GAMETEXT_AREA42_WORLD,      "Turtles old lair",         "7-2",    1,  STAGEID::ID_ST42NB,     1,  ANTIQUEID::ID_AT38,     GAMETIME_HMS(0,  3, 30),  GAMETIME_HMS(0,  6,  0),    },
    /* 40 -- ID_AREA43  8-1     */  { GAMETEXT_AREA43_NAME,   GAMETEXT_AREA43_WORLD,      "Park",                     "8-1",    1,  STAGEID::ID_ST43N,      1,  ANTIQUEID::ID_AT39,     GAMETIME_HMS(0,  4,  0),  GAMETIME_HMS(0,  6, 30),    },
    /* 41 -- ID_AREA44  8-2     */  { GAMETEXT_AREA44_NAME,   GAMETEXT_AREA44_WORLD,      "Bridge",                   "8-2",    1,  STAGEID::ID_ST44N,      2,  ANTIQUEID::ID_AT40,     GAMETIME_HMS(0,  3, 30),  GAMETIME_HMS(0, 10,  0),    },
    /* 42 -- ID_AREA45  9-1     */  { GAMETEXT_AREA45_NAME,   GAMETEXT_AREA45_WORLD,      "Mean streets",             "9-1",    1,  STAGEID::ID_ST45N,      1,  ANTIQUEID::ID_AT41,     GAMETIME_HMS(0,  6,  0),  GAMETIME_HMS(0, 10,  0),    },
    /* 43 -- ID_AREA46  9-2     */  { GAMETEXT_AREA46_NAME,   GAMETEXT_AREA46_WORLD,      "The sewers",               "9-2",    1,  STAGEID::ID_ST46R,      1,  ANTIQUEID::ID_AT42,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  3,  0),    },
    /* 44 -- ID_AREA47  9-3     */  { GAMETEXT_AREA47_NAME,   GAMETEXT_AREA47_WORLD,      "The sewers",               "9-3",    1,  STAGEID::ID_ST47O,      2,  ANTIQUEID::ID_AT43,     GAMETIME_HMS(0,  2, 30),  GAMETIME_HMS(0,  5,  0),    },
    /* 45 -- ID_AREA48  10-1    */  { GAMETEXT_AREA48_NAME,   GAMETEXT_AREA48_WORLD,      "Subway tunnel",            "10-1",   3,  STAGEID::ID_ST48N,      1,  ANTIQUEID::ID_AT44,     GAMETIME_HMS(0,  3, 45),  GAMETIME_HMS(0,  6, 45),    },
    /* 46 -- ID_AREA49  10-2    */  { GAMETEXT_AREA49_NAME,   GAMETEXT_AREA49_WORLD,      "Collapsed area",           "10-2",   3,  STAGEID::ID_ST49N,      1,  ANTIQUEID::ID_AT45,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  6,  0),    },
    /* 47 -- ID_AREA50  10-3    */  { GAMETEXT_AREA50_NAME,   GAMETEXT_AREA50_WORLD,      "Air",                      "10-3",   3,  STAGEID::ID_ST50NB,     1,  ANTIQUEID::ID_AT46,     GAMETIME_HMS(0,  1, 50),  GAMETIME_HMS(0,  4,  0),    },
    /* 48 -- ID_AREA52  11-1    */  { GAMETEXT_AREA52_NAME,   GAMETEXT_AREA52_WORLD,      "Docks",                    "11-1",   0,  STAGEID::ID_ST52F,      2,  ANTIQUEID::ID_AT47,     GAMETIME_HMS(0,  5,  0),  GAMETIME_HMS(0,  9,  0),    },
    /* 49 -- ID_AREA53  11-2    */  { GAMETEXT_AREA53_NAME,   GAMETEXT_AREA53_WORLD,      "Storage",                  "11-2",   0,  STAGEID::ID_ST53F,      2,  ANTIQUEID::ID_AT48,     GAMETIME_HMS(0,  5,  0),  GAMETIME_HMS(0,  9,  0),    },
    /* 50 -- ID_AREA54  11-3    */  { GAMETEXT_AREA54_NAME,   GAMETEXT_AREA54_WORLD,      "Foot lab",                 "11-3",   0,  STAGEID::ID_ST54F,      2,  ANTIQUEID::ID_AT49,     GAMETIME_HMS(0,  9,  0),  GAMETIME_HMS(0,  8,  0),    },
    /* 51 -- ID_AREA55  11-4    */  { GAMETEXT_AREA55_NAME,   GAMETEXT_AREA55_WORLD,      "Engine room",              "11-4",   0,  STAGEID::ID_ST55F,      2,  ANTIQUEID::ID_AT50,     GAMETIME_HMS(0,  4, 40),  GAMETIME_HMS(0,  7,  0),    },
    /* 52 -- ID_AREA56  11-5    */  { GAMETEXT_AREA56_NAME,   GAMETEXT_AREA56_WORLD,      "Weapons depot",            "11-5",   1,  STAGEID::ID_ST56NB,     1,  ANTIQUEID::ID_AT51,     GAMETIME_HMS(0,  1, 50),  GAMETIME_HMS(0,  4,  0),    },
    /* 53 -- ID_AREA57  11-6    */  { GAMETEXT_AREA57_NAME,   GAMETEXT_AREA57_WORLD,      "The freighter bridge",     "11-6",   1,  STAGEID::ID_ST57NB,     1,  ANTIQUEID::ID_AT52,     GAMETIME_HMS(0,  1, 50),  GAMETIME_HMS(0,  4,  0),    },
    /* 54 -- ID_AREA58  11-7    */  { GAMETEXT_AREA58_NAME,   GAMETEXT_AREA58_WORLD,      "Deck",                     "11-7",   1,  STAGEID::ID_ST58OB1,    2,  ANTIQUEID::ID_AT53,     GAMETIME_HMS(0,  3,  0),  GAMETIME_HMS(0,  7,  0),    },
};


static const AREAINFO s_aNexusAreaInfo[] =
{
    /*  0 -- ID_AREA60_A  0-1   */  { GAMETEXT_AREA60A_NAME,  GAMETEXT_AREA60_WORLD,      "Kitty open",               "0-0",    0,  STAGEID::ID_ST60X_A01, 10, ANTIQUEID::ID_NONE,     GAMETIME_HMS(0, 0, 0),    GAMETIME_HMS(0, 0, 0),      },
    /*  1 -- ID_AREA60_B  0-2   */  { GAMETEXT_AREA60B_NAME,  GAMETEXT_AREA60_WORLD,      "Monster open",             "0-1",    1,  STAGEID::ID_ST60X_B01, 10, ANTIQUEID::ID_NONE,     GAMETIME_HMS(0, 0, 0),    GAMETIME_HMS(0, 0, 0),      },
    /*  2 -- ID_AREA60_C  0-3   */  { GAMETEXT_AREA60C_NAME,  GAMETEXT_AREA60_WORLD,      "Foot battle",              "0-2",    1,  STAGEID::ID_ST60X_C01, 10, ANTIQUEID::ID_AT54,     GAMETIME_HMS(0, 0, 0),    GAMETIME_HMS(0, 0, 0),      },
    /*  3 -- ID_AREA60_D  0-4   */  { GAMETEXT_AREA60D_NAME,  GAMETEXT_AREA60_WORLD,      "Battle nexus",             "0-3",    1,  STAGEID::ID_ST60X_D01, 10, ANTIQUEID::ID_AT55,     GAMETIME_HMS(0, 0, 0),    GAMETIME_HMS(0, 0, 0),      },
};


static const AREAINFO& AreaInfo(AREAID::VALUE idArea)
{
    if ((idArea >= AREAID::NEXUSSTART) &&
        (idArea <  AREAID::NEXUSMAX))
    {
        return s_aNexusAreaInfo[idArea - AREAID::NEXUSSTART];
    }
    else if ((idArea >= AREAID::NORMALSTART) &&
             (idArea <  AREAID::NORMALMAX))
    {
        return s_aNormalAreaInfo[idArea];
    }
    else
    {
        return s_aNormalAreaInfo[AREAID::ID_NONE];
    };
};


/*static*/ const wchar* CAreaInfo::GetDispName(AREAID::VALUE idArea)
{
    return CGameText::GetText(AreaInfo(idArea).m_AreaDispName);
};


/*static*/ const wchar* CAreaInfo::GetDispWorldName(AREAID::VALUE idArea)
{
    return CGameText::GetText(AreaInfo(idArea).m_WorldDispName);
};


/*static*/ const char* CAreaInfo::GetEpisode(AREAID::VALUE idArea)
{
    return AreaInfo(idArea).m_pszEpisodeNo;
};


/*static*/ int32 CAreaInfo::GetWindowNo(AREAID::VALUE idArea)
{
    return AreaInfo(idArea).m_iWindowNo;
};


/*static*/ STAGEID::VALUE CAreaInfo::GetStageID(AREAID::VALUE idArea, int32 index)
{
    return STAGEID::VALUE(AreaInfo(idArea).m_StageID + index);
};


/*static*/ int32 CAreaInfo::IndexOfStage(AREAID::VALUE idArea, STAGEID::VALUE idStage)
{
    int32 iIndex = idStage - AreaInfo(idArea).m_StageID;
    
    if ((iIndex < 0) || (iIndex >= AreaInfo(idArea).m_iStageNum))
        return -1;
    
    return iIndex;
};


/*static*/ int32 CAreaInfo::GetStageNum(AREAID::VALUE idArea)
{
    return AreaInfo(idArea).m_iStageNum;
};


/*static*/ ANTIQUEID::VALUE CAreaInfo::GetAntiqueID(AREAID::VALUE idArea)
{
    return AreaInfo(idArea).m_AntiqueID;
};


/*static*/ AREAID::VALUE CAreaInfo::FindAntiqueArea(ANTIQUEID::VALUE idAntique)
{
    for (int32 i = 0; i < AREAID::SELECTABLEMAX; ++i)
    {
        if (AreaInfo(AREAID::VALUE(i)).m_AntiqueID == idAntique)
            return AREAID::VALUE(i);
    };

    for (int32 i = AREAID::NEXUSSTART; i < AREAID::NEXUSMAX; ++i)
    {
        if (AreaInfo(AREAID::VALUE(i)).m_AntiqueID == idAntique)
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
        return MOVIEID::VALUE(40);

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
    return AreaInfo(idArea).m_uClearTimeBest;
};


/*static*/ uint32 CAreaInfo::GetWorstRankClearTime(AREAID::VALUE idArea)
{
    return AreaInfo(idArea).m_uClearTimeWorst;
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