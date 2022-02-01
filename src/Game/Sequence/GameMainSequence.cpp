#include "GameMainSequence.hpp"

#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/GameText.hpp"
#include "Game/System/SaveLoad/SaveLoad.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/SoftwareReset.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/GraphicsDevice.hpp"
#include "System/Common/File/AfsFileID.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/SystemText.hpp"

#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"


/*static*/ CProcess* CGameMainSequence::Instance(void)
{
    return new CGameMainSequence;
};


CGameMainSequence::CGameMainSequence(void)
: m_step(STEP_LOAD_TEXTURE)
, m_iLabelNext(PROCESSTYPES::LABEL_EOL)
, m_iLabelPrev(PROCESSTYPES::LABEL_EOL)
, m_iLabelCurrent(PROCESSTYPES::LABEL_EOL)
, m_param(nullptr)
, m_fTime(0.0f)
, m_iIntroMovieID(0)
{
    ;
};


CGameMainSequence::~CGameMainSequence(void)
{
    ;
};


bool CGameMainSequence::OnAttach(const void* param)
{
    Math::SRand(123456);
    CScreen::SetFlipEnable(false);
    CDataLoader::Initialize();
    CTextureManager::Initialize();
    CGameSound::Initialize();
    CSystemText::Initialize();
    CGameText::Initialize();

    if (CGameFont::Initialize())
    {
        CScreenFade::Initialize();
        CLoadingDisplay::Initialize();

        CDataLoader::Regist(AFSFILEID::ID_STRINGS);
        CDataLoader::Regist(AFSFILEID::ID_FONT);
        
        CGameData::Initialize();

		CGamepad::EnableStickToDigitalMapping(true);

        m_fTime = 0.0f;
        m_iIntroMovieID = MOVIECODE(1);
        m_step = STEP_LOAD_TEXTURE;

#ifdef _DEBUG
        CGimmickDebug::SHOW_AREA = false;
        CGimmickDebug::SHOW_ATTACK = false;
        CGimmickDebug::SHOW_CATCH = false;
        CGimmickDebug::SHOW_MODEL = false;
        CGimmickDebug::SHOW_ME = false;
        CHitAttackManager::m_bDebugDrawHitSphere = false;
#endif            

        return true;
    };

    return false;
};


void CGameMainSequence::OnDetach(void)
{
    CGameData::Terminate();

    CLoadingDisplay::Terminate();
    CScreenFade::Terminate();

    CGameFont::Terminate();
    CGameText::Terminate();
    CSystemText::Terminate();
    CGameSound::Terminate();
    CTextureManager::Terminate();
    CDataLoader::Terminate();
};


