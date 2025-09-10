#include "GameSound.hpp"
#include "Sound.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Player/Player.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"


static const int32 s_dmKoeHigh[] =
{
    // leo
    SDCODE_SE(0x2027),
    SDCODE_SE(0x2028),
    SDCODE_SE(0x2029),
    // rap
    SDCODE_SE(0x2040),
    SDCODE_SE(0x2041),
    SDCODE_SE(0x2042),
    // mic
    SDCODE_SE(0x2059),
    SDCODE_SE(0x205A),
    SDCODE_SE(0x205B),
    // don
    SDCODE_SE(0x2072),
    SDCODE_SE(0x2073),
    SDCODE_SE(0x2074),
    // sla
    SDCODE_SE(0x20DB),
    SDCODE_SE(0x20DC),
    SDCODE_SE(0x20DD),
    // cas
    SDCODE_SE(0x20A5),
    SDCODE_SE(0x20A6),
    SDCODE_SE(0x20A7),
    // kar
    SDCODE_SE(0x20BF),
    SDCODE_SE(0x20C0),
    SDCODE_SE(0x20C1),
    // spl
    SDCODE_SE(0x208B),
    SDCODE_SE(0x208C),
    SDCODE_SE(0x208D),
};


static const int32 s_dmKoeLow[] =
{
    // leo
    SDCODE_SE(0x2023),
    SDCODE_SE(0x2024),
    SDCODE_SE(0x2025),
    // rap
    SDCODE_SE(0x203C),
    SDCODE_SE(0x203D),
    SDCODE_SE(0x203E),
    // mic
    SDCODE_SE(0x2055),
    SDCODE_SE(0x2056),
    SDCODE_SE(0x2057),
    // don
    SDCODE_SE(0x206E),
    SDCODE_SE(0x206F),
    SDCODE_SE(0x2070),
    // sla
    SDCODE_SE(0x20D7),
    SDCODE_SE(0x20D8),
    SDCODE_SE(0x20D9),
    // cas
    SDCODE_SE(0x20A1),
    SDCODE_SE(0x20A2),
    SDCODE_SE(0x20A3),
    // kar
    SDCODE_SE(0x20BB),
    SDCODE_SE(0x20BC),
    SDCODE_SE(0x20BD),
    // spl
    SDCODE_SE(0x2087),
    SDCODE_SE(0x2088),
    SDCODE_SE(0x2089),
};


static const int32 s_atKoeHigh[] =
{
    // leo
    SDCODE_SE(0x2020),
    SDCODE_SE(0x2021),
    SDCODE_SE(0x2022),
    // rap
    SDCODE_SE(0x2039),
    SDCODE_SE(0x203A),
    SDCODE_SE(0x203B),
    // mic
    SDCODE_SE(0x2052),
    SDCODE_SE(0x2053),
    SDCODE_SE(0x2054),
    // don
    SDCODE_SE(0x206B),
    SDCODE_SE(0x206C),
    SDCODE_SE(0x206D),
    // sla
    SDCODE_SE(0x20D4),
    SDCODE_SE(0x20D5),
    SDCODE_SE(0x20D6),
    // cas
    SDCODE_SE(0x209E),
    SDCODE_SE(0x209F),
    SDCODE_SE(0x20A0),
    // kar
    SDCODE_SE(0x20B8),
    SDCODE_SE(0x20B9),
    SDCODE_SE(0x20BA),
    // spl
    SDCODE_SE(0x2084),
    SDCODE_SE(0x2085),
    SDCODE_SE(0x2086),
};


static const int32 s_atKoeLow[] =
{
    // leo
    SDCODE_SE(0x201B),
    SDCODE_SE(0x201C),
    SDCODE_SE(0x201D),
    // rap
    SDCODE_SE(0x2034),
    SDCODE_SE(0x2035),
    SDCODE_SE(0x2036),
    // mic
    SDCODE_SE(0x204D),
    SDCODE_SE(0x204E),
    SDCODE_SE(0x204F),
    // don
    SDCODE_SE(0x2066),
    SDCODE_SE(0x2067),
    SDCODE_SE(0x2068),
    // sla
    SDCODE_SE(0x20CF),
    SDCODE_SE(0x20D0),
    SDCODE_SE(0x20D1),
    // cas
    SDCODE_SE(0x2099),
    SDCODE_SE(0x209A),
    SDCODE_SE(0x209B),
    // kar
    SDCODE_SE(0x20B3),
    SDCODE_SE(0x20B4),
    SDCODE_SE(0x20B5),
    // spl
    SDCODE_SE(0x207F),
    SDCODE_SE(0x2080),
    SDCODE_SE(0x2081),
};


static_assert((COUNT_OF(s_dmKoeHigh) / 3) == PLAYERID::ID_MAX, "update me");
static_assert((COUNT_OF(s_dmKoeLow)  / 3) == PLAYERID::ID_MAX, "update me");
static_assert((COUNT_OF(s_atKoeHigh) / 3) == PLAYERID::ID_MAX, "update me");
static_assert((COUNT_OF(s_atKoeLow)  / 3) == PLAYERID::ID_MAX, "update me");


static int32 SdSetDamageGetKoeHigh(int32 DefenderId, int32 Rnd)
{
    static const int32* KoeHighCodeArray[] =
    {
        &s_dmKoeHigh[0],
        &s_dmKoeHigh[3],
        &s_dmKoeHigh[6],
        &s_dmKoeHigh[9],
        &s_dmKoeHigh[12],
        &s_dmKoeHigh[15],
        &s_dmKoeHigh[18],
        &s_dmKoeHigh[21],
    };

    static_assert(COUNT_OF(KoeHighCodeArray) == PLAYERID::ID_MAX, "update me");
    
    ASSERT(DefenderId >= 0);
    ASSERT(DefenderId < COUNT_OF(KoeHighCodeArray));

    ASSERT(Rnd >= 0);
    ASSERT(Rnd <  3);

    return KoeHighCodeArray[DefenderId][Rnd];
};


static inline int32 SdSetDamageGetKoeLow(int32 DefenderId, int32 Rnd)
{
    static const int32* KoeLowCodeArray[] =
    {
        &s_dmKoeLow[0],
        &s_dmKoeLow[3],
        &s_dmKoeLow[6],
        &s_dmKoeLow[9],
        &s_dmKoeLow[12],
        &s_dmKoeLow[15],
        &s_dmKoeLow[18],
        &s_dmKoeLow[21],
    };

    static_assert(COUNT_OF(KoeLowCodeArray) == PLAYERID::ID_MAX, "update me");
    
    ASSERT(DefenderId >= 0);
    ASSERT(DefenderId < COUNT_OF(KoeLowCodeArray));

    ASSERT(Rnd >= 0);
    ASSERT(Rnd <  3);

    return KoeLowCodeArray[DefenderId][Rnd];
};


