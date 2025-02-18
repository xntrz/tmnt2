#include "AreaPlaySequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/ProcessList.hpp"


/*static*/ CProcess* CAreaPlaySequence::Instance(void)
{
    return new CAreaPlaySequence;
};


CAreaPlaySequence::CAreaPlaySequence(void)
: m_idLastStage(STAGEID::ID_NONE)
, m_step(STEP_NONE)
, m_substep(0)
#ifdef _DEBUG
, m_bSkipAllMovies(false)
#endif /* _DEBUG */
{
    ;
};


CAreaPlaySequence::~CAreaPlaySequence(void)
{
    ;
};


bool CAreaPlaySequence::OnAttach(const void* pParam)
{
    m_idLastStage = STAGEID::ID_NONE;
    changeStep(STEP_STAGESTART);
    
    CGameData::OnBeginArea();

#ifdef BUILD_TRIAL
    CTimeoutProcess::Enable(this, true);
    CTimeoutProcess::Start(this);
#endif /* BUILD_TRIAL */

#ifdef _DEBUG
	m_bSkipAllMovies = (pParam != nullptr);
#endif /* _DEBUG */

    return true;
};


void CAreaPlaySequence::OnDetach(void)
{
    ;
};


void CAreaPlaySequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_step)
    {
    case STEP_STAGESTART:
        {
            if (!m_substep)
            {
                STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();

                if (CStageInfo::IsPlayMovieOfStageIn(idStage))
                    callMovieSequence(CStageInfo::GetStageInMovieID(idStage));

                ++m_substep;
            }
            else
            {
                changeStep(STEP_STAGERUN);
                CScreenFade::BlackOut(0.0f);
            };
        }
        break;

    case STEP_STAGERUN:
        {
            if (!m_substep)
            {
                m_idLastStage = CGameData::PlayParam().GetStage();
                callStageSequence(m_idLastStage);
                ++m_substep;
            }
            else
            {
                if (CGameData::PlayParam().GetAreaPlaymode() == CGamePlayParam::AREAPLAYMODE_ONE_STAGE)
                {
                    changeStep(STEP_STAGEEND);
                }
                else
                {
                    CGamePlayResult::AREARESULT result = CGameData::PlayResult().GetAreaResult();
					if (result == CGamePlayResult::AREARESULT_EXIT)
					{
						changeStep(STEP_EOL);
					}
					else if (result != CGamePlayResult::AREARESULT_GAMEOVER)
					{
						changeStep(STEP_STAGEEND);
					}
					else if (CStageInfo::GetMode(m_idLastStage) == GAMETYPES::STAGEMODE_NEXUS)
					{
						changeStep(STEP_RESULT);
					}
					else
					{
						changeStep(STEP_EOL);
					};
                };
            };
        }
        break;

    case STEP_STAGEEND:
        {
            if (m_substep)
            {
                CScreenFade::BlackOut(0.0f);

                CGamePlayParam::AREAPLAYMODE playmode = CGameData::PlayParam().GetAreaPlaymode();
                if (playmode == CGamePlayParam::AREAPLAYMODE_ONE_STAGE)
                {
                    CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_GAMECLEAR);
                    changeStep(STEP_EOL);
                }
                else
                {
                    if (CGameData::PlayParam().NextStage())
                    {
                        changeStep(STEP_STAGESTART);
                    }
                    else
                    {
                        CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_GAMECLEAR);
                        changeStep(STEP_RESULT);
                    };
                };
            }
            else
            {
                STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();

                if (CStageInfo::IsPlayMovieOfStageOut(idStage))
                    callMovieSequence(CStageInfo::GetStageOutMovieID(idStage));

                ++m_substep;
            };
        }
        break;

    case STEP_RESULT:
        {
            if (m_substep)
            {
                CGameData::OnPostResult();
                changeStep(STEP_ENBU);
            }
            else
            {
                initResultType();
                CGameData::OnPreResult();
                callResultSequence();
                ++m_substep;
            };
        }
        break;

    case STEP_ENBU:
        {
            if (m_substep)
            {
                changeStep(STEP_UNLOCK);
            }
            else
            {
                callEnbuSequence();
                ++m_substep;
            };
        }
        break;

    case STEP_UNLOCK:
        {
            if (m_substep)
            {
                changeStep(STEP_AREAMOVIEOUT);
            }
            else
            {
                if (!CGameData::PlayResult().IsScenarioEnding())
                    callUnlockSequence();

                ++m_substep;
            };
        }
        break;

    case STEP_AREAMOVIEOUT:
        {
            if (m_substep)
            {
                CScreenFade::BlackOut(0.0f);
                changeStep(STEP_AUTOSAVE);
            }
            else
            {
                AREAID::VALUE idArea = CGameData::PlayParam().GetArea();

                if (CAreaInfo::IsPlayMovieOfAreaOut(idArea))
                    callMovieSequence(CAreaInfo::GetAreaOutMovieID(idArea));

                ++m_substep;
            };
        }
        break;

    case STEP_AUTOSAVE:
        {
            if (m_substep)
            {
                changeStep(STEP_EOL);
            }
            else
            {
                if (!CGameData::PlayResult().IsEnding())
                {
                    if (CGameData::Option().Play().IsAutosaveEnabled())
                        Call(PROCLABEL_SEQ_SAVELOADAUTO);
                };

                ++m_substep;
            };
        }
        break;

    case STEP_EOL:
        Ret();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CAreaPlaySequence::OnDraw(void) const
{
    ;
};


