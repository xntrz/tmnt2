#include "EnemyID.hpp"

#include "Game/System/GameObject/GameObjectTypes.hpp"
#include "Game/System/Text/GameText.hpp"


struct ENEMYINFO
{
    ENEMYID::VALUE  m_eID;
    const char*     m_pszName;
    GAMETEXT        m_DispNameID;
    char            m_szExtName[GAMEOBJECTTYPES::NAME_MAX];
    bool            m_bImplemented;
};


static ENEMYINFO s_aEnemyInfo[] =
{
    { ENEMYID::ID_DUMMY,                        "dummy",                        GAMETEXT(0x00), "", false },
    { ENEMYID::ID_PURPLE_DRAGON_GANG,           "PURPLE DRAGON GANG",           GAMETEXT(0x9C), "", true  },
    { ENEMYID::ID_FOOT_NINJA_SWORD,             "FOOT NINJA SWORD",             GAMETEXT(0x9D), "", true  },
    { ENEMYID::ID_FOOT_NINJA_STAFF,             "FOOT NINJA STAFF",             GAMETEXT(0x9E), "", true  },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD,      "FEUDAL FOOT NINJA SWORD",      GAMETEXT(0x9F), "", true  },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF,      "FEUDAL FOOT NINJA STAFF",      GAMETEXT(0xA0), "", true  },
    { ENEMYID::ID_UTROMS_SECURITY_PATROL,       "UTROMS SECURITY PATROL",       GAMETEXT(0xA1), "", true  },
    { ENEMYID::ID_FEDERATION_SOLDIER,           "FEDERATION SOLDIER",           GAMETEXT(0xA2), "", true  },
    { ENEMYID::ID_MOBSTER,                      "MOBSTER",                      GAMETEXT(0xA3), "", true  },
    { ENEMYID::ID_FOOT_NINJA_ARCHER,            "FOOT ARCHER NINJA",            GAMETEXT(0xA4), "", true  },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER,     "FEUDAL FOOT ARCHER NINJA",     GAMETEXT(0xA5), "", true  },
    { ENEMYID::ID_FEDERATION_SHOOTER,           "FEDERATION SHOOTER",           GAMETEXT(0xA6), "", true  },
    { ENEMYID::ID_EXO_SUITED_UTROMS,            "EXO-SUITED UTROMS",            GAMETEXT(0xA7), "", true  },
    { ENEMYID::ID_CAPTAIN_OF_FEDERATION,        "CAPTAIN OF FEDERATION",        GAMETEXT(0xA8), "", true  },
    { ENEMYID::ID_LARGE_MOBSTER,                "LARGE MOBSTER",                GAMETEXT(0xA9), "", true  },
    { ENEMYID::ID_LARGE_FOOT_NINJA,             "LARGE FOOT NINJA",             GAMETEXT(0xAA), "", true  },
    { ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA,      "FEUDAL LARGE FOOT NINJA",      GAMETEXT(0xAB), "", true  },
    { ENEMYID::ID_LARGE_FEDERATION_SOLDIET,     "LARGE FEDERATION SOLDIER",     GAMETEXT(0xAC), "", true  },
    { ENEMYID::ID_LARGE_EXO_SUITED_UTROMS,      "LARGE EXO-SUITED UTROMS",      GAMETEXT(0xAD), "", true  },
    { ENEMYID::ID_FOOT_TECH_NINJA,              "FOOT TECH NINJA",              GAMETEXT(0xAE), "", true  },
    { ENEMYID::ID_FOOT_SUMO_NINJA,              "FOOT SUMO NINJA",              GAMETEXT(0xAF), "", true  },
    { ENEMYID::ID_FOOT_GUNNER_FOOT_MECH,        "GUNNER FOOT MECH",             GAMETEXT(0xB0), "", true  },
    { ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA, "CAPTAIN OF FEUDAL FOOT NINJA", GAMETEXT(0xB1), "", true  },
    { ENEMYID::ID_TRICERATION_SOLDIER,          "TRICERATON SOLDIER",           GAMETEXT(0xB2), "", true  },
    { ENEMYID::ID_TRICERATION_SHOOTER,          "TRICERATON SHOOTER",           GAMETEXT(0xB3), "", true  },
    { ENEMYID::ID_TRICERATION_FLYING_HARNESS,   "TRICERATON FLYING HARNESS",    GAMETEXT(0xB4), "", true  },
    { ENEMYID::ID_STONE_BITER,                  "STONEBITTER",                  GAMETEXT(0xB5), "", false },
    { ENEMYID::ID_BERGOX,                       "BERGOX",                       GAMETEXT(0xB6), "", false },
    { ENEMYID::ID_SPIDER_MONSTER,               "SPIDER MONSTER",               GAMETEXT(0xB7), "", false },
    { ENEMYID::ID_REDURION,                     "REDURIAN",                     GAMETEXT(0xB8), "", false },
    { ENEMYID::ID_KING_NAIL,                    "KING NAIL",                    GAMETEXT(0xB9), "", false },
    { ENEMYID::ID_GREENPUS,                     "GREENPUS",                     GAMETEXT(0xBA), "", false },
    { ENEMYID::ID_RAZORFIST,                    "RAZORFIST",                    GAMETEXT(0xBB), "", false },
    { ENEMYID::ID_MAD_MUNCHER,                  "MAD MUNCHER",                  GAMETEXT(0xBC), "", false },
    { ENEMYID::ID_RYNOKK,                       "RYNOKK",                       GAMETEXT(0xBD), "", false },
    { ENEMYID::ID_BLOOD_SUCKER,                 "BLOOD SUCKER",                 GAMETEXT(0xBE), "", true  },
    { ENEMYID::ID_POISON_BAT,                   "POISON BAT",                   GAMETEXT(0xBF), "", true  },
    { ENEMYID::ID_HYPNOSIS_BAT,                 "HYPNOSIS BAT",                 GAMETEXT(0xC0), "", true  },
    { ENEMYID::ID_BOMB_BAT_MECH,                "BOMB BAT MECH",                GAMETEXT(0xC1), "", true  },
    { ENEMYID::ID_KURAGE,                       "KURAGE",                       GAMETEXT(0xC2), "", true  },
    { ENEMYID::ID_KABUTO,                       "KABUTO",                       GAMETEXT(0xC3), "", true  },
    { ENEMYID::ID_UTROM_SAUCER,                 "UTROM SAUCER",                 GAMETEXT(0xC4), "", true  },
    { ENEMYID::ID_MOUSER_ROBOT,                 "MOUSER ROBOT",                 GAMETEXT(0xC5), "", false },
    { ENEMYID::ID_MOUSER_ROBOT_B,               "PROTOTYPE MOUSER",             GAMETEXT(0xC6), "", false },
    { ENEMYID::ID_KROKODIL_MOUSER,              "KROKODIL MOUSER",              GAMETEXT(0xC7), "", false },
    { ENEMYID::ID_RAPTOR,                       "RAPTOR",                       GAMETEXT(0xC8), "", false },
    { ENEMYID::ID_CARNIVOROUS_PLANT,            "CARNIVOROUS PLANT",            GAMETEXT(0xC9), "", false },
    { ENEMYID::ID_MURAL_CARNIVOROUS_PLANT,      "MURAL CARNIVOROUS PLANT",      GAMETEXT(0xCA), "", false },
    { ENEMYID::ID_NINJA_RATS_A,                 "NINJA RATS A",                 GAMETEXT(0xCB), "", false },
    { ENEMYID::ID_NINJA_RATS_B,                 "NINJA RATS B",                 GAMETEXT(0xCC), "", false },
    { ENEMYID::ID_NINJA_RATS_C,                 "NINJA RATS C",                 GAMETEXT(0xCD), "", false },
    { ENEMYID::ID_NINJA_RATS_D,                 "NINJA RATS D",                 GAMETEXT(0xCE), "", false },
    { ENEMYID::ID_TRAXIMUS,                     "TRAXIMUS",                     GAMETEXT(0xCF), "", false },
    { ENEMYID::ID_LEATHER_HEAD,                 "LEATHER HEAD",                 GAMETEXT(0xD0), "", false },
    { ENEMYID::ID_ELITE_FOOT_A,                 "ELITE FOOT A",                 GAMETEXT(0xD1), "", false },
    { ENEMYID::ID_ELITE_FOOT_B,                 "ELITE FOOT B",                 GAMETEXT(0xD2), "", false },
    { ENEMYID::ID_ELITE_FOOT_C,                 "ELITE FOOT C",                 GAMETEXT(0xD3), "", false },
    { ENEMYID::ID_ELITE_FOOT_D,                 "ELITE FOOT D",                 GAMETEXT(0xD4), "", false },
    { ENEMYID::ID_ULTIMATE_NINJA,               "ULTIMATE NINJA",               GAMETEXT(0xD5), "", false },
    { ENEMYID::ID_SPASMOSAUR,                   "SPASMOSAUR",                   GAMETEXT(0xD6), "", false },
    { ENEMYID::ID_HUN_A,                        "HUN A",                        GAMETEXT(0xD7), "", false },
    { ENEMYID::ID_HUN_B,                        "HUN B",                        GAMETEXT(0xD8), "", false },
    { ENEMYID::ID_KARAI,                        "KARAI",                        GAMETEXT(0xD9), "", false },
    { ENEMYID::ID_MIYAMOTO_USAGI,               "MIYAMOTO USAGI",               GAMETEXT(0xDA), "", false },
    { ENEMYID::ID_FOOT_MECH_SPLINTER,           "FOOTMECH SPLINTER",            GAMETEXT(0xDB), "", false },
    { ENEMYID::ID_SLASSHUR,                     "SLASSHUR",                     GAMETEXT(0xDC), "", false },
    { ENEMYID::ID_SHREDDER,                     "SHREDDER",                     GAMETEXT(0xDD), "", false },
    { ENEMYID::ID_FEUDAL_JAPAN_SHREDDER,        "FEUDAL JAPAN SHREDDER",        GAMETEXT(0xDE), "", false },
    { ENEMYID::ID_ULTIMATE_SHREDDER,            "ULTIMATE SHREDDER",            GAMETEXT(0xDF), "", false },
    { ENEMYID::ID_DORAKO,                       "DORAKO",                       GAMETEXT(0xE0), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_A,           "ELITE FOOT DUMMY A",           GAMETEXT(0xE1), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_B,           "ELITE FOOT DUMMY B",           GAMETEXT(0xE2), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_C,           "ELITE FOOT DUMMY C",           GAMETEXT(0xE3), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_D,           "ELITE FOOT DUMMY D",           GAMETEXT(0xE4), "", false },
    { ENEMYID::ID_SPLINTER,                     "SPLINTER",                     GAMETEXT(0xE5), "", false },
    { ENEMYID::ID_FUGITOID,                     "FUGITOID",                     GAMETEXT(0xE7), "", true  },
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
            return CGameText::GetText(pEnemyInfo->m_DispNameID);
        
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