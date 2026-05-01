#include "Timeout.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Screen.hpp"


class CTimeoutProcess::CTimerObj
{
public:
    CTimerObj(void);
    virtual ~CTimerObj(void);
    virtual void Update(bool bInteractive);
    void SetTime(float fTime);
    void Reset(void);
    bool IsTimeUp(void);

private:
    bool isActive(void);

protected:
    float m_fTargetSec;
    float m_fElapsedTime;
    int32 m_nSuspendCount;
};


class CTimeoutProcess::CNonInputTimer : public CTimerObj
{
private:
    static const int16 ANALOG_RESPONSE_MIN;

public:
    CNonInputTimer(void);
    virtual ~CNonInputTimer(void);
    virtual void Update(bool bInteractive) override;

private:
    bool isInputOccured(void);
};


CTimeoutProcess::CTimerObj::CTimerObj(void)
: m_fTargetSec(0.0f)
, m_fElapsedTime(0.0f)
, m_nSuspendCount(0)
{
    ;
};


CTimeoutProcess::CTimerObj::~CTimerObj(void)
{
    ;
};


void CTimeoutProcess::CTimerObj::Update(bool bInteractive)
{
    if (isActive())
        m_fElapsedTime += CScreen::TimerStride();
};


void CTimeoutProcess::CTimerObj::SetTime(float fTime)
{
    m_fTargetSec = fTime;
};


void CTimeoutProcess::CTimerObj::Reset(void)
{
    m_fElapsedTime = 0.0f;
};


bool CTimeoutProcess::CTimerObj::IsTimeUp(void)
{
    if (isActive())
        return (m_fTargetSec <= m_fElapsedTime);
    
    return false;
};


bool CTimeoutProcess::CTimerObj::isActive(void)
{
    return (m_nSuspendCount <= 0);
};


/*static*/ const int16 CTimeoutProcess::CNonInputTimer::ANALOG_RESPONSE_MIN = 0x3000;


CTimeoutProcess::CNonInputTimer::CNonInputTimer(void)
{
    ;
};


CTimeoutProcess::CNonInputTimer::~CNonInputTimer(void)
{
    ;
};


void CTimeoutProcess::CNonInputTimer::Update(bool bInteractive)
{
    if (!bInteractive || isInputOccured())
        m_fElapsedTime = 0.0f;
    else
        m_fElapsedTime += CScreen::TimerStride();
};


bool CTimeoutProcess::CNonInputTimer::isInputOccured(void)
{
    if (CController::GetDigital(CController::CONTROLLER_LOCKED_ON_VIRTUAL) ||
        CController::GetDigital(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL))
        return true;

    int32 controllerMax = CController::Max();
    for (int32 i = 0; i < controllerMax; ++i)
    {
        if (!CController::IsLocked(i))
            continue;
        
        if (CController::GetState(i) == CController::STATE_CONNECT)
        {
            int16 lx = CController::GetAnalog(i, CController::ANALOG_LSTICK_X);
            int16 ly = CController::GetAnalog(i, CController::ANALOG_LSTICK_Y);

            if ((lx > ANALOG_RESPONSE_MIN) ||
                (lx < -ANALOG_RESPONSE_MIN))
                return true;

            if ((ly > ANALOG_RESPONSE_MIN) ||
                (ly < -ANALOG_RESPONSE_MIN))
                return true;
        };
    };

    return false;
};


/*static*/ const float CTimeoutProcess::TIMEOUT_SEC_NONINPUT = 45.0f;
/*static*/ const float CTimeoutProcess::TIMEOUT_SEC_TOTAL = 300.0f;


/*static*/ CProcess* CTimeoutProcess::Instance(void)
{
    return new CTimeoutProcess;
};


/*static*/ void CTimeoutProcess::Initialize(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_TIMEOUT, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CTimeoutProcess::Terminate(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_TIMEOUT, PROCESSTYPES::MAIL::TYPE_DETACH);
};


/*static*/ void CTimeoutProcess::Start(CProcess* pSender)
{
    static MESSAGE s_message = {};
    
    s_message.m_type = MESSAGE::TYPE_START;
    
    postPrivateMessage(pSender, &s_message);
};


/*static*/ void CTimeoutProcess::Reset(CProcess* pSender)
{
    static MESSAGE s_message = {};

    s_message.m_type = MESSAGE::TYPE_RESET;
    
    postPrivateMessage(pSender, &s_message);
};


