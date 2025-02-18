#include "NexusMenuSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameMain/NexusInfo.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/2d/MenuSound.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Text/GameText.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/System2D.hpp"
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
#ifdef TMNT2_BUILD_EU
    "nexusmenu_185",
    "bt_mak_chara_101",
    "bt_mak_chara_101_k",
    "nexusmenu_187",
    "bt_mak_chara_102",
    "bt_mak_chara_102_k",
    "nexusmenu_189",
    "bt_mak_chara_103",
    "bt_mak_chara_103_k",
    "nexusmenu_191",
    "bt_mak_chara_104",
    "bt_mak_chara_104_k",
    "nexusmenu_193",
    "bt_mak_chara_105",
    "bt_mak_chara_105_k",
    "nexusmenu_147",
    "bt_mak_chara_106",
    "bt_mak_chara_106_k",
    "nexusmenu_149",
    "bt_mak_chara_107",
    "bt_mak_chara_107_k",
    "nexusmenu_151",
    "bt_mak_chara_108",
    "bt_mak_chara_108_k",
    "nexusmenu_153",
    "bt_mak_chara_109",
    "bt_mak_chara_109_k",
    "nexusmenu_155",
    "bt_mak_chara_110",
    "bt_mak_chara_110_k",
#else /* TMNT2_BUILD_EU */
    "nexusmenu_186",
    "bt_mak_chara_101",
    "bt_mak_chara_101_k",
    "nexusmenu_188",
    "bt_mak_chara_102",
    "bt_mak_chara_102_k",
    "nexusmenu_190",
    "bt_mak_chara_103",
    "bt_mak_chara_103_k",
    "nexusmenu_192",
    "bt_mak_chara_104",
    "bt_mak_chara_104_k",
    "nexusmenu_194",
    "bt_mak_chara_105",
    "bt_mak_chara_105_k",
    "nexusmenu_148",
    "bt_mak_chara_106",
    "bt_mak_chara_106_k",
    "nexusmenu_150",
    "bt_mak_chara_107",
    "bt_mak_chara_107_k",
    "nexusmenu_152",
    "bt_mak_chara_108",
    "bt_mak_chara_108_k",
    "nexusmenu_154",
    "bt_mak_chara_109",
    "bt_mak_chara_109_k",
    "nexusmenu_156",
    "bt_mak_chara_110",
    "bt_mak_chara_110_k",
#endif /* TMNT2_BUILD_EU */
};


static const char* s_apszMonsterTexInfo[] =
{
#ifdef TMNT2_BUILD_EU
    "nexusmenu_275",
    "bt_mak_chara_201",
    "bt_mak_chara_201_k",
    "nexusmenu_277",
    "bt_mak_chara_202",
    "bt_mak_chara_202_k",
    "nexusmenu_279",
    "bt_mak_chara_203",
    "bt_mak_chara_203_k",
    "nexusmenu_281",
    "bt_mak_chara_204",
    "bt_mak_chara_204_k",
    "nexusmenu_283",
    "bt_mak_chara_205",
    "bt_mak_chara_205_k",
    "nexusmenu_238",
    "bt_mak_chara_206",
    "bt_mak_chara_206_k",
    "nexusmenu_240",
    "bt_mak_chara_207",
    "bt_mak_chara_207_k",
    "nexusmenu_242",
    "bt_mak_chara_208",
    "bt_mak_chara_208_k",
    "nexusmenu_244",
    "bt_mak_chara_209",
    "bt_mak_chara_209_k",
    "nexusmenu_246",
    "bt_mak_chara_210",
    "bt_mak_chara_210_k",
#else /* TMNT2_BUILD_EU */
    "nexusmenu_277",
    "bt_mak_chara_201",
    "bt_mak_chara_201_k",
    "nexusmenu_279",
    "bt_mak_chara_202",
    "bt_mak_chara_202_k",
    "nexusmenu_281",
    "bt_mak_chara_203",
    "bt_mak_chara_203_k",
    "nexusmenu_283",
    "bt_mak_chara_204",
    "bt_mak_chara_204_k",
    "nexusmenu_285",
    "bt_mak_chara_205",
    "bt_mak_chara_205_k",
    "nexusmenu_240",
    "bt_mak_chara_206",
    "bt_mak_chara_206_k",
    "nexusmenu_242",
    "bt_mak_chara_207",
    "bt_mak_chara_207_k",
    "nexusmenu_244",
    "bt_mak_chara_208",
    "bt_mak_chara_208_k",
    "nexusmenu_246",
    "bt_mak_chara_209",
    "bt_mak_chara_209_k",
    "nexusmenu_248",
    "bt_mak_chara_210",
    "bt_mak_chara_210_k",
#endif /* TMNT2_BUILD_EU */
};


