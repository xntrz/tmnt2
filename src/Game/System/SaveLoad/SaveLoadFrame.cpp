#include "SaveLoadFrame.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/Component/Menu/Dialog.hpp"
#include "Game/System/2d/MenuSound.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"


CSaveLoadFrame::CSaveLoadFrame(void)
: m_pWindow(nullptr)
, m_fTimer(0.0f)
{
    ;
};


CSaveLoadFrame::~CSaveLoadFrame(void)
{
    ;
};


void CSaveLoadFrame::Start(const wchar* pwszMessage, const wchar* pwszTitle)
{
    m_pWindow = nullptr;
    m_fTimer = 0.0f;

    CSaveLoadFrameBase::Start(pwszMessage, pwszTitle);
};


void CSaveLoadFrame::Stop(void)
{
    CSaveLoadFrameBase::Stop();
    
    DestroyMessageWindow();
};


void CSaveLoadFrame::SetModeMessage(void)
{
    ASSERT(m_mode >= 0);
    ASSERT(m_mode < MODEMAX);

    CSaveLoadFrameBase::SetModeMessage();
    DestroyMessageWindow();
    CreateMessageWindow();
};


void CSaveLoadFrame::SetModeSelect(SELECTITEM defselect)
{
    ASSERT(m_mode >= 0);
    ASSERT(m_mode < MODEMAX);

    if (m_mode != MODE_SELECT)
    {
        CSaveLoadFrameBase::SetModeSelect(defselect);
        DestroyMessageWindow();
        CreateMessageWindow();
    };

    CDialog* pDialog = static_cast<CDialog*>(m_pWindow);
    
    switch (defselect)
    {
    case SELECTITEM_YES:
        pDialog->SetStatus(CDialog::STATUS_YES);
        break;

    case SELECTITEM_NO:
        pDialog->SetStatus(CDialog::STATUS_NO);
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CSaveLoadFrame::SetModeError(PADBTN ConfirmPadbtn, float fConfirmtimeLimit)
{
    ASSERT(ConfirmPadbtn >= 0);
    ASSERT(ConfirmPadbtn < PADBTNMAX);
    ASSERT(fConfirmtimeLimit >= 0.0f);

    if (m_mode != MODE_ERROR)
    {
        CSaveLoadFrameBase::SetModeError(ConfirmPadbtn, fConfirmtimeLimit);
        DestroyMessageWindow();
        CreateMessageWindow();
    };
};


bool CSaveLoadFrame::IsErrorConfirmed(void) const
{
    ASSERT(m_mode >= 0);
    ASSERT(m_mode < MODEMAX);
    ASSERT(m_mode == MODE_ERROR);
    ASSERT(m_pWindow);

    int32 iController = CGameData::Attribute().GetVirtualPad();

	if ((m_ConfirmPadbtn == PADBTN_OK) && CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_OK);
		return true;
	};

	if ((m_ConfirmPadbtn == PADBTN_CANCEL) && CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_CANCEL);
		return true;
	};

	return  (m_fConfirmtimeLimit > 0.0f) && ((CScreen::TimeElapsed() - m_fConfirmtimeStart) >= m_fConfirmtimeLimit);
};


bool CSaveLoadFrame::IsSelected(void) const
{
    ASSERT(m_mode >= 0);
    ASSERT(m_mode < MODEMAX);
    ASSERT(m_mode == MODE_SELECT);
    ASSERT(m_pWindow);

    return (m_pWindow && !m_pWindow->IsOpen());
};


CSaveLoadFrame::SELECTITEM CSaveLoadFrame::GetSelectItem(void) const
{
    ASSERT(m_mode >= 0);
    ASSERT(m_mode < MODEMAX);
    ASSERT(m_mode == MODE_SELECT);
    ASSERT(m_pWindow);

    CDialog* pDialog = static_cast<CDialog*>(m_pWindow);
    
    switch (pDialog->GetStatus())
    {
    case CDialog::STATUS_YES:
        return SELECTITEM_YES;

    case CDialog::STATUS_NO:
        return SELECTITEM_NO;

    default:
        ASSERT(false);
        return SELECTITEM_ERROR;
    };
};


void CSaveLoadFrame::SetTitle(const wchar* pwszTitle)
{
    CSaveLoadFrameBase::SetTitle(pwszTitle);

    if (m_pWindow)
    {
        ASSERT(m_pwszTitle);
        m_pWindow->SetTitle(m_pwszTitle);
    };
};


void CSaveLoadFrame::RemoveTitle(void)
{
    CSaveLoadFrameBase::RemoveTitle();

    if (m_pWindow)
        m_pWindow->SetTitle(nullptr);
};


void CSaveLoadFrame::SetMessage(const wchar* pwszMsg)
{
    CSaveLoadFrameBase::SetMessage(pwszMsg);

    if (m_pWindow)
    {
        ASSERT(m_pwszMessage);
        m_pWindow->SetText(m_pwszMessage);
    };
};


void CSaveLoadFrame::RemoveMessage(void)
{
    CSaveLoadFrameBase::RemoveMessage();

    if (m_pWindow)
        m_pWindow->SetText(nullptr);
};


void CSaveLoadFrame::CreateMessageWindow(void)
{
    ASSERT(!m_pWindow);
    ASSERT(m_mode >= 0);
    ASSERT(m_mode < MODEMAX);

    switch (m_mode)
    {
	case MODE_ERROR:
    case MODE_MESSAGE:
        m_pWindow = new CMessageWindow(CMessageWindow::COLOR_NORMAL);
        break;

    case MODE_SELECT:
        m_pWindow = new CDialog(CDialog::COLOR_NORMAL, CDialog::STATUS_YES, CGameData::Attribute().GetVirtualPad());
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_pWindow)
    {
        ASSERT(m_pwszMessage);
        ASSERT(m_pwszTitle);

        m_pWindow->SetOpenAction(false);
        m_pWindow->SetTitle(m_pwszTitle, 16.0f, { 0xFF, 0xFF, 0xFF, 0xFF });
        m_pWindow->SetText(m_pwszMessage, 16.0f, { 0xFF, 0xFF, 0xFF, 0xFF });
        m_pWindow->Set(
            0.0f,
            0.0f,
            CSprite::m_fVirtualScreenW,
            CSprite::m_fVirtualScreenH * 0.75f
        );

        m_pWindow->Open();

        ASSERT(m_pWindow->IsOpen());
    };
};


void CSaveLoadFrame::DestroyMessageWindow(void)
{
    if (m_pWindow)
    {
        m_pWindow->InputAction();
        
        delete m_pWindow;
        m_pWindow = nullptr;
    };
};
