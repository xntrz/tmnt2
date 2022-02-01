#include "SecretInfo.hpp"
#include "AreaInfo.hpp"
#include "GameTypes.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameData/GameTime.hpp"
#include "System/Common/SystemTime.hpp"


struct CSecretInfo::PASSWORDINFO
{
    const char* m_pszPassword;
    SECRETID::VALUE m_PasswordID;
    SECRETID::VALUE m_SecretID;
};


/*static*/ const CSecretInfo::PASSWORDINFO CSecretInfo::m_aPasswordList[] =
{
    { "LSDRRDR", SECRETID::ID_PASSWORD_CHEATCODE_MIGHTYTURTLE,  SECRETID::ID_CHEATCODE_MIGHTYTURTLE,    },
    { "DSRDMRM", SECRETID::ID_PASSWORD_CHEATCODE_HEALTH,        SECRETID::ID_CHEATCODE_HEALTH,          },
    { "MRLMRMR", SECRETID::ID_PASSWORD_CHEATCODE_PIZZAPARADISE, SECRETID::ID_CHEATCODE_PIZZAPARADISE,   },
    { "DRMSRLR", SECRETID::ID_PASSWORD_CHEATCODE_SELFRECOVERY,  SECRETID::ID_CHEATCODE_SELFRECOVERY,    },
    { "RSRLRSM", SECRETID::ID_PASSWORD_CHEATCODE_SHURIKEN,      SECRETID::ID_CHEATCODE_SHURIKEN,        },
    { "SDLSRLL", SECRETID::ID_PASSWORD_CHEATCODE_SUPERATTACK,   SECRETID::ID_CHEATCODE_SUPERATTACK,     },
    { "LDRMRLM", SECRETID::ID_PASSWORD_CHEATCODE_SUPERDEFENCE,  SECRETID::ID_CHEATCODE_SUPERDEFENCE,    },
    { "MRMDRMD", SECRETID::ID_PASSWORD_CHALLENGE_ENDURANCE,     SECRETID::ID_CHALLENGE_ENDURANCE,       },
    { "RLMRDSL", SECRETID::ID_PASSWORD_CHALLENGE_SHURIKEN,      SECRETID::ID_CHALLENGE_SHURIKEN,        },
    { "LRSRDRD", SECRETID::ID_PASSWORD_CHALLENGE_FATALBLOW,     SECRETID::ID_CHALLENGE_FATALBLOW,       },
    { "SDSDRLD", SECRETID::ID_PASSWORD_CHALLENGE_ABYSS,         SECRETID::ID_CHALLENGE_ABYSS,           },
    { "DRSLLSR", SECRETID::ID_PASSWORD_CHALLENGE_POISON,        SECRETID::ID_CHALLENGE_POISON,          },
    { "RDSRMRL", SECRETID::ID_PASSWORD_CHALLENGE_SUPERTOUGH,    SECRETID::ID_CHALLENGE_SUPERTOUGH,      },
    { "SLSDRDL", SECRETID::ID_PASSWORD_CHALLENGE_NIGHTMARE,     SECRETID::ID_CHALLENGE_NIGHTMARE,       },
    { "SSSMRDD", SECRETID::ID_PASSWORD_CHEATCODE_TODDLING,      SECRETID::ID_TODDLING,                  },
    { "MLDSRDM", SECRETID::ID_PASSWORD_CHEATCODE_SQUEAKING,     SECRETID::ID_SQUEAKING,                 },
    { "SMRDLML", SECRETID::ID_PASSWORD_GAMEBOY_SECRET_1,        SECRETID::ID_NONE,                      },
    { "DDRSMSR", SECRETID::ID_PASSWORD_GAMEBOY_SECRET_2,        SECRETID::ID_NONE,                      },
    { "MDSMSDM", SECRETID::ID_PASSWORD_GAMEBOY_SECRET_3,        SECRETID::ID_NONE,                      },
    { "RDLDSMD", SECRETID::ID_PASSWORD_GAMEBOY_SECRET_4,        SECRETID::ID_NONE,                      },
    { "LMSLSRS", SECRETID::ID_PASSWORD_GAMEBOY_SECRET_5,        SECRETID::ID_NONE,                      },
    { "LMRMDRD", SECRETID::ID_HOME_NEXUS_LEO,                   SECRETID::ID_HOME_NEXUS_LEO,            },
    { "RMSRMDR", SECRETID::ID_HOME_NEXUS_RAP,                   SECRETID::ID_HOME_NEXUS_RAP,            },
    { "MLMRDRM", SECRETID::ID_HOME_NEXUS_MIC,                   SECRETID::ID_HOME_NEXUS_MIC,            },
    { "DSLRDRM", SECRETID::ID_HOME_NEXUS_DON,                   SECRETID::ID_HOME_NEXUS_DON,            },
    { "SRMLDDR", SECRETID::ID_DATABASE_CG,                      SECRETID::ID_DATABASE_CG,               },
};


