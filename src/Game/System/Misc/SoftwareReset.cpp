#include "SoftwareReset.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/Sequence.hpp"


/*static*/ int32 CSoftwareResetProcess::m_iRefCount = 0;
/*static*/ bool CSoftwareResetProcess::m_bEnable = false;
/*static*/ float CSoftwareResetProcess::m_fResetResponseSec = 0.0f;


/*static*/ CProcess* CSoftwareResetProcess::Instance(void)
{
    return new CSoftwareResetProcess;
};


/*static*/ bool CSoftwareResetProcess::Initialize(CProcess* pSender, MODE mode)
{
    if (m_iRefCount)
        return true;

    bool bResult = pSender->Mail().Send(PROCLABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_ATTACH);
    if (bResult)
    {
        static MESSAGE s_message = {};

        s_message.m_type = MESSAGE::TYPE_MODE;
        s_message.m_param.m_mode = mode;

        bResult = pSender->Mail().Send(PROCLABEL_SOFTWARERESET,
                                       PROCESSTYPES::MAIL::TYPE_MSG,
                                       &s_message);

        if (bResult)
            ++m_iRefCount;
    };

    return bResult;
};


/*static*/ void CSoftwareResetProcess::Terminate(CProcess* pSender)
{
    if (m_iRefCount > 0)
    {
        if (!--m_iRefCount)
            pSender->Mail().Send(PROCLABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_DETACH);
    };
};


/*static*/ void CSoftwareResetProcess::SetEnable(CProcess* pSender, bool bState)
{
    m_bEnable = bState;
};


/*static*/ void CSoftwareResetProcess::SetResponse(CProcess* pSender, float fResponse)
{
    static MESSAGE s_message = {};

    s_message.m_type = MESSAGE::TYPE_RESPOSNE;
    s_message.m_param.m_fResponse = fResponse;

    pSender->Mail().Send(PROCLABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_MSG, &s_message);
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

    bool bPress = false;

    if (m_bEnable && CGameData::Attribute().IsInteractive())
    {
        int32 iController = CGameData::Attribute().GetVirtualPad();

        if (m_mode == MODE_DEBUGMENU)
            iController = CController::CONTROLLER_UNLOCKED_ON_VIRTUAL;

        if (CController::GetDigital(iController, CController::DIGITAL_SELECT) &&
            CController::GetDigital(iController, CController::DIGITAL_START))
        {
            bPress = true;
        };
    };

    if (bPress)
    {
        m_fKeyTimer += CScreen::TimerStride();
        if (m_fKeyTimer >= m_fResetResponseSec)
            execReset();
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
    int32 iRootSeqLabel = m_iRootSeqLabel;
    clear(m_mode);

    int32 iCurrentSeqLabel = CSequence::GetCurrently();
    CSequence& seq = static_cast<CSequence&>(Info().Process(iCurrentSeqLabel));

    int32 iCallSeqLabel = PROCLABEL_SEQ_GAMEMAIN;
#ifdef _DEBUG
    iCallSeqLabel = PROCLABEL_SEQ_DBGMAIN;
#endif /* _DEBUG */
    
    seq.Kill(iRootSeqLabel, reinterpret_cast<const void*>(iCallSeqLabel));
};


void CSoftwareResetProcess::messageProc(void)
{
    PROCESSTYPES::MAIL mail;

    while (Mail().Recv(mail))
    {
        if (mail.m_type == PROCESSTYPES::MAIL::TYPE_MSG)
        {
            const MESSAGE* pMessage =
                reinterpret_cast<const MESSAGE*>(mail.m_param);
            
            ASSERT(pMessage);

            switch (pMessage->m_type)
            {
            case MESSAGE::TYPE_MODE:
                m_mode = pMessage->m_param.m_mode;
                break;

            case MESSAGE::TYPE_ENABLE:
                m_bEnable = pMessage->m_param.m_bEnable;
                break;

            case MESSAGE::TYPE_ROOTSEQ:
                m_iRootSeqLabel = pMessage->m_param.m_iRootSeqLabel;
                break;

            case MESSAGE::TYPE_RESPOSNE:
                m_fResetResponseSec = pMessage->m_param.m_fResponse;
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
    m_mode = mode;
    m_bEnable = false;
    m_fKeyTimer = 0.0f;
    m_iRootSeqLabel = PROCESSTYPES::LABEL_TOP;
};