static void SdSetDamage(const SE_DAMAGE_PARAM* pParam)
{
    ASSERT(pParam);

    const char* AttackerMotion = pParam->AttackerMotion;
    GAMEOBJECTTYPE::VALUE AttackerType = GAMEOBJECTTYPE::VALUE(pParam->AttackerType);
    int32 AttackerSubType = pParam->AttackerSubType;
    int32 AttackerId = pParam->AttackerId;
    const char* AttackerName = pParam->Attacker->GetName();
    int32 Rnd = Math::RandRange(0, 2);
    int32 Code = SD_NOCODE;
    int32 KoeCode = SD_NOCODE;

    switch (AttackerType)
    {
    case GAMEOBJECTTYPE::SHOT:
        {
            Code = SDCODE_SE(0x1024);

            if (pParam->DefenderType == CCharacter::TYPE_PLAYER)
                KoeCode = SdSetDamageGetKoeLow(pParam->DefenderId, Rnd);
        }
        break;  // GAMEOBJECTTYPE::SHOT

    case GAMEOBJECTTYPE::EFFECT:
        {
            Code = SDCODE_SE(0x1024);
            
            STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();
            switch (AttackerSubType)
            {
            case EFFECTID::ID_EXPL_B1_PLAYER:
                switch (idStage)
                {
                case STAGEID::ID_ST38NB:
                case STAGEID::ID_ST40OB:
                case STAGEID::ID_ST58OB1:
                case STAGEID::ID_ST58OB2:
                case STAGEID::ID_ST60X_C10:                    
                    Code = SDCODE_SE(0x1107);
                    break;
                }
                break;
                
            case EFFECTID::ID_SLEEPGAS:
            case EFFECTID::ID_SPSM_TBSHOT:
                Code = SDCODE_SE(0x1024);
                break;
                
            case EFFECTID::ID_FIRE_TORCH:
                Code = SDCODE_SE(0x1021);
                break;
                
            case EFFECTID::ID_SHRD_ELBALL:
                Code = SDCODE_SE(0x110B);
                break;
                
            case EFFECTID::ID_LASER_BEAM_FED:
            case EFFECTID::ID_LASER_BEAM_TRI:
            case EFFECTID::ID_GUNBALL:
            case EFFECTID::ID_LASER_BEAM_ALL:
                Code = SDCODE_SE(0x1030);
                break;

            default:
                break;
            };

            if (pParam->DefenderType == CCharacter::TYPE_PLAYER)
                KoeCode = SdSetDamageGetKoeHigh(pParam->DefenderId, Rnd);
        }
        break;  // GAMEOBJECTTYPE::EFFECT

    case GAMEOBJECTTYPE::GIMMICK:
        {
            Code = SDCODE_SE(0x101D);
            if (!std::strcmp(pParam->Attacker->GetName(), "GMNROLOBJ_A000"))
                Code = SDCODE_SE(0x10FF);
            if (!std::strcmp(pParam->Attacker->GetName(), "GMNFALOBJ_A008"))
                Code = SDCODE_SE(0x10FA);

            switch (pParam->AttackerSubType)
            {
            case GIMMICKID::ID_N_LASER:
                Code = SDCODE_SE(0x1030);
                break;
                
            case GIMMICKID::ID_N_GUILLO:
                Code = SDCODE_SE(0x104D);
                break;
                
            case GIMMICKID::ID_N_BRKOBJ:
                Code = SDCODE_SE(0x1056);
                break;
                
            case GIMMICKID::ID_N_ROLOBJ:
                Code = SDCODE_SE(0x1055);
                break;

            default:
                break;
            };

            if (pParam->DefenderType == CCharacter::TYPE_PLAYER)
                KoeCode = SdSetDamageGetKoeHigh(pParam->DefenderId, Rnd);
        }
        break;  // GAMEOBJECTTYPE::GIMMICK

    case GAMEOBJECTTYPE::CHARACTER:
        {
            switch (AttackerSubType)
            {
            case CCharacter::TYPE_PLAYER:
                {
                    int32 DefenderArmorType = 0;
                    
                    if (pParam->AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD)
                    {
                        switch (pParam->DefenderId)
                        {
                        case ENEMYID::ID_NINJA_RATS_A:
                            Code = SDCODE_SE(0x20ED);
                            break;

                        case ENEMYID::ID_NINJA_RATS_B:
                            Code = SDCODE_SE(0x20F0);
                            break;

                        case ENEMYID::ID_NINJA_RATS_C:
                            Code = SDCODE_SE(0x20F3);
                            break;

                        case ENEMYID::ID_NINJA_RATS_D:
                            Code = SDCODE_SE(0x20EA);
                            break;

                        case ENEMYID::ID_TRAXIMUS:
                            Code = SDCODE_SE(0x20FA);
                            break;

                        case ENEMYID::ID_ELITE_FOOT_A:
                        case ENEMYID::ID_ELITE_FOOT_B:
                        case ENEMYID::ID_ELITE_FOOT_C:
                        case ENEMYID::ID_ELITE_FOOT_D:
                        case ENEMYID::ID_ELITE_FOOT_DUMMY_A:
                        case ENEMYID::ID_ELITE_FOOT_DUMMY_B:
                        case ENEMYID::ID_ELITE_FOOT_DUMMY_C:
                        case ENEMYID::ID_ELITE_FOOT_DUMMY_D:
                            Code = Rnd + SDCODE_SE(0x2102);
                            break;

                        case ENEMYID::ID_ULTIMATE_NINJA:
                            Code = Rnd + SDCODE_SE(0x2106);
                            break;

                        case ENEMYID::ID_KARAI:
                            Code = Rnd + SDCODE_SE(0x20BB);
                            if (pParam->AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK)
                                Code = Rnd + SDCODE_SE(0x20BF);
                            break;

                        case ENEMYID::ID_MIYAMOTO_USAGI:
                            Code = Rnd + SDCODE_SE(0x210C);
                            break;

                        case ENEMYID::ID_FOOT_MECH_SPLINTER:
                        case ENEMYID::ID_SPLINTER:
                            Code = Rnd + SDCODE_SE(0x2087);
                            if (pParam->AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK)
                                Code = Rnd + SDCODE_SE(0x208B);
                            break;

                        case ENEMYID::ID_SLASSHUR:
                            Code = Rnd + SDCODE_SE(0x20D7);
                            if (pParam->AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK)
                                Code = Rnd + SDCODE_SE(0x20DB);
                            break;

                        case ENEMYID::ID_SHREDDER:
                        case ENEMYID::ID_FEUDAL_JAPAN_SHREDDER:
                        case ENEMYID::ID_ULTIMATE_SHREDDER:
                            Code = Rnd + SDCODE_SE(0x2111);
                            break;

                        case ENEMYID::ID_DORAKO:
                            Code = Rnd + SDCODE_SE(0x2116);
                            break;

                        default:
                            break;
                        };

                        if (Code)
                            CGameSound::PlayPositionSE(&pParam->Pos, Code, 0);
                        
                        Code = SD_NOCODE;
                    };

                    if (pParam->DefenderId == ENEMYID::ID_FUGITOID
                        || (pParam->DefenderId == ENEMYID::ID_ULTIMATE_SHREDDER)
                        || (pParam->DefenderId == ENEMYID::ID_FEUDAL_JAPAN_SHREDDER)
                        || (pParam->DefenderId == ENEMYID::ID_SHREDDER)
                        || (pParam->DefenderId == ENEMYID::ID_SLASSHUR)
                        || (pParam->DefenderId == ENEMYID::ID_FOOT_MECH_SPLINTER)
                        || (pParam->DefenderId == ENEMYID::ID_KROKODIL_MOUSER)
                        || (pParam->DefenderId == ENEMYID::ID_MOUSER_ROBOT_B)
                        || (pParam->DefenderId == ENEMYID::ID_MOUSER_ROBOT)
                        || (pParam->DefenderId == ENEMYID::ID_UTROM_SAUCER)
                        || (pParam->DefenderId == ENEMYID::ID_KABUTO)
                        || (pParam->DefenderId == ENEMYID::ID_KURAGE)
                        || (pParam->DefenderId == ENEMYID::ID_FOOT_GUNNER_FOOT_MECH)
                        || (pParam->DefenderId == ENEMYID::ID_LARGE_EXO_SUITED_UTROMS)
                        || (pParam->DefenderId == ENEMYID::ID_EXO_SUITED_UTROMS)
                        || (pParam->DefenderId == ENEMYID::ID_UTROMS_SECURITY_PATROL))
                    {
                        // robot
                        DefenderArmorType = 2;
                    };

                    if (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD)
                    {
                        Code = SDCODE_SE(0x10D6);
                        
                        switch (pParam->DefenderId)
                        {
                        case ENEMYID::ID_UTROMS_SECURITY_PATROL:
                        case ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF:
                        case ENEMYID::ID_FOOT_NINJA_STAFF:
                            Code = SDCODE_SE(0x10CF);
                            break;
                            
                        case ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER:
                        case ENEMYID::ID_FOOT_NINJA_ARCHER:
                            Code = SDCODE_SE(0x10D1);
                            break;
                            
                        case ENEMYID::ID_ELITE_FOOT_DUMMY_A:
                        case ENEMYID::ID_ELITE_FOOT_A:
                        case ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD:
                        case ENEMYID::ID_FOOT_NINJA_SWORD:
                            Code = SDCODE_SE(0x10D0);
                            break;

                        default:
                            break;
                        };
                    };

                    if ((pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK)
                        || (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE)
                        || (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_BIND))
                    {
                        switch (AttackerId)
                        {
                        case PLAYERID::ID_LEO:
                            Code = DefenderArmorType + SDCODE_SE(0x10A9);
                            if (!strcmp(AttackerMotion, "JAttack"))
                                Code = 0x101DLL;
                            if (!strcmp(AttackerMotion, "RunAttack"))
                                Code = DefenderArmorType + SDCODE_SE(0x10AA);
                            break;

                        case PLAYERID::ID_RAP:
                            Code = DefenderArmorType + SDCODE_SE(0x10B1);
                            if (!strcmp(AttackerMotion, "RunAttack"))
                                Code = DefenderArmorType + SDCODE_SE(0x10B2);
                            break;

                        case PLAYERID::ID_MIC:
                            Code = DefenderArmorType + SDCODE_SE(0x10B9);
                            if (!strcmp(AttackerMotion, "JAttack"))
                                Code = SDCODE_SE(0x101D);
                            if (!strcmp(AttackerMotion, "RunAttack"))
                            {
                                Code = SDCODE_SE(0x101D);
                                if (DefenderArmorType)
                                    Code = SDCODE_SE(0x10A8);
                            }
                            break;

                        case PLAYERID::ID_DON:
                            Code = DefenderArmorType + SDCODE_SE(0x10C2);
                            if (!strcmp(AttackerMotion, "AA"))
                                Code = SDCODE_SE(0x101D);
                            if (!strcmp(AttackerMotion, "RunAttack"))
                                Code = DefenderArmorType + SDCODE_SE(0x10C3);
                            break;

                        case PLAYERID::ID_SLA:
                            Code = DefenderArmorType + SDCODE_SE(0x10A9);
                            if (DefenderArmorType)
                                Code = SDCODE_SE(0x10B4);
                            if (!strcmp(AttackerMotion, "RunAttack"))
                            {
                                Code = SDCODE_SE(0x112E);
                                if (DefenderArmorType)
                                    Code = SDCODE_SE(0x10AC);
                            }
                            break;

                        case PLAYERID::ID_CAS:
                            Code = DefenderArmorType + SDCODE_SE(0x10B1);
                            if (!strcmp(AttackerMotion, "JAttack"))
                                Code = SDCODE_SE(0x101D);
                            if (!strcmp(AttackerMotion, "RunAttack"))
                                Code = DefenderArmorType + SDCODE_SE(0x10B2);
                            break;

                        case PLAYERID::ID_KAR:
                            Code = SDCODE_SE(0x101DLL);
                            if (DefenderArmorType)
                                Code = SDCODE_SE(0x10A8LL);
                            if (!strcmp(AttackerMotion, "RunAttack"))
                            {
                                Code = SDCODE_SE(0x101D);
                                if (DefenderArmorType)
                                    Code = SDCODE_SE(0x10A8);
                            }
                            break;

                        case PLAYERID::ID_SPL:
                            Code = DefenderArmorType + SDCODE_SE(0x10C2);
                            if (!strcmp(AttackerMotion, "JAttack"))
                                Code = SDCODE_SE(0x101D);
                            if (!strcmp(AttackerMotion, "RunAttack"))
                                Code = DefenderArmorType + SDCODE_SE(0x10B2);
                            break;

                        default:
                            break;
                        };
                    };
                    
                    if ((pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY)
                        || (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_STUN)
                        || (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP)
                        || (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_BREAK)
                        || (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE))
                    {
                        switch (AttackerId)
                        {
                        case PLAYERID::ID_LEO:
                            {
                                Code = DefenderArmorType + SDCODE_SE(0x10AA);                                
                                if (!std::strcmp(AttackerMotion, "JAttack"))
                                    Code = SDCODE_SE(0x101D);
                                if (!std::strcmp(AttackerMotion, "RunAttack"))
                                    Code = DefenderArmorType + SDCODE_SE(0x10AA);
                            }
                            break;

                        case PLAYERID::ID_RAP:
                            {
                                Code = DefenderArmorType + SDCODE_SE(0x10B2);
                                std::strcmp(AttackerMotion, "RunAttack");
                            }
                            break;

                        case PLAYERID::ID_MIC:
                            {
                                Code = DefenderArmorType + SDCODE_SE(0x10BA);
                                if (!std::strcmp(AttackerMotion, "JAttack"))
                                    Code = SDCODE_SE(0x101D);
                                if (!std::strcmp(AttackerMotion, "RunAttack"))
                                {
                                    Code = SDCODE_SE(0x101D);
                                    if (DefenderArmorType)
                                        Code = SDCODE_SE(0x10A8);
                                }
                            }
                            break;

                        case PLAYERID::ID_DON:
                            {
                                Code = DefenderArmorType + SDCODE_SE(0x10C2);
                                if (!std::strcmp(AttackerMotion, "AA"))
                                    Code = SDCODE_SE(0x101D);
                                if (!std::strcmp(AttackerMotion, "RunAttack"))
                                    Code = DefenderArmorType + SDCODE_SE(0x10C3);
                            }
                            break;

                        case PLAYERID::ID_SLA:
                            {
                                Code = SDCODE_SE(0x112E);
                                if (DefenderArmorType)
                                    Code = SDCODE_SE(0x10AC);
                                
                                if (!std::strcmp(AttackerMotion, "RunAttack"))
                                {
                                    Code = SDCODE_SE(0x112E);
                                    if (DefenderArmorType)
                                        Code = SDCODE_SE(0x10AC);
                                };
                            }
                            break;

                        case PLAYERID::ID_CAS:
                            {
                                Code = DefenderArmorType + SDCODE_SE(0x10B2);

                                if (!std::strcmp(AttackerMotion, "JAttack"))
                                    Code = SDCODE_SE(0x101D);

                                if (!std::strcmp(AttackerMotion, "RunAttack"))
                                    Code = DefenderArmorType + SDCODE_SE(0x10B2);
                            }
                            break;

                        case PLAYERID::ID_KAR:
                            {
                                Code = SDCODE_SE(0x113A);
                                if (DefenderArmorType)
                                    Code = SDCODE_SE(0x10AC);

                                if (!std::strcmp(AttackerMotion, "JAttack"))
                                    Code = SDCODE_SE(0x1075);

                                if (!std::strcmp(AttackerMotion, "RunAttack"))
                                {
                                    Code = SDCODE_SE(0x101D);
                                    if (DefenderArmorType)
                                        Code = SDCODE_SE(0x10A8);
                                };
                            }
                            break;

                        case PLAYERID::ID_SPL:
                            {
                                Code = DefenderArmorType + SDCODE_SE(0x10C3);

                                if (!std::strcmp(AttackerMotion, "JAttack"))
                                    Code = SDCODE_SE(0x101D);

                                if (!std::strcmp(AttackerMotion, "RunAttack"))
                                    Code = DefenderArmorType + SDCODE_SE(0x10B2);
                            }
                            break;

                        default:
                            break;
                        };
                    };

                    if ((!std::strcmp(AttackerMotion, "YBackFuttobi1"))
                        || (!std::strcmp(AttackerMotion, "YFrontFuttobi1"))
                        || (!std::strcmp(AttackerMotion, "YBackFuttobi2"))
                        || (!std::strcmp(AttackerMotion, "YFrontFuttobi2")))
                    {
                        Code = SDCODE_SE(0x1024);
                    };

                    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_SQUEAKING))
                        Code = SDCODE_SE(0x1089);
                }
                break;  // CCharacter::TYPE_PLAYER

            case CCharacter::TYPE_ENEMY:
                {
                    Code = SDCODE_SE(0x10CA);
                    
                    switch (AttackerId)
                    {
                    case ENEMYID::ID_SLASSHUR:
                        Code = SDCODE_SE(0x10A9);
                        if (!std::strcmp(AttackerMotion, "RunAttack"))
                            Code = SDCODE_SE(0x112E);
                        if (!std::strcmp(AttackerMotion, "AAB"))
                            Code = SDCODE_SE(0x101D);
                        break;

                    case ENEMYID::ID_KARAI:
                        Code = SDCODE_SE(0x113A);
                        break;

                    case ENEMYID::ID_ULTIMATE_SHREDDER:
                    case ENEMYID::ID_FEUDAL_JAPAN_SHREDDER:
                    case ENEMYID::ID_SHREDDER:
                        Code = SDCODE_SE(0x1106);
                        break;

                    case ENEMYID::ID_MIYAMOTO_USAGI:
                        Code = SDCODE_SE(0x113D);
                        break;

                    case ENEMYID::ID_ULTIMATE_NINJA:
                        Code = SDCODE_SE(0x1126);
                        if (!std::strcmp(AttackerMotion, "AA") && SoundPlaybackCheck(SDCODE_SE(0x1126)))
                            Code = SDCODE_SE(0x101D);
                        break;

                    case ENEMYID::ID_ELITE_FOOT_D:
                    case ENEMYID::ID_ELITE_FOOT_DUMMY_D:
                        Code = SDCODE_SE(0x111E);
                        break;

                    case ENEMYID::ID_ELITE_FOOT_B:
                    case ENEMYID::ID_ELITE_FOOT_C:
                    case ENEMYID::ID_ELITE_FOOT_DUMMY_B:
                    case ENEMYID::ID_ELITE_FOOT_DUMMY_C:
                        Code = SDCODE_SE(0x111A);
                        break;

                    case ENEMYID::ID_ELITE_FOOT_A:
                    case ENEMYID::ID_ELITE_FOOT_DUMMY_A:
                        Code = SDCODE_SE(0x1118);
                        break;

                    case ENEMYID::ID_TRAXIMUS:
                        Code = SDCODE_SE(0x1124);
                        if (!std::strcmp(AttackerName, "B"))
                            Code = SDCODE_SE(0x10DE);
                        break;

                    case ENEMYID::ID_FOOT_NINJA_ARCHER:
                    case ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER:
                        Code = SDCODE_SE(0x10CE);
                        break;

                    case ENEMYID::ID_LARGE_FOOT_NINJA:
                    case ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA:
                        Code = SDCODE_SE(0x10DF);
                        break;

                    case ENEMYID::ID_SPLINTER:
                    case ENEMYID::ID_FOOT_MECH_SPLINTER:
                    case ENEMYID::ID_FOOT_NINJA_STAFF:
                    case ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF:
                        Code = SDCODE_SE(0x10CA);
                        break;

                    case ENEMYID::ID_FOOT_NINJA_SWORD:
                    case ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD:
                        Code = SDCODE_SE(0x10CC);
                        break;

                    case ENEMYID::ID_RAPTOR:
                        Code = SDCODE_SE(0x10E1);
                        break;

                    case ENEMYID::ID_TRICERATION_SOLDIER:
                        Code = SDCODE_SE(0x10DE);
                        break;

                    case ENEMYID::ID_MURAL_CARNIVOROUS_PLANT:
                        Code = SDCODE_SE(0x10DA);
                        break;

                    case ENEMYID::ID_CARNIVOROUS_PLANT:
                        Code = SDCODE_SE(0x10D8);
                        break;

                    case ENEMYID::ID_SPASMOSAUR:
                        if (!std::strcmp(AttackerName, "enemy081_0000"))
                            Code = SDCODE_SE(0x10FA);
                        break;

                    default:
                        break;
                    };

                    if (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD)
                    {
                        const uint32 CodeArray[] =
                        {
                            SDCODE_SE(0x10A8),  // leo
                            SDCODE_SE(0x10B0),  // rap
                            SDCODE_SE(0x10B8),  // mic
                            SDCODE_SE(0x10C1),  // don
                            SDCODE_SE(0x10C1),  // sla
                            SDCODE_SE(0x10C1),  // cas
                            SDCODE_SE(0x10C1),  // kar
                            SDCODE_SE(0x10A8)   // spl
                        };

                        static_assert(COUNT_OF(CodeArray) == PLAYERID::ID_MAX, "update me");

						if (pParam->DefenderType == CCharacter::TYPE_PLAYER)
						{
							ASSERT(pParam->DefenderId >= 0);
							ASSERT(pParam->DefenderId < COUNT_OF(CodeArray));
							Code = CodeArray[pParam->DefenderId];
						};
                    }
                    else if (pParam->AttackResult == CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY)
                    {
                        if (pParam->DefenderType == CCharacter::TYPE_PLAYER)
                            KoeCode = SdSetDamageGetKoeHigh(pParam->DefenderId, Rnd);
                    }
                    else
                    {
                        if (pParam->DefenderType == CCharacter::TYPE_PLAYER)
                            KoeCode = SdSetDamageGetKoeLow(pParam->DefenderId, Rnd);
                    };
                }
                break;  // CCharacter::TYPE_ENEMY
            };
        }
        break;  // GAMEOBJECTTYPE::CHARACTER
    };

    CGameSound::PlayPositionSE(&pParam->Pos, Code, 0);

    if (pParam->DefenderType == CCharacter::TYPE_PLAYER)
    {
        if (!CGameSound::IsIDPlaying(PLAYERID::VALUE(pParam->DefenderId)))
            CGameSound::PlayPositionSE(&pParam->Pos, KoeCode, 0);
    };
};


