#include "DebugProc.hpp"
#include "System/Common/Controller.hpp"

#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/Sequence.hpp"

#include "rtcharse.h"


class CDebugProcCtrl
{
public:
    CDebugProcCtrl(void){};
    ~CDebugProcCtrl(void){};
    void Move(void){};
    void Draw(void){};

private:
    RtCharset m_pRtCharset;
};


static CDebugProcCtrl* s_pDebugProcCtrl = nullptr;
static bool s_bDebugProcCtrlEnable = false;


/*static*/ CProcess* CDebugProc::Instance(void)
{
    return new CDebugProc;
};


/*static*/ void CDebugProc::Initialize(CProcess* pCurrent)
{
    pCurrent->Mail().Send(PROCESSTYPES::LABEL_DEBUGPROC, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CDebugProc::Terminate(CProcess* pCurrent)
{
    pCurrent->Mail().Send(PROCESSTYPES::LABEL_DEBUGPROC, PROCESSTYPES::MAIL::TYPE_DETACH);
};


CDebugProc::CDebugProc(void)
{
    ;
};


CDebugProc::~CDebugProc(void)
{
    ;
};


bool CDebugProc::Attach(void)
{
    s_pDebugProcCtrl = new CDebugProcCtrl;
	return true;
};


void CDebugProc::Detach(void)
{
    if (s_pDebugProcCtrl)
    {
        delete s_pDebugProcCtrl;
        s_pDebugProcCtrl = nullptr;
    };
};


void CDebugProc::Move(void)
{
    if (CController::GetDigitalTrigger(0, CController::DIGITAL_SELECT) &&
        CController::GetDigitalTrigger(0, CController::DIGITAL_START))
    {
        s_bDebugProcCtrlEnable = !s_bDebugProcCtrlEnable;

        int32 iCurrentSeq = CSequence::GetCurrently();
        CSequence& SeqProc = (CSequence&)Info().Process(iCurrentSeq);        
        SeqProc.Mail().Send(
            SeqProc.Info().Label(),
            s_bDebugProcCtrlEnable ? PROCESSTYPES::MAIL::TYPE_RUN_DISABLE : PROCESSTYPES::MAIL::TYPE_RUN_ENABLE
        );
    };

    if (s_bDebugProcCtrlEnable)
        s_pDebugProcCtrl->Move();
};


void CDebugProc::Draw(void) const
{
    if (s_bDebugProcCtrlEnable)
        s_pDebugProcCtrl->Draw();
};