#include "NexusRetrySequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Menu/Dialog.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Misc/ScreenFade.hpp"


/*static*/ CProcess* CNexusRetrySequence::Instance(void)
{
    return new CNexusRetrySequence;
};


CNexusRetrySequence::CNexusRetrySequence(void)
: m_step(STEP_START)
, m_pDlg(nullptr)
, m_bRetryFlag(false)
{
    ;
};


CNexusRetrySequence::~CNexusRetrySequence(void)
{
    ASSERT(!m_pDlg);
};


bool CNexusRetrySequence::OnAttach(const void* pParam)
{
    m_step = STEP_START;
    m_bRetryFlag = false;

    m_pDlg = new CDialog;
    m_pDlg->Set(0.0f, 0.0f, 640.0f, 128.0f);
    m_pDlg->SetStatus(CDialog::STATUS_NO);
    m_pDlg->SetOpenAction(false);
    m_pDlg->SetText(CGameText::GetText(GAMETEXT_GG_TOURNEY_RETRY),
                    16.0f,
                    { 0xFF, 0xFF, 0xFF, 0xFF });
    
    CScreenFade::BlackOut(0.0f);

    return true;
};


void CNexusRetrySequence::OnDetach(void)
{
    CGameData::Attribute().SetInteractive(false);
    CScreenFade::BlackIn(0.0f);
    
    if (m_pDlg)
    {
        delete m_pDlg;
        m_pDlg = nullptr;
    };
};


void CNexusRetrySequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_step)
    {
    case STEP_START:
        {
            CScreenFade::BlackIn();
            m_pDlg->Open();
            m_step = STEP_FADEOUT;
        }
        break;

    case STEP_FADEOUT:
        {
            if (!CScreenFade::IsFading())
            {
                CGameData::Attribute().SetInteractive(true);
                m_step = STEP_RUN;
            };
        }
        break;

    case STEP_RUN:
        {
            if (!m_pDlg->IsOpen())
            {
                m_bRetryFlag = (m_pDlg->GetStatus() == CDialog::STATUS_YES ? true : false);
                m_step = STEP_FADEIN;
                CScreenFade::BlackOut();
            };
        }
        break;

    case STEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
            {
                CGameData::Attribute().SetInteractive(false);
                m_step = STEP_EOL;
            };
        }
        break;

    case STEP_EOL:
        {
            Ret(reinterpret_cast<const void*>(m_bRetryFlag));
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CNexusRetrySequence::OnDraw(void) const
{
    ;
};