static void SdSetAttackPlayer(const SE_ATTACK_PARAM* pParam)
{
    ASSERT(pParam);

    const char* Motion = pParam->Motion;
    int32 Rnd = Math::RandRange(0, 2);
    int32 Code = SD_NOCODE;
    int32 KoeCode = SD_NOCODE;

#define INIT_CODE_AND_KOE_MACRO(DefaultCodeHigh, DefaultCodeLow)    	\
     if (!std::strcmp(Motion, "A"))                                   	\
     {                                                                	\
         KoeCode = s_atKoeLow[(pParam->Id * 3) + Rnd];                 	\
         Code = SDCODE_SE(DefaultCodeLow);                            	\
     }                                                                 	\
     else                                                              	\
     {                                                                 	\
         KoeCode = s_atKoeHigh[(pParam->Id * 3) + Rnd];                	\
         Code = SDCODE_SE(DefaultCodeHigh);                            	\
     };

    if ((!std::strcmp(Motion, "Shuriken")) ||
        (!std::strcmp(Motion, "JShuriken")))
    {
        SoundSet(SDCODE_SE(0x1020));
    }
    else if ((*Motion == 'A') ||
            (*Motion == 'B')  ||
            (!std::strcmp(Motion, "RunAttack")) ||
            (!std::strcmp(Motion, "JAttack"))   ||
            (!std::strcmp(Motion, "JAttack1")))
    {
        switch (pParam->Id)
        {
        case PLAYERID::ID_LEO:
            INIT_CODE_AND_KOE_MACRO(0x10A7, 0x10A6);
            break;

        case PLAYERID::ID_RAP:            
            INIT_CODE_AND_KOE_MACRO(0x10AF, 0x10AE);
            break;

        case PLAYERID::ID_MIC:
            if (!std::strcmp(Motion, "JAttack"))
                break;
            INIT_CODE_AND_KOE_MACRO(0x10B7, 0x10B6);
            break;

        case PLAYERID::ID_DON:
            INIT_CODE_AND_KOE_MACRO(0x10C0, 0x10BF);
            break;

        case PLAYERID::ID_SLA:
            INIT_CODE_AND_KOE_MACRO(0x112D, 0x112D);
            break;

        case PLAYERID::ID_CAS:
            INIT_CODE_AND_KOE_MACRO(0x10C0, 0x10C0);
            break;

        case PLAYERID::ID_KAR:
            INIT_CODE_AND_KOE_MACRO(0x1139, 0x1139);
            break;

        case PLAYERID::ID_SPL:
            INIT_CODE_AND_KOE_MACRO(0x10A7, 0x10A6);
            break;

        default:
            ASSERT(false);
            break;
        };

        CGameSound::PlayPositionSE(&pParam->Pos, Code, 0);
        if (!CGameSound::IsIDPlaying(PLAYERID::VALUE(pParam->Id)))
        {
            if (std::strcmp(Motion, "AABBB") && std::strcmp(Motion, "AABBC"))
                CGameSound::PlayPositionSE(&pParam->Pos, KoeCode, 0);
        };
    };
};