void CAreaPlaySequence::changeStep(STEP step)
{
    m_step = step;
    m_substep = 0;
};


void CAreaPlaySequence::initResultType(void)
{
    switch (CStageInfo::GetMode(m_idLastStage))
    {
    case GAMETYPES::STAGEMODE_NORMAL:
        CGameData::PlayResult().SetResultType(GAMETYPES::RESULTTYPE_NORMAL);
        break;

    case GAMETYPES::STAGEMODE_NEXUS:
        CGameData::PlayResult().SetResultType(GAMETYPES::RESULTTYPE_NEXUS);
        break;

    case GAMETYPES::STAGEMODE_RIDE:
        CGameData::PlayResult().SetResultType(GAMETYPES::RESULTTYPE_RIDE);
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CAreaPlaySequence::callStageSequence(STAGEID::VALUE idStage)
{
    switch (CStageInfo::GetMode(m_idLastStage))
    {
    case GAMETYPES::STAGEMODE_NORMAL:
        Call(PROCLABEL_SEQ_NORMALSTAGE);
        break;

    case GAMETYPES::STAGEMODE_RIDE:
        Call(PROCLABEL_SEQ_RIDESTAGE);
        break;

    case GAMETYPES::STAGEMODE_NEXUS:
        Call(PROCLABEL_SEQ_NEXUSSTAGE);
        break;

    case GAMETYPES::STAGEMODE_PLAYDEMO:
        Call(PROCLABEL_SEQ_PLAYDEMO);
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CAreaPlaySequence::callResultSequence(void)
{
    switch (CStageInfo::GetMode(m_idLastStage))
    {
    case GAMETYPES::STAGEMODE_NORMAL:
    case GAMETYPES::STAGEMODE_NEXUS:
    case GAMETYPES::STAGEMODE_RIDE:
        Call(PROCLABEL_SEQ_RESULT);
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CAreaPlaySequence::callEnbuSequence(void)
{
    switch (CStageInfo::GetMode(m_idLastStage))
    {
    case GAMETYPES::STAGEMODE_NORMAL:
	case GAMETYPES::STAGEMODE_RIDE:
        Call(PROCLABEL_SEQ_ENBU);
        break;
    };
};


void CAreaPlaySequence::callUnlockSequence(void)
{
    Call(PROCLABEL_SEQ_UNLOCK);
};


void CAreaPlaySequence::callMovieSequence(MOVIEID::VALUE idMovie)
{
#ifdef _DEBUG
    if (m_bSkipAllMovies)
        return;
#endif /* _DEBUG */
    
    CScreenFade::BlackIn(0.0f);
    Call(PROCLABEL_SEQ_MOVIE, reinterpret_cast<const void*>(idMovie));
};