static const char* s_apszFootTexInfo[] =
{
#ifdef TMNT2_BUILD_EU
    "nexusmenu_355",
    "bt_mak_chara_301",
    "bt_mak_chara_301_k",
    "nexusmenu_357",
    "bt_mak_chara_302",
    "bt_mak_chara_302_k",
    "nexusmenu_359",
    "bt_mak_chara_303",
    "bt_mak_chara_303_k",
    "nexusmenu_361",
    "bt_mak_chara_304",
    "bt_mak_chara_304_k",
    "nexusmenu_363",
    "bt_mak_chara_305",
    "bt_mak_chara_305_k",
    "nexusmenu_318",
    "bt_mak_chara_306",
    "bt_mak_chara_306_k",
    "nexusmenu_320",
    "bt_mak_chara_307",
    "bt_mak_chara_307_k",
    "nexusmenu_322",
    "bt_mak_chara_308",
    "bt_mak_chara_308_k",
    "nexusmenu_324",
    "bt_mak_chara_309",
    "bt_mak_chara_309_k",
    "nexusmenu_326",
    "bt_mak_chara_310",
    "bt_mak_chara_310_k",
#else /* TMNT2_BUILD_EU */
    "nexusmenu_357",
    "bt_mak_chara_301",
    "bt_mak_chara_301_k",
    "nexusmenu_359",
    "bt_mak_chara_302",
    "bt_mak_chara_302_k",
    "nexusmenu_361",
    "bt_mak_chara_303",
    "bt_mak_chara_303_k",
    "nexusmenu_363",
    "bt_mak_chara_304",
    "bt_mak_chara_304_k",
    "nexusmenu_365",
    "bt_mak_chara_305",
    "bt_mak_chara_305_k",
    "nexusmenu_320",
    "bt_mak_chara_306",
    "bt_mak_chara_306_k",
    "nexusmenu_322",
    "bt_mak_chara_307",
    "bt_mak_chara_307_k",
    "nexusmenu_324",
    "bt_mak_chara_308",
    "bt_mak_chara_308_k",
    "nexusmenu_326",
    "bt_mak_chara_309",
    "bt_mak_chara_309_k",
    "nexusmenu_328",
    "bt_mak_chara_310",
    "bt_mak_chara_310_k",
#endif /* TMNT2_BUILD_EU */
};


static const char* s_apszBattleNexusTexInfo[] =
{
#ifdef TMNT2_BUILD_EU
    "nexusmenu_433",
    "bt_mak_chara_401",
    "bt_mak_chara_401_k",
    "nexusmenu_435",
    "bt_mak_chara_402",
    "bt_mak_chara_402_k",
    "nexusmenu_437",
    "bt_mak_chara_403",
    "bt_mak_chara_403_k",
    "nexusmenu_439",
    "bt_mak_chara_404",
    "bt_mak_chara_404_k",
    "nexusmenu_441",
    "bt_mak_chara_405",
    "bt_mak_chara_405_k",
    "nexusmenu_398",
    "bt_mak_chara_406",
    "bt_mak_chara_406_k",
    "nexusmenu_400",
    "bt_mak_chara_407",
    "bt_mak_chara_407_k",
    "nexusmenu_402",
    "bt_mak_chara_408",
    "bt_mak_chara_408_k",
    "nexusmenu_404",
    "bt_mak_chara_409",
    "bt_mak_chara_409_k",
    "nexusmenu_406",
    "bt_mak_chara_410",
    "bt_mak_chara_410_k",
#else /* TMNT2_BUILD_EU */
    "nexusmenu_435",
    "bt_mak_chara_401",
    "bt_mak_chara_401_k",
    "nexusmenu_437",
    "bt_mak_chara_402",
    "bt_mak_chara_402_k",
    "nexusmenu_439",
    "bt_mak_chara_403",
    "bt_mak_chara_403_k",
    "nexusmenu_441",
    "bt_mak_chara_404",
    "bt_mak_chara_404_k",
    "nexusmenu_443",
    "bt_mak_chara_405",
    "bt_mak_chara_405_k",
    "nexusmenu_400",
    "bt_mak_chara_406",
    "bt_mak_chara_406_k",
    "nexusmenu_402",
    "bt_mak_chara_407",
    "bt_mak_chara_407_k",
    "nexusmenu_404",
    "bt_mak_chara_408",
    "bt_mak_chara_408_k",
    "nexusmenu_406",
    "bt_mak_chara_409",
    "bt_mak_chara_409_k",
    "nexusmenu_408",
    "bt_mak_chara_410",
    "bt_mak_chara_410_k",
#endif /* TMNT2_BUILD_EU */
};