static void SdSetAttackEnemy(const SE_ATTACK_PARAM* pParam)
{
    ASSERT(pParam);
    
    const char* Motion = pParam->Motion;
    int32 Rnd = Math::RandRange(0, 2);
    int32 Code = SD_NOCODE;
    int32 KoeCode = SD_NOCODE;
    
    switch (pParam->Id)
    {
    case ENEMYID::ID_PURPLE_DRAGON_GANG:
		Code = SDCODE_SE(0x10E2);
		if (std::strcmp(Motion, "C"))
			Code = SDCODE_SE(0x10CF);
        break;

    case ENEMYID::ID_FOOT_NINJA_SWORD:
    case ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD:
        Code = SDCODE_SE(0x10CB);
        break;

    case ENEMYID::ID_FOOT_NINJA_STAFF:
    case ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF:
    case ENEMYID::ID_UTROMS_SECURITY_PATROL:
    case ENEMYID::ID_FEDERATION_SOLDIER:
        Code = SDCODE_SE(0x10C9);
        break;

    case ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER:
        Code = SDCODE_SE(0x10CD);
        break;

    case ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA:
        if (!std::strcmp(Motion, "B") || !std::strcmp(Motion, "C"))
            Code = SDCODE_SE(0x10D4);
        break;

    case ENEMYID::ID_LARGE_EXO_SUITED_UTROMS:
        if (!std::strcmp(Motion, "C"))
            Code = SDCODE_SE(0x10E7);
        break;

    case ENEMYID::ID_TRICERATION_SOLDIER:
        Code = SDCODE_SE(0x10DD);
        break;

    case ENEMYID::ID_BLOOD_SUCKER:
    case ENEMYID::ID_POISON_BAT:
    case ENEMYID::ID_HYPNOSIS_BAT:
    case ENEMYID::ID_BOMB_BAT_MECH:
        if (!std::strcmp(Motion, "A"))
            Code = SDCODE_SE(0x10EF);
        break;

    case ENEMYID::ID_KURAGE:
        if (!std::strcmp(Motion, "B"))
            Code = SDCODE_SE(0x10EB);
        break;

    case ENEMYID::ID_MOUSER_ROBOT:
    case ENEMYID::ID_MOUSER_ROBOT_B:
    case ENEMYID::ID_KROKODIL_MOUSER:
        Code = SDCODE_SE(0x10F3);
        break;

    case ENEMYID::ID_RAPTOR:
        Code = SDCODE_SE(0x10E0);
        break;

    case ENEMYID::ID_CARNIVOROUS_PLANT:
        Code = SDCODE_SE(0x10D7);
        break;

    case ENEMYID::ID_MURAL_CARNIVOROUS_PLANT:
        Code = SDCODE_SE(0x10D9);
        break;

    case ENEMYID::ID_TRAXIMUS:
        KoeCode = Math::RandRange(0, 2) + SDCODE_SE(0x20F7);
        break;

    case ENEMYID::ID_LEATHER_HEAD:
        Code = SDCODE_SE(0x110C);
        break;

    case ENEMYID::ID_ELITE_FOOT_A:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_A:
        if (!std::strcmp(Motion, "A"))
            KoeCode = Math::RandRange(0, 1) + SDCODE_SE(0x20FE);
        else
            KoeCode = SDCODE_SE(0x2100);
        Code = SDCODE_SE(0x1119);
        break;

    case ENEMYID::ID_ELITE_FOOT_B:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_B:
        if (!std::strcmp(Motion, "A"))
            KoeCode = Math::RandRange(0, 1) + SDCODE_SE(0x20FE);
        else
            KoeCode = SDCODE_SE(0x2100);
        Code = SDCODE_SE(0x1116);
        break;

    case ENEMYID::ID_ELITE_FOOT_C:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_C:
        if (!std::strcmp(Motion, "A"))
            KoeCode = Math::RandRange(0, 1) + SDCODE_SE(0x20FE);
        else
            KoeCode = SDCODE_SE(0x2100);
        Code = SDCODE_SE(0x111B);
        break;

    case ENEMYID::ID_ELITE_FOOT_D:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_D:
        if (!std::strcmp(Motion, "A"))
            KoeCode = Math::RandRange(0, 1) + SDCODE_SE(0x20FE);
        else
            KoeCode = SDCODE_SE(0x2100);
        Code = SDCODE_SE(0x111F);
        break;

    case ENEMYID::ID_ULTIMATE_NINJA:
        Code = SDCODE_SE(0x1125);
        break;

    case ENEMYID::ID_SPASMOSAUR:
        if (std::strcmp(Motion, "C2"))
            Code = SDCODE_SE(0x10FB);
        break;

    case ENEMYID::ID_HUN_A:
    case ENEMYID::ID_HUN_B:
        Code = SDCODE_SE(0x1138);
        break;

    case ENEMYID::ID_KARAI:
        Code = SDCODE_SE(0x1139);
        KoeCode = Rnd + SDCODE_SE(0x20B8);
        if (!std::strcmp(Motion, "A"))
            KoeCode = Rnd + SDCODE_SE(0x20B3);
        break;

    case ENEMYID::ID_MIYAMOTO_USAGI:
        Code = SDCODE_SE(0x113C);
        break;

    case ENEMYID::ID_FOOT_MECH_SPLINTER:
    case ENEMYID::ID_SPLINTER:
        Code = SDCODE_SE(0x10A7);
        KoeCode = Rnd + SDCODE_SE(0x2084);
        if (!std::strcmp(Motion, "A"))
        {
            Code = SDCODE_SE(0x10A6);
            KoeCode = Rnd + SDCODE_SE(0x207F);
        }
        break;

    case ENEMYID::ID_SLASSHUR:
        Code = SDCODE_SE(0x112D);
        KoeCode = Rnd + SDCODE_SE(0x20D4);
        if (!std::strcmp(Motion, "A"))
            KoeCode = Rnd + SDCODE_SE(0x20CF);
        if (!std::strcmp(Motion, "AAB"))
        {
            KoeCode = 0;
            Code = 0;
        }
        break;

    case ENEMYID::ID_SHREDDER:
        Code = SDCODE_SE(0x1102);
        break;

    case ENEMYID::ID_FEUDAL_JAPAN_SHREDDER:
    case ENEMYID::ID_ULTIMATE_SHREDDER:
        Code = SDCODE_SE(0x1105);
        break;

    default:
        break;
    };
    
    CGameSound::PlayPositionSE(&pParam->Pos, Code, 0);    
    if (KoeCode)
        CGameSound::PlayPositionSE(&pParam->Pos, KoeCode, 0);
};


/*static*/ int32 CGameSound::m_nPauseLevel = 0;
/*static*/ int32 CGameSound::m_nSeConfig = 0;
/*static*/ int32 CGameSound::m_nBgmConfig = 0;
/*static*/ int32 CGameSound::m_nVoiceConfig = 0;
/*static*/ CGameSound::MODE CGameSound::m_mode = CGameSound::MODE_STEREO;
/*static*/ RwCamera* CGameSound::m_pCamera = nullptr;
/*static*/ CGameSound::VOICE_HIST CGameSound::m_aVoiceHist[2] = { 0 };
/*static*/ int32 CGameSound::m_nVoiceHistSide = 0;
/*static*/ CGameSound::LOOP_SE CGameSound::m_aLoopSe [] =
{
    { 0, SDCODE_SE(0x1018) },
    { 0, SDCODE_SE(0x101E) },
    { 0, SDCODE_SE(0x1029) },
    { 0, SDCODE_SE(0x102A) },
    { 0, SDCODE_SE(0x102F) },
    { 0, SDCODE_SE(0x1035) },
    { 0, SDCODE_SE(0x1037) },
    { 0, SDCODE_SE(0x1038) },
    { 0, SDCODE_SE(0x1039) },
    { 0, SDCODE_SE(0x103A) },
    { 0, SDCODE_SE(0x110D) },
    { 0, SDCODE_SE(0x10A0) },
    { 0, SDCODE_SE(0x10A1) },
    { 0, SDCODE_SE(0x10A2) },
};


/*static*/ void CGameSound::Initialize(void)
{
    m_nPauseLevel = 0;
    m_nBgmConfig = 0;
    m_nSeConfig = 0;
    m_nVoiceConfig = 0;
    m_mode = MODE_STEREO;
    m_pCamera = nullptr;

    for (int32 i = 0; i < COUNT_OF(m_aLoopSe); ++i)
        m_aLoopSe[i].m_nRefCount = 0;
};


/*static*/ void CGameSound::Terminate(void)
{
    ;
};


/*static*/ void CGameSound::SetMode(MODE mode)
{
	m_mode = mode;

	uint32 DriverCodes[] = { 0x405, 0x404, 0x406 }; // set mono, set stereo, set surround

	ASSERT(m_mode >= 0);
	ASSERT(m_mode < COUNT_OF(DriverCodes));

	static_assert(COUNT_OF(DriverCodes) == MODE_NUM, "update me");

	SoundSet(DriverCodes[m_mode]);
};


