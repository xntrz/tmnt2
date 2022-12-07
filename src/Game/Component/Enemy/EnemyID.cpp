#include "EnemyID.hpp"

#include "Game/System/2d/GameText.hpp"


struct ENEMYINFO
{
    ENEMYID::VALUE m_eID;
    const char* m_pszName;
    GAMETEXT::VALUE m_DispNameID;
    char m_szExtName[16];
    bool m_bImplemented;
};


static ENEMYINFO s_aEnemyInfo[] =
{
    { ENEMYID::ID_DUMMY,                        "dummy",                        GAMETEXT::VALUE(0x00), "", false },
    { ENEMYID::ID_PURPLE_DRAGON_GANG,           "PURPLE DRAGON GANG",           GAMETEXT::VALUE(0x9C), "", false },
    { ENEMYID::ID_FOOT_NINJA_SWORD,             "FOOT NINJA",                   GAMETEXT::VALUE(0x9D), "", false },
    { ENEMYID::ID_FOOT_NINJA_STAFF,             "FOOT NINJA",                   GAMETEXT::VALUE(0x9E), "", false },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD,      "FEUDAL FOOT NINJA",            GAMETEXT::VALUE(0x9F), "", false },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF,      "FEUDAL FOOT NINJA",            GAMETEXT::VALUE(0xA0), "", false },
    { ENEMYID::ID_UTROMS_SECURITY_PATROL,       "UTROMS SECURITY PATROL",       GAMETEXT::VALUE(0xA1), "", false },
    { ENEMYID::ID_FEDERATION_SOLDIER,           "FEDERATION SOLDIER",           GAMETEXT::VALUE(0xA2), "", false },
    { ENEMYID::ID_MOBSTER,                      "MOBSTER",                      GAMETEXT::VALUE(0xA3), "", false },
    { ENEMYID::ID_FOOT_NINJA_ARCHER,            "FOOT ARCHER NINJA",            GAMETEXT::VALUE(0xA4), "", false },
    { ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER,     "FEUDAL FOOT ARCHER NINJA",     GAMETEXT::VALUE(0xA5), "", false },
    { ENEMYID::ID_FEDERATION_SHOOTER,           "FEDERATION SHOOTER",           GAMETEXT::VALUE(0xA6), "", false },
    { ENEMYID::ID_EXO_SUITED_UTROMS,            "EXO-SUITED UTROMS",            GAMETEXT::VALUE(0xA7), "", false },
    { ENEMYID::ID_CAPTAIN_OF_FEDERATION,        "CAPTAIN OF FEDERATION",        GAMETEXT::VALUE(0xA8), "", false },
    { ENEMYID::ID_LARGE_MOBSTER,                "LARGE MOBSTER",                GAMETEXT::VALUE(0xA9), "", false },
    { ENEMYID::ID_LARGE_FOOT_NINJA,             "LARGE FOOT NINJA",             GAMETEXT::VALUE(0xAA), "", false },
    { ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA,      "FEUDAL LARGE FOOT NINJA",      GAMETEXT::VALUE(0xAB), "", false },
    { ENEMYID::ID_LARGE_FEDERATION_SOLDIET,     "LARGE FEDERATION SOLDIER",     GAMETEXT::VALUE(0xAC), "", false },
    { ENEMYID::ID_LARGE_EXO_SUITED_UTROMS,      "LARGE EXO-SUITED UTROMS",      GAMETEXT::VALUE(0xAD), "", false },
    { ENEMYID::ID_FOOT_TECH_NINJA,              "FOOT TECH NINJA",              GAMETEXT::VALUE(0xAE), "", false },
    { ENEMYID::ID_FOOT_SUMO_NINJA,              "FOOT SUMO NINJA",              GAMETEXT::VALUE(0xAF), "", false },
    { ENEMYID::ID_FOOT_GUNNER_FOOT_MECH,        "GUNNER FOOT MECH",             GAMETEXT::VALUE(0xB0), "", false },
    { ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA, "CAPTAIN OF FEUDAL FOOT NINJA", GAMETEXT::VALUE(0xB1), "", false },
    { ENEMYID::ID_TRICERATION_SOLDIER,          "TRICERATON SOLDIER",           GAMETEXT::VALUE(0xB2), "", false },
    { ENEMYID::ID_TRICERATION_SHOOTER,          "TRICERATON SHOOTER",           GAMETEXT::VALUE(0xB3), "", false },
    { ENEMYID::ID_TRICERATION_FLYING_HARNESS,   "TRICERATON FLYING HARNESS",    GAMETEXT::VALUE(0xB4), "", false },
    { ENEMYID::ID_STONE_BITER,                  "STONEBITTER",                  GAMETEXT::VALUE(0xB5), "", false },
    { ENEMYID::ID_BERGOX,                       "BERGOX",                       GAMETEXT::VALUE(0xB6), "", false },
    { ENEMYID::ID_SPIDER_MONSTER,               "SPIDER MONSTER",               GAMETEXT::VALUE(0xB7), "", false },
    { ENEMYID::ID_REDURION,                     "REDURIAN",                     GAMETEXT::VALUE(0xB8), "", false },
    { ENEMYID::ID_KING_NAIL,                    "KING NAIL",                    GAMETEXT::VALUE(0xB9), "", false },
    { ENEMYID::ID_GREENPUS,                     "GREENPUS",                     GAMETEXT::VALUE(0xBA), "", false },
    { ENEMYID::ID_RAZORFIST,                    "RAZORFIST",                    GAMETEXT::VALUE(0xBB), "", false },
    { ENEMYID::ID_MAD_MUNCHER,                  "MAD MUNCHER",                  GAMETEXT::VALUE(0xBC), "", false },
    { ENEMYID::ID_RYNOKK,                       "RYNOKK",                       GAMETEXT::VALUE(0xBD), "", false },
    { ENEMYID::ID_BLOOD_SUCKER,                 "BLOOD SUCKER",                 GAMETEXT::VALUE(0xBE), "", false },
    { ENEMYID::ID_POISON_BAT,                   "POISON BAT",                   GAMETEXT::VALUE(0xBF), "", false },
    { ENEMYID::ID_HYPNOSIS_BAT,                 "HYPNOSIS BAT",                 GAMETEXT::VALUE(0xC0), "", false },
    { ENEMYID::ID_BOMB_BAT_MECH,                "BOMB BAT MECH",                GAMETEXT::VALUE(0xC1), "", false },
    { ENEMYID::ID_KURAGE,                       "KURAGE",                       GAMETEXT::VALUE(0xC2), "", false },
    { ENEMYID::ID_KABUTO,                       "KABUTO",                       GAMETEXT::VALUE(0xC3), "", false },
    { ENEMYID::ID_UTROM_SAUCER,                 "UTROM SAUCER",                 GAMETEXT::VALUE(0xC4), "", false },
    { ENEMYID::ID_MOUSER_ROBOT,                 "MOUSER ROBOT",                 GAMETEXT::VALUE(0xC5), "", false },
    { ENEMYID::ID_MOUSER_ROBOT_B,               "PROTOTYPE MOUSER",             GAMETEXT::VALUE(0xC6), "", false },
    { ENEMYID::ID_KROKODIL_MOUSER,              "KROKODIL MOUSER",              GAMETEXT::VALUE(0xC7), "", false },
    { ENEMYID::ID_RAPTOR,                       "RAPTOR",                       GAMETEXT::VALUE(0xC8), "", false },
    { ENEMYID::ID_CARNIVOROUS_PLANT,            "CARNIVOROUS PLANT",            GAMETEXT::VALUE(0xC9), "", false },
    { ENEMYID::ID_MURAL_CARNIVOROUS_PLANT,      "MURAL CARNIVOROUS PLANT",      GAMETEXT::VALUE(0xCA), "", false },
    { ENEMYID::ID_NINJA_RATS_A,                 "NINJA RATS",                   GAMETEXT::VALUE(0xCB), "", false },
    { ENEMYID::ID_NINJA_RATS_B,                 "NINJA RATS",                   GAMETEXT::VALUE(0xCC), "", false },
    { ENEMYID::ID_NINJA_RATS_C,                 "NINJA RATS",                   GAMETEXT::VALUE(0xCD), "", false },
    { ENEMYID::ID_NINJA_RATS_D,                 "NINJA RATS",                   GAMETEXT::VALUE(0xCE), "", false },
    { ENEMYID::ID_TRAXIMUS,                     "TRAXIMUS",                     GAMETEXT::VALUE(0xCF), "", false },
    { ENEMYID::ID_LEATHER_HEAD,                 "LEATHER HEAD",                 GAMETEXT::VALUE(0xD0), "", false },
    { ENEMYID::ID_ELITE_FOOT_A,                 "ELITE FOOT",                   GAMETEXT::VALUE(0xD1), "", false },
    { ENEMYID::ID_ELITE_FOOT_B,                 "ELITE FOOT",                   GAMETEXT::VALUE(0xD2), "", false },
    { ENEMYID::ID_ELITE_FOOT_C,                 "ELITE FOOT",                   GAMETEXT::VALUE(0xD3), "", false },
    { ENEMYID::ID_ELITE_FOOT_D,                 "ELITE FOOT",                   GAMETEXT::VALUE(0xD4), "", false },
    { ENEMYID::ID_ULTIMATE_NINJA,               "ULTIMATE NINJA",               GAMETEXT::VALUE(0xD5), "", false },
    { ENEMYID::ID_SPASMOSAUR,                   "SPASMOSAUR",                   GAMETEXT::VALUE(0xD6), "", false },
    { ENEMYID::ID_HUN_A,                        "HUN",                          GAMETEXT::VALUE(0xD7), "", false },
    { ENEMYID::ID_HUN_B,                        "HUN",                          GAMETEXT::VALUE(0xD8), "", false },
    { ENEMYID::ID_KARAI,                        "KARAI",                        GAMETEXT::VALUE(0xD9), "", false },
    { ENEMYID::ID_MIYAMOTO_USAGI,               "MIYAMOTO USAGI",               GAMETEXT::VALUE(0xDA), "", false },
    { ENEMYID::ID_FOOT_MECH_SPLINTER,           "FOOTMECH SPLINTER",            GAMETEXT::VALUE(0xDB), "", false },
    { ENEMYID::ID_SLASSHUR,                     "SLASSHUR",                     GAMETEXT::VALUE(0xDC), "", false },
    { ENEMYID::ID_SHREDDER,                     "SHREDDER",                     GAMETEXT::VALUE(0xDD), "", false },
    { ENEMYID::ID_FEUDAL_JAPAN_SHREDDER,        "FEUDAL JAPAN SHREDDER",        GAMETEXT::VALUE(0xDE), "", false },
    { ENEMYID::ID_ULTIMATE_SHREDDER,            "ULTIMATE SHREDDER",            GAMETEXT::VALUE(0xDF), "", false },
    { ENEMYID::ID_DORAKO,                       "DORAKO",                       GAMETEXT::VALUE(0xE0), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_A,           "ELITE FOOT",                   GAMETEXT::VALUE(0xE1), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_B,           "ELITE FOOT",                   GAMETEXT::VALUE(0xE2), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_C,           "ELITE FOOT",                   GAMETEXT::VALUE(0xE3), "", false },
    { ENEMYID::ID_ELITE_FOOT_DUMMY_D,           "ELITE FOOT",                   GAMETEXT::VALUE(0xE4), "", false },
    { ENEMYID::ID_SPLINTER,                     "SPLINTER",                     GAMETEXT::VALUE(0xE5), "", false },
    { ENEMYID::ID_FUGITOID,                     "FUGITOID",                     GAMETEXT::VALUE(0xE7), "", false },
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
        else
            return CGameText::EmptyText();
    };


    bool GetImplementProgress(VALUE id)
    {
        ENEMYINFO* pEnemyInfo = GetEnemyInfo(id);
        if (pEnemyInfo)
            return pEnemyInfo->m_bImplemented;
        else
            return false;
    };
};