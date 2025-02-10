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
#include "Game/System/Movie/MovieID.hpp"
#include "Game/System/Movie/MovieManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/SystemText.hpp"


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
, m_movieId(MOVIEID::ID_OP_TMNT1)
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
    CScreen::SetFlipInterval(1);
    CDataLoader::Initialize();
    CTextureManager::Initialize();
    CMovieManager::Initialize();
    CGameSound::Initialize();
    CSystemText::Initialize();
    CGameText::Initialize();

    if (!CGameFont::Initialize())
        return false;

    if (!CScreenFadeProcess::Initialize(this))
        return false;

    if (!CLoadingDisplay::Initialize(this))
        return false;

    CDataLoader::Regist(FILEID::ID_STRINGS);
    CDataLoader::Regist(FILEID::ID_FONT);
    CGameData::Initialize();
    EnableStickToDirButton(true);

    m_fTime = 0.0f;
    m_movieId = MOVIEID::ID_OP_TMNT2;
    m_step = STEP_LOAD_TEXTURE;

    return true;
};


void CGameMainSequence::OnDetach(void)
{
#ifdef TARGET_PS2
    CPadConnectCheckProcess::Terminate(this);
#endif

    CGameData::Terminate();
    CLoadingDisplay::Terminate(this);
    CScreenFadeProcess::Terminate(this);
    
    CGameFont::Terminate();

    CGameText::Terminate();
    CSystemText::Terminate();
    CGameSound::Terminate();
    CMovieManager::Terminate();
    CTextureManager::Terminate();
    CDataLoader::Terminate();
};


void CGameMainSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
    {
#ifdef BUILD_TRIAL        
        CTimeoutProcess::Reset(this);
#endif
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
            PreMovie();
        else if (m_iLabelCurrent == PROCLABEL_SEQ_AREA)
            CGameData::ClearNewGameFlag();
        
        Call(m_iLabelCurrent);
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
                CGameSound::LoadWave(0);
                CGameSound::LoadWave(1);
                m_step = STEP_LOAD_SOUND;
#ifdef TARGET_PS2                    
                CPadConnectCheckProcess::Initialize(this);
#endif                    
            }
            break;

        case STEP_LOAD_SOUND:
            {
                if (!CGameSound::IsLoadEnd())
                    break;

                m_iLabelNext = m_iLabelPrev = m_iLabelCurrent = PROCLABEL_SEQ_SAVELOADCHECK;
                m_param = nullptr;
                m_step = STEP_RUN;

                CScreen::SetFlipEnable(true);
#ifdef _DEBUG
                Call(PROCLABEL_SEQ_DBGMAIN);
#else
                Call(PROCLABEL_SEQ_SAVELOADCHECK);
#endif                
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
                return (idAreaNow >= AREAID::NORMALMAX) ? PROCLABEL_SEQ_AREA : PROCLABEL_SEQ_AREAPLAY;        
        }
		break;

    case PROCLABEL_SEQ_SAVELOADCHECK:
        return PROCLABEL_SEQ_LOGODISP;

    case PROCLABEL_SEQ_LOGODISP:
#ifdef _DEBUG
        return PROCLABEL_SEQ_TITLE;
#else
        return (CConfigure::GetLaunchMode() != TYPEDEF::CONFIG_LAUNCH_NORMAL ? PROCLABEL_SEQ_TITLE : PROCLABEL_SEQ_MOVIE);
#endif        

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
                return (CGameData::Record().Area().GetCurrentSelectedArea() == AREAID::ID_AREA58) ? PROCLABEL_SEQ_ENDING : PROCLABEL_SEQ_AREA;                

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
    CMovieManager::PreCreateMovieInstance(m_movieId);
    CScreenFade::BlackIn(0.0f);

    if (m_movieId == MOVIEID::ID_OP_TMNT1)
        m_movieId = MOVIEID::ID_OP_TMNT2;
    else
        m_movieId = MOVIEID::ID_OP_TMNT1;
};


void CGameMainSequence::PostMovie(void)
{
    CMovieManager::DeleteMovieInstance();
};