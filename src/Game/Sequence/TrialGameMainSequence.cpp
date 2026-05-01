#include "TrialGameMainSequence.hpp"

#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/SoftwareReset.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Misc/PadConnectCheck.hpp"
#include "Game/System/Misc/TouchController.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Screen.hpp"

#if defined(TARGET_WEB)
#include "System/Web/WebSpecific.hpp"
#include "System/Web/File/WebFile.hpp"
#endif /* defined(TARGET_WEB) */


/*static*/ CProcess* CTrialGameMainSequence::Instance(void)
{
    return new CTrialGameMainSequence;
};


CTrialGameMainSequence::CTrialGameMainSequence(void)
{
    ;
};


CTrialGameMainSequence::~CTrialGameMainSequence(void)
{
    ;
};


bool CTrialGameMainSequence::OnAttach(const void* pParam)
{
    bool bResult = CGameMainSequence::OnAttach(pParam);
    if (bResult)
    {
        CGameData::Record().Item().SetItemTaken(ITEMID::ID_DON_LASER);

        CTimeoutProcess::Initialize(this);
    };

    return bResult;
};


void CTrialGameMainSequence::OnDetach(void)
{
    CTimeoutProcess::Terminate(this);

    CGameMainSequence::OnDetach();
};


void CTrialGameMainSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
    {
        CTimeoutProcess::Reset(this);
        
        if (pReturnValue)
        {
            m_iLabelNext = reinterpret_cast<int32>(pReturnValue);
            if (m_iLabelNext == PROCESSTYPES::LABEL_EOL)
            {
                Kill(PROCESSTYPES::LABEL_EOL);
                return;
            }
            else if (m_iLabelNext == PROCLABEL_SEQ_SAVELOADCHECK)
            {
                m_iLabelNext = PROCLABEL_LOADDISP;
            };
        }
        else
        {
            m_iLabelNext = Branch(m_iLabelNext);
            if (m_iLabelNext == PROCESSTYPES::LABEL_SEQ_PREV)
                m_iLabelNext = m_iLabelPrev;
        };        

        m_iLabelPrev = m_iLabelCurrent;
        m_iLabelCurrent = m_iLabelNext;
        
        Call(m_iLabelCurrent, m_param);
        m_param = nullptr;
    }
    else
    {
        switch (m_step)
        {
        case STEP_LOAD_TEXTURE:
            {
                CDataLoader::Period();
                if (!CDataLoader::IsLoadEnd())
                    break;

                CMessageWindow::Load();                
                CGameSound::LoadWave(1);
                
                m_step = STEP_LOAD_SOUND;
#ifdef TARGET_PS2
                CPadConnectCheckProcess::Initialize(this);
#endif /* TARGET_PS2 */

#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
                if (CWebSpecific::IsMobilePlatform())
                    CTouchControllerProcess::Initialize(this);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */
            }
            break;

        case STEP_LOAD_SOUND:
            {
                if (!CGameSound::IsLoadEnd())
                    break;
                
                const int32 iFirstLabel = PROCLABEL_SEQ_SAVELOADCHECK;

                m_iLabelNext = iFirstLabel;
                m_iLabelPrev = iFirstLabel;
                m_iLabelCurrent = iFirstLabel;
                m_param = nullptr;
                m_step = STEP_RUN;

                CScreen::SetFlipEnable(true);
#ifdef _DEBUG
                CSoftwareResetProcess::SetEnable(this, true);
                CSoftwareResetProcess::SetResponse(this, 1.5f);

                Call(PROCLABEL_SEQ_DBGMAIN);
#else /* _DEBUG */
                Call(m_iLabelCurrent);
#endif /* _DEBUG */
            }
            break;

        default:
            break;
        };
    };
};


int32 CTrialGameMainSequence::Branch(int32 iLabel)
{
    switch (iLabel)
    {
    case PROCLABEL_SEQ_OPTIONS:
        return PROCESSTYPES::LABEL_SEQ_PREV;

    case PROCLABEL_SEQ_LOGODISP:
        return PROCLABEL_SEQ_TITLE;

    case PROCLABEL_SEQ_TITLE:
        return PROCLABEL_SEQ_CHARASELECT;

    case PROCLABEL_SEQ_CHARASELECT:
        CGameData::PlayParam().SetStartArea(AREAID::ID_AREA02, 0); // st02n & st02nb
        return PROCLABEL_SEQ_AREAPLAY;

    case PROCLABEL_SEQ_AREAPLAY:
        {
            CGamePlayResult::AREARESULT arearesult = CGameData::PlayResult().GetAreaResult();
            if ((arearesult == CGamePlayResult::AREARESULT_GAMECLEAR) ||
                (arearesult == CGamePlayResult::AREARESULT_GAMEOVER))
            {
                return PROCLABEL_SEQ_TR_COMINGSOON;
            };            
        }
        return PROCLABEL_SEQ_LOGODISP;

    case PROCLABEL_SEQ_TR_COMINGSOON:
        return PROCLABEL_SEQ_LOGODISP;

    default:
        ASSERT(false);
		break;
    };

    return PROCLABEL_SEQ_LOGODISP;
};