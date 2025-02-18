#include "EnemyID.hpp"

#include "Game/System/GameObject/GameObjectTypes.hpp"
#include "Game/System/Text/GameText.hpp"


struct ENEMYINFO
{
    ENEMYID::VALUE  m_eID;
    const char*     m_pszName;
    GAMETEXT        m_dispNameId;
    char            m_szExtName[GAMEOBJECTTYPES::NAME_MAX];
    bool            m_bImplemented;
};


static ENEMYINFO s_aEnemyInfo[] =
{
    { ENEMYID::ID_DUMMY,                        "dummy",                        GAMETEXT_EMPTY,         "",     false   },
    { ENEMYID::ID_PURPLE_DRAGON_GANG,           "PURPLE DRAGON GANG",           GAMETEXT_ENEMY_001,     "",     true    },
    { ENEMYID::ID_FOOT_NINJA_SWORD,             "FOOT NINJA SWORD",             GAMETEXT_ENEMY_002,     "",     true    },
    { ENEMYID::ID_FOOT_NINJA_STAFF,             "FOOT NINJA STAFF",             GAMETEXT_ENEMY_003,     "",     true    },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD,      "FEUDAL FOOT NINJA SWORD",      GAMETEXT_ENEMY_004,     "",     true    },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF,      "FEUDAL FOOT NINJA STAFF",      GAMETEXT_ENEMY_005,     "",     true    },
    { ENEMYID::ID_UTROMS_SECURITY_PATROL,       "UTROMS SECURITY PATROL",       GAMETEXT_ENEMY_006,     "",     true    },
    { ENEMYID::ID_FEDERATION_SOLDIER,           "FEDERATION SOLDIER",           GAMETEXT_ENEMY_007,     "",     true    },
    { ENEMYID::ID_MOBSTER,                      "MOBSTER",                      GAMETEXT_ENEMY_010,     "",     true    },
    { ENEMYID::ID_FOOT_NINJA_ARCHER,            "FOOT ARCHER NINJA",            GAMETEXT_ENEMY_011,     "",     true    },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER,     "FEUDAL FOOT ARCHER NINJA",     GAMETEXT_ENEMY_012,     "",     true    },
    { ENEMYID::ID_FEDERATION_SHOOTER,           "FEDERATION SHOOTER",           GAMETEXT_ENEMY_013,     "",     true    },
    { ENEMYID::ID_EXO_SUITED_UTROMS,            "EXO-SUITED UTROMS",            GAMETEXT_ENEMY_014,     "",     true    },
    { ENEMYID::ID_CAPTAIN_OF_FEDERATION,        "CAPTAIN OF FEDERATION",        GAMETEXT_ENEMY_015,     "",     true    },
    { ENEMYID::ID_LARGE_MOBSTER,                "LARGE MOBSTER",                GAMETEXT_ENEMY_020,     "",     true    },
    { ENEMYID::ID_LARGE_FOOT_NINJA,             "LARGE FOOT NINJA",             GAMETEXT_ENEMY_021,     "",     true    },
    { ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA,      "FEUDAL LARGE FOOT NINJA",      GAMETEXT_ENEMY_022,     "",     true    },
    { ENEMYID::ID_LARGE_FEDERATION_SOLDIET,     "LARGE FEDERATION SOLDIER",     GAMETEXT_ENEMY_023,     "",     true    },
    { ENEMYID::ID_LARGE_EXO_SUITED_UTROMS,      "LARGE EXO-SUITED UTROMS",      GAMETEXT_ENEMY_024,     "",     true    },
    { ENEMYID::ID_FOOT_TECH_NINJA,              "FOOT TECH NINJA",              GAMETEXT_ENEMY_030,     "",     true    },
    { ENEMYID::ID_FOOT_SUMO_NINJA,              "FOOT SUMO NINJA",              GAMETEXT_ENEMY_031,     "",     true    },
    { ENEMYID::ID_FOOT_GUNNER_FOOT_MECH,        "GUNNER FOOT MECH",             GAMETEXT_ENEMY_032,     "",     true    },
    { ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA, "CAPTAIN OF FEUDAL FOOT NINJA", GAMETEXT_ENEMY_033,     "",     true    },
    { ENEMYID::ID_TRICERATION_SOLDIER,          "TRICERATON SOLDIER",           GAMETEXT_ENEMY_035,     "",     true    },
    { ENEMYID::ID_TRICERATION_SHOOTER,          "TRICERATON SHOOTER",           GAMETEXT_ENEMY_036,     "",     true    },
    { ENEMYID::ID_TRICERATION_FLYING_HARNESS,   "TRICERATON FLYING HARNESS",    GAMETEXT_ENEMY_037,     "",     true    },
    { ENEMYID::ID_STONE_BITER,                  "STONEBITTER",                  GAMETEXT_ENEMY_040,     "",     false   },
    { ENEMYID::ID_BERGOX,                       "BERGOX",                       GAMETEXT_ENEMY_041,     "",     false   },
    { ENEMYID::ID_SPIDER_MONSTER,               "SPIDER MONSTER",               GAMETEXT_ENEMY_042,     "",     false   },
    { ENEMYID::ID_REDURION,                     "REDURIAN",                     GAMETEXT_ENEMY_043,     "",     false   },
    { ENEMYID::ID_KING_NAIL,                    "KING NAIL",                    GAMETEXT_ENEMY_044,     "",     false   },
    { ENEMYID::ID_GREENPUS,                     "GREENPUS",                     GAMETEXT_ENEMY_045,     "",     false   },
    { ENEMYID::ID_RAZORFIST,                    "RAZORFIST",                    GAMETEXT_ENEMY_046,     "",     false   },
    { ENEMYID::ID_MAD_MUNCHER,                  "MAD MUNCHER",                  GAMETEXT_ENEMY_047,     "",     false   },
    { ENEMYID::ID_RYNOKK,                       "RYNOKK",                       GAMETEXT_ENEMY_048,     "",     false   },
    { ENEMYID::ID_BLOOD_SUCKER,                 "BLOOD SUCKER",                 GAMETEXT_ENEMY_050,     "",     true    },
    { ENEMYID::ID_POISON_BAT,                   "POISON BAT",                   GAMETEXT_ENEMY_051,     "",     true    },
    { ENEMYID::ID_HYPNOSIS_BAT,                 "HYPNOSIS BAT",                 GAMETEXT_ENEMY_052,     "",     true    },
    { ENEMYID::ID_BOMB_BAT_MECH,                "BOMB BAT MECH",                GAMETEXT_ENEMY_053,     "",     true    },
    { ENEMYID::ID_KURAGE,                       "KURAGE",                       GAMETEXT_ENEMY_055,     "",     true    },
    { ENEMYID::ID_KABUTO,                       "KABUTO",                       GAMETEXT_ENEMY_056,     "",     true    },
    { ENEMYID::ID_UTROM_SAUCER,                 "UTROM SAUCER",                 GAMETEXT_ENEMY_057,     "",     true    },
    { ENEMYID::ID_MOUSER_ROBOT,                 "MOUSER ROBOT",                 GAMETEXT_ENEMY_060,     "",     false   },
    { ENEMYID::ID_MOUSER_ROBOT_B,               "PROTOTYPE MOUSER",             GAMETEXT_ENEMY_061,     "",     false   },
    { ENEMYID::ID_KROKODIL_MOUSER,              "KROKODIL MOUSER",              GAMETEXT_ENEMY_062,     "",     false   },
    { ENEMYID::ID_RAPTOR,                       "RAPTOR",                       GAMETEXT_ENEMY_063,     "",     false   },
    { ENEMYID::ID_CARNIVOROUS_PLANT,            "CARNIVOROUS PLANT",            GAMETEXT_ENEMY_065,     "",     false   },
    { ENEMYID::ID_MURAL_CARNIVOROUS_PLANT,      "MURAL CARNIVOROUS PLANT",      GAMETEXT_ENEMY_066,     "",     false   },
    { ENEMYID::ID_NINJA_RATS_A,                 "NINJA RATS A",                 GAMETEXT_ENEMY_070,     "",     false   },
    { ENEMYID::ID_NINJA_RATS_B,                 "NINJA RATS B",                 GAMETEXT_ENEMY_071,     "",     false   },
    { ENEMYID::ID_NINJA_RATS_C,                 "NINJA RATS C",                 GAMETEXT_ENEMY_072,     "",     false   },
    { ENEMYID::ID_NINJA_RATS_D,                 "NINJA RATS D",                 GAMETEXT_ENEMY_073,     "",     false   },
    { ENEMYID::ID_TRAXIMUS,                     "TRAXIMUS",                     GAMETEXT_ENEMY_074,     "",     false   },
    { ENEMYID::ID_LEATHER_HEAD,                 "LEATHER HEAD",                 GAMETEXT_ENEMY_075,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_A,                 "ELITE FOOT A",                 GAMETEXT_ENEMY_076,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_B,                 "ELITE FOOT B",                 GAMETEXT_ENEMY_077,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_C,                 "ELITE FOOT C",                 GAMETEXT_ENEMY_078,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_D,                 "ELITE FOOT D",                 GAMETEXT_ENEMY_079,     "",     false   },
    { ENEMYID::ID_ULTIMATE_NINJA,               "ULTIMATE NINJA",               GAMETEXT_ENEMY_080,     "",     false   },
    { ENEMYID::ID_SPASMOSAUR,                   "SPASMOSAUR",                   GAMETEXT_ENEMY_081,     "",     false   },
    { ENEMYID::ID_HUN_A,                        "HUN A",                        GAMETEXT_ENEMY_082,     "",     false   },
    { ENEMYID::ID_HUN_B,                        "HUN B",                        GAMETEXT_ENEMY_083,     "",     false   },
    { ENEMYID::ID_KARAI,                        "KARAI",                        GAMETEXT_ENEMY_084,     "",     false   },
    { ENEMYID::ID_MIYAMOTO_USAGI,               "MIYAMOTO USAGI",               GAMETEXT_ENEMY_085,     "",     false   },
    { ENEMYID::ID_FOOT_MECH_SPLINTER,           "FOOTMECH SPLINTER",            GAMETEXT_ENEMY_086,     "",     false   },
    { ENEMYID::ID_SLASSHUR,                     "SLASSHUR",                     GAMETEXT_ENEMY_087,     "",     false   },
    { ENEMYID::ID_SHREDDER,                     "SHREDDER",                     GAMETEXT_ENEMY_088,     "",     false   },
    { ENEMYID::ID_FEUDAL_JAPAN_SHREDDER,        "FEUDAL JAPAN SHREDDER",        GAMETEXT_ENEMY_089,     "",     false   },
    { ENEMYID::ID_ULTIMATE_SHREDDER,            "ULTIMATE SHREDDER",            GAMETEXT_ENEMY_090,     "",     false   },
    { ENEMYID::ID_DORAKO,                       "DORAKO",                       GAMETEXT_ENEMY_091,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_A,           "ELITE FOOT DUMMY A",           GAMETEXT_ENEMY_092,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_B,           "ELITE FOOT DUMMY B",           GAMETEXT_ENEMY_093,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_C,           "ELITE FOOT DUMMY C",           GAMETEXT_ENEMY_094,     "",     false   },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_D,           "ELITE FOOT DUMMY D",           GAMETEXT_ENEMY_095,     "",     false   },
    { ENEMYID::ID_SPLINTER,                     "SPLINTER",                     GAMETEXT_ENEMY_096,     "",     false   },
    { ENEMYID::ID_FUGITOID,                     "FUGITOID",                     GAMETEXT_ENEMY_098,     "",     true    },
};


