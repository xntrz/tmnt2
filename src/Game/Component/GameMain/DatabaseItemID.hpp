#pragma once


namespace DBITEMID
{
    enum VALUE
    {
        ID_NONE = 0,
        ID_FIRST,
        
        CHARACTERSTART = ID_FIRST,
        ID_CHARACTER_LEO_A = CHARACTERSTART,
        ID_CHARACTER_LEO_B,
        ID_CHARACTER_SLASHUUR,
        ID_CHARACTER_RAP_A,
        ID_CHARACTER_RAP_B,
        ID_CHARACTER_CASEY,
        ID_CHARACTER_MIC_A,
        ID_CHARACTER_MIC_B,
        ID_CHARACTER_KARAI,
        ID_CHARACTER_DON_A,
        ID_CHARACTER_DON_B,
        ID_CHARACTER_SPLINTER,
        ID_CHARACTER_APRIL,
        ID_CHARACTER_USAGI,
        CHARACTERMAX,

        ENEMYSTART = CHARACTERMAX,
        ID_ENEMY_SHREDDER = ENEMYSTART,
        ID_ENEMY_ENEMYDORAKO,
        ID_ENEMY_NINJA_RATS,
        ID_ENEMY_TRAXIMUS,
        ID_ENEMY_LEATHERHEAD,
        ID_ENEMY_ELITE_FOOT,
        ID_ENEMY_SPASMOSAUR,
        ID_ENEMY_HUN,
        ID_ENEMY_ULTIMATE_NINJA,
        ID_ENEMY_ZAKO_A,
        ID_ENEMY_ZAKO_B,
        ID_ENEMY_OTHERS,
        ENEMYMAX,

        ARTSTART = ENEMYMAX,
        ID_ART_NY = ARTSTART,
        ID_ART_DHOONNIB,
        ID_ART_JAPAN,
        ID_ART_KURAIYAMA,
        ID_ART_TRANS_DIMENSION,
        ID_ART_MACHINE,
        ID_ART_ANIME,
        ID_ART_CG,
        ARTMAX,

        ETCSTART = ARTMAX,
        ID_ETC_PLAYMATES_A = ETCSTART,
        ID_ETC_PLAYMATES_B,
        ID_ETC_PLAYMATES_C,
        ID_ETC_PLAYMATES_D,
        ID_ETC_GALLERY,
        ETCMAX,

        ID_MAX = ETCMAX,
    };
};