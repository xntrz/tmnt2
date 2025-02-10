#include "SaveLoadFrame.hpp"

#include "System/Common/TextData.hpp"
#include "System/Common/Screen.hpp"


CSaveLoadFrameBase::CSaveLoadFrameBase(void)
: m_mode(MODE_MESSAGE)
, m_defaultSelect(SELECTITEM_NO)
, m_pwszTitle(nullptr)
, m_pwszMessage(nullptr)
{
    ;
};


CSaveLoadFrameBase::~CSaveLoadFrameBase(void)
{
    ASSERT(!m_pwszTitle);
    ASSERT(!m_pwszMessage);
};


void CSaveLoadFrameBase::Start(const wchar* pwszMessage, const wchar* pwszTitle)
{
    m_mode = MODE_MESSAGE;
    m_defaultSelect = SELECTITEM_YES;
    m_pwszTitle = nullptr;
    m_pwszMessage = nullptr;

    SetTitle(pwszTitle);
    SetMessage(pwszMessage);
    SetModeMessage();
};


void CSaveLoadFrameBase::Stop(void)
{
    CScreen::ResetClearColor();
    RemoveMessage();
    RemoveTitle();
};


void CSaveLoadFrameBase::SetModeMessage(void)
{
    m_mode = MODE_MESSAGE;
};


void CSaveLoadFrameBase::SetModeSelect(SELECTITEM defselect)
{
    ASSERT(defselect >= 0);
    ASSERT(defselect < SELECTITEMMAX);

    m_mode = MODE_SELECT;
    m_defaultSelect = defselect;
};


void CSaveLoadFrameBase::SetModeError(PADBTN ConfirmPadbtn, float fConfirmtimeLimit)
{
    m_mode = MODE_ERROR;
    m_ConfirmPadbtn = ConfirmPadbtn;
    m_fConfirmtimeStart = CScreen::TimeElapsed();
    m_fConfirmtimeLimit = fConfirmtimeLimit;
};


void CSaveLoadFrameBase::SetTitle(const wchar* pwszTitle)
{
    ASSERT(pwszTitle);

    RemoveTitle();
    ASSERT(!m_pwszTitle);

    m_pwszTitle = new wchar[CTextData::StrLen(pwszTitle) + 1];
  
    CTextData::StrCpy(m_pwszTitle, pwszTitle);
};


void CSaveLoadFrameBase::RemoveTitle(void)
{
    if (m_pwszTitle)
    {
        delete[] m_pwszTitle;
        m_pwszTitle = nullptr;
    };
};


void CSaveLoadFrameBase::SetMessage(const wchar* pwszMsg)
{
    ASSERT(pwszMsg);
    
    RemoveMessage();
    ASSERT(!m_pwszMessage);

    m_pwszMessage = new wchar[CTextData::StrLen(pwszMsg) + 1];

    CTextData::StrCpy(m_pwszMessage, pwszMsg);
};


void CSaveLoadFrameBase::RemoveMessage(void)
{
    if (m_pwszMessage)
    {
        delete[] m_pwszMessage;
        m_pwszMessage = nullptr;
    };
};