/*static*/ void CSecretInfo::CheckStartArea(AREAID::VALUE idArea)
{
    if (!CGameData::Attribute().IsPlayDemoMode())
        CheckTimeRelease();
};


/*static*/ void CSecretInfo::CheckEndArea(AREAID::VALUE idArea)
{
    if (!CGameData::Attribute().IsPlayDemoMode())
    {
        CheckNormalResult(idArea);
        CheckAreaClearTerms(idArea);
        CheckCrystal();
        CheckDatabase();
        CheckTimeRelease();
    };
};


/*static*/ void CSecretInfo::CheckStartStage(STAGEID::VALUE idStage)
{
    if (!CGameData::Attribute().IsPlayDemoMode())
    {
        CheckStagePlay(idStage);
        CheckTimeRelease();
    };
};


/*static*/ void CSecretInfo::CheckEndStage(STAGEID::VALUE idStage)
{
    if (!CGameData::Attribute().IsPlayDemoMode())
    {
        CheckStageClear(idStage);
        CheckDatabase();
        CheckTimeRelease();
    };
};


/*static*/ void CSecretInfo::CheckEnding(void)
{
    if (!CGameData::Attribute().IsPlayDemoMode())
    {
        CheckEndingResult();
        CheckTimeRelease();
    };
};


/*static*/ void CSecretInfo::CheckAntiqueSequence(void)
{
    if (!CGameData::Attribute().IsPlayDemoMode())
    {
        if (!CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_DATABASE_APRIL))
            CGameData::Record().Secret().UnlockSecret(SECRETID::ID_DATABASE_APRIL);

        CheckDatabase();
        CheckAntique();
        CheckAntiqueRoomOut();
    };
};


/*static*/ void CSecretInfo::CheckGetBonusMaterials(AREAID::VALUE idArea)
{
    if (!CGameData::Attribute().IsPlayDemoMode())
        CheckGift(idArea);
};


/*static*/ void CSecretInfo::CheckStagePlay(STAGEID::VALUE idStage)
{
    switch (idStage)
    {
    case STAGEID::ID_ST60X_D07:
        Unlock(SECRETID::ID_DATABASE_USAGI);
        break;

    case STAGEID::ID_ST60X_D09:
        Unlock(SECRETID::ID_DATABASE_SPLINTER);
        break;

    case STAGEID::ID_ST60X_D10:
        Unlock(SECRETID::ID_DATABASE_DRAKO);
        break;
    };
};


