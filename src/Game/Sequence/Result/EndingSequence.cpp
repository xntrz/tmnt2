#include "EndingSequence.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/MovieID.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/Process/ProcessList.hpp"


/*static*/ CProcess* CEndingSequence::Instance(void)
{
    return new CEndingSequence;
};


CEndingSequence::CEndingSequence(void)
: m_iType(GAMETYPES::ENDINGTYPE_STORY)
, m_step(STEP_INIT)
{
    ;
};


CEndingSequence::~CEndingSequence(void)
{
    ;
};


bool CEndingSequence::OnAttach(const void* param)
{
    m_iType = int32(param);
    m_step = STEP_INIT;
    return true;
};


void CEndingSequence::OnDetach(void)
{
    ;
};


void CEndingSequence::OnMove(bool bResume, const void* param)
{
    PostSequence();
    Branch();
};


void CEndingSequence::OnDraw(void) const
{
    ;
};


void CEndingSequence::PostSequence(void)
{
    switch (m_step)
    {
    case STEP_INIT:
        m_step = STEP_MOVIE;
        break;
        
    case STEP_MOVIE:
        CScreenFade::StartIn(0.0f);
        m_step = STEP_STAFFROLL;
        break;
        
    case STEP_STAFFROLL:
        m_step = STEP_RESULT;
        break;
        
    case STEP_RESULT:
        m_step = STEP_UNLOCK;
        break;
        
    case STEP_UNLOCK:
        m_step = STEP_SAVE;
        break;
        
    case STEP_SAVE:
        m_step = STEP_EOL;
        break;
        
    case STEP_EOL:
        break;
    };    
};


void CEndingSequence::Branch(void)
{
    switch (m_step)
    {
    case STEP_INIT:
        {
            ;
        }
        break;

    case STEP_MOVIE:
        {
            CScreenFade::StartOut(0.0f);

            switch (m_iType)
            {
            case GAMETYPES::ENDINGTYPE_STORY:
                Call(PROCESSTYPES::LABEL_SEQ_MOVIE, false, (const void*)MOVIEID::VALUE(70));
                break;

            case GAMETYPES::ENDINGTYPE_NEXUS:
                Call(PROCESSTYPES::LABEL_SEQ_MOVIE, false, (const void*)MOVIEID::VALUE(77));
                break;

            case GAMETYPES::ENDINGTYPE_SHOP:
                Call(PROCESSTYPES::LABEL_SEQ_MOVIE, false, (const void*)MOVIEID::VALUE(78));
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case STEP_STAFFROLL:
        {
            CScreenFade::StartOut(0.0f);
            Call(PROCESSTYPES::LABEL_SEQ_STAFFROLL);
        }
        break;

    case STEP_RESULT:
        {
            if (m_iType == GAMETYPES::ENDINGTYPE_STORY)
            {
                CGameData::PlayResult().SetResultType(GAMETYPES::RESULTTYPE_ENDING);
                Call(PROCESSTYPES::LABEL_SEQ_RESULT);
            };
        }
        break;

    case STEP_UNLOCK:
        {
            CGameData::OnEndEnding();
            Call(PROCESSTYPES::LABEL_SEQ_UNLOCK);
        }
        break;

    case STEP_SAVE:
        {
            if (CGameData::Option().Play().IsAutosaveEnabled())
                Call(PROCESSTYPES::LABEL_SEQ_SAVELOADAUTO);
            else
                Call(PROCESSTYPES::LABEL_SEQ_SAVELOADMENUSAVE);
        }
        break;

    case STEP_EOL:
        {
            Ret();
        }
        break;
    };
};