#include "SoftwareReset.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/Sequence.hpp"


/*static*/ bool CSoftwareReset::m_bEnable = false;


/*static*/ CProcess* CSoftwareReset::Instance(void)
{
    return new CSoftwareReset;
};


/*static*/ bool CSoftwareReset::Initialize(CProcess* pCurrent)
{
    return pCurrent->Mail().Send(PROCESSTYPES::LABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CSoftwareReset::Terminate(CProcess* pCurrent)
{
    pCurrent->Mail().Send(PROCESSTYPES::LABEL_SOFTWARERESET, PROCESSTYPES::MAIL::TYPE_DETACH);
};


CSoftwareReset::CSoftwareReset(void)
: m_fKeyTimer(0.0f)
, m_iRootSeqLabel(0)
, m_mode(MODE_NORMAL)
{
    ;
};


CSoftwareReset::~CSoftwareReset(void)
{
    ;
};


bool CSoftwareReset::Attach(void)
{
    clear();
    messageProc();
	return true;
};


void CSoftwareReset::Detach(void)
{
    ;
};


void CSoftwareReset::Move(void)
{
    messageProc();

    if (m_bEnable && CGameData::Attribute().IsInteractive())
    {
        int32 iController = CGameData::Attribute().GetVirtualPad();
        if (m_mode == MODE_DEBUGMENU)
            iController = CController::CONTROLLER_UNLOCKED_ON_VIRTUAL;

        if (CController::GetDigital(iController, CController::DIGITAL_LEFT_BUMPER) &&
            CController::GetDigital(iController, CController::DIGITAL_RIGHT_BUMPER))
        {
            m_fKeyTimer += CScreen::TimerStride();
            if (m_fKeyTimer >= 0.0f)
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


void CSoftwareReset::Draw(void) const
{
    ;
};


void CSoftwareReset::execReset(void)
{
    clear();
    
    int32 iCurrentSeq = CSequence::GetCurrently();
    CSequence& SeqProc = (CSequence&)Info().Process(iCurrentSeq);
    SeqProc.Kill(m_iRootSeqLabel);
};


void CSoftwareReset::messageProc(void)
{
    PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;

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


void CSoftwareReset::clear(void)
{
    m_bEnable = false;
    m_mode = MODE_NORMAL;
    m_fKeyTimer = 0.0f;
    m_iRootSeqLabel = 0;
};