/*static*/ void CSecretInfo::CheckStageClear(STAGEID::VALUE idStage)
{
    switch (idStage)
    {
    case STAGEID::ID_ST02NB:
        Unlock(SECRETID::ID_DATABASE_HUN);
        break;
        
    case STAGEID::ID_ST18FB:
        Unlock(SECRETID::ID_DATABASE_RATS);
        break;
        
    case STAGEID::ID_ST20FB:
        Unlock(SECRETID::ID_DATABASE_SLASHUUR);
        break;
        
    case STAGEID::ID_ST24N:
        Unlock(SECRETID::ID_DATABASE_DHOONNIB);
        break;
        
    case STAGEID::ID_ST30NB:
        Unlock(SECRETID::ID_DATABASE_TRAXIMUS);
        break;
        
    case STAGEID::ID_ST31NB:
        Unlock(SECRETID::ID_DATABASE_SPASMOSAUR);
        break;
        
    case STAGEID::ID_ST32R:
        if(CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_KITTY_OPEN) == CNexusRecord::STATE_CLEAR)
            Unlock(SECRETID::ID_HOME_TOURNAMENT_MONSTER);
        break;
        
    case STAGEID::ID_ST38NB:
        Unlock(SECRETID::ID_DATABASE_JAPAN);
        break;
        
    case STAGEID::ID_ST40OB:
        Unlock(SECRETID::ID_DATABASE_VEHICLES);
        break;
        
    case STAGEID::ID_ST42NB:
        Unlock(SECRETID::ID_DATABASE_LEATHERHEAD);
        break;
        
    case STAGEID::ID_ST44NB:
        Unlock(SECRETID::ID_DATABASE_ULTIMATE);
        break;
        
    case STAGEID::ID_ST45N:
        Unlock(SECRETID::ID_DATABASE_OTHERS);
        break;
        
    case STAGEID::ID_ST47OB:
        Unlock(SECRETID::ID_DATABASE_SHREDDER);
        break;
        
    case STAGEID::ID_ST50NB:
        Unlock(SECRETID::ID_DATABASE_NY);
        break;
        
    case STAGEID::ID_ST54FB:
        Unlock(SECRETID::ID_DATABASE_PLAYMATES);
        break;
        
    case STAGEID::ID_ST55FB:
        Unlock(SECRETID::ID_DATABASE_ELITE);
        break;
        
    case STAGEID::ID_ST57NB:
        Unlock(SECRETID::ID_DATABASE_KARAI);
        Unlock(SECRETID::ID_DATABASE_KURAIYAMA);
        break;
        
    case STAGEID::ID_ST58OB2:
        if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN) == CNexusRecord::STATE_CLEAR)
            Unlock(SECRETID::ID_HOME_TOURNAMENT_FOOT);
        break;
        
    case STAGEID::ID_ST60X_A01:
        Unlock(SECRETID::ID_DATABASE_ANOTHER_DIMENSION);
        break;
        
    case STAGEID::ID_ST60X_A10:
        if(CGameData::Record().Area().GetAreaState(AREAID::ID_AREA32) == CAreaRecord::STATE_CLEAR)
            Unlock(SECRETID::ID_HOME_TOURNAMENT_MONSTER);
        
        Unlock(SECRETID::ID_SEARCH_ANTIQUE);
        Unlock(SECRETID::ID_CHARACTER_CASEY);
        Unlock(SECRETID::ID_DATABASE_CASEY);
        break;
        
    case STAGEID::ID_ST60X_B10:
        if (CGameData::Record().Area().GetAreaState(AREAID::ID_AREA58) == CAreaRecord::STATE_CLEAR)
            Unlock(SECRETID::ID_HOME_TOURNAMENT_FOOT);
        
        Unlock(SECRETID::ID_HOME_NEXUS_LEO);
        break;
        
    case STAGEID::ID_ST60X_C09:
        {
            const CGameTime& cleartime = CGameData::Record().Nexus().GetStageClearTime(GAMETYPES::NEXUSID_FOOT_COMBAT, 8);
            if(cleartime <= CGameTime(GAMETIME_HMS(0, 2, 30)))
                Unlock(SECRETID::ID_CHARACTER_KARAI);
        }
        break;
        
    case STAGEID::ID_ST60X_C10:
        Unlock(SECRETID::ID_HOME_TOURNAMENT_BATTLENEXUS);
        Unlock(SECRETID::ID_HOME_NEXUS_RAP);
        break;
        
    case STAGEID::ID_ST60X_D10:
        Unlock(SECRETID::ID_CHARACTER_SLASHUUR);
        break;
    };
};


