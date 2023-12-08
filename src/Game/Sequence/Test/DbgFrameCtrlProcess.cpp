#include "DbgFrameCtrlProcess.hpp"

#ifdef _DEBUG

#include "Game/System/Misc/Gamepad.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Process/ProcessMail.hpp"


/*static*/ CProcess* CDbgFrameCtrlProcess::Instance(void)
{
    return new CDbgFrameCtrlProcess;
};


/*static*/ void CDbgFrameCtrlProcess::Initialize(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_DBGFRAMECTRL, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CDbgFrameCtrlProcess::Terminate(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_DBGFRAMECTRL, PROCESSTYPES::MAIL::TYPE_DETACH);
};


bool CDbgFrameCtrlProcess::Attach(void)
{
    m_bEnable = false;
    m_bFrameProcessing = false;
    
    return true;
};


void CDbgFrameCtrlProcess::Move(void)
{
    //
    //  check if sequence processed prev frame sleep it again
    //
    if (m_bFrameProcessing)
    {
        ASSERT(m_bEnable);
        
        m_bFrameProcessing = false;
		Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_DISABLE);
		return;
	};

    //
    //  get input
    //
    uint32 Digital = 0;
    Digital |= IPad::GetDigital(IPad::CONTROLLER_LOCKED_ON_VIRTUAL);
    Digital |= IPad::GetDigital(IPad::CONTROLLER_UNLOCKED_ON_VIRTUAL);
    
    uint32 DigitalT = 0;
    DigitalT |= IPad::GetDigitalTrigger(IPad::CONTROLLER_LOCKED_ON_VIRTUAL);
    DigitalT |= IPad::GetDigitalTrigger(IPad::CONTROLLER_UNLOCKED_ON_VIRTUAL);

    //
    //  check for enable 
    //
    if ((Digital & IPad::DIGITAL_START) && (DigitalT & IPad::DIGITAL_L3) && !m_bFrameProcessing)
    {
		m_bEnable = !m_bEnable;
        Mail().Send(CSequence::GetCurrently(), (m_bEnable ? PROCESSTYPES::MAIL::TYPE_MOVE_DISABLE : PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE));
    };

    //
    //  check for frame step
    //
    if ((DigitalT & IPad::DIGITAL_LRIGHT) && m_bEnable)
    {       
		m_bFrameProcessing = true;
		Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE);     
    };
};

#endif /* _DEBUG */