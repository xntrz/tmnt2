#include "Dialog.hpp"

#include "Game/System/2d/MenuSound.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"


CDialog::CDialog(COLOR color /*= COLOR_NORMAL*/,
                 STATUS defaultStatus /*= STATUS_YES*/,
                 int32 iController /*= CController::CONTROLLER_LOCKED_ON_VIRTUAL*/)
: CMessageWindow(color)
, m_status(defaultStatus)
, m_fCursorRotation(0.0f)
, m_iController(iController)
{
    ;
};


CDialog::~CDialog(void)
{
    ;
};


void CDialog::SetSprite(float x, float y, float w, float h)
{
    CMessageWindow::SetSprite(x, y, w, h);
    MoveCursor();	
};


void CDialog::DrawInWindow(const Rt2dBBox& bbox) const
{
    if (m_textTitle.Text())
    {
        Rt2dBBox bboxTitle;
        bboxTitle.x = bbox.x - bbox.w * 0.45f;
        bboxTitle.y = (bbox.h * 0.2f) - bbox.y;
        bboxTitle.w = bbox.w * 0.9f;
        bboxTitle.h = bbox.h * 0.2f;

        Rt2dBBox bboxText;
        bboxText.x = bbox.x - bbox.w * 0.45f;
        bboxText.y = bbox.h * -0.2f - bbox.y;
        bboxText.w = bbox.w * 0.9f;
        bboxText.h = bbox.h * 0.4f;

        m_textTitle.Draw(bboxTitle);
        m_textMsg.Draw(bboxText);
    }
    else if (m_textMsg.Text())
    {
        Rt2dBBox bboxText;
        bboxText.x = bbox.x - bbox.w * 0.45f;
        bboxText.y = bbox.h * -0.2f - bbox.y;
        bboxText.w = bbox.w * 0.9f;
        bboxText.h = bbox.h * 0.6f;

        m_textMsg.Draw(bboxText);
    };

	CGameFont::SetHeight(20.0f);

    const wchar* pwszYes = CGameText::GetText(GAMETEXT_YES);
    if (m_status == STATUS_YES)
        CGameFont::SetRGBA(0, 255, 0, 255);
    else
        CGameFont::SetRGBA(255, 180, 0, 255);

    CGameFont::Show(pwszYes,
                    m_bboxDraw.x - m_bboxDraw.w * 0.15f,
                    m_bboxDraw.h * 0.3f + m_bboxDraw.y);
    
    const wchar* pwszNo = CGameText::GetText(GAMETEXT_NO);
    if (m_status == STATUS_NO)
        CGameFont::SetRGBA(0, 255, 0, 255);
    else
        CGameFont::SetRGBA(255, 180, 0, 255);

    CGameFont::Show(pwszNo,
                    m_bboxDraw.w * 0.15f + m_bboxDraw.x,
                    m_bboxDraw.h * 0.3f + m_bboxDraw.y);

    m_aSprite[3].DrawRotate();
};


void CDialog::Input(void)
{
    RotateCursor();
    MoveCursor();

	int32 status = m_status;

    if (CController::GetDigitalTrigger(m_iController, CController::DIGITAL_LLEFT))
    {
        CMenuSound::PlaySE(CMenuSound::SOUND_ID_SELECT);
		m_status = STATUS(InvClamp(--status, 0, 1));
    }
    else if (CController::GetDigitalTrigger(m_iController, CController::DIGITAL_LRIGHT))
    {
        CMenuSound::PlaySE(CMenuSound::SOUND_ID_SELECT);
		m_status = STATUS(InvClamp(++status, 0, 1));
    }
    else if (CController::GetDigitalTrigger(m_iController, CController::DIGITAL_OK))
    {
        CMenuSound::PlaySE(CMenuSound::SOUND_ID_OK);
        DoneInput();
    }
    else if (CController::GetDigitalTrigger(m_iController, CController::DIGITAL_CANCEL))
    {
        CMenuSound::PlaySE(CMenuSound::SOUND_ID_CANCEL);

        if (m_status == STATUS_NO)
            DoneInput();
        else
            m_status = STATUS_NO;        
    };
};


void CDialog::MoveCursor(void)
{
    RwV2d vPosition = Math::VECTOR2_ZERO;
    
    GetPositionYesNo(&vPosition, m_status);

    m_aSprite[3].Move(vPosition.x - 24.0f, vPosition.y);
    m_aSprite[3].ResizeStrict(28.0f, 28.0f);
};


void CDialog::RotateCursor(void)
{
    m_fCursorRotation += (CScreen::TimerStride() * 4.0f);
    if (m_fCursorRotation >= 360.0f)
        m_fCursorRotation -= 360.0f;
    
    m_aSprite[3].SetRotate(m_fCursorRotation);
};


void CDialog::GetPositionYesNo(RwV2d* pPos, STATUS status) const
{
    ASSERT(pPos);
    
    float fOffset = 0.0f;

    switch (status)
    {
    case STATUS_NO:
        fOffset = 0.15f;
        break;

    case STATUS_YES:
        fOffset = -0.15f;
        break;

    default:
        ASSERT(false);
        break;
    };

    pPos->x = fOffset * m_bboxDraw.w + m_bboxDraw.x;
    pPos->y = m_bboxDraw.h * 0.3f + m_bboxDraw.y;
};


void CDialog::SetStatus(STATUS status)
{
    m_status = status;
};


CDialog::STATUS CDialog::GetStatus(void) const
{
    return m_status;
};


void CDialog::SetController(int32 iController)
{
    m_iController = iController;
};


int32 CDialog::GetController(void) const
{
    return m_iController;
};