/*static*/ void CSecretInfo::CheckAntique(void)
{
    for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
    {
        if (CGameData::Record().Antique().IsAntiqueIdentified(ANTIQUEID::VALUE(i)))
            AnalyzeAntique(ANTIQUEID::VALUE(i));
    };
};


/*static*/ void CSecretInfo::CheckAntiqueRoomOut(void)
{
    if (IsCompleteAntique())
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_MIGHTYTURTLE);
    
    if (IsCompleteAntiqueAtNewYork())
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_HEALTH);

    if (IsCompleteAntiqueAtJapan())
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_PIZZAPARADISE);

    if (IsCompleteAntiqueAtKuraiyama())
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_SELFRECOVERY);
};


/*static*/ void CSecretInfo::CheckNormalResult(AREAID::VALUE idArea)
{
    GAMETYPES::CLEARRANK clearrank = CGameData::Record().Area().GetAreaClearRank(idArea);
    
    switch (idArea)
    {
    case AREAID::ID_AREA38:
        if(clearrank >= GAMETYPES::CLEARRANK_S)
            Unlock(SECRETID::ID_HOME_NEXUS_MIC);
        break;
        
    case AREAID::ID_AREA43:
        if (clearrank >= GAMETYPES::CLEARRANK_S)
            Unlock(SECRETID::ID_PASSWORD_CHALLENGE_ENDURANCE);
        break;
        
    case AREAID::ID_AREA54:
        if (clearrank >= GAMETYPES::CLEARRANK_S)
            Unlock(SECRETID::ID_HOME_NEXUS_DON);
        break;
        
    case AREAID::ID_AREA58:
        Unlock(SECRETID::ID_CHARACTER_SPLINTER);
        break;
    };
};


/*static*/ void CSecretInfo::CheckAreaClearTerms(AREAID::VALUE idArea)
{
    switch (idArea)
    {
    case AREAID::ID_AREA11:
        if (CGameData::Record().Area().IsAreaRootCleared(AREAID::ID_AREA11, CAreaRecord::CLEAR_ROOT_B))
            Unlock(SECRETID::ID_PASSWORD_CHEATCODE_SHURIKEN);        
        break;
    };
};


/*static*/ void CSecretInfo::CheckEndingResult(void)
{
    CGameTime cleartime;
    CGameData::Record().Area().CalcTotalClearTime(cleartime);
    
    GAMETYPES::CLEARRANK clearrankTotal = CGameData::Record().Area().CalcTotalClearRank();
    GAMETYPES::CLEARRANK clearrankTotalTime = CGameData::Record().Area().CalcTotalClearTimeRank();

    switch (clearrankTotal)
    {
    case GAMETYPES::CLEARRANK_A:
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_SHURIKEN);
        break;

    case GAMETYPES::CLEARRANK_S:
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_SHURIKEN);
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_FATALBLOW);
        break;

    case GAMETYPES::CLEARRANK_SS:
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_SHURIKEN);
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_FATALBLOW);
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_SUPERATTACK);
        break;
    };
    
    switch (clearrankTotalTime)
    {
    case GAMETYPES::CLEARRANK_A:
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_POISON);
        break;
        
    case GAMETYPES::CLEARRANK_S:
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_POISON);
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_ABYSS);
        break;
        
    case GAMETYPES::CLEARRANK_SS:
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_POISON);
        Unlock(SECRETID::ID_PASSWORD_CHALLENGE_ABYSS);
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_SUPERDEFENCE);
        break;
    };
};


