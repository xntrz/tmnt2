#include "NexusMenuSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameMain/NexusInfo.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/2d/MenuSound.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/TextData.hpp"


static const char* s_apszTournamentFlagTexName[] =
{
    "nexusmenu_105",
    "nexusmenu_088",
    "nexusmenu_095",
    "nexusmenu_100",
};


static const char* s_apszKittyTexInfo[] =
{
    "nexusmenu_186",
    "bt_mak_chara_101",
    "nexusmenu_188",
    "bt_mak_chara_102",
    "nexusmenu_190",
    "bt_mak_chara_103",
    "nexusmenu_192",
    "bt_mak_chara_104",
    "nexusmenu_194",
    "bt_mak_chara_105",
    "nexusmenu_148",
    "bt_mak_chara_106",
    "nexusmenu_150",
    "bt_mak_chara_107",
    "nexusmenu_152",
    "bt_mak_chara_108",
    "nexusmenu_154",
    "bt_mak_chara_109",
    "nexusmenu_156",
    "bt_mak_chara_110",
};


static const char* s_apszMonsterTexInfo[] =
{
    "nexusmenu_277",
    "bt_mak_chara_201",
    "nexusmenu_279",
    "bt_mak_chara_202",
    "nexusmenu_281",
    "bt_mak_chara_203",
    "nexusmenu_283",
    "bt_mak_chara_204",
    "nexusmenu_285",
    "bt_mak_chara_205",
    "nexusmenu_240",
    "bt_mak_chara_206",
    "nexusmenu_242",
    "bt_mak_chara_207",
    "nexusmenu_244",
    "bt_mak_chara_208",
    "nexusmenu_246",
    "bt_mak_chara_209",
    "nexusmenu_248",
    "bt_mak_chara_210",
};


static const char* s_apszFootTexInfo[] =
{
    "nexusmenu_357",
    "bt_mak_chara_301",
    "nexusmenu_359",
    "bt_mak_chara_302",
    "nexusmenu_361",
    "bt_mak_chara_303",
    "nexusmenu_363",
    "bt_mak_chara_304",
    "nexusmenu_365",
    "bt_mak_chara_305",
    "nexusmenu_320",
    "bt_mak_chara_306",
    "nexusmenu_322",
    "bt_mak_chara_307",
    "nexusmenu_324",
    "bt_mak_chara_308",
    "nexusmenu_326",
    "bt_mak_chara_309",
    "nexusmenu_328",
    "bt_mak_chara_310",
};


static const char* s_apszBattleNexusTexInfo[] =
{
    "nexusmenu_435",
    "bt_mak_chara_401",
    "nexusmenu_437",
    "bt_mak_chara_402",
    "nexusmenu_439",
    "bt_mak_chara_403",
    "nexusmenu_441",
    "bt_mak_chara_404",
    "nexusmenu_443",
    "bt_mak_chara_405",
    "nexusmenu_400",
    "bt_mak_chara_406",
    "nexusmenu_402",
    "bt_mak_chara_407",
    "nexusmenu_404",
    "bt_mak_chara_408",
    "nexusmenu_406",
    "bt_mak_chara_409",
    "nexusmenu_408",
    "bt_mak_chara_410",
};


/*static*/ CProcess* CNexusMenuSequence::Instance(void)
{
    return new CNexusMenuSequence;
};


CNexusMenuSequence::CNexusMenuSequence(void)
: CAnim2DSequence("nexusmenu")
, m_nexusmenu(NEXUSMENU_NONE)
{
    ;
};


CNexusMenuSequence::~CNexusMenuSequence(void)
{
    ;
};


bool CNexusMenuSequence::OnAttach(const void* pParam)
{
    m_nexusmenu = NEXUSMENU_NONE;
    return CAnim2DSequence::OnAttach(FILEID::ID_NEXUSMENU);
};


void CNexusMenuSequence::OnDetach(void)
{
    CGameSound::Stop();
    CAnim2DSequence::OnDetach();
};


void CNexusMenuSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_animstep)
    {
    case ANIMSTEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
                CGameSound::PlayBGM(SDCODE_BGM(12321));
        }
        break;

    case ANIMSTEP_DRAW:
        {
            if (menuMessageProc())
                BeginFadeout();
        }
        break;

    case ANIMSTEP_END:
        {
            CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);

            if (m_nexusmenu == NEXUSMENU_EXIT)
            {
                Ret();
            }
            else
            {
                AREAID::VALUE idArea = AREAID::ID_NONE;

                switch (m_nexusmenu)
                {
                case NEXUSMENU_KITTY_OPEN:
                    idArea = AREAID::ID_AREA60_A;
                    break;

                case NEXUSMENU_MONSTER_OPEN:
                    idArea = AREAID::ID_AREA60_B;
                    break;

                case NEXUSMENU_FOOT_COMBAT:
                    idArea = AREAID::ID_AREA60_C;
                    break;

                case NEXUSMENU_BATTLE_NEXUS:
                    idArea = AREAID::ID_AREA60_D;
                    break;

                default:
                    ASSERT(false);
                    break;
                };

                GAMETYPES::NEXUSID idNexus = CAreaInfo::GetNexusID(idArea);
                ASSERT(idNexus >= 0 && idNexus < GAMETYPES::NEXUSID_NUM);

                CGameData::Record().Nexus().SetTournamentState(idNexus, CNexusRecord::STATE_NORMAL);
                CGameData::PlayParam().SetStartArea(idArea, 0);

                Ret();
            };

        }
        break;
    };

    CAnim2DSequence::OnMove(bRet, pReturnValue);
};


void CNexusMenuSequence::BeginFadein(void)
{
    //
    // Replace all text before it will be converted to unicode
    //

    replaceText();
    replaceTexture();

    CAnim2DSequence::BeginFadein();
};