/*static*/ void CGameSound::LoadWave(int32 nWaveNo)
{
    SoundLoad(nWaveNo, 0);
};


/*static*/ bool CGameSound::IsLoadEnd(void)
{
    return !SoundGetLoadInfo();
};


/*static*/ void CGameSound::PlaySE(int32 nSE)
{
    ASSERT((nSE == SD_NOCODE) || ((nSE >= 0x1000) && (nSE <= 0x2FFF)));
    SoundSet(nSE);
};


/*static*/ void CGameSound::StopSE(int32 nSE)
{
    if (nSE != SD_NOCODE)
    {
        ASSERT((nSE >= 0x1000) && (nSE <= 0x2FFF));
        SoundSetEx(nSE | SD_MASTERCODE_FADE, 0, 0, 0);
    };

    clearReferenceCounter(nSE);
};


/*static*/ void CGameSound::FadeOutSE(int32 nSE, FADESPEED speed)
{
	if (nSE == SD_NOCODE)
		return;
	
	ASSERT((nSE >= 0x1000) && (nSE <= 0x2FFF));

	// in milliseconds
	const uint32 DriverParam[] = { 0, 1000, 2000 }; // fast, norma, slow

	ASSERT(speed >= 0);
	ASSERT(speed < COUNT_OF(DriverParam));

	static_assert(COUNT_OF(DriverParam) == FADESPEEDNUM, "update me");

	SoundSetEx(nSE | SD_MASTERCODE_FADE, DriverParam[speed], 0, 0);
};


/*static*/ void CGameSound::PlayBGM(int32 nBGM)
{
    ASSERT((nBGM == SD_NOCODE) || ((nBGM >= 0x3000) && (nBGM <= (0x3000 + SD_STRBGM_NUM))));
    SoundSet(nBGM);
};


/*static*/ void CGameSound::PlayVoice(int32 nVoice, PLAYERID::VALUE idPlayer /*= PLAYERID::ID_INVALID*/)
{
    ASSERT((nVoice == SD_NOCODE) || (nVoice >= 0x4000));

    m_aVoiceHist[m_nVoiceHistSide++ % COUNT_OF(m_aVoiceHist)] = { nVoice, idPlayer };

    if (idPlayer == PLAYERID::ID_INVALID)
    {
        SoundSetEx(nVoice, 0, 0, uint32(idPlayer));
    }
    else
	{
		// leo
		static uint32 SdSeFadeTblLeo[] =
		{
			SDCODE_SE(0x2000),
			SDCODE_SE(0x201A),
			SDCODE_SE(0x201B),
			SDCODE_SE(0x201C),
			SDCODE_SE(0x201D),
			SDCODE_SE(0x201E),
			SDCODE_SE(0x201F),
			SDCODE_SE(0x2020),
			SDCODE_SE(0x2021),
			SDCODE_SE(0x2022),
			SDCODE_SE(0x2023),
			SDCODE_SE(0x2024),
			SDCODE_SE(0x2025),
			SDCODE_SE(0x2026),
			SDCODE_SE(0x2027),
			SDCODE_SE(0x2028),
			SDCODE_SE(0x2029),
			SDCODE_SE(0x202A),
			SDCODE_SE(0x202B),
			SDCODE_SE(0x202C),
			SDCODE_SE(0x202D),
			SDCODE_SE(0x202E),
			SDCODE_SE(0x202F),
			SDCODE_SE(0x2030),
			SDCODE_SE(0x2031),
			SDCODE_SE(0x2032),
		};

		// rap
		static uint32 SdSeFadeTblRap[] =
		{
			SDCODE_SE(0x2001),
			SDCODE_SE(0x2033),
			SDCODE_SE(0x2034),
			SDCODE_SE(0x2035),
			SDCODE_SE(0x2036),
			SDCODE_SE(0x2037),
			SDCODE_SE(0x2038),
			SDCODE_SE(0x2039),
			SDCODE_SE(0x203A),
			SDCODE_SE(0x203B),
			SDCODE_SE(0x203C),
			SDCODE_SE(0x203D),
			SDCODE_SE(0x203E),
			SDCODE_SE(0x203F),
			SDCODE_SE(0x2040),
			SDCODE_SE(0x2041),
			SDCODE_SE(0x2042),
			SDCODE_SE(0x2043),
			SDCODE_SE(0x2044),
			SDCODE_SE(0x2045),
			SDCODE_SE(0x2046),
			SDCODE_SE(0x2047),
			SDCODE_SE(0x2048),
			SDCODE_SE(0x2049),
			SDCODE_SE(0x204A),
			SDCODE_SE(0x204B),
		};

		// mic
		static uint32 SdSeFadeTblMic[] =
		{
			SDCODE_SE(0x2002),
			SDCODE_SE(0x204C),
			SDCODE_SE(0x204D),
			SDCODE_SE(0x204E),
			SDCODE_SE(0x204F),
			SDCODE_SE(0x2050),
			SDCODE_SE(0x2051),
			SDCODE_SE(0x2052),
			SDCODE_SE(0x2053),
			SDCODE_SE(0x2054),
			SDCODE_SE(0x2055),
			SDCODE_SE(0x2056),
			SDCODE_SE(0x2057),
			SDCODE_SE(0x2058),
			SDCODE_SE(0x2059),
			SDCODE_SE(0x205A),
			SDCODE_SE(0x205B),
			SDCODE_SE(0x205C),
			SDCODE_SE(0x205D),
			SDCODE_SE(0x205E),
			SDCODE_SE(0x205F),
			SDCODE_SE(0x2060),
			SDCODE_SE(0x2061),
			SDCODE_SE(0x2062),
			SDCODE_SE(0x2063),
			SDCODE_SE(0x2064),
		};

		// don
		static uint32 SdSeFadeTblDon[] =
		{
			SDCODE_SE(0x2003),
			SDCODE_SE(0x2065),
			SDCODE_SE(0x2066),
			SDCODE_SE(0x2067),
			SDCODE_SE(0x2068),
			SDCODE_SE(0x2069),
			SDCODE_SE(0x206A),
			SDCODE_SE(0x206B),
			SDCODE_SE(0x206C),
			SDCODE_SE(0x206D),
			SDCODE_SE(0x206E),
			SDCODE_SE(0x206F),
			SDCODE_SE(0x2070),
			SDCODE_SE(0x2071),
			SDCODE_SE(0x2072),
			SDCODE_SE(0x2073),
			SDCODE_SE(0x2074),
			SDCODE_SE(0x2075),
			SDCODE_SE(0x2076),
			SDCODE_SE(0x2077),
			SDCODE_SE(0x2078),
			SDCODE_SE(0x2079),
			SDCODE_SE(0x207A),
			SDCODE_SE(0x207B),
			SDCODE_SE(0x207C),
			SDCODE_SE(0x207D),
		};

		// sla
		static uint32 SdSeFadeTblSla[] =
		{
			SDCODE_SE(0x20CE),
			SDCODE_SE(0x20CF),
			SDCODE_SE(0x20D0),
			SDCODE_SE(0x20D1),
			SDCODE_SE(0x20D2),
			SDCODE_SE(0x20D3),
			SDCODE_SE(0x20D4),
			SDCODE_SE(0x20D5),
			SDCODE_SE(0x20D6),
			SDCODE_SE(0x20D7),
			SDCODE_SE(0x20D8),
			SDCODE_SE(0x20D9),
			SDCODE_SE(0x20DA),
			SDCODE_SE(0x20DB),
			SDCODE_SE(0x20DC),
			SDCODE_SE(0x20DD),
			SDCODE_SE(0x20DF),
			SDCODE_SE(0x20E1),
			SDCODE_SE(0x20E0),
			SDCODE_SE(0x20E2),
			SDCODE_SE(0x20E3),
			SDCODE_SE(0x20E4),
			SDCODE_SE(0x20E6),
			SDCODE_SE(0x20E8),
			SDCODE_SE(0x20E9),
		};

		//cas 
		static uint32 SdSeFadeTblCas[] =
		{
			SDCODE_SE(0x2098),
			SDCODE_SE(0x2099),
			SDCODE_SE(0x209A),
			SDCODE_SE(0x209B),
			SDCODE_SE(0x209C),
			SDCODE_SE(0x209D),
			SDCODE_SE(0x209E),
			SDCODE_SE(0x209F),
			SDCODE_SE(0x20A0),
			SDCODE_SE(0x20A1),
			SDCODE_SE(0x20A2),
			SDCODE_SE(0x20A3),
			SDCODE_SE(0x20A4),
			SDCODE_SE(0x20A5),
			SDCODE_SE(0x20A6),
			SDCODE_SE(0x20A7),
			SDCODE_SE(0x20A8),
			SDCODE_SE(0x20A9),
			SDCODE_SE(0x20AA),
			SDCODE_SE(0x20AB),
			SDCODE_SE(0x20AC),
			SDCODE_SE(0x20AD),
			SDCODE_SE(0x20AE),
			SDCODE_SE(0x20B0),
			SDCODE_SE(0x20B1),
		};

		// kar
		static uint32 SdSeFadeTblKar[] =
		{
			SDCODE_SE(0x20B2),
			SDCODE_SE(0x20B3),
			SDCODE_SE(0x20B4),
			SDCODE_SE(0x20B5),
			SDCODE_SE(0x20B6),
			SDCODE_SE(0x20B7),
			SDCODE_SE(0x20B8),
			SDCODE_SE(0x20B9),
			SDCODE_SE(0x20BA),
			SDCODE_SE(0x20BB),
			SDCODE_SE(0x20BC),
			SDCODE_SE(0x20BD),
			SDCODE_SE(0x20BE),
			SDCODE_SE(0x20BF),
			SDCODE_SE(0x20C0),
			SDCODE_SE(0x20C1),
			SDCODE_SE(0x20C2),
			SDCODE_SE(0x20C4),
			SDCODE_SE(0x20C5),
			SDCODE_SE(0x20C6),
			SDCODE_SE(0x20C7),
			SDCODE_SE(0x20C8),
			SDCODE_SE(0x20CA),
			SDCODE_SE(0x20CC),
			SDCODE_SE(0x20CD),
		};

		// spl
		static uint32 SdSeFadeTblSpl[] =
		{
			SDCODE_SE(0x207E),
			SDCODE_SE(0x207F),
			SDCODE_SE(0x2080),
			SDCODE_SE(0x2081),
			SDCODE_SE(0x2082),
			SDCODE_SE(0x2083),
			SDCODE_SE(0x2084),
			SDCODE_SE(0x2085),
			SDCODE_SE(0x2086),
			SDCODE_SE(0x2087),
			SDCODE_SE(0x2088),
			SDCODE_SE(0x2089),
			SDCODE_SE(0x208A),
			SDCODE_SE(0x208B),
			SDCODE_SE(0x208C),
			SDCODE_SE(0x208D),
			SDCODE_SE(0x208E),
			SDCODE_SE(0x208F),
			SDCODE_SE(0x2090),
			SDCODE_SE(0x2091),
			SDCODE_SE(0x2092),
			SDCODE_SE(0x2093),
			SDCODE_SE(0x2094),
			SDCODE_SE(0x2096),
			SDCODE_SE(0x2097),
		};

		static struct FADETABLEDATA
		{
			uint32* Data;
			int32 Count;
		} FadeTableData[] =
		{
			{ SdSeFadeTblLeo, COUNT_OF(SdSeFadeTblLeo) },
			{ SdSeFadeTblRap, COUNT_OF(SdSeFadeTblRap) },
			{ SdSeFadeTblMic, COUNT_OF(SdSeFadeTblMic) },
			{ SdSeFadeTblDon, COUNT_OF(SdSeFadeTblDon) },
			{ SdSeFadeTblSla, COUNT_OF(SdSeFadeTblSla) },
			{ SdSeFadeTblCas, COUNT_OF(SdSeFadeTblCas) },
			{ SdSeFadeTblKar, COUNT_OF(SdSeFadeTblKar) },
			{ SdSeFadeTblSpl, COUNT_OF(SdSeFadeTblSpl) },
		};

		static_assert(COUNT_OF(FadeTableData) == PLAYERID::ID_MAX, "update me");

		ASSERT(idPlayer >= 0);
		ASSERT(idPlayer < COUNT_OF(FadeTableData));

		for (int32 i = 0; i < FadeTableData[idPlayer].Count; ++i)
			SoundSet(FadeTableData[idPlayer].Data[i] | SD_MASTERCODE_FADE);
		
		SoundSetEx(nVoice, 0, 0, (uint32)idPlayer);
    };
};


