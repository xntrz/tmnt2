#include "AreaPlaySequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/Process/ProcessList.hpp"


/*static*/ CProcess* CAreaPlaySequence::Instance(void)
{
    return new CAreaPlaySequence;
};


CAreaPlaySequence::CAreaPlaySequence(void)
: m_idLastStage(STAGEID::ID_NONE)
, m_step(STEP_NONE)
, m_substep(0)
{
    ;
};


CAreaPlaySequence::~CAreaPlaySequence(void)
{
    ;
};


bool CAreaPlaySequence::OnAttach(const void* param)
{
    m_idLastStage = STAGEID::ID_NONE;
    changeStep(STEP_STAGESTART);
    CGameData::OnBeginArea();
    return true;
};


void CAreaPlaySequence::OnDetach(void)
{
    ;
};


void CAreaPlaySequence::OnMove(bool bRet, const void* param)
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
                CScreenFade::StartIn(0.0f);
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
                CScreenFade::StartIn(0.0f);

                CGamePlayParam::AREAPLAYMODE playmode = CGameData::PlayParam().GetAreaPlaymode();
                if (playmode == CGamePlayParam::AREAPLAYMODE_ONE_STAGE)
                {
                    CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_CLEAR);
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
                        CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_CLEAR);
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
                CScreenFade::StartIn(0.0f);
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
                        Call(PROCESSTYPES::LABEL_SEQ_SAVELOADAUTO);
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
        Call(PROCESSTYPES::LABEL_SEQ_STAGENORMAL);
        break;

    case GAMETYPES::STAGEMODE_RIDE:
        Call(PROCESSTYPES::LABEL_SEQ_STAGERIDE);
        break;

    case GAMETYPES::STAGEMODE_NEXUS:
        Call(PROCESSTYPES::LABEL_SEQ_STAGENEXUS);
        break;

    case GAMETYPES::STAGEMODE_PLAYDEMO:
        Call(PROCESSTYPES::LABEL_SEQ_STAGEDEMO);
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
        Call(PROCESSTYPES::LABEL_SEQ_RESULT);
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
        Call(PROCESSTYPES::LABEL_SEQ_ENBU);
        break;
    };
};


void CAreaPlaySequence::callUnlockSequence(void)
{
    Call(PROCESSTYPES::LABEL_SEQ_UNLOCK);
};


void CAreaPlaySequence::callMovieSequence(MOVIEID::VALUE idMovie)
{
    CScreenFade::StartOut(0.0f);
    Call(PROCESSTYPES::LABEL_SEQ_MOVIE, false, (const void*)idMovie);
};

