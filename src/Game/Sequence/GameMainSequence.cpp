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
#include "Game/System/Misc/PadConnectCheck.hpp"
#include "Game/System/Movie/MovieManager.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/GraphicsDevice.hpp"
#include "System/Common/File/FileID.hpp"
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
, m_iMovieID(0)
{
    ;
};


CGameMainSequence::~CGameMainSequence(void)
{
    ;
};


bool CGameMainSequence::OnAttach(const void* param)
{
    CScreen::SetFlipEnable(false);
#ifdef _TARGET_PC    
    CScreen::SetFlipInterval(1);    // 60
#else
    CScreen::SetFlipInterval(2);    // 30
#endif    
    Math::SRand(123456);
    CDataLoader::Initialize();
    CTextureManager::Initialize();
    CMovieManager::Initialize();
    CGameSound::Initialize();
    CSystemText::Initialize();
    CGameText::Initialize();

    if (CGameFont::Initialize())
    {        
        if (CScreenFade::Initialize(this) &&
            CLoadingDisplay::Initialize(this))
        {
            CDataLoader::Regist(FILEID::ID_STRINGS);
            CDataLoader::Regist(FILEID::ID_FONT);
            CGameData::Initialize();
            CGamepad::EnableStickToDigitalMapping(true);

            m_fTime = 0.0f;
            m_iMovieID = MOVIECODE(0);
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
    };

    return false;
};


void CGameMainSequence::OnDetach(void)
{
#ifdef _TARGET_PS2
    CPadConnectCheckProcess::Terminate(this);
#endif
    
    CGameData::Terminate();
    CLoadingDisplay::Terminate(this);
    CScreenFade::Terminate(this);
    
    CGameFont::Terminate();

    CGameText::Terminate();
    CSystemText::Terminate();
    CGameSound::Terminate();
    CMovieManager::Terminate();
    CTextureManager::Terminate();
    CDataLoader::Terminate();
};


void CGameMainSequence::OnMove(bool bRet, const void* param)
{
    if (bRet)
    {
		//for (int32 i = 0; i < CSystemText::GetTextNum(); ++i)
		//	wprintf(L"%x) %s\n", i, CSystemText::GetText(SYSTEXT::VALUE(i)));

        //for (int32 i = AREAID::SELECTABLESTART; i < AREAID::SELECTABLEMAX; i++)
        //{
        //    CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_A);
        //    CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_B);
        //    CGameData::Record().Area().SetAreaRank(AREAID::VALUE(i), GAMETYPES::CLEARRANK_SS);
        //};

        //for (int32 i = 0; i < CGameText::GetTextNum(); ++i)
        //    wprintf(L"%x) %s\n", i, CGameText::GetText(GAMETEXT::VALUE(i)));
        
        //for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
        //{
        //    if (!CGameData::Record().Antique().IsAntiqueTaken(ANTIQUEID::VALUE(i)))
        //        CGameData::Record().Antique().SetAntiqueTaken(ANTIQUEID::VALUE(i));
        //};

        //for (int32 i = DBITEMID::ID_FIRST; i < DBITEMID::ID_MAX; ++i)
        //{
        //    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::VALUE(i)))
        //        CGameData::Record().Database().SetItemUnlocked(DBITEMID::VALUE(i));
        //};
        
        if (param)
        {
            m_iLabelNext = int32(param);
            if (m_iLabelNext == PROCESSTYPES::LABEL_EOL)
            {
                Kill(PROCESSTYPES::LABEL_EOL);
                return;
            };
        }
        else
        {
            m_iLabelNext = Branch(m_iLabelNext);
            if (m_iLabelNext == PROCESSTYPES::LABEL_PREV)
                m_iLabelNext = m_iLabelPrev;
        };        

        m_iLabelPrev = m_iLabelCurrent;
        m_iLabelCurrent = m_iLabelNext;
        
        if (m_iLabelCurrent == PROCESSTYPES::LABEL_SEQ_MOVIE)
        {
            PreMovie();
        }
        else if (m_iLabelCurrent == PROCESSTYPES::LABEL_SEQ_AREA)
        {
            CGameData::ClearNewGameFlag();
        };

        OUTPUT(
            "GameMainSeq: branching to \"%s\", prev \"%s\"\n",
            g_aProcessList[m_iLabelNext == PROCESSTYPES::LABEL_EOL ? PROCESSTYPES::LABEL_MAX : m_iLabelNext].m_pszName,
            g_aProcessList[m_iLabelPrev].m_pszName
        );

        Call(m_iLabelCurrent);
    }
    else
    {
        m_fTime += CScreen::TimerStride();
        
        switch (m_step)
        {
        case STEP_LOAD_TEXTURE:
            {
                if (CDataLoader::IsLoadEnd())
                {
                    CScreenFade::StartIn(0.0f);
                    CMessageWindow::Load();
                    CGameSound::LoadWave(1);
                    m_step = STEP_LOAD_SOUND;

#ifdef _TARGET_PS2                    
                    CPadConnectCheckProcess::Initialize(this);
#endif                    
                }
                else
                {
                    CDataLoader::Period();
                };
            }
            break;

        case STEP_LOAD_SOUND:
            {
                if (CGameSound::IsLoadEnd())
                {
                    CScreen::SetFlipEnable(true);
                    Call(PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK);
                    m_iLabelNext = PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK;
                    m_iLabelPrev = PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK;
                    m_iLabelCurrent = PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK;
                    m_param = nullptr;
                    m_step = STEP_RUN;
                };
            }
            break;
        };
    };
};


void CGameMainSequence::OnDraw(void) const
{
    ;
};


int32 CGameMainSequence::Branch(int32 iLabel)
{
    switch (iLabel)
    {
#ifdef _DEBUG
    case PROCESSTYPES::LABEL_SEQ_TESTMV:
    case PROCESSTYPES::LABEL_SEQ_TESTSD:
        return PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK;
#endif
        
    case PROCESSTYPES::LABEL_SEQ_MOVIE:
        PostMovie();
        return PROCESSTYPES::LABEL_SEQ_TITLE;
        
    case PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK:
#ifdef _DEBUG        
        return BranchTest(PROCESSTYPES::LABEL_SEQ_TITLE);
#else
        return PROCESSTYPES::LABEL_SEQ_LOGO;
#endif        

    case PROCESSTYPES::LABEL_SEQ_LOGO:
        return (CConfigure::m_launchmode != CConfigure::LAUNCHMODE_NORMAL ? PROCESSTYPES::LABEL_SEQ_TITLE : PROCESSTYPES::LABEL_SEQ_MOVIE);

    case PROCESSTYPES::LABEL_SEQ_STAGEDEMO:
    case PROCESSTYPES::LABEL_SEQ_ENDING:
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

    case PROCESSTYPES::LABEL_SEQ_OPTION:        
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
                    return PROCESSTYPES::LABEL_SEQ_ENDING;
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
		OUTPUT("Unknown branching for: %s\n", g_aProcessList[iLabel].m_pszName);
        ASSERT(false);
        return PROCESSTYPES::LABEL_EOL;
    };
};


void CGameMainSequence::PreMovie(void)
{
    CMovieManager::PreCreateMovieInstance(m_iMovieID);
    CScreenFade::StartOut(0.0f);
    m_iMovieID = (m_iMovieID == 0);
};


void CGameMainSequence::PostMovie(void)
{
    CMovieManager::DeleteMovieInstance();
};


int32 CGameMainSequence::BranchTest(int32 iDefaultLabel)
{
    struct TESTSEQ_INFO
    {
        int32 m_iLabel;
        int32 m_Pad;
        uint32 m_DigitalBtn;
    };

    static const TESTSEQ_INFO s_aTestSeqInfo[] =
    {
        { PROCESSTYPES::LABEL_SEQ_TESTMV, 0, CController::DIGITAL_SELECT | CController::DIGITAL_UP      },
        { PROCESSTYPES::LABEL_SEQ_TESTSD, 0, CController::DIGITAL_SELECT | CController::DIGITAL_LEFT    },
    };

    for (int32 i = 0; i < COUNT_OF(s_aTestSeqInfo); ++i)
    {
        const TESTSEQ_INFO* pTestSeqInfo = &s_aTestSeqInfo[i];
        uint32 DigitalBtn = CController::GetDigital(pTestSeqInfo->m_Pad);
        if (FLAG_TEST(DigitalBtn, pTestSeqInfo->m_DigitalBtn))
            return pTestSeqInfo->m_iLabel;
    };

    return iDefaultLabel;
};