/*static*/ bool CGameSound::IsVoicePlaying(void)
{
    if (!SoundVoiceBusyCheck())
    {
        std::memset(m_aVoiceHist, 0x00, sizeof(m_aVoiceHist));
        return false;
    };
    
    return true;
};


/*static*/ bool CGameSound::IsIDPlaying(PLAYERID::VALUE idPlayer)
{
    for (int32 i = 0; i < COUNT_OF(m_aVoiceHist); ++i)
    {
        if (m_aVoiceHist[i].m_PlayerID != idPlayer)
            continue;

        if (m_aVoiceHist[i].m_nVoiceCode == SD_NOCODE)
            continue;

		int32 voiceServerNo = int32(idPlayer) & 1;
		int32 voiceCode = SoundVoxCodeCheck(voiceServerNo);

		if (voiceCode == m_aVoiceHist[i].m_nVoiceCode)
            return true;
    };

    return false;
};


/*static*/ void CGameSound::FadeOut(FADESPEED speed)
{
	uint32 DriverCode[] = { 0x407, 0x408, 0x409 }; // fast, normal, slow

	ASSERT(speed >= 0);
	ASSERT(speed < COUNT_OF(DriverCode));
	
	static_assert(COUNT_OF(DriverCode) == FADESPEEDNUM, "update me");
    
    SoundSet(DriverCode[speed]);
};


/*static*/ void CGameSound::Stop(void)
{
    SoundSet(0x400);
    clearReferenceAll();
};


/*static*/ void CGameSound::Pause(void)
{
    ASSERT(m_nPauseLevel >= 0);
    
    if (!m_nPauseLevel++)
        SoundSet(0x402);
};


/*static*/ void CGameSound::Resume(void)
{
    ASSERT(m_nPauseLevel > 0);
    
    if (!--m_nPauseLevel)
        SoundSet(0x403);
};


/*static*/ void CGameSound::PlayPositionSE(const RwV3d* pPos, int32 nSE, int32 id /*= 0*/)
{
	if (!pPos)
	{
		PlaySE(nSE);
		return;
	};

	if (Math::Vec3_Equal(pPos, &Math::VECTOR3_ZERO))
	{
		SoundSetEx(nSE | SD_MASTERCODE_SURCTRL, 0, 0, id);
		return;
	};

	int32 distance = 0;
	int32 rotation = 0;

	if (!calcSEPositionInfo(pPos, &distance, &rotation))
		PlaySE(nSE);

	distance -= 10;
	distance = Clamp(distance, 0, distance); // clamp for min only

	SoundSetEx(
		nSE | SD_MASTERCODE_SURCTRL,
		(uint32)rotation,
		(uint32)((0x10000 / 50) * distance),
		(uint32)id
	);
};


/*static*/ void CGameSound::PlayObjectSE(const CCharacter* pCharacter, int32 nSE, int32 id /*= 0*/)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    pCharacter->GetPosition(&vPosition);
    PlayPositionSE(&vPosition, nSE, id);
};


/*static*/ void CGameSound::PlayObjectSE(const CGimmick* pGimmick, int32 nSE, int32 id /*= 0*/)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    pGimmick->GetPosition(&vPosition);
    PlayPositionSE(&vPosition, nSE, id);
};


/*static*/ void CGameSound::PlayObjectSE(const CEffect* pEffect, int32 nSE, int32 id /*= 0*/)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    pEffect->GetPosition(&vPosition);
    PlayPositionSE(&vPosition, nSE, id);
};


/*static*/ void CGameSound::PlayObjectSE(const CPlayer* pPlayer, int32 nSE, int32 id /*= 0*/)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    pPlayer->GetPosition(&vPosition);
    PlayPositionSE(&vPosition, nSE, id);
};


/*static*/ void CGameSound::PlayAttackSE(const CCharacter* pCharacter)
{
	if (!pCharacter)
		return;

	SE_ATTACK_PARAM param;
	std::memset(&param, 0, sizeof(param));

	param.Type = pCharacter->GetCharacterType();
	param.Motion = pCharacter->GetMotionName();
	pCharacter->GetBodyPosition(&param.Pos);

	if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
	{
		param.Id = static_cast<const CPlayerCharacter*>(pCharacter)->GetID();
		SdSetAttackPlayer(&param);
	}
	else
	{
		param.Id = static_cast<const CCharacterCompositor*>(pCharacter)->GetID();
		SdSetAttackEnemy(&param);
	};
};


/*static*/ void CGameSound::PlayDamageSE(SE_DAMAGE_PARAM* pParam)
{
    SdSetDamage(pParam);
};


/*static*/ void CGameSound::PlayDeathSE(const CCharacter* pCharacter)
{
	if (!pCharacter)
		return;

	if (pCharacter->GetCharacterType() != CCharacter::TYPE_ENEMY)
		return;

	RwV3d vPos = Math::VECTOR3_ZERO;
	pCharacter->GetBodyPosition(&vPos);

	const CCharacterCompositor* pCompositor = static_cast<const CCharacterCompositor*>(pCharacter);

	switch (pCompositor->GetID())
	{
	case ENEMYID::ID_PURPLE_DRAGON_GANG:
	case ENEMYID::ID_FOOT_NINJA_SWORD:
	case ENEMYID::ID_FOOT_NINJA_STAFF:
	case ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD:
	case ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF:
	case ENEMYID::ID_UTROMS_SECURITY_PATROL:
	case ENEMYID::ID_FEDERATION_SOLDIER:
	case ENEMYID::ID_MOBSTER:
	case ENEMYID::ID_FOOT_NINJA_ARCHER:
	case ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER:
	case ENEMYID::ID_FEDERATION_SHOOTER:
	case ENEMYID::ID_EXO_SUITED_UTROMS:
	case ENEMYID::ID_CAPTAIN_OF_FEDERATION:
	case ENEMYID::ID_LARGE_MOBSTER:
	case ENEMYID::ID_LARGE_FOOT_NINJA:
	case ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA:
	case ENEMYID::ID_LARGE_FEDERATION_SOLDIET:
	case ENEMYID::ID_LARGE_EXO_SUITED_UTROMS:
	case ENEMYID::ID_FOOT_TECH_NINJA:
	case ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA:
		PlayPositionSE(&vPos, SDCODE_SE(0x10D5));
		break;

	case ENEMYID::ID_SPASMOSAUR:
		PlayPositionSE(&vPos, SDCODE_SE(0x2006));
        break;

    default:
        break;
    };
};