/*static*/ void CTimeoutProcess::Enable(CProcess* pSender, bool bEnable)
{
    static MESSAGE s_message = {};

    s_message.m_type = MESSAGE::TYPE_ENABLE;
    s_message.m_param.m_bEnable = bEnable;

    postPrivateMessage(pSender, &s_message);
};


/*static*/ void CTimeoutProcess::SetInteractive(CProcess* pSender, bool bInteractive)
{
    static MESSAGE s_message = {};
    
    s_message.m_type = MESSAGE::TYPE_INTERACTIVE;
    s_message.m_param.m_bInteractive = bInteractive;
    
    postPrivateMessage(pSender, &s_message);

    Enable(pSender, bInteractive);
};


/*static*/ bool CTimeoutProcess::postPrivateMessage(CProcess* pSender, MESSAGE* pMessage)
{
    if (pSender->Info().IsProcessExist(PROCLABEL_TIMEOUT))
        return pSender->Mail().Send(PROCLABEL_TIMEOUT, PROCESSTYPES::MAIL::TYPE_MSG, pMessage);

    return false;
};


CTimeoutProcess::CTimeoutProcess(void)
: m_eState(STATE_IDLE)
, m_iRootSeqLabel(PROCLABEL_SEQ_GAMEMAIN)
, m_apTimer()
, m_nTimeoutTimerNo(-1)
, m_bEnable(false)
, m_bInteractive(false)
{
    ;
};


CTimeoutProcess::~CTimeoutProcess(void)
{
    ;
};


bool CTimeoutProcess::Attach(void)
{
    m_apTimer[0] = new CTimerObj;
    m_apTimer[0]->SetTime(TIMEOUT_SEC_TOTAL);

    m_apTimer[1] = new CNonInputTimer;
    m_apTimer[1]->SetTime(TIMEOUT_SEC_NONINPUT);

    resetObject();
    messageProc();

    return true;
};


void CTimeoutProcess::Detach(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apTimer); ++i)
    {
        if (m_apTimer[i])
        {
            delete m_apTimer[i];
            m_apTimer[i] = nullptr;
        };
    };
};


void CTimeoutProcess::Move(void)
{
    messageProc();

    switch (m_eState)
    {
    case STATE_WATCHTIMER:
        {
            for (int32 i = 0; i < COUNT_OF(m_apTimer); ++i)
            {
                CTimerObj* pTimer = m_apTimer[i];

                if (!pTimer)
                    continue;

                pTimer->Update(CGameData::Attribute().IsInteractive());
                if (pTimer->IsTimeUp())
                {
                    m_nTimeoutTimerNo = i;
                    changeState(STATE_TIMEISUP);
                    break;
                };
            };
        }
        break;

    case STATE_TIMEISUP:
        {
            if (m_bEnable)
                execTimeout();
        }
        break;

    default:
        break;
    };
};


void CTimeoutProcess::Draw(void) const
{
    ;
};


void CTimeoutProcess::execTimeout(void)
{
    int32 iRootSeqLabel = m_iRootSeqLabel;
    resetObject();

    int32 iCurrentSeqLabel = CSequence::GetCurrently();
    CSequence& seq = static_cast<CSequence&>(Info().Process(iCurrentSeqLabel));

    int32 iCallSeqLabel = PROCLABEL_SEQ_GAMEMAIN;
#ifdef _DEBUG
    iCallSeqLabel = PROCLABEL_SEQ_DBGMAIN;
#endif /* _DEBUG */

    seq.Kill(iRootSeqLabel, reinterpret_cast<const void*>(iCallSeqLabel));
};


void CTimeoutProcess::messageProc(void)
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
            case MESSAGE::TYPE_START:
                {
                    if (m_eState == STATE_IDLE)
                        changeState(STATE_WATCHTIMER);
                }
                break;

            case MESSAGE::TYPE_RESET:
                resetObject();
                break;

            case MESSAGE::TYPE_ENABLE:
                m_bEnable = pMessage->m_param.m_bEnable;
                break;

            case MESSAGE::TYPE_INTERACTIVE:
                m_bInteractive = pMessage->m_param.m_bInteractive;
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};


void CTimeoutProcess::resetObject(void)
{
    m_eState = STATE_IDLE;
    m_nTimeoutTimerNo = -1;
    m_bEnable = false;
    m_bInteractive = false;

    for (int32 i = 0; i < COUNT_OF(m_apTimer); ++i)
    {
        if (m_apTimer[i])
            m_apTimer[i]->Reset();
    };
};


void CTimeoutProcess::changeState(STATE eState)
{
    m_eState = eState;
};