static inline ENEMYINFO* GetEnemyInfo(ENEMYID::VALUE id)
{
    for (int32 i = 0; i < COUNT_OF(s_aEnemyInfo); ++i)
    {
        if (s_aEnemyInfo[i].m_eID == id)
            return &s_aEnemyInfo[i];
    };

    return nullptr;
};


namespace ENEMYID
{
    const char* GetName(VALUE id)
    {
        ENEMYINFO* pEnemyInfo = GetEnemyInfo(id);
        if (!pEnemyInfo)
            return nullptr;

        return pEnemyInfo->m_pszName;
    };


    const char* GetExtName(VALUE id)
    {
        ENEMYINFO* pEnemyInfo = GetEnemyInfo(id);
        if (!pEnemyInfo)
            return nullptr;

        std::sprintf(pEnemyInfo->m_szExtName, "enemy%03d", id);
        
        return pEnemyInfo->m_szExtName;
    };


    const wchar* GetDispName(VALUE id)
    {
        ENEMYINFO* pEnemyInfo = GetEnemyInfo(id);
        if (pEnemyInfo)
            return CGameText::GetText(pEnemyInfo->m_dispNameId);
        
        return CGameText::EmptyText();
    };


    bool GetImplementProgress(VALUE id)
    {
        ENEMYINFO* pEnemyInfo = GetEnemyInfo(id);
        if (pEnemyInfo)
            return pEnemyInfo->m_bImplemented;
        
        return false;
    };
};