static_assert((COUNT_OF(s_apszKittyTexInfo) / 3)       == GAMETYPES::STAGE_MAX, "string table incorrect");
static_assert((COUNT_OF(s_apszMonsterTexInfo) / 3)     == GAMETYPES::STAGE_MAX, "string table incorrect");
static_assert((COUNT_OF(s_apszFootTexInfo) / 3)        == GAMETYPES::STAGE_MAX, "string table incorrect");
static_assert((COUNT_OF(s_apszBattleNexusTexInfo) / 3) == GAMETYPES::STAGE_MAX, "string table incorrect");


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

#ifdef TMNT2_BUILD_EU
    CDataLoader::Regist(FILEID::ID_NEXUSMENU);
    return CAnim2DSequence::OnAttach(FILEID::ID_LANG_NEXUSMENU);
#else /* TMNT2_BUILD_EU */
    return CAnim2DSequence::OnAttach(FILEID::ID_NEXUSMENU);
#endif /* TMNT2_BUILD_EU */
};


void CNexusMenuSequence::OnDetach(void)
{
    CGameSound::Stop();
    CAnim2DSequence::OnDetach();
};


void CNexusMenuSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (AnimStep())
    {
    case ANIMSTEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
                CGameSound::PlayBGM(SDCODE_BGM(12321));
        }
        break;

    case ANIMSTEP_DRAW:
        {
            MessageProc();
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
                ASSERT(idNexus >= 0);
                ASSERT(idNexus < GAMETYPES::NEXUSID_NUM);

                CGameData::Record().Nexus().SetTournamentState(idNexus, CNexusRecord::STATE_NORMAL);
                CGameData::PlayParam().SetStartArea(idArea, 0);

                Ret();
            };
        }
        break;

    default:
        break;
    };

    CAnim2DSequence::OnMove(bRet, pReturnValue);
};


void CNexusMenuSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();

#ifdef TMNT2_BUILD_EU
    CSystem2D::PushRenderState();

    CGameFont::SetHeightScaled(2.0f);
    CGameFont::SetRGBA(255, 255, 255, 255);

    const wchar* pwszText = CGameText::GetText(GAMETEXT_EU_HELP_17);

    if (CGameFont::GetStringWidth(pwszText) >= 500.0f)
    {
        Rt2dBBox bbox;
        bbox.x = -272.0f;
        bbox.y = -230.0f;
        bbox.w =  500.0f;
        bbox.h =   60.0f;

        CGameFont::Flow(pwszText, &bbox);
    }
    else
    {
        CGameFont::Show(pwszText, -272.0f, 190.0f);
    };

    CSystem2D::PopRenderState();
#endif /* TMNT2_BUILD_EU */
};


void CNexusMenuSequence::BeginFadeIn(void)
{
    ReplaceText();
    ReplaceTexture();

    CAnim2DSequence::BeginFadeIn();
};