/*static*/ void CGameSound::PlayOtherDamageSE(const CGameObject* pObjAttacker, const CGameObject* pObjDefender)
{
    static const int32 aPlayerHitSE[] =
    {
        // leo
        SDCODE_SE(0x10A9),
        SDCODE_SE(0x10AA),
        SDCODE_SE(0x10AB),
        SDCODE_SE(0x10AC),

        // rap
        SDCODE_SE(0x10B1),
        SDCODE_SE(0x10B2),
        SDCODE_SE(0x10B3),
        SDCODE_SE(0x10B4),

        // mic
        SDCODE_SE(0x10B9),
        SDCODE_SE(0x10BA),
        SDCODE_SE(0x10BB),
        SDCODE_SE(0x10BC),

        // don
        SDCODE_SE(0x10C2),
        SDCODE_SE(0x10C3),
        SDCODE_SE(0x10C4),
        SDCODE_SE(0x10C5),

        // sla
        SDCODE_SE(0x10AC),
        SDCODE_SE(0x10AC),
        SDCODE_SE(0x10AC),
        SDCODE_SE(0x10AC),

        // cas
        SDCODE_SE(0x10B4),
        SDCODE_SE(0x10B4),
        SDCODE_SE(0x10B4),
        SDCODE_SE(0x10B4),

        // kar
        SDCODE_SE(0x10AC),
        SDCODE_SE(0x10AC),
        SDCODE_SE(0x10AC),
        SDCODE_SE(0x10AC),

        // spl
        SDCODE_SE(0x10C5),
        SDCODE_SE(0x10C5),
        SDCODE_SE(0x10C5),
        SDCODE_SE(0x10C5),
    };

    static_assert(COUNT_OF(aPlayerHitSE) == (PLAYERID::ID_MAX * 4), "update me");

    if (pObjDefender->GetType() != GAMEOBJECTTYPE::GIMMICK)
        return;

    if (pObjAttacker->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;
    
	const CCharacter* pAttacker = reinterpret_cast<const CCharacter*>(pObjAttacker);
	if ((pAttacker->GetCharacterType() != CCharacter::TYPE_PLAYER))
		return;
	
	const CGimmick* pDefender = reinterpret_cast<const CGimmick*>(pObjDefender);

    if ((pDefender->GetID() == GIMMICKID::ID_G_SSHIPS) ||
		(pDefender->GetID() == GIMMICKID::ID_N_BRKCAR))
	{
        RwV3d vGimmickPos = Math::VECTOR3_ZERO;
		pDefender->GetPosition(&vGimmickPos);

		const CPlayerCharacter* pPlayer = reinterpret_cast<const CPlayerCharacter*>(pAttacker);
		const int32* pSE = &aPlayerHitSE[pPlayer->GetID() * 4];
		
		PlayPositionSE(&vGimmickPos, pSE[2], 0);
    };
};


/*static*/ void CGameSound::PlayWalkSE(SE_WALK_PARAM* pParam)
{
    ASSERT(pParam);

    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_TODDLING))
        PlayPositionSE(&pParam->Pos, SDCODE_SE(0x108A), 0);
    
    if (pParam->GroundParam == MAPTYPES::ATTRIBUTE_WATER)
        PlayPositionSE(&pParam->Pos, SDCODE_SE(0x109F), 0);
};


/*static*/ void CGameSound::PlayEffectSE(CEffect* pEffect, EFFECTID::VALUE idEffect)
{
    int32 nSE = getEffectSE(idEffect);
    if (nSE == SD_NOCODE)
        return;

    PlayObjectSE(pEffect, nSE);
    addReferenceCounter(nSE);
};


/*static*/ void CGameSound::StopEffectSE(CEffect* pEffect, EFFECTID::VALUE idEffect)
{
    int32 nSE = getEffectSE(idEffect);
    if (nSE == SD_NOCODE)
        return;

    if (isLoopSE(nSE) && subtractReferenceCounter(nSE))
        StopSE(nSE);
};


/*static*/ void CGameSound::StageBefore(STAGEID::VALUE idStage)
{
    int32 Param = 0;
    int32 Command = 2;
    
    switch (idStage)
    {
    case STAGEID::ID_ST02N:
    case STAGEID::ID_ST05N:
    case STAGEID::ID_ST06R:
    case STAGEID::ID_ST09N:
    case STAGEID::ID_ST17N:
    case STAGEID::ID_ST18F:
    case STAGEID::ID_ST18FB:
    case STAGEID::ID_ST33J:
    case STAGEID::ID_ST35N:
    case STAGEID::ID_ST39N:
    case STAGEID::ID_ST41N:
    case STAGEID::ID_ST42NB:
    case STAGEID::ID_ST46R:
    case STAGEID::ID_ST47OB:
    case STAGEID::ID_ST48N:
    case STAGEID::ID_ST49N:
        Command = 3;
        break;

    case STAGEID::ID_ST02NB:
    case STAGEID::ID_ST34N:
    case STAGEID::ID_ST36N:
    case STAGEID::ID_ST43N:
    case STAGEID::ID_ST45N:
    case STAGEID::ID_ST58OB1:
    case STAGEID::ID_ST58OB2:
        Command = 4;
        break;

    case STAGEID::ID_ST13R:
    case STAGEID::ID_ST14N:
        Command = 5;
        break;

    case STAGEID::ID_ST15N:
        Command = 12;
        break;

    case STAGEID::ID_ST23N:
        Command = 7;
        break;

    case STAGEID::ID_ST31NB:
        Command = 8;
        break;

    case STAGEID::ID_ST37N:
    case STAGEID::ID_ST44NB:
        Command = 6;
        break;

    case STAGEID::ID_ST50NB:
        Command = 9;
        break;

    case STAGEID::ID_ST52F:
    case STAGEID::ID_ST52FB:
        Command = 10;
        break;

    case STAGEID::ID_ST60X_A01:
    case STAGEID::ID_ST60X_A02:
    case STAGEID::ID_ST60X_A03:
    case STAGEID::ID_ST60X_A04:
    case STAGEID::ID_ST60X_A05:
    case STAGEID::ID_ST60X_A06:
    case STAGEID::ID_ST60X_A07:
    case STAGEID::ID_ST60X_A08:
    case STAGEID::ID_ST60X_A09:
    case STAGEID::ID_ST60X_A10:
    case STAGEID::ID_ST60X_B01:
    case STAGEID::ID_ST60X_B02:
    case STAGEID::ID_ST60X_B03:
    case STAGEID::ID_ST60X_B04:
    case STAGEID::ID_ST60X_B05:
    case STAGEID::ID_ST60X_B06:
    case STAGEID::ID_ST60X_B07:
    case STAGEID::ID_ST60X_B08:
    case STAGEID::ID_ST60X_B09:
    case STAGEID::ID_ST60X_B10:
    case STAGEID::ID_ST60X_C01:
    case STAGEID::ID_ST60X_C02:
    case STAGEID::ID_ST60X_C03:
    case STAGEID::ID_ST60X_C04:
    case STAGEID::ID_ST60X_C05:
    case STAGEID::ID_ST60X_C06:
    case STAGEID::ID_ST60X_C07:
    case STAGEID::ID_ST60X_C08:
    case STAGEID::ID_ST60X_C09:
    case STAGEID::ID_ST60X_C10:
    case STAGEID::ID_ST60X_D01:
    case STAGEID::ID_ST60X_D02:
    case STAGEID::ID_ST60X_D03:
    case STAGEID::ID_ST60X_D04:
    case STAGEID::ID_ST60X_D05:
    case STAGEID::ID_ST60X_D06:
    case STAGEID::ID_ST60X_D07:
    case STAGEID::ID_ST60X_D08:
    case STAGEID::ID_ST60X_D09:
    case STAGEID::ID_ST60X_D10:
        Command = 11;
        break;

	default:
		break;
    };
    
    switch (idStage)
    {
    case STAGEID::ID_ST02N:
        SoundLoad(Command, 1);
        break;

    case STAGEID::ID_ST18FB:
        SoundLoad(Command, 2);
        break;

    case STAGEID::ID_ST42NB:
        SoundLoad(Command, 3);
        break;

    case STAGEID::ID_ST49N:
        SoundLoad(Command, 4);
        break;

    default:
        SoundLoad(Command, Param);
        break;
    };
};


/*static*/ void CGameSound::StageAfter(STAGEID::VALUE idStage)
{
	;
};


/*static*/ bool CGameSound::calcSEPositionInfo(const RwV3d* pvPos, int32* pDist, int32* pRot)
{
	ASSERT(pvPos);
	ASSERT(pDist);
	ASSERT(pRot);

	if (!m_pCamera)
	{
		*pDist = 0;
		*pRot = 0;
		return false;
	};

	RwV3d vCameraPos = Math::VECTOR3_ZERO;
	RwV3d vCameraAt = Math::VECTOR3_ZERO;
	getCameraInfo(m_pCamera, &vCameraPos, &vCameraAt);
	vCameraAt.y = 0.0f;

	RwV3d vDir = Math::VECTOR3_ZERO;
	Math::Vec3_Sub(&vDir, pvPos, &vCameraPos);
	vDir.y = 0.0f;

	float d = Math::Vec3_Length(&vDir);

	RwV3d vUp = Math::VECTOR3_ZERO;
	Math::Vec3_Cross(&vUp, &vCameraAt, &vDir);

	float z = Math::Vec3_Dot(&vDir, &vCameraAt);
	float x = Math::Vec3_Length(&vUp);
	if (vUp.y < 0.0f)
		x = -x;

	float angle = Math::ATan2(x, z);
	if (angle < 0.0f)
		angle += MATH_PI2;

	*pDist = (int32)d;
	*pRot = (int32)(((float)0x10000 / (MATH_PI * 2.0f)) * angle);

	return true;
};


/*static*/ void CGameSound::getCameraInfo(const RwCamera* pCamera, RwV3d* pvPos, RwV3d* pvAt)
{
    ASSERT(pCamera);
    
    RwFrame* pFrame = RwCameraGetFrameMacro(pCamera);
    ASSERT(pFrame);
    
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);
    ASSERT(pMatrix);

    *pvPos = pMatrix->pos;
    *pvAt  = pMatrix->at;
};