void CGameMainSequence::OnMove(bool bResume, const void* param)
{
    switch (m_step)
    {
    case STEP_LOAD_TEXTURE:
        OnLoadTexture();
        break;

    case STEP_LOAD_SOUND:
        OnLoadSound();
        break;

    case STEP_RUN:
        OnRun(bResume, int32(param));
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CGameMainSequence::OnDraw(void) const
{
    ;
};


void CGameMainSequence::OnLoadTexture(void)
{
    if (CDataLoader::IsLoadEnd())
    {
        CScreenFade::StartIn(0.0f);
        CMessageWindow::Load();
        CGameSound::LoadWave(1);
        m_step = STEP_LOAD_SOUND;
    }
    else
    {
        CDataLoader::Period();
    };
};


void CGameMainSequence::OnLoadSound(void)
{
    if (!CGameSound::IsLoadEnd())
        return;

    CScreen::SetFlipInterval(CGraphicsDevice::FLIPINTERVAL_60);
    CScreen::SetFlipEnable(true);

    Call(PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK);

    m_iLabelNext = PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK;
    m_iLabelPrev = PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK;
	m_iLabelCurrent = PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK;
    m_param = nullptr;
    m_step = STEP_RUN;
};


void CGameMainSequence::OnRun(bool bResume, int32 iLabel)
{
    bool bResult = false;

    if (iLabel)
    {
        ASSERT(bResume);
        m_iLabelNext = iLabel;
    }
    else
    {
        m_iLabelNext = Branch(m_iLabelNext);
        if (m_iLabelNext == PROCESSTYPES::LABEL_PREV)
            m_iLabelNext = m_iLabelPrev;
    };

    m_iLabelPrev = m_iLabelCurrent;
    m_iLabelCurrent = m_iLabelNext;

	OUTPUT(
		"[GAME] GameMainSeq: branching to \"%s\", prev \"%s\"\n",
		g_aProcessList[m_iLabelNext == PROCESSTYPES::LABEL_EOL ? PROCESSTYPES::LABEL_MAX : m_iLabelNext].m_pszName,
		g_aProcessList[m_iLabelPrev].m_pszName
	);

    if (m_iLabelNext == PROCESSTYPES::LABEL_EOL)
        bResult = Ret();
    else
        bResult = Call(m_iLabelNext, false, m_param);

    ASSERT(bResult);

    m_param = nullptr;
};


int32 CGameMainSequence::Branch(int32 iLabel)
{
    switch (iLabel)
    {
    case PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK:
        //return PROCESSTYPES::LABEL_SEQ_TITLE;
#ifdef _DEBUG
        {
            bool bBasicDebug = true;
            bool bOverwriteSave = false;

            //
            //  Hold left bumper to enchance game features
            //
            if (CController::GetDigital(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::DIGITAL_LEFT_BUMPER) ||
                CController::GetDigital(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::DIGITAL_LEFT_BUMPER))
                bBasicDebug = false;

            //
            //   Hold right bumper to overwrite save file even if its not new
            //
            if (CController::GetDigital(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::DIGITAL_RIGHT_BUMPER) ||
                CController::GetDigital(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::DIGITAL_RIGHT_BUMPER))
                bOverwriteSave = true;

            if (CSaveLoad::IsNewSave() || bOverwriteSave)
            {
                OUTPUT(
                    "[GAME] %s! Setting %s debug game data...\n",
                    (bOverwriteSave ? "Overriding save file" : "Is new game save"),
                    (bBasicDebug ? "BASIC" : "FULL")
                );

                CGameData::ClearNewGameFlag();
                
                for (int32 i = AREAID::SELECTABLESTART; i < AREAID::SELECTABLEMAX; i++)
                {
                    CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_A);
                    CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_B);
                    CGameData::Record().Area().SetAreaRank(AREAID::VALUE(i), GAMETYPES::CLEARRANK_SS);
                };
                
                for (int32 i = 0; i < 30; ++i)
                {
                    //bool bTaken = (i <= 8);
                    bool bTaken = true;

                    CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_RED, i, bTaken);
                    CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_GREEN, i, bTaken);
                    CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_ORANGE, i, bTaken);
                    CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_WHITE, i, bTaken);
                };

                CGameData::Record().Area().SetCurrentSelectedArea(AREAID::ID_MNY_STN);

                if (!bBasicDebug)
                {
                    CSecretRecord::m_bDebugNotifyUnlocked = false;
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_CHARACTER_SLASHUUR);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_CHARACTER_CASEY);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_CHARACTER_KARAI);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_CHARACTER_SPLINTER);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_NEXUS_LEO);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_NEXUS_RAP);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_NEXUS_MIC);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_NEXUS_DON);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_SAMURAI_LEO);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_SAMURAI_RAP);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_SAMURAI_MIC);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_SAMURAI_DON);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_SHOP);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_BONUSMATERIAL);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_TOURNAMENT_KITTY);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_TOURNAMENT_MONSTER);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_TOURNAMENT_FOOT);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_HOME_TOURNAMENT_BATTLENEXUS);
                    CGameData::Record().Secret().UnlockSecret(SECRETID::ID_CHEATCODE_SHURIKEN);

                    CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_KITTY_OPEN, CNexusRecord::STATE_CLEAR);
                    CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN, CNexusRecord::STATE_CLEAR);
                    CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT, CNexusRecord::STATE_CLEAR);
                    CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS, CNexusRecord::STATE_CLEAR);

					for (int32 i = DBITEMID::ID_FIRST; i < DBITEMID::ID_MAX; ++i)
					{
						if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::VALUE(i)))
							CGameData::Record().Database().SetItemUnlocked(DBITEMID::VALUE(i));
					};

					for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
					{
						if(!CGameData::Record().Antique().IsAntiqueTaken(ANTIQUEID::VALUE(i)))
							CGameData::Record().Antique().SetAntiqueTaken(ANTIQUEID::VALUE(i));
					};

                    CSecretRecord::m_bDebugNotifyUnlocked = true;
                };
            };
        }
        return PROCESSTYPES::LABEL_SEQ_CHARSELECT;