/*static*/ void CSecretInfo::CheckTimeRelease(void)
{
    CTimeObj TimeInit = CGameData::Record().GetInitDate();
    CTimeObj TimeNow = CSystemTime::Instance().GetLocalTime();
    CTimeSpanObj TimeSpan = CTimeObj::CalcSpan(TimeNow, TimeInit);

    if (TimeSpan.GetDays() >= 5)
        Unlock(SECRETID::ID_CHARACTER_CASEY);

    if (TimeSpan.GetDays() >= 10)
        Unlock(SECRETID::ID_CHARACTER_SPLINTER);

    if (TimeSpan.GetDays() >= 14)
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_PIZZAPARADISE);

    if (TimeSpan.GetDays() >= 21)
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_HEALTH);

    if (TimeSpan.GetDays() >= 28)
        Unlock(SECRETID::ID_PASSWORD_CHEATCODE_SELFRECOVERY);

    if (TimeSpan.GetDays() >= 35)
        Unlock(SECRETID::ID_CHARACTER_KARAI);

    if (TimeSpan.GetDays() >= 45)
        Unlock(SECRETID::ID_CHARACTER_SLASHUUR);
};


/*static*/ void CSecretInfo::CheckGift(AREAID::VALUE idArea)
{
    switch (idArea)
    {
    case AREAID::ID_AREA06:
        Unlock(SECRETID::ID_DATABASE_PROMOTION_ARTS);
        break;

    case AREAID::ID_AREA13:
        Unlock(SECRETID::ID_DATABASE_ANIMS);
        break;

    case AREAID::ID_AREA22:
        Unlock(SECRETID::ID_DATABASE_CG);
        break;

    case AREAID::ID_AREA32:
        Unlock(SECRETID::ID_DATABASE_SECONDARY_A);
        break;

    case AREAID::ID_AREA46:
        Unlock(SECRETID::ID_DATABASE_SECONDARY_B);
        break;
    };
};


/*static*/ void CSecretInfo::CheckCrystal(void)
{
    //
    //  Leo
    //
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_RED) >= 10)
        Unlock(SECRETID::ID_DATABASE_LEO_A);
    
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_RED) >= 20)
        Unlock(SECRETID::ID_DATABASE_LEO_B);

    //
    //  Raph
    //
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_GREEN) >= 10)
        Unlock(SECRETID::ID_DATABASE_RAP_A);
    
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_GREEN) >= 20)
        Unlock(SECRETID::ID_DATABASE_RAP_B);

    //
    //  Don
    //
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_ORANGE) >= 10)
        Unlock(SECRETID::ID_DATABASE_DON_A);
    
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_ORANGE) >= 20)
        Unlock(SECRETID::ID_DATABASE_DON_B);

    //
    //  Mic
    //
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_WHITE) >= 10)
        Unlock(SECRETID::ID_DATABASE_MIC_A);
    
    if (CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_WHITE) >= 20)
        Unlock(SECRETID::ID_DATABASE_MIC_B);
};


/*static*/ void CSecretInfo::CheckDatabase(void)
{
    for (int32 i = DBITEMID::ID_FIRST; i < DBITEMID::ID_MAX; ++i)
    {
        if (CGameData::Record().Database().IsItemUnlocked(DBITEMID::VALUE(i)))
        {
            Unlock(SECRETID::ID_HOME_BONUSMATERIAL);
            return;
        };
    };
};


/*static*/ int32 CSecretInfo::GetGameText(SECRETID::VALUE idSecret)
{
    ASSERT(idSecret >= SECRETID::ID_FIRST && idSecret < SECRETID::ID_MAX);

    return idSecret + 564;
};


/*static*/ DBITEMID::VALUE CSecretInfo::GetDatabase(SECRETID::VALUE idSecret)
{
    switch (idSecret)
    {
    case SECRETID::ID_DATABASE_PLAYMATES:
        return DBITEMID::ID_ETC_PLAYMATES_A;

    case SECRETID::ID_DATABASE_PROMOTION_ARTS:
        return DBITEMID::ID_ETC_GALLERY;
    };

    DBITEMID::VALUE idDbitem = DBITEMID::VALUE(idSecret - (SECRETID::DATABASESTART - 1));
    ASSERT(idDbitem >= DBITEMID::ID_FIRST && idDbitem < DBITEMID::ID_MAX);
    return idDbitem;
};