/*static*/ int32 CGameSound::getEffectSE(EFFECTID::VALUE idEffect)
{
    int32 nSE = SD_NOCODE;

    switch (idEffect)
    {
    case EFFECTID::ID_LEO_HIT_S:
    case EFFECTID::ID_LEO_HIT_B:
    case EFFECTID::ID_LEO_SP:
    case EFFECTID::ID_RAP_HIT_S:
    case EFFECTID::ID_RAP_HIT_B:
    case EFFECTID::ID_RAP_SP:
    case EFFECTID::ID_DON_HIT_S:
    case EFFECTID::ID_DON_HIT_B:
    case EFFECTID::ID_DON_LASER_FIRE:
    case EFFECTID::ID_DON_LASER_HIT:
    case EFFECTID::ID_MIC_HIT_S:
    case EFFECTID::ID_MIC_HIT_B:
    case EFFECTID::ID_MIC_SP_BALL:
    case EFFECTID::ID_MIC_SP_END:
    case EFFECTID::ID_SLA_SP:
    case EFFECTID::ID_KAR_SP:
    case EFFECTID::ID_CAS_SP_BALL:
    case EFFECTID::ID_CAS_SP_END:
    case EFFECTID::ID_ALL_HIT_S:
    case EFFECTID::ID_ALL_HIT_B:
    case EFFECTID::ID_ALL_PIYOHIT:
    case EFFECTID::ID_ALL_GUARD1:
    case EFFECTID::ID_ALL_GUARD2:
    case EFFECTID::ID_ALL_GUARDBREAK:
    case EFFECTID::ID_ALL_COUNT:
    case EFFECTID::ID_MIC_REFLECT:
    case EFFECTID::ID_ALL_SLEEP:
    case EFFECTID::ID_ALL_WAKE:
    case EFFECTID::ID_ALL_NENSHI1:
    case EFFECTID::ID_ALL_NENCUT:
    case EFFECTID::ID_ALL_FREEZEND:
    case EFFECTID::ID_ALL_DASH1:
    case EFFECTID::ID_ALL_DASH2:
    case EFFECTID::ID_BARRIER_START:
    case EFFECTID::ID_BIRIBIRI:
    case EFFECTID::ID_ALL_TAME_FLASH:
    case EFFECTID::ID_ALL_TAG:
    case EFFECTID::ID_SETUP_METUKEI:
    case EFFECTID::ID_SETUP_ATTACK:
    case EFFECTID::ID_SETUP_CHARGE:
    case EFFECTID::ID_SETUP_DEFENCE:
    case EFFECTID::ID_SETUP_KNIFE:
    case EFFECTID::ID_I_CRYSHINE_R:
    case EFFECTID::ID_I_CRYSHINE_G:
    case EFFECTID::ID_I_CRYSHINE_O:
    case EFFECTID::ID_I_CRYSHINE_W:
    case EFFECTID::ID_ALL_JUMP:
    case EFFECTID::ID_ALL_PITAN:
    case EFFECTID::ID_ALL_DOWNSMOKE:
    case EFFECTID::ID_ALL_JUMPFAIL:
    case EFFECTID::ID_EXPL_B1:
    case EFFECTID::ID_EXPL_B1_PLAYER:
    case EFFECTID::ID_EXPL_B1_CHARA:
    case EFFECTID::ID_SLEEPGAS:
    case EFFECTID::ID_FREEZEGAS:
    case EFFECTID::ID_ALL_W_HAMON:
    case EFFECTID::ID_ALL_DASH_SNOW:
    case EFFECTID::ID_ALL_BREATH:
    case EFFECTID::ID_WHITESMOKE:
    case EFFECTID::ID_BLACKSMOKE:
    case EFFECTID::ID_FIRE_TORCH:
    case EFFECTID::ID_ROCK:
    case EFFECTID::ID_WOOD:
    case EFFECTID::ID_METAL:
    case EFFECTID::ID_GRASS:
    case EFFECTID::ID_DUSTBOX:
    case EFFECTID::ID_CERAMICS:
    case EFFECTID::ID_METEORITE:
    case EFFECTID::ID_SPARKS:
    case EFFECTID::ID_PIPE_STEAM:
    case EFFECTID::ID_PIPE_FREEZEGAS:
    case EFFECTID::ID_SPRAYWTER:
    case EFFECTID::ID_SWEEP_SPLASH:
    case EFFECTID::ID_FIRE_ARROWSHOT:
    case EFFECTID::ID_SLEEP_ARROWSHOT:
    case EFFECTID::ID_FIRE_ARROW:
    case EFFECTID::ID_SLEEP_ARROW:
    case EFFECTID::ID_SPSM_YODAR2:
    case EFFECTID::ID_SPSM_YODAR1:
    case EFFECTID::ID_SPSM_KABECRASH:
    case EFFECTID::ID_SPSM_RUNSMOKE:
    case EFFECTID::ID_SPSM_TBSHOT:
    case EFFECTID::ID_ROD_FLASH:
    case EFFECTID::ID_DRKBALL_START:
    case EFFECTID::ID_DRKBALL:
    case EFFECTID::ID_DRKBALL_END:
    case EFFECTID::ID_DRK_PILLAR:
    case EFFECTID::ID_ROD_BEAM:
    case EFFECTID::ID_ROD_BEAM_HIT:
    case EFFECTID::ID_DRAGONSMOKE:
    case EFFECTID::ID_WINDWARP_START:
    case EFFECTID::ID_WINDWARP_OUT:
    case EFFECTID::ID_ULNIN_ENMAKU:
    case EFFECTID::ID_WARP_START:
    case EFFECTID::ID_WARP_OUT:
    case EFFECTID::ID_HPSTEAL_LIGHT:
    case EFFECTID::ID_SICKLE_WARP:
    case EFFECTID::ID_SLA_BARRIER:
    case EFFECTID::ID_SLA_SHADOW:
    case EFFECTID::ID_HPSTEAL:
    case EFFECTID::ID_SLABALL:
    case EFFECTID::ID_SLABALL_END:
    case EFFECTID::ID_BLADE_FLASh:
    case EFFECTID::ID_LASER_FIRE_FED:
    case EFFECTID::ID_LASER_HIT_FED:
    case EFFECTID::ID_LASER_FIRE_TRI:
    case EFFECTID::ID_LASER_HIT_TRI:
    case EFFECTID::ID_GUNBALL_FIRE:
    case EFFECTID::ID_SPIDERNET:
    case EFFECTID::ID_SPIDERSMOKE:
    case EFFECTID::ID_TBSHOT:
    case EFFECTID::ID_TBSHOT_MARK:
    case EFFECTID::ID_LASER_FIRE_ALL:
    case EFFECTID::ID_LASER_HIT_ALL:
    case EFFECTID::ID_FLAME_MAP:
    case EFFECTID::ID_M12_BREAKROCK:
    case EFFECTID::ID_ENGINESPARK:
    case EFFECTID::ID_SPARK_SLOW:
    case EFFECTID::ID_BUBBLE:
    case EFFECTID::ID_WFALL_SMOKE:
    case EFFECTID::ID_WFALL_SPRASH:
    case EFFECTID::ID_RIDE_ENGINE:
    case EFFECTID::ID_RIDE_PARTICLE:
    case EFFECTID::ID_RIDE_WIND:
        nSE = SDCODE_SE(0);
        break;

    case EFFECTID::ID_DON_LASER_BEAM:
        nSE = SDCODE_SE(0x10C8);
        break;

    case EFFECTID::ID_ALL_PIYORI:
        nSE = SDCODE_SE(0x101E);
        break;

    case EFFECTID::ID_ALL_TAME3:
        nSE = SDCODE_SE(0x10A2);
        break;

    case EFFECTID::ID_ALL_TAME2:
        nSE = SDCODE_SE(0x10A1);
        break;

    case EFFECTID::ID_ALL_TAME1:
        nSE = SDCODE_SE(0x10A0);
        break;

    case EFFECTID::ID_I_RECOVER_B:
    case EFFECTID::ID_I_RECOVER_S:
        nSE = SDCODE_SE(0x1018);
        break;

    case EFFECTID::ID_I_CRYGET_R:
    case EFFECTID::ID_I_CRYGET_G:
    case EFFECTID::ID_I_CRYGET_O:
    case EFFECTID::ID_I_CRYGET_W:
        nSE = SDCODE_SE(0x1017);
        break;

    case EFFECTID::ID_I_GETANTIQUE:
        nSE = SDCODE_SE(0x1012);
        break;

    case EFFECTID::ID_I_GET:
        nSE = SDCODE_SE(0x101B);
        break;

    case EFFECTID::ID_KO_FLASH:
        nSE = SDCODE_SE(0x10D2);
        break;

    case EFFECTID::ID_KO_EXPL:
        nSE = SDCODE_SE(0x10F2);
        break;

    case EFFECTID::ID_KO_SMOKE:
        nSE = SDCODE_SE(0x10DC);
        break;

    case EFFECTID::ID_ALL_W_DOBON:
        nSE = SDCODE_SE(0x110E);
        break;

    case EFFECTID::ID_ALL_W_SPLASH:
        nSE = SDCODE_SE(0x110D);
        break;

    case EFFECTID::ID_BREAKSIGN:
        nSE = SDCODE_SE(0x10FD);
        break;

    case EFFECTID::ID_TENGSWORD:
        nSE = SDCODE_SE(0x1105);
        break;

    case EFFECTID::ID_SHRD_ELEC:
        nSE = SDCODE_SE(0x1103);
        break;

    case EFFECTID::ID_THUNDER:
        nSE = SDCODE_SE(0x1109);
        break;

    case EFFECTID::ID_SHRD_ELBALL:
        nSE = SDCODE_SE(0x1108);
        break;

    case EFFECTID::ID_SHRD_ELEND:
        nSE = SDCODE_SE(0x80001108);
        break;

    case EFFECTID::ID_WIND_PRESS:
        nSE = SDCODE_SE(0x1079);
        break;

    case EFFECTID::ID_LASER_BEAM_FED:
    case EFFECTID::ID_LASER_BEAM_TRI:
    case EFFECTID::ID_LASER_BEAM_ALL:
        nSE = SDCODE_SE(0x1031);
        break;

    case EFFECTID::ID_GUNBALL:
        nSE = SDCODE_SE(0x10E9);
        break;

    case EFFECTID::ID_BREAKICE:
        nSE = SDCODE_SE(0x10FC);
        break;

    default:
        ASSERT(false);
        break;
    };
    
    return nSE;
};


/*static*/ bool CGameSound::isLoopSE(int32 nSE)
{
    for (int32 i = 0; i < COUNT_OF(m_aLoopSe); ++i)
    {
        if (m_aLoopSe[i].m_nSeCode == nSE)
            return true;
    };

    return false;
};


/*static*/ void CGameSound::addReferenceCounter(int32 nSE)
{
    for (int32 i = 0; i < COUNT_OF(m_aLoopSe); ++i)
    {
        if (m_aLoopSe[i].m_nSeCode == nSE)
            m_aLoopSe[i].m_nRefCount++;
    };
};


/*static*/ bool CGameSound::subtractReferenceCounter(int32 nSE)
{
    for (int32 i = 0; i < COUNT_OF(m_aLoopSe); ++i)
    {
		if (m_aLoopSe[i].m_nSeCode == nSE)
		{
			if (m_aLoopSe[i].m_nRefCount)
				return (--m_aLoopSe[i].m_nRefCount == 0);
		};
    };

	return false;
};


/*static*/ void CGameSound::clearReferenceCounter(int32 nSE)
{
	;
};


/*static*/ void CGameSound::clearReferenceAll(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aLoopSe); ++i)
        m_aLoopSe[i].m_nRefCount = 0;
};


/*static*/ int32 CGameSound::convertToVolume(int32 nConfig)
{
    return (VOLUME_MAX - nConfig);
};


/*static*/ int32 CGameSound::convertToRawConfig(int32 nVolume)
{
    return (VOLUME_MAX - Clamp(nVolume, 0, VOLUME_MAX));
};

