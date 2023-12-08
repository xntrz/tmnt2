#include "SaveLoadSequence.hpp"

#include "Game/System/Misc/ScreenFade.hpp"


CSaveLoadSequence::CSaveLoadSequence(CSaveLoad::TYPE type)
: m_phase(PHASE_FADEIN)
, m_type(type)
{
    ;
};


CSaveLoadSequence::~CSaveLoadSequence(void)
{
    ;
};


bool CSaveLoadSequence::OnAttach(const void* pParam)
{
    m_phase = PHASE_FADEIN;
    CSaveLoad::Initialize(m_type);
    return true;
};


void CSaveLoadSequence::OnDetach(void)
{
    CSaveLoad::Terminate();
};


void CSaveLoadSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_phase)
    {
    case PHASE_FADEIN:
        {
            CScreenFade::BlackIn(1.0f);
            m_phase = PHASE_FADEIN_WAIT;
        }
        break;

    case PHASE_FADEIN_WAIT:
        {
            if (!CScreenFade::IsFading())
                m_phase = PHASE_RUN;
        }
        break;

    case PHASE_RUN:
        {
            if (CSaveLoad::Run())
                m_phase = PHASE_FADEOUT;
        }
        break;

    case PHASE_FADEOUT:
        {
            CScreenFade::BlackOut(1.0f);
            m_phase = PHASE_FADEOUT_WAIT;
        }
        break;

    case PHASE_FADEOUT_WAIT:
        {
            if (!CScreenFade::IsFading())
                m_phase = PHASE_EOL;
        }
        break;

    case PHASE_EOL:
        {
            Ret();
        }        
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CSaveLoadSequence::OnDraw(void) const
{
    ;
};

