#include "EndingSequence.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/ProcessList.hpp"


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


bool CEndingSequence::OnAttach(const void* pParam)
{
    m_iType = reinterpret_cast<int32>(pParam);
    m_step = STEP_INIT;
    return true;
};


void CEndingSequence::OnDetach(void)
{
    ;
};


void CEndingSequence::OnMove(bool bRet, const void* pReturnValue)
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
        CScreenFade::BlackOut(0.0f);
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

    default:
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
            CScreenFade::BlackIn(0.0f);

            switch (m_iType)
            {
            case GAMETYPES::ENDINGTYPE_STORY:
                Call(PROCLABEL_SEQ_MOVIE, reinterpret_cast<const void*>(MVPATH("m58ob02E.sfd")));
                break;

            case GAMETYPES::ENDINGTYPE_NEXUS:
                Call(PROCLABEL_SEQ_MOVIE, reinterpret_cast<const void*>(MVPATH("m60x03E.sfd")));
                break;

            case GAMETYPES::ENDINGTYPE_SHOP:
                Call(PROCLABEL_SEQ_MOVIE, reinterpret_cast<const void*>(MVPATH("m62x01E.sfd")));
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case STEP_STAFFROLL:
        {
            CScreenFade::BlackIn(0.0f);
            Call(PROCLABEL_SEQ_STAFFROLL);
        }
        break;

    case STEP_RESULT:
        {
            if (m_iType == GAMETYPES::ENDINGTYPE_STORY)
            {
                CGameData::PlayResult().SetResultType(GAMETYPES::RESULTTYPE_ENDING);
                Call(PROCLABEL_SEQ_RESULT);
            };
        }
        break;

    case STEP_UNLOCK:
        {
            CGameData::OnEndEnding();
            Call(PROCLABEL_SEQ_UNLOCK);
        }
        break;

    case STEP_SAVE:
        {
            if (CGameData::Option().Play().IsAutosaveEnabled())
                Call(PROCLABEL_SEQ_SAVELOADAUTO);
            else
                Call(PROCLABEL_SEQ_SAVELOADMENUSAVE);
        }
        break;

    case STEP_EOL:
        {
            Ret();
        }
        break;

    default:
        break;
    };
};