void CNexusMenuSequence::replaceText(void)
{
    static const int32 TOURNEYPRIZE_NUM = 2;
    
    //
    //  TODO load from game text
    //
    static const char* s_apszTournamentPrizeOpen[] =
    {
        "CRYSTAL",
        "ANTIQUE SEARCH",
        "CRYSTAL",
        "",
        "CRYSTAL",
        "ANTIQUE",
        "CRYSTAL",
        "ANTIQUE",
    };

    static const char* s_apszTournamentPrizeCleared[] =
    {
        "DOPPELGANGER",
        "",
        "DOPPELGANGER",
        "",
        "DOPPELGANGER",
        "",
        "DOPPELGANGER",
        "",
    };

    static const char* s_apszTournamentHeadLabel[] =
    {
        "k_",	// KITTY
        "m_",	// MONSTER
        "f_",	// FOOT
        "b_",	// BATTLE NEXUS
    };

    static_assert(COUNT_OF(s_apszTournamentHeadLabel) == GAMETYPES::NEXUSID_NUM, "update me");
    
	static const char* s_pszHatena = "??????";

    static char s_szTmpRep[128];
    static char s_szTmpOrg[128];

    s_szTmpRep[0] = '\0';
    s_szTmpOrg[0] = '\0';

    int32 nIndex = 0;
    for (int32 i = 0; i < GAMETYPES::NEXUSID_NUM; ++i)
    {
        bool bIsTourneyCleared = false;

        if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID(i)))
            bIsTourneyCleared = true;

        for (int32 j = 0; j < TOURNEYPRIZE_NUM; ++j)
        {
            std::sprintf(s_szTmpRep, "%sprize%d", s_apszTournamentHeadLabel[i], j + 1);

            if(CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID(i)) == CNexusRecord::STATE_CLEAR)
                std::sprintf(s_szTmpOrg, "%s", s_apszTournamentPrizeCleared[(i * TOURNEYPRIZE_NUM) + j]);
            else
                std::sprintf(s_szTmpOrg, "%s", s_apszTournamentPrizeOpen[(i * TOURNEYPRIZE_NUM) + j]);

            if (bIsTourneyCleared)
                m_pAnimation2D->SetText(s_szTmpRep, s_szTmpOrg);
            else
                m_pAnimation2D->SetText(s_szTmpRep, "");
        };

        for (int32 j = 0; j < GAMETYPES::STAGE_MAX; ++j)
        {
            const CGameTime& StageClearTime = CGameData::Record().Nexus().GetStageClearTime(GAMETYPES::NEXUSID(i), j);
            std::sprintf(s_szTmpRep, "%stime_%d", s_apszTournamentHeadLabel[i], j);
            std::sprintf(s_szTmpOrg, "%02d:%02d:%02d", StageClearTime.GetHour(), StageClearTime.GetMinute(), StageClearTime.GetSecond());
            m_pAnimation2D->SetText(s_szTmpRep, s_szTmpOrg);

            int32 nStageClearCount = CGameData::Record().Nexus().GetStageClearCount(GAMETYPES::NEXUSID(i), j);
            std::sprintf(s_szTmpRep, "%swin_%d", s_apszTournamentHeadLabel[i], j);
            std::sprintf(s_szTmpOrg, "%dwins", nStageClearCount);
            m_pAnimation2D->SetText(s_szTmpRep, s_szTmpOrg);

            int32 nStagePlayCount = CGameData::Record().Nexus().GetStagePlayCount(GAMETYPES::NEXUSID(i), j);
            std::sprintf(s_szTmpRep, "/%sbat_%d", s_apszTournamentHeadLabel[i], j);
            std::sprintf(s_szTmpOrg, "/%d", nStagePlayCount);
            m_pAnimation2D->SetText(s_szTmpRep, s_szTmpOrg);

            int32 nStageClearPercent = CGameData::Record().Nexus().GetStageClearPercent(GAMETYPES::NEXUSID(i), j);
            std::sprintf(s_szTmpRep, "%sper_%d", s_apszTournamentHeadLabel[i], j);
            std::sprintf(s_szTmpOrg, "%d%%", nStageClearPercent);
            m_pAnimation2D->SetText(s_szTmpRep, s_szTmpOrg);

            int32 nPlayedStage = CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID(i));
            std::sprintf(s_szTmpRep, "%steam_%d", s_apszTournamentHeadLabel[i], j);
            if (j >= nPlayedStage)
            {
                m_pAnimation2D->SetText(s_szTmpRep, s_pszHatena);
            }
            else
            {
                char szBuff[256];
                szBuff[0] = '\0';

                const wchar* pwszText = CNexusInfo::GetTeamName(GAMETYPES::NEXUSID(i), j);
                CTextData::ToMultibyte(szBuff, sizeof(szBuff), pwszText);
                    
                m_pAnimation2D->SetText(s_szTmpRep, szBuff);
            };
        };

        const CGameTime& TourneyClearTime = CGameData::Record().Nexus().GetTournamentClearTime(GAMETYPES::NEXUSID(i));
        std::sprintf(s_szTmpRep, "%stotaltime", s_apszTournamentHeadLabel[i]);
        std::sprintf(s_szTmpOrg, "Total Time %02d:%02d:%02d", TourneyClearTime.GetHour(), TourneyClearTime.GetMinute(), TourneyClearTime.GetSecond());
        m_pAnimation2D->SetText(s_szTmpRep, s_szTmpOrg);        
    };
};


