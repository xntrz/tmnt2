#include "PreArcade.hpp"

#ifdef TMNT2_BUILD_EU

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/SystemText.hpp"


/*static*/ CProcess* CPreArcadeSequence::Instance(void)
{
    return new CPreArcadeSequence;
};


bool CPreArcadeSequence::OnAttach(const void* pParam)
{
    m_step = 0;
    m_fTimer = 0.0f;

    return true;
};


void CPreArcadeSequence::OnDetach(void)
{
    ;
};


void CPreArcadeSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_step)
    {
    case STEP_RET:
        {
            if (bRet)
            {
                if (CConfigure::GetTVMode() != TYPEDEF::CONFIG_TV_PAL)
                {
                    CScreenFade::BlackIn(0.5f);
                    m_step = STEP_FADE_IN;
                }
                else
                {
                    m_step = STEP_EXIT;
                };
            }
            else
            {
                if (CGameData::Option().Play().IsAutosaveEnabled())
                    Call(PROCLABEL_SEQ_SAVELOADAUTO);
                else
                    Call(PROCLABEL_SEQ_SAVELOADMENUSAVE);
            };
        }
        break;
        
    case STEP_FADE_IN:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_DISP_WARNING;
        }
        break;
        
    case STEP_DISP_WARNING:
        {
            m_fTimer += CScreen::TimerStride();
            if ((m_fTimer > 4.0f) || CController::GetDigitalTrigger(CController::CONTROLLER_LOCKED_ON_VIRTUAL))
            {
                CScreenFade::BlackOut(0.5f);
                m_step = STEP_FADE_OUT;
            };
        }
        break;
        
    case STEP_FADE_OUT:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_EXIT;
        }
        break;
        
    case STEP_EXIT:
        {
            Ret();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CPreArcadeSequence::OnDraw(void) const
{
    switch (m_step)
    {
    case STEP_FADE_IN:
    case STEP_DISP_WARNING:
    case STEP_FADE_OUT:
    case STEP_EXIT:
        {
            CGameFont::SetRGBA({ 0xFF, 0xFF, 0, 0xFF });
            CGameFont::SetHeight(12.0f);

            /* draw title */
            Rt2dBBox bboxTitle = { -270.0f, 110.0f, 540.0f, 30.0f };
            const wchar* pwszTitle = CSystemText::GetText(SYSTEXT(9));

            CGameFont::Flow(pwszTitle, &bboxTitle, rt2dJUSTIFYCENTER);
            
            /* draw msg */
            Rt2dBBox bboxMsg = { -270.0f, -140.0f, 540.0f, 250.0f };
            const wchar* pwszMsg = CSystemText::GetText(SYSTEXT(10));

            CGameFont::Flow(pwszMsg, &bboxMsg, rt2dJUSTIFYLEFT);
        }
        break;

    default:
        break;
    };
};

#endif /* TMNT2_BUILD_EU */