#else        
        return PROCESSTYPES::LABEL_SEQ_LOGO;
#endif        

    case PROCESSTYPES::LABEL_SEQ_LOGO:
        return PROCESSTYPES::LABEL_SEQ_MOVIE;

    case PROCESSTYPES::LABEL_SEQ_MOVIE:
    case PROCESSTYPES::LABEL_SEQ_STAGEDEMO:
        return PROCESSTYPES::LABEL_SEQ_TITLE;

    case PROCESSTYPES::LABEL_SEQ_TITLE:
        return PROCESSTYPES::LABEL_SEQ_CHARSELECT;

    case PROCESSTYPES::LABEL_SEQ_CHARSELECT:
        {
            if (!CGameData::IsNewGame())
                return PROCESSTYPES::LABEL_SEQ_AREA;

            if (CGameData::Record().Area().GetCurrentSelectedArea() != AREAID::ID_AREA01)
                return PROCESSTYPES::LABEL_SEQ_AREA;

            CGameData::PlayParam().SetStartArea(AREAID::ID_AREA01, 0);
            CGameData::ClearNewGameFlag();
            return PROCESSTYPES::LABEL_SEQ_AREAPLAY;
        }
        break;
        
    case PROCESSTYPES::LABEL_SEQ_UNLOCK:
        return PROCESSTYPES::LABEL_EOL;

    case PROCESSTYPES::LABEL_SEQ_SAVELOADMENULOAD:
    case PROCESSTYPES::LABEL_SEQ_SAVELOADMENUSAVE:
        return PROCESSTYPES::LABEL_PREV;

    case PROCESSTYPES::LABEL_SEQ_HOME:
        return PROCESSTYPES::LABEL_SEQ_AREA;

    case PROCESSTYPES::LABEL_SEQ_AREA:
        {
            AREAID::VALUE idAreaNow = CGameData::Record().Area().GetCurrentSelectedArea();

            if (idAreaNow == AREAID::HOME)
                return PROCESSTYPES::LABEL_SEQ_HOME;
            else if (idAreaNow >= AREAID::NORMALMAX)
                return PROCESSTYPES::LABEL_SEQ_AREA;
            else
                return PROCESSTYPES::LABEL_SEQ_AREAPLAY;
        }
        return PROCESSTYPES::LABEL_EOL;

	case PROCESSTYPES::LABEL_SEQ_AREAPLAY:
        {
            CGamePlayResult::AREARESULT arearesult = CGameData::PlayResult().GetAreaResult();

            switch (arearesult)
            {
            case CGamePlayResult::AREARESULT_CLEAR:
                if (CGameData::Record().Area().GetCurrentSelectedArea() == AREAID::ID_AREA58)
                    return PROCESSTYPES::LABEL_EOL; // TODO ending
                else
                    return PROCESSTYPES::LABEL_SEQ_AREA;

            case CGamePlayResult::AREARESULT_GAMEOVER:
                return PROCESSTYPES::LABEL_SEQ_AREA;
                
            case CGamePlayResult::AREARESULT_EXIT:
                {
                    CGamePlayResult::EXITSUB exitsub = CGameData::PlayResult().GetExitSub();
                    switch (exitsub)
                    {
                    case CGamePlayResult::EXITSUB_AREASEL:
                        return PROCESSTYPES::LABEL_SEQ_AREA;
                        
                    case CGamePlayResult::EXITSUB_TITLE:
                        return PROCESSTYPES::LABEL_SEQ_TITLE;

                    case CGamePlayResult::EXITSUB_CHARASEL:
                        return PROCESSTYPES::LABEL_SEQ_CHARSELECT;

                    default:
                        ASSERT(false);
                        return PROCESSTYPES::LABEL_SEQ_TITLE;
                    };
                }
                break;

            default:
                return PROCESSTYPES::LABEL_SEQ_TITLE;
            };
        }
		break;

    default:
		OUTPUT("[GAME] Unknown branching for: %s\n", g_aProcessList[iLabel].m_pszName);
        ASSERT(false);
        return PROCESSTYPES::LABEL_EOL;
    };
};