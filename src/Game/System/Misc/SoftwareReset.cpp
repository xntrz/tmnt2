#include "SoftwareReset.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/Sequence.hpp"


static bool s_bRequested = false;
static bool s_bExecuting = false;


/*static*/ CProcess* CSoftwareReset::Instance(void)
{
    return new CSoftwareReset;
};


/*static*/ void CSoftwareReset::Request(void)
{
    s_bRequested = true;
};


/*static*/ bool CSoftwareReset::IsRequested(void)
{
    return s_bExecuting;
};


/*static*/ void CSoftwareReset::Complete(void)
{
    s_bExecuting = false;
};


/*static*/ void CSoftwareReset::Initialize(void)
{
    CProcessDispatcher::AttachProcess(PROCESSTYPES::LABEL_SOFTWARERESET);
};


/*static*/ void CSoftwareReset::Terminate(void)
{
    CProcessDispatcher::DetachProcess(PROCESSTYPES::LABEL_SOFTWARERESET);
};


CSoftwareReset::CSoftwareReset(void)
: m_fTimer(0.0f)
{
    ;
};


CSoftwareReset::~CSoftwareReset(void)
{
    ;
};


bool CSoftwareReset::Attach(void)
{
    m_fTimer = 0.0f;
	return true;
};


void CSoftwareReset::Detach(void)
{
    ;
};


void CSoftwareReset::Move(void)
{
    if (!CGameData::Attribute().IsInteractive())
    {
        m_fTimer = 0.0f;
        return;
    };

#ifdef _DEBUG
    CheckDebugRequest();
#endif

    if (s_bRequested)
    {
        s_bRequested = false;
        s_bExecuting = true;
        
        ExecReset();
    };
};


void CSoftwareReset::Draw(void) const
{
	;
};


void CSoftwareReset::ExecReset(void)
{
    int32 iRootSeq = PROCESSTYPES::LABEL_TOP;
    int32 iCurrentSeq = CSequence::GetCurrently();

    if(Info().IsProcessExist(iCurrentSeq))
        ((CSequence&)Info().Process(iCurrentSeq)).Kill(iRootSeq);
};


void CSoftwareReset::CheckDebugRequest(void)
{    
    if (CController::GetDigital(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::DIGITAL_SELECT) &&
        CController::GetDigital(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::DIGITAL_START) ||
        CController::GetDigital(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::DIGITAL_SELECT) &&
        CController::GetDigital(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::DIGITAL_START))
    {
        m_fTimer += CScreen::TimerStride();
        if (m_fTimer > 1.0f)
        {
            s_bRequested = true;
            m_fTimer = 0.0f;
        };
    }
    else
    {
        m_fTimer -= CScreen::TimerStride();
        if (m_fTimer < 0.0f)
        {
            s_bRequested = false;
            m_fTimer = 0.0f;
        };
    };
};