#include "GameMainSequence.hpp"

#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/SoftwareReset.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Misc/PadConnectCheck.hpp"
#include "Game/System/Misc/TouchController.hpp"
#include "Game/System/Movie/MovieManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/SystemText.hpp"

#if defined(TARGET_WEB)
#include "System/Web/WebSpecific.hpp"
#include "System/Web/File/WebFile.hpp"
#endif /* defined(TARGET_WEB) */


#define MVPATH_OP1 (MVPATH("OP_TMNT1.sfd"))
#define MVPATH_OP2 (MVPATH("OP_TMNT2.sfd"))


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
, m_movieId(MVPATH_OP1)
{
    ;
};


CGameMainSequence::~CGameMainSequence(void)
{
    ;
};


bool CGameMainSequence::OnAttach(const void* pParam)
{
    Math::SRand(123456);
#if defined(TARGET_PC)    
    CScreen::SetFlipInterval(1);
#endif /* defined(TARGET_PC) */
    CDataLoader::Initialize();
    CTextureManager::Initialize();
#if defined(TMNT2_FEATURE_MOVIE)    
    CMovieManager::Initialize();
#endif /* defined(TMNT2_FEATURE_MOVIE) */
    CGameSound::Initialize();
    CSystemText::Initialize();
    CGameText::Initialize();

    if (!CGameFont::Initialize())
        return false;

    if (!CScreenFadeProcess::Initialize(this))
        return false;

    if (!CLoadingDisplay::Initialize(this))
        return false;
    
    CDataLoader::Regist(FPATH_LANG("Language/English/Text/Text.lpac"));
    CDataLoader::Regist(FPATH("Common/Fonts/Fonts.lpac"));
#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    //if (CWebSpecific::IsMobilePlatform())
        //CDataLoader::Regist(CWebFile::MakePath("touch.txd"), true);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */

    CGameData::Initialize();
    EnableStickToDirButton(true);

#ifdef _DEBUG
    CSoftwareResetProcess::Initialize(this, CSoftwareResetProcess::MODE_DEBUGMENU);
#endif /* _DEBUG */
    
    m_fTime = 0.0f;
    m_movieId = MVPATH_OP2;
    m_step = STEP_LOAD_TEXTURE;

    return true;
};


void CGameMainSequence::OnDetach(void)
{
#ifdef _DEBUG
    CSoftwareResetProcess::Terminate(this);
#endif /* _DEBUG */

#ifdef TARGET_PS2
    CPadConnectCheckProcess::Terminate(this);
#endif /* TARGET_PS2 */

#if defined(TARGET_WEB)
    if (CWebSpecific::IsMobilePlatform())
        CTouchControllerProcess::Terminate(this);
#endif /* defined(TARGET_WEB) */

    CGameData::Terminate();
    CLoadingDisplay::Terminate(this);
    CScreenFadeProcess::Terminate(this);
    
    CGameFont::Terminate();

    CGameText::Terminate();
    CSystemText::Terminate();
    CGameSound::Terminate();
#if defined(TMNT2_FEATURE_MOVIE)    
    CMovieManager::Terminate();
#endif /* defined(TMNT2_FEATURE_MOVIE) */
    CTextureManager::Terminate();
    CDataLoader::Terminate();
};


void CGameMainSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
    {
        if (pReturnValue)
        {
            m_iLabelNext = reinterpret_cast<int32>(pReturnValue);
            if (m_iLabelNext == PROCESSTYPES::LABEL_EOL)
            {
                Kill(PROCESSTYPES::LABEL_EOL);
                return;
            };
        }
        else
        {
            m_iLabelNext = Branch(m_iLabelNext);
            if (m_iLabelNext == PROCESSTYPES::LABEL_SEQ_PREV)
                m_iLabelNext = m_iLabelPrev;
        };        

        m_iLabelPrev = m_iLabelCurrent;
        m_iLabelCurrent = m_iLabelNext;
        
        if (m_iLabelCurrent == PROCLABEL_SEQ_MOVIE)
        {
            PreMovie();
        }
        else if (m_iLabelCurrent == PROCLABEL_SEQ_AREA)
        {
            CGameData::ClearNewGameFlag();
        };
        
        Call(m_iLabelCurrent, m_param);
        m_param = nullptr;
    }
    else
    {
        m_fTime += CScreen::TimerStride();
        
        switch (m_step)
        {
        case STEP_LOAD_TEXTURE:
            {
                CDataLoader::Period();
                if (!CDataLoader::IsLoadEnd())
                    break;

                CMessageWindow::Load();                
                CGameSound::LoadWave(1);
                
                m_step = STEP_LOAD_SOUND;
#ifdef TARGET_PS2
                CPadConnectCheckProcess::Initialize(this);
#endif /* TARGET_PS2 */

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
                if (CWebSpecific::IsMobilePlatform())
                    CTouchControllerProcess::Initialize(this);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */
            }
            break;

        case STEP_LOAD_SOUND:
            {
                if (!CGameSound::IsLoadEnd())
                    break;

                const int32 iFirstLabel = PROCLABEL_SEQ_SAVELOADCHECK;

                m_iLabelNext = iFirstLabel;
                m_iLabelPrev = iFirstLabel;
                m_iLabelCurrent = iFirstLabel;
                m_param = nullptr;
                m_step = STEP_RUN;

                CScreen::SetFlipEnable(true);
#ifdef _DEBUG
                CSoftwareResetProcess::SetEnable(this, true);
                CSoftwareResetProcess::SetResponse(this, 1.5f);

                Call(PROCLABEL_SEQ_DBGMAIN);
#else /* _DEBUG */
                Call(m_iLabelCurrent);
#endif /* _DEBUG */

            }
            break;

        default:
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
    case PROCLABEL_SEQ_MOVIE:
        PostMovie();
        return PROCLABEL_SEQ_TITLE;

    case PROCLABEL_SEQ_HOME:
        return PROCLABEL_SEQ_AREA;

    case PROCLABEL_SEQ_OPTIONS:
    case PROCLABEL_SEQ_SAVELOADMENULOAD:
    case PROCLABEL_SEQ_SAVELOADMENUSAVE:
        return PROCESSTYPES::LABEL_SEQ_PREV;

    case PROCLABEL_SEQ_AREA:
        {
            AREAID::VALUE idAreaNow = CGameData::Record().Area().GetCurrentSelectedArea();
            if (idAreaNow == AREAID::HOME)
                return PROCLABEL_SEQ_HOME;
            else
                return (idAreaNow >= AREAID::NORMALMAX) ? PROCLABEL_SEQ_AREA :
                                                          PROCLABEL_SEQ_AREAPLAY;
        }
		break;

    case PROCLABEL_SEQ_SAVELOADCHECK:
#ifdef _DEBUG
        return PROCLABEL_SEQ_TITLE;
#else /* _DEBUG */
        return PROCLABEL_SEQ_LOGODISP;
#endif /* _DEBUG */

    case PROCLABEL_SEQ_LOGODISP:
#ifdef TMNT2_FEATURE_MOVIE
        return (CConfigure::GetLaunchMode() != TYPEDEF::CONFIG_LAUNCH_NORMAL ? PROCLABEL_SEQ_TITLE :
                                                                               PROCLABEL_SEQ_MOVIE);
#else /* TMNT2_FEATURE_MOVIE */
        return PROCLABEL_SEQ_TITLE;
#endif /* TMNT2_FEATURE_MOVIE */

    case PROCLABEL_SEQ_TITLE:
        return PROCLABEL_SEQ_CHARASELECT;

    case PROCLABEL_SEQ_PLAYDEMO:
    case PROCLABEL_SEQ_ENDING:
        return PROCLABEL_SEQ_TITLE;

    case PROCLABEL_SEQ_CHARASELECT:
        {
            if (!CGameData::IsNewGame())
                return PROCLABEL_SEQ_AREA;
            
            if (CGameData::Record().Area().GetCurrentSelectedArea() != AREAID::ID_AREA01)
                return PROCLABEL_SEQ_AREA;
            
            CGameData::PlayParam().SetStartArea(AREAID::ID_AREA01, 0);
            CGameData::ClearNewGameFlag();
        }
        return PROCLABEL_SEQ_AREAPLAY;

    case PROCLABEL_SEQ_AREAPLAY:
        {
            CGamePlayResult::AREARESULT arearesult = CGameData::PlayResult().GetAreaResult();
            switch (arearesult)
            {
            case CGamePlayResult::AREARESULT_GAMECLEAR:
                {
                    AREAID::VALUE idAreaNow = CGameData::Record().Area().GetCurrentSelectedArea();
                    if (idAreaNow == AREAID::ID_AREA58)
                    {
                        m_param = reinterpret_cast<void*>(GAMETYPES::ENDINGTYPE_STORY);
                        return PROCLABEL_SEQ_ENDING;
                    };
                }
                return PROCLABEL_SEQ_AREA;

            case CGamePlayResult::AREARESULT_GAMEOVER:
                return PROCLABEL_SEQ_AREA;

            case CGamePlayResult::AREARESULT_EXIT:
                {
                    CGamePlayResult::EXITSUB exitsub = CGameData::PlayResult().GetExitSub();
                    switch (exitsub)
                    {
                    case CGamePlayResult::EXITSUB_TO_AREASEL:
                        return PROCLABEL_SEQ_AREA;

                    case CGamePlayResult::EXITSUB_TO_TITLE:
                        return PROCLABEL_SEQ_TITLE;

                    case CGamePlayResult::EXITSUB_TO_CHARASEL:
                        return PROCLABEL_SEQ_CHARASELECT;

                    default:
                        ASSERT(false);
                        return PROCLABEL_SEQ_TITLE;
                    };
                }
                break;

            default:
                return PROCLABEL_SEQ_TITLE;
            };
        }
		break;

    default:
        ASSERT(false);
		break;
    };

	return PROCESSTYPES::LABEL_EOL;
};


void CGameMainSequence::PreMovie(void)
{
#if defined(TMNT2_FEATURE_MOVIE)
    CMovieManager::PreCreateMovieInstance(m_movieId);
    CScreenFade::BlackIn(0.0f);

    if (MVNAME_EQUAL(m_movieId, MVPATH_OP1))
        m_movieId = MVPATH_OP2;
    else
        m_movieId = MVPATH_OP1;
#endif /* defined(TMNT2_FEATURE_MOVIE) */
};


void CGameMainSequence::PostMovie(void)
{
#if defined(TMNT2_FEATURE_MOVIE)
    CMovieManager::DeleteMovieInstance();
#endif /* defined(TMNT2_FEATURE_MOVIE) */
};