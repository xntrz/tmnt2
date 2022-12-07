#include "SaveLoadSequence.hpp"

#include "Game/System/Misc/ScreenFade.hpp"


CSaveLoadSequenceBase::CSaveLoadSequenceBase(CSaveLoad::TYPE type)
: m_phase(PHASE_FADEOUT)
, m_type(type)
{
    ;
};


CSaveLoadSequenceBase::~CSaveLoadSequenceBase(void)
{
    ;
};


bool CSaveLoadSequenceBase::OnAttach(const void* param)
{
    m_phase = PHASE_FADEOUT;
    CSaveLoad::Initialize(m_type);
    return true;
};


void CSaveLoadSequenceBase::OnDetach(void)
{
    CSaveLoad::Terminate();
};


void CSaveLoadSequenceBase::OnMove(bool bRet, const void* param)
{
    switch (m_phase)
    {
    case PHASE_FADEOUT:
        CScreenFade::StartOut(1.0f);
        m_phase = PHASE_FADEOUT_WAIT;
        break;

    case PHASE_FADEOUT_WAIT:
        if (!CScreenFade::IsFading())
            m_phase = PHASE_RUN;
        break;

    case PHASE_RUN:
        if (CSaveLoad::Run())
            m_phase = PHASE_FADEIN;        
        break;

    case PHASE_FADEIN:
        CScreenFade::StartIn(1.0f);
        m_phase = PHASE_FADEIN_WAIT;
        break;

    case PHASE_FADEIN_WAIT:
        if (!CScreenFade::IsFading())
            m_phase = PHASE_EOL;        
        break;

    case PHASE_EOL:
        Ret();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CSaveLoadSequenceBase::OnDraw(void) const
{
    ;
};

