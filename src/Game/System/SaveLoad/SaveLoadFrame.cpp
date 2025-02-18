#include "SaveLoadFrame.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/Component/Menu/Dialog.hpp"
#include "Game/System/2d/MenuSound.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"


class CSaveLoadMessageWindow : public CMessageWindow
{
public:
    inline CSaveLoadMessageWindow(void) : CMessageWindow(COLOR_NORMAL) {};
    inline virtual ~CSaveLoadMessageWindow(void) {};

    virtual void DrawInWindow(const Rt2dBBox& bbox) const override;
};


class CSaveLoadSelectWindow : public CDialog    
{
public:
    inline CSaveLoadSelectWindow(void) : CDialog(COLOR_NORMAL, STATUS_NO, CGameData::Attribute().GetVirtualPad()) {};
    inline virtual ~CSaveLoadSelectWindow(void) {};

    virtual void DrawInWindow(const Rt2dBBox& bbox) const override;
};


//
// *********************************************************************************
//


/*virtual*/ void CSaveLoadMessageWindow::DrawInWindow(const Rt2dBBox& bbox) const /*override*/
{
    if (m_textTitle.Text())
    {
        Rt2dBBox bboxTitle;
        bboxTitle.x = bbox.x - bbox.w * 0.44f;
        bboxTitle.y = (bbox.h * 0.2f) - bbox.y;
        bboxTitle.w = bbox.w * 0.88f;
        bboxTitle.h = bbox.h * 0.2f;

        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.44f;
        bboxMsg.y = bbox.h * -0.4f - bbox.y;
        bboxMsg.w = bbox.w * 0.88f;
        bboxMsg.h = bbox.h * 0.6f;

        m_textTitle.Draw(bboxTitle);
        m_textMsg.Draw(bboxMsg);
    }
    else if (m_textMsg.Text())
    {
        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.44f;
        bboxMsg.y = bbox.h * -0.4f - bbox.y;
        bboxMsg.w = bbox.w * 0.88f;
        bboxMsg.h = bbox.h * 0.8f;

        m_textMsg.Draw(bboxMsg);
    };
};


//
// *********************************************************************************
//


/*virtual*/ void CSaveLoadSelectWindow::DrawInWindow(const Rt2dBBox& bbox) const /*override*/
{
    if (m_textTitle.Text())
    {
        Rt2dBBox bboxTitle;
        bboxTitle.x = bbox.x - bbox.w * 0.44f;
        bboxTitle.y = (bbox.h * 0.2f) - bbox.y;
        bboxTitle.w = bbox.w * 0.88f;
        bboxTitle.h = bbox.h * 0.2f;

        Rt2dBBox bboxText;
        bboxText.x = bbox.x - bbox.w * 0.44f;
        bboxText.y = bbox.h * -0.2f - bbox.y;
        bboxText.w = bbox.w * 0.88f;
        bboxText.h = bbox.h * 0.4f;

        m_textTitle.Draw(bboxTitle);
        m_textMsg.Draw(bboxText);
    }
    else if (m_textMsg.Text())
    {
        Rt2dBBox bboxText;
        bboxText.x = bbox.x - bbox.w * 0.44f;
        bboxText.y = bbox.h * -0.2f - bbox.y;
        bboxText.w = bbox.w * 0.88f;
        bboxText.h = bbox.h * 0.6f;

        m_textMsg.Draw(bboxText);
    };

	CGameFont::SetHeight(20.0f);

    /* yes btn */
    const wchar* pwszYes = CGameText::GetText(GAMETEXT_YES);
    if (m_status == STATUS_YES)
        CGameFont::SetRGBA(0, 255, 0, 255);
    else
        CGameFont::SetRGBA(255, 180, 0, 255);

    RwV2d vecPosYes = Math::VECTOR2_ZERO;
    GetPositionYesNo(&vecPosYes, STATUS_YES);

    CGameFont::Show(pwszYes, &vecPosYes);

    /* no btn */
    const wchar* pwszNo = CGameText::GetText(GAMETEXT_NO);
    if (m_status == STATUS_NO)
        CGameFont::SetRGBA(0, 255, 0, 255);
    else
        CGameFont::SetRGBA(255, 180, 0, 255);

    RwV2d vecPosNo = Math::VECTOR2_ZERO;
    GetPositionYesNo(&vecPosNo, STATUS_NO);

    CGameFont::Show(pwszNo, &vecPosNo);

    /* cursor */
    m_aSprite[3].DrawRotate();
};


//
// *********************************************************************************
//


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
        m_pWindow = new CSaveLoadMessageWindow;
        break;

    case MODE_SELECT:
        m_pWindow = new CSaveLoadSelectWindow;
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
        m_pWindow->Set(0.0f,
                       0.0f,
                       CSprite::m_fVirtualScreenW,
                       CSprite::m_fVirtualScreenH * 0.75f);

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