/*static*/ SECRETID::VALUE CSecretInfo::CheckPassword(const char* pszPassword)
{
    ASSERT(pszPassword);
    
    for (int32 i = 0; i < COUNT_OF(m_aPasswordList); ++i)
    {
        if (!std::strcmp(m_aPasswordList[i].m_pszPassword, pszPassword))
        {
            Unlock(m_aPasswordList[i].m_SecretID);
            CGameData::Record().Secret().NotifySecret(m_aPasswordList[i].m_SecretID);

            return m_aPasswordList[i].m_SecretID;
        };
    };

    return SECRETID::ID_NONE;
};


/*static*/ const char* CSecretInfo::GetPassword(SECRETID::VALUE idSecret)
{
    for (int32 i = 0; i < COUNT_OF(m_aPasswordList); ++i)
    {
        if (m_aPasswordList[i].m_PasswordID == idSecret)
            return m_aPasswordList[i].m_pszPassword;
    };

    return nullptr;
};


/*static*/ void CSecretInfo::Unlock(SECRETID::VALUE idSecret)
{
    if (!CGameData::Record().Secret().IsUnlockedSecret(idSecret))
        CGameData::Record().Secret().UnlockSecret(idSecret);
};


/*static*/ void CSecretInfo::AnalyzeAntique(ANTIQUEID::VALUE idAntique)
{
    struct AREASECRET
    {
        AREAID::VALUE m_AreaID;
        SECRETID::VALUE m_SecretID;
    };

    static const AREASECRET s_aAreaSecret[] =
    {
        { AREAID::ID_AREA34, SECRETID::ID_HOME_SAMURAI_LEO,             },
        { AREAID::ID_AREA33, SECRETID::ID_HOME_SAMURAI_RAP,             },
        { AREAID::ID_AREA38, SECRETID::ID_HOME_SAMURAI_MIC,             },
        { AREAID::ID_AREA35, SECRETID::ID_HOME_SAMURAI_DON,             },
        { AREAID::ID_AREA46, SECRETID::ID_PASSWORD_CHALLENGE_SUPERTOUGH,},
        { AREAID::ID_AREA58, SECRETID::ID_PASSWORD_CHALLENGE_NIGHTMARE, },
        { AREAID::ID_AREA30, SECRETID::ID_PASSWORD_CHEATCODE_TODDLING,  },
        { AREAID::ID_AREA02, SECRETID::ID_PASSWORD_CHEATCODE_SQUEAKING, },
    };

    for (int32 i = 0; i < COUNT_OF(s_aAreaSecret); ++i)
    {
        if (idAntique == CAreaInfo::GetAntiqueID(s_aAreaSecret[i].m_AreaID))
            Unlock(s_aAreaSecret[i].m_SecretID);
    };
};


/*static*/ bool CSecretInfo::IsCompleteAntique(void)
{
    for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
    {
        if (!CGameData::Record().Antique().IsAntiqueIdentified(ANTIQUEID::VALUE(i)))
            return false;
    };

    return true;
};


/*static*/ bool CSecretInfo::IsCompleteAntiqueAtNewYork(void)
{
    return isCompleteAntiqueAtWorld(WORLDID::ID_MNY);
};


/*static*/ bool CSecretInfo::IsCompleteAntiqueAtJapan(void)
{
    return isCompleteAntiqueAtWorld(WORLDID::ID_JPN);
};


/*static*/ bool CSecretInfo::IsCompleteAntiqueAtKuraiyama(void)
{
    return isCompleteAntiqueAtWorld(WORLDID::ID_KUR);
};


/*static*/ bool CSecretInfo::isCompleteAntiqueAtWorld(WORLDID::VALUE idWorld)
{
    for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
    {
        ANTIQUEID::VALUE idAntique = ANTIQUEID::VALUE(i);
        AREAID::VALUE idArea = CAreaInfo::FindAntiqueArea(idAntique);

        if (CAreaInfo::GetWorldNo(idArea) == idWorld)
        {
            if (!CGameData::Record().Antique().IsAntiqueIdentified(idAntique))
                return false;
        };
    };

    return true;
};