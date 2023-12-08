#include "SoftwareReset.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/Sequence.hpp"


/*static*/ bool CSoftwareResetProcess::m_bEnable = false;
/*static*/ float CSoftwareResetProcess::m_fResetResponseSec = 0.0f;


/*static*/ CProcess* CSoftwareResetProcess::Instance(void)
{
    return new CSoftwareResetProcess;
};


/*static*/ bool CSoftwareResetProcess::Initialize(CProcess* pSender, MODE mode)
{
    bool bResult = pSender->Mail().Send(PROCLABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_ATTACH);
    if (bResult)
    {
        static MESSAGE s_message = {};

        s_message.m_type = MESSAGE::TYPE_MODE;
        s_message.m_param.m_mode = mode;

        bResult = postPrivateMessage(pSender, &s_message);
    };

    return bResult;
};


/*static*/ void CSoftwareResetProcess::Terminate(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_DETACH);
};


/*static*/ void CSoftwareResetProcess::SetEnable(CProcess* pSender, bool bState)
{
    m_bEnable = bState;
};


/*static*/ bool CSoftwareResetProcess::postPrivateMessage(CProcess* pSender, MESSAGE* pMsg)
{
	return pSender->Mail().Send(PROCLABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_MSG, pMsg);
};


CSoftwareResetProcess::CSoftwareResetProcess(void)
: m_fKeyTimer(0.0f)
, m_iRootSeqLabel(PROCESSTYPES::LABEL_TOP)
, m_mode(MODE_NORMAL)
{
    ;
};


CSoftwareResetProcess::~CSoftwareResetProcess(void)
{
    ;
};


bool CSoftwareResetProcess::Attach(void)
{
    clear(MODE_NORMAL);
    messageProc();
	return true;
};


void CSoftwareResetProcess::Detach(void)
{
    ;
};


void CSoftwareResetProcess::Move(void)
{
    messageProc();

    if (m_bEnable && CGameData::Attribute().IsInteractive())
    {
        int32 iController = CGameData::Attribute().GetVirtualPad();
        if (m_mode == MODE_DEBUGMENU)
            iController = CController::CONTROLLER_UNLOCKED_ON_VIRTUAL;

        if (CController::GetDigital(iController, CController::DIGITAL_SELECT) &&
            CController::GetDigital(iController, CController::DIGITAL_START))
        {
            m_fKeyTimer += CScreen::TimerStride();
            if (m_fKeyTimer >= m_fResetResponseSec)
                execReset();
        }
        else
        {
            m_fKeyTimer = 0.0f;
        };
    }
    else
    {
        m_fKeyTimer = 0.0f;
    };
};


void CSoftwareResetProcess::Draw(void) const
{
    ;
};


void CSoftwareResetProcess::execReset(void)
{
    clear(m_mode);

    int32 iCurrentSeq = CSequence::GetCurrently();
    CSequence& SeqProc = (CSequence&)Info().Process(iCurrentSeq);
    SeqProc.Kill(m_iRootSeqLabel); 
};


void CSoftwareResetProcess::messageProc(void)
{
    PROCESSTYPES::MAIL mail;

    while (Mail().Recv(mail))
    {
        if (mail.m_type == PROCESSTYPES::MAIL::TYPE_MSG)
        {
            MESSAGE* pMessage = (MESSAGE*)mail.m_param;
            ASSERT(pMessage);

            switch (pMessage->m_type)
            {
            case MESSAGE::TYPE_MODE:
                {
                    ASSERT((pMessage->m_param.m_mode == MODE_NORMAL) || (pMessage->m_param.m_mode == MODE_DEBUGMENU));
                    m_mode = pMessage->m_param.m_mode;
                }
                break;

            case MESSAGE::TYPE_ENABLE:
                {
                    m_bEnable = pMessage->m_param.m_bEnable;
                }
                break;

            case MESSAGE::TYPE_ROOTSEQ:
                {
                    m_iRootSeqLabel = pMessage->m_param.m_iRootSeqLabel;
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};


void CSoftwareResetProcess::clear(MODE mode)
{
#ifdef _DEBUG
    m_mode = mode;
#else
    m_mode = MODE_NORMAL;
#endif    
    m_bEnable = false;
    m_fKeyTimer = 0.0f;
#ifdef _DEBUG    
    m_iRootSeqLabel = (m_mode == MODE_DEBUGMENU ? PROCLABEL_SEQ_DBGMENU : PROCLABEL_SEQ_GAMEMAIN);
#else
    m_iRootSeqLabel = PROCLABEL_SEQ_GAMEMAIN;
#endif    
};