void CNexusMenuSequence::ReplaceText(void)
{
    static const int32 TOURNEYPRIZE_NUM = 2;
    
    const wchar* apwszTournamentPrizeOpen[] =
    {
        CGameText::GetText(GAMETEXT_PRIZE_CRY),
        CGameText::GetText(GAMETEXT_AT_SEARCH),

        CGameText::GetText(GAMETEXT_PRIZE_CRY),
        CGameText::GetText(GAMETEXT_EMPTY),

        CGameText::GetText(GAMETEXT_PRIZE_CRY),
        CGameText::GetText(GAMETEXT_PRIZE_ANTIQ),

        CGameText::GetText(GAMETEXT_PRIZE_CRY),
        CGameText::GetText(GAMETEXT_PRIZE_ANTIQ),
    };

    const wchar* apwszTournamentPrizeCleared[] =
    {
        CGameText::GetText(GAMETEXT_PRIZE_DEKU),
        CGameText::GetText(GAMETEXT_EMPTY),

        CGameText::GetText(GAMETEXT_PRIZE_DEKU),
        CGameText::GetText(GAMETEXT_EMPTY),

        CGameText::GetText(GAMETEXT_PRIZE_DEKU),
        CGameText::GetText(GAMETEXT_EMPTY),

        CGameText::GetText(GAMETEXT_PRIZE_DEKU),
        CGameText::GetText(GAMETEXT_EMPTY),
    };

    const char* apszTournamentHeadLabel[] =
    {
        "k_",	// KITTY
        "m_",	// MONSTER
        "f_",	// FOOT
        "b_",	// BATTLE NEXUS
    };

    static_assert(COUNT_OF(apszTournamentHeadLabel)     == GAMETYPES::NEXUSID_NUM, "table incorrect");
    static_assert(COUNT_OF(apwszTournamentPrizeOpen)    == (GAMETYPES::NEXUSID_NUM * TOURNEYPRIZE_NUM), "table incorrect");
    static_assert(COUNT_OF(apwszTournamentPrizeCleared) == (GAMETYPES::NEXUSID_NUM * TOURNEYPRIZE_NUM), "table incorrect");

    char szOrgStr[128];
    szOrgStr[0] = '\0';

    char szRepStr[128];
    szRepStr[0] = '\0';

    for (int32 i = 0; i < GAMETYPES::NEXUSID_NUM; ++i)
    {
        bool bIsTourneyCleared = false;

        if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID(i)))
            bIsTourneyCleared = true;

        for (int32 j = 0; j < TOURNEYPRIZE_NUM; ++j)
        {
            std::sprintf(szOrgStr, "%sprize%d", apszTournamentHeadLabel[i], j + 1);

            const wchar* pwszRep = apwszTournamentPrizeOpen[(i * TOURNEYPRIZE_NUM) + j];

            if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID(i)) == CNexusRecord::STATE_CLEAR)
                pwszRep = apwszTournamentPrizeCleared[(i * TOURNEYPRIZE_NUM) + j];

            if (bIsTourneyCleared)
                Animation2D().SetText(szOrgStr, pwszRep);
            else
                Animation2D().SetText(szOrgStr, "");
        };

        for (int32 j = 0; j < GAMETYPES::STAGE_MAX; ++j)
        {
            CGameTime clearTime = CGameData::Record().Nexus().GetStageClearTime(GAMETYPES::NEXUSID(i), j);
            std::sprintf(szOrgStr, "%stime_%d", apszTournamentHeadLabel[i], j);
            std::sprintf(szRepStr,
                         "%02d:%02d:%02d",
                         clearTime.GetHour(),
                         clearTime.GetMinute(),
                         clearTime.GetSecond());
            Animation2D().SetText(szOrgStr, szRepStr);

#ifdef TMNT2_BUILD_EU
            int32 numPlay = CGameData::Record().Nexus().GetStagePlayCount(GAMETYPES::NEXUSID(i), j);
            int32 numClear = CGameData::Record().Nexus().GetStageClearCount(GAMETYPES::NEXUSID(i), j);
            int32 clearPercent = CGameData::Record().Nexus().GetStageClearPercent(GAMETYPES::NEXUSID(i), j);

            const wchar* pwszFormat = CGameText::GetText(GAMETEXT_GG_TOURNEY_WIN);

            wchar wszRepStr[256];
            wszRepStr[0] = UTEXT('\0');
            CTextData::Sprintf(wszRepStr, pwszFormat, numClear, numPlay, clearPercent);

            std::sprintf(szOrgStr, "%swin_%d", apszTournamentHeadLabel[i], j);
            Animation2D().SetText(szOrgStr, wszRepStr);

            std::sprintf(szOrgStr, "/%sbat_%d", apszTournamentHeadLabel[i], j);
            Animation2D().SetText(szOrgStr, CGameText::GetText(GAMETEXT_EU_EMPTY));

            std::sprintf(szOrgStr, "%sper_%d", apszTournamentHeadLabel[i], j);
            Animation2D().SetText(szOrgStr, CGameText::GetText(GAMETEXT_EU_EMPTY));
#else /* TMNT2_BUILD_EU */
            int32 numClear = CGameData::Record().Nexus().GetStageClearCount(GAMETYPES::NEXUSID(i), j);
            std::sprintf(szOrgStr, "%swin_%d", apszTournamentHeadLabel[i], j);
            std::sprintf(szRepStr, "%dwins", numClear);
            Animation2D().SetText(szOrgStr, szRepStr);

            int32 numPlay = CGameData::Record().Nexus().GetStagePlayCount(GAMETYPES::NEXUSID(i), j);
            std::sprintf(szOrgStr, "/%sbat_%d", apszTournamentHeadLabel[i], j);
            std::sprintf(szRepStr, "/%d", numPlay);
            Animation2D().SetText(szOrgStr, szRepStr);

            int32 clearPercent = CGameData::Record().Nexus().GetStageClearPercent(GAMETYPES::NEXUSID(i), j);
            std::sprintf(szOrgStr, "%sper_%d", apszTournamentHeadLabel[i], j);
            std::sprintf(szRepStr, "%d%%", clearPercent);
            Animation2D().SetText(szOrgStr, szRepStr);
#endif /* TMNT2_BUILD_EU */

            std::sprintf(szOrgStr, "%steam_%d", apszTournamentHeadLabel[i], j);
            int32 maxPlayedStageNo = CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID(i));
            if (j >= maxPlayedStageNo)
                Animation2D().SetText(szOrgStr, CGameText::GetText(GAMETEXT_DB_HATENA));
            else
                Animation2D().SetText(szOrgStr, CNexusInfo::GetTeamName(GAMETYPES::NEXUSID(i), j));
        };

        CGameTime totalClearTime = CGameData::Record().Nexus().GetTournamentClearTime(GAMETYPES::NEXUSID(i));

        char szClearTimeAsStr[64];
        szClearTimeAsStr[0] = '\0';
        std::sprintf(szClearTimeAsStr,
                     " %02d:%02d:%02d",
                     totalClearTime.GetHour(),
                     totalClearTime.GetMinute(),
                     totalClearTime.GetSecond());

        wchar wszClearTimeAsStr[64];
        wszClearTimeAsStr[0] = UTEXT('\0');
        CGameFont::ConvertToUnicode(wszClearTimeAsStr, szClearTimeAsStr);
    
        wchar wszRepStr[128];
        wszRepStr[0] = UTEXT('\0');
