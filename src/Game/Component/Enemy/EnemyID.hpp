#pragma once


namespace ENEMYID
{
    enum VALUE
    {
        ID_START = 0,
        
        ID_DUMMY = ID_START,
        ID_PURPLE_DRAGON_GANG = 0x1,
        ID_FOOT_NINJA_SWORD = 0x2,
        ID_FOOT_NINJA_STAFF = 0x3,
        ID_FEUDAL_FOOT_NINJA_SWORD = 0x4,
        ID_FEUDAL_FOOT_NINJA_STAFF = 0x5,
        ID_UTROMS_SECURITY_PATROL = 0x6,
        ID_FEDERATION_SOLDIER = 0x7,

        ID_MOBSTER = 0xA,
        ID_FOOT_NINJA_ARCHER = 0xB,
        ID_FEUDAL_FOOT_NINJA_ARCHER = 0xC,
        ID_FEDERATION_SHOOTER = 0xD,
        ID_EXO_SUITED_UTROMS = 0xE,
        ID_CAPTAIN_OF_FEDERATION = 0xF,

        ID_LARGE_MOBSTER = 0x14,
        ID_LARGE_FOOT_NINJA = 0x15,
        ID_FEUDAL_LARGE_FOOT_NINJA = 0x16,
        ID_LARGE_FEDERATION_SOLDIET = 0x17,
        ID_LARGE_EXO_SUITED_UTROMS = 0x18,

        ID_FOOT_TECH_NINJA = 0x1E,
        ID_FOOT_SUMO_NINJA = 0x1F,
        ID_FOOT_GUNNER_FOOT_MECH = 0x20,
        ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA = 0x21,

        ID_TRICERATION_SOLDIER = 0x23,
        ID_TRICERATION_SHOOTER = 0x24,
        ID_TRICERATION_FLYING_HARNESS = 0x25,

        ID_STONE_BITER = 0x28,
        ID_BERGOX = 0x29,
        ID_SPIDER_MONSTER = 0x2A,
        ID_REDURION = 0x2B,
        ID_KING_NAIL = 0x2C,
        ID_GREENPUS = 0x2D,
        ID_RAZORFIST = 0x2E,
        ID_MAD_MUNCHER = 0x2F,
        ID_RYNOKK = 0x30,

        ID_BLOOD_SUCKER = 0x32,
        ID_POISON_BAT = 0x33,
        ID_HYPNOSIS_BAT = 0x34,
        ID_BOMB_BAT_MECH = 0x35,
        ID_KURAGE = 0x37,
        ID_KABUTO = 0x38,
        ID_UTROM_SAUCER = 0x39,
        ID_MOUSER_ROBOT = 0x3C,
        ID_MOUSER_ROBOT_B = 0x3D,
        ID_KROKODIL_MOUSER = 0x3E,
        ID_RAPTOR = 0x3F,
        ID_CARNIVOROUS_PLANT = 0x41,
        ID_MURAL_CARNIVOROUS_PLANT = 0x42,

        BOSSBEGIN = 70,
        ID_NINJA_RATS_A = BOSSBEGIN,
        ID_NINJA_RATS_B,
        ID_NINJA_RATS_C,
        ID_NINJA_RATS_D,
        ID_TRAXIMUS,
        ID_LEATHER_HEAD,
        ID_ELITE_FOOT_A,
        ID_ELITE_FOOT_B,
        ID_ELITE_FOOT_C,
        ID_ELITE_FOOT_D,
        ID_ULTIMATE_NINJA,
        ID_SPASMOSAUR,
        ID_HUN_A,
        ID_HUN_B,
        ID_KARAI,
        ID_MIYAMOTO_USAGI,
        ID_FOOT_MECH_SPLINTER,
        ID_SLASSHUR,
        ID_SHREDDER,
        ID_FEUDAL_JAPAN_SHREDDER,
        ID_ULTIMATE_SHREDDER,
        ID_DORAKO,
        ID_ELITE_FOOT_DUMMY_A,
        ID_ELITE_FOOT_DUMMY_B,
        ID_ELITE_FOOT_DUMMY_C,
        ID_ELITE_FOOT_DUMMY_D,
        ID_SPLINTER,
        BOSSEND,

        NEUTRALBEGIN,
        ID_FUGITOID = NEUTRALBEGIN,
        NEUTRALEND,
        
        ID_MAX = NEUTRALEND,
    };

    const char* GetName(VALUE id);
    const char* GetExtName(VALUE id);
    const wchar* GetDispName(VALUE id);
    bool GetImplementProgress(VALUE id);
};