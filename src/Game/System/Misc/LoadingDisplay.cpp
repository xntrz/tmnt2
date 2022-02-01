#include "LoadingDisplay.hpp"

#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/GameText.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/TextData.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/ProcessList.hpp"


/*static*/ CLoadingDisplay::MESSAGE CLoadingDisplay::m_message;


/*static*/ CProcess* CLoadingDisplay::Instance(void)
{
    return new CLoadingDisplay;
};


/*static*/ void CLoadingDisplay::Initialize(void)
{
    CProcessDispatcher::AttachProcess(PROCESSTYPES::LABEL_LOADINGDISPLAY);
};


/*static*/ void CLoadingDisplay::Terminate(void)
{
    CProcessDispatcher::DetachProcess(PROCESSTYPES::LABEL_LOADINGDISPLAY);
};


/*static*/ void CLoadingDisplay::Start(MODE mode)
{
    m_message.m_type = MESSAGE::TYPE_START;
    m_message.m_mode = mode;

    PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;

    mail.m_type     = PROCESSTYPES::MAIL::TYPE_MSG;
    mail.m_iLabel   = PROCESSTYPES::LABEL_LOADINGDISPLAY;
    mail.m_param    = &m_message;

    CProcessDispatcher::SendMail(mail);
};


/*static*/ void CLoadingDisplay::Stop(void)
{
    m_message.m_type = MESSAGE::TYPE_STOP;

    PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;

    mail.m_type     = PROCESSTYPES::MAIL::TYPE_MSG;
    mail.m_iLabel   = PROCESSTYPES::LABEL_LOADINGDISPLAY;
    mail.m_param    = &m_message;

    CProcessDispatcher::SendMail(mail);
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

    wchar* pwszPhase = nullptr;
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
    
    const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(3));
    CTextData::StrCpy(wszBuffer, pwszText);
    CTextData::StrCat(wszBuffer, pwszPhase);

    CGameFont::m_pFont->SetRGBA(216, 255, 0, 255);
    CGameFont::m_pFont->Show(wszBuffer, 28.0f, -250.0f, 192.0f);
};


void CLoadingDisplay::messageProc(void)
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
            case MESSAGE::TYPE_START:
                m_mode = pMessage->m_mode;
                m_state = STATE_DISPLAY;
                m_fPhase = 0.0f;
                break;

            case MESSAGE::TYPE_STOP:
                m_state = STATE_NONE;
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};