#ifdef TMNT2_BUILD_EU
        CTextData::Sprintf(wszRepStr, CGameText::GetText(GAMETEXT_GG_TIME), wszClearTimeAsStr);
#else /* TMNT2_BUILD_EU */
        CTextData::Sprintf(wszRepStr, UTEXT("%s%s"), CGameText::GetText(GAMETEXT_GG_TIME), wszClearTimeAsStr);
#endif /* TMNT2_BUILD_EU */

        std::sprintf(szOrgStr, "%stotaltime", apszTournamentHeadLabel[i]);

        Animation2D().SetText(szOrgStr, wszRepStr);
    };

#ifdef TMNT2_BUILD_EU
    Animation2D().SetText("NX_T_00", CGameText::GetText(GAMETEXT_GG_TOURNEY_SEL));
    Animation2D().SetText("NX_T_01", CGameText::GetText(GAMETEXT_GG_TOURNEY_JOIN));
    Animation2D().SetText("NX_T_02", CGameText::GetText(GAMETEXT_GG_PRIZE));

    Animation2D().SetText("NX_S_00", CGameText::GetText(GAMETEXT_AREA60A_NAME));
    Animation2D().SetText("NX_S_01", CGameText::GetText(GAMETEXT_AREA60B_NAME));
    Animation2D().SetText("NX_S_02", CGameText::GetText(GAMETEXT_AREA60C_NAME));
    Animation2D().SetText("NX_S_03", CGameText::GetText(GAMETEXT_AREA60D_NAME));

    Animation2D().SetTexture("nexusmenu_107", "nexus_t1");
    Animation2D().SetTexture("nexusmenu_092", "nexus_t2");
    Animation2D().SetTexture("nexusmenu_097", "nexus_t3");
    Animation2D().SetTexture("nexusmenu_102", "nexus_t4");
#endif /* TMNT2_BUILD_EU */
};


