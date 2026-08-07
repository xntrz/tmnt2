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
#endif /* defined(TARGET_WEB) */


/*static*/ CProcess* CTrialGameMainSequence::Instance(void)
{
    return new CTrialGameMainSequence;
};


CTrialGameMainSequence::CTrialGameMainSequence(void)
: m_iAreaNo(0)
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
        CGameData::Option().Play().SetEnableAutosave(false);
        CGameData::Record().Item().SetItemTaken(ITEMID::ID_DON_LASER);
    };

    return bResult;
};


void CTrialGameMainSequence::OnDetach(void)
{
#if defined(TMNT2_FEATURE_TOUCHCONTROLLER)
    if (CWebSpecific::IsMobilePlatform())
        CTouchController::Terminate(this);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */

    CGameMainSequence::OnDetach();
};


void CTrialGameMainSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
    {
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
                m_iLabelNext = PROCLABEL_SEQ_LOGODISP;
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
                    CTouchController::Initialize(this);
#endif /* defined(TMNT2_FEATURE_TOUCHCONTROLLER) */
            }
            break;

        case STEP_LOAD_SOUND:
            {
                if (!CGameSound::IsLoadEnd())
                    break;
                
                const int32 iFirstLabel = PROCLABEL_SEQ_LOGODISP;

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
        return BranchAreaPlay(iLabel);

    case PROCLABEL_SEQ_AREAPLAY:
        return BranchAreaPlay(iLabel);

    case PROCLABEL_SEQ_TR_COMINGSOON:
        return PROCLABEL_SEQ_LOGODISP;

    default:
        ASSERT(false);
		break;
    };

    return PROCLABEL_SEQ_LOGODISP;
};


int32 CTrialGameMainSequence::BranchAreaPlay(int32 iLabel)
{
    static const AREAID::VALUE s_aTrialAreaId[] =
    {
        AREAID::ID_AREA12,
        AREAID::ID_AREA13,
        AREAID::ID_AREA14,
        AREAID::ID_AREA19,
        AREAID::ID_AREA20,
    };

    if (iLabel == PROCLABEL_SEQ_CHARASELECT)
    {
        m_iAreaNo = 0;
        CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_NONE);
    };

    CGamePlayResult::AREARESULT arearesult = CGameData::PlayResult().GetAreaResult();
    
    if ((arearesult == CGamePlayResult::AREARESULT_NONE) ||
        (arearesult == CGamePlayResult::AREARESULT_GAMECLEAR))
    {
        if (m_iAreaNo < COUNT_OF(s_aTrialAreaId))
        {
            CGameData::PlayParam().SetStartArea(s_aTrialAreaId[m_iAreaNo++], 0);
            return PROCLABEL_SEQ_AREAPLAY;
        };
        
        return PROCLABEL_SEQ_TR_COMINGSOON;
    }
    else if (arearesult == CGamePlayResult::AREARESULT_GAMEOVER)
    {
        return PROCLABEL_SEQ_TR_COMINGSOON;
    };

    return PROCLABEL_SEQ_LOGODISP;
};