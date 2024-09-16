#include "LoadingDisplay.hpp"

#include "Game/ProcessList.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/TextData.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/ProcessMail.hpp"


/*static*/ CProcess* CLoadingDisplay::Instance(void)
{
    return new CLoadingDisplay;
};


/*static*/ bool CLoadingDisplay::Initialize(CProcess* pCurrent)
{
    return pCurrent->Mail().Send(PROCLABEL_LOADDISP, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CLoadingDisplay::Terminate(CProcess* pCurrent)
{
    pCurrent->Mail().Send(PROCLABEL_LOADDISP, PROCESSTYPES::MAIL::TYPE_DETACH);
};


/*static*/ bool CLoadingDisplay::Start(CProcess* pSender, MODE mode)
{
    static MESSAGE s_message;

    s_message.m_type = MESSAGE::TYPE_START;
    s_message.m_mode = mode;

    return postPrivateMessage(pSender, &s_message);
};


/*static*/ void CLoadingDisplay::Stop(CProcess* pSender)
{
    static MESSAGE s_message;
    
    s_message.m_type = MESSAGE::TYPE_STOP;

    postPrivateMessage(pSender, &s_message);
};


/*static*/ bool CLoadingDisplay::postPrivateMessage(CProcess* pSender, MESSAGE* pMessage)
{
    bool bResult = false;
    
    if (pSender->Info().IsProcessExist(PROCLABEL_LOADDISP))
        bResult = pSender->Mail().Send(PROCLABEL_LOADDISP, PROCESSTYPES::MAIL::TYPE_MSG, pMessage);
    
    return bResult;
};


CLoadingDisplay::CLoadingDisplay(void)
: m_mode(MODE_NORMAL)
, m_state(STATE_NONE)
, m_fPhase(0.0f)
{
    ;
};


CLoadingDisplay::~CLoadingDisplay(void)
{
    ;
};


bool CLoadingDisplay::Attach(void)
{
    ASSERT(Info().State() == PROCESSTYPES::STATE_START);

    m_mode = MODE_NORMAL;
    m_state = STATE_NONE;
    m_fPhase = 0.0f;

    messageProc();
    
    return true;
};


void CLoadingDisplay::Detach(void)
{
    ;
};


void CLoadingDisplay::Move(void)
{
    messageProc();

    if (m_state == STATE_DISPLAY)
    {
        m_fPhase += (CScreen::TimerStride() * Math::PI);
        if (m_fPhase >= Math::PI2)
            m_fPhase -= Math::PI2;
    };
};


void CLoadingDisplay::Draw(void) const
{
    if (m_state != STATE_DISPLAY)
        return;

    const wchar* pwszPhase = nullptr;
    wchar wszBuffer[32];
    
    wszBuffer[0] = UTEXT('\0');

    if (m_fPhase >= (Math::PI * 1.5f))
        pwszPhase = UTEXT("...");
    else if (m_fPhase >= Math::PI)
        pwszPhase = UTEXT("..");
    else if (m_fPhase >= (Math::PI * 0.5f))
        pwszPhase = UTEXT(".");
    else
        pwszPhase = UTEXT("");

    ASSERT(pwszPhase);
    
    const wchar* pwszText = CGameText::GetText(GAMETEXT(3));
    CTextData::StrCpy(wszBuffer, pwszText);
    CTextData::StrCat(wszBuffer, pwszPhase);

	CGameFont::SetHeight(28.0f);
    CGameFont::SetRGBA(216, 255, 0, 255);
    CGameFont::Show(wszBuffer, -250.0f, 192.0f);
};


void CLoadingDisplay::messageProc(void)
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
            case MESSAGE::TYPE_START:
                {
                    m_mode = pMessage->m_mode;
                    m_state = STATE_DISPLAY;
                    m_fPhase = 0.0f;
                }
                break;

            case MESSAGE::TYPE_STOP:
                {
                    m_state = STATE_NONE;
                }                
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};