void CNexusMenuSequence::ReplaceTexture(void)
{
    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN) == CNexusRecord::STATE_NONE)
        Animation2D().SetTexture(s_apszTournamentFlagTexName[GAMETYPES::NEXUSID_MONSTER_OPEN], "bt_flag_monster_fu");

    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT) == CNexusRecord::STATE_NONE)
        Animation2D().SetTexture(s_apszTournamentFlagTexName[GAMETYPES::NEXUSID_FOOT_COMBAT], "bt_flag_foot_fu");

    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS) == CNexusRecord::STATE_NONE)
        Animation2D().SetTexture(s_apszTournamentFlagTexName[GAMETYPES::NEXUSID_BATTLE_NEXUS], "bt_flag_battle_fu");

    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_KITTY_OPEN);   ++i, j += 3)
        Animation2D().SetTexture(s_apszKittyTexInfo[j], s_apszKittyTexInfo[j + 1]);

    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_MONSTER_OPEN); ++i, j += 3)
        Animation2D().SetTexture(s_apszMonsterTexInfo[j], s_apszMonsterTexInfo[j + 1]);

    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_FOOT_COMBAT);  ++i, j += 3)
        Animation2D().SetTexture(s_apszFootTexInfo[j], s_apszFootTexInfo[j + 1]);
    
    for (int32 i = 0, j = 0; i < CGameData::Record().Nexus().GetTournamentPlayedStage(GAMETYPES::NEXUSID_BATTLE_NEXUS); ++i, j += 3)
        Animation2D().SetTexture(s_apszBattleNexusTexInfo[j], s_apszBattleNexusTexInfo[j + 1]);
};


void CNexusMenuSequence::MessageProc(void)
{
    bool bResult = false;
    
    m_nexusmenu = NEXUSMENU_NONE;

    if (Animation2D().CheckMessageGetURL("Exit"))
    {
        m_nexusmenu = NEXUSMENU_EXIT;
        bResult = true;
    }    
    else if (Animation2D().CheckMessageGetURL("OkKitty") ||
             Animation2D().CheckMessageGetURL("MenuKitty"))
    {
        PressFlashKeyByNexusState(GAMETYPES::NEXUSID_KITTY_OPEN);
    }
    else if (Animation2D().CheckMessageGetURL("OkMons") ||
             Animation2D().CheckMessageGetURL("MenuMons"))
    {
        PressFlashKeyByNexusState(GAMETYPES::NEXUSID_MONSTER_OPEN);
    }    
    else if (Animation2D().CheckMessageGetURL("OkFoot") ||
             Animation2D().CheckMessageGetURL("MenuFoot"))
    {
        PressFlashKeyByNexusState(GAMETYPES::NEXUSID_FOOT_COMBAT);
    }
    else if (Animation2D().CheckMessageGetURL("OkNexus") ||
             Animation2D().CheckMessageGetURL("MenuNexus"))
    {
        PressFlashKeyByNexusState(GAMETYPES::NEXUSID_BATTLE_NEXUS);
    }    
    else if (Animation2D().CheckMessageGetURL("Kitty"))
    {
        m_nexusmenu = NEXUSMENU_KITTY_OPEN;
        bResult = true;
    }    
    else if (Animation2D().CheckMessageGetURL("Mons"))
    {
        m_nexusmenu = NEXUSMENU_MONSTER_OPEN;
        bResult = true;
    }    
    else if (Animation2D().CheckMessageGetURL("Foot"))
    {
        m_nexusmenu = NEXUSMENU_FOOT_COMBAT;
        bResult = true;
    }    
    else if (Animation2D().CheckMessageGetURL("Nexus"))
    {
        m_nexusmenu = NEXUSMENU_BATTLE_NEXUS;
        bResult = true;
    }    
    else if (Animation2D().CheckMessageGetURL("AnimeStart"))
    {
        CMenuController::KeyLock(CController::DIGITAL_OK);
        CMenuController::KeyLock(CController::DIGITAL_CANCEL);
    }    
    else if (Animation2D().CheckMessageGetURL("AnimeEnd"))
    {
        CMenuController::KeyUnlock(CController::DIGITAL_OK);
        CMenuController::KeyUnlock(CController::DIGITAL_CANCEL);
    };

    if (bResult)
        BeginFadeOut();
};


void CNexusMenuSequence::PressFlashKeyByNexusState(GAMETYPES::NEXUSID idNexus)
{
    CMenuController::KeyLock(CController::DIGITAL_LUP);
    CMenuController::KeyLock(CController::DIGITAL_LDOWN);

    if (CGameData::Record().Nexus().GetTournamentState(idNexus) == CNexusRecord::STATE_NONE)
        Animation2D().FlashKeyPress(CController::DIGITAL_LDOWN);
    else
        Animation2D().FlashKeyPress(CController::DIGITAL_LUP);

    CMenuController::KeyUnlock(CController::DIGITAL_LUP);
    CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
};