void CNexusMenuSequence::replaceTexture(void)
{
    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN) == CNexusRecord::STATE_NONE)
        m_pAnimation2D->SetTexture(s_apszTournamentFlagTexName[GAMETYPES::NEXUSID_MONSTER_OPEN], "bt_flag_monster_fu");

    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT) == CNexusRecord::STATE_NONE)
        m_pAnimation2D->SetTexture(s_apszTournamentFlagTexName[GAMETYPES::NEXUSID_FOOT_COMBAT], "bt_flag_foot_fu");

    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS) == CNexusRecord::STATE_NONE)
        m_pAnimation2D->SetTexture(s_apszTournamentFlagTexName[GAMETYPES::NEXUSID_BATTLE_NEXUS], "bt_flag_battle_fu");

    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_KITTY_OPEN); ++i, j += 2)
        m_pAnimation2D->SetTexture(s_apszKittyTexInfo[j], s_apszKittyTexInfo[j + 1]);

    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_MONSTER_OPEN); ++i, j += 2)
        m_pAnimation2D->SetTexture(s_apszMonsterTexInfo[j], s_apszMonsterTexInfo[j + 1]);

    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_FOOT_COMBAT); ++i, j += 2)
        m_pAnimation2D->SetTexture(s_apszFootTexInfo[j], s_apszFootTexInfo[j + 1]);
    
    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_BATTLE_NEXUS); ++i, j += 2)
        m_pAnimation2D->SetTexture(s_apszBattleNexusTexInfo[j], s_apszBattleNexusTexInfo[j + 1]);
};


bool CNexusMenuSequence::menuMessageProc(void)
{
    bool bResult = false;
    
    m_nexusmenu = NEXUSMENU_NONE;

    if (m_pAnimation2D->CheckMessageGetURL("Exit"))
    {
        m_nexusmenu = NEXUSMENU_EXIT;
        bResult = true;
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("OkKitty") ||
             m_pAnimation2D->CheckMessageGetURL("MenuKitty"))
    {
        pressFlashKeyByNexusState(GAMETYPES::NEXUSID_KITTY_OPEN);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("OkMons") ||
             m_pAnimation2D->CheckMessageGetURL("MenuMons"))
    {
        pressFlashKeyByNexusState(GAMETYPES::NEXUSID_MONSTER_OPEN);
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("OkFoot") ||
             m_pAnimation2D->CheckMessageGetURL("MenuFoot"))
    {
        pressFlashKeyByNexusState(GAMETYPES::NEXUSID_FOOT_COMBAT);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("OkNexus") ||
             m_pAnimation2D->CheckMessageGetURL("MenuNexus"))
    {
        pressFlashKeyByNexusState(GAMETYPES::NEXUSID_BATTLE_NEXUS);
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("Kitty"))
    {
        m_nexusmenu = NEXUSMENU_KITTY_OPEN;
        bResult = true;
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("Mons"))
    {
        m_nexusmenu = NEXUSMENU_MONSTER_OPEN;
        bResult = true;
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("Foot"))
    {
        m_nexusmenu = NEXUSMENU_FOOT_COMBAT;
        bResult = true;
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("Nexus"))
    {
        m_nexusmenu = NEXUSMENU_BATTLE_NEXUS;
        bResult = true;
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("AnimeStart"))
    {
        CMenuController::KeyLock(CController::DIGITAL_OK);
        CMenuController::KeyLock(CController::DIGITAL_CANCEL);
    }    
    else if (m_pAnimation2D->CheckMessageGetURL("AnimeEnd"))
    {
        CMenuController::KeyUnlock(CController::DIGITAL_OK);
        CMenuController::KeyUnlock(CController::DIGITAL_CANCEL);
    };

    return bResult;
};


void CNexusMenuSequence::pressFlashKeyByNexusState(GAMETYPES::NEXUSID idNexus)
{
    CMenuController::KeyLock(CController::DIGITAL_LUP);
    CMenuController::KeyLock(CController::DIGITAL_LDOWN);

    if (CGameData::Record().Nexus().GetTournamentState(idNexus) == CNexusRecord::STATE_NONE)
        m_pAnimation2D->FlashKeyPress(CController::DIGITAL_LDOWN);
    else
        m_pAnimation2D->FlashKeyPress(CController::DIGITAL_LUP);

    CMenuController::KeyUnlock(CController::DIGITAL_LUP);
    CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
};