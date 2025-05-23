#include "GameStagePause.hpp"
#include "GameStage.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Gauge/GaugeInformation.hpp"
#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Sound/VoiceManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "System/Common/Controller.hpp"


/*static*/ CDummyStagePause* CDummyStagePause::Instance(void)
{
    static CDummyStagePause s_DummyStagePause;
    return &s_DummyStagePause;
};


void CDummyStagePause::Start(void* param)
{
    ;
};


void CDummyStagePause::Stop(void)
{
    ;
};


bool CDummyStagePause::Update(void)
{
    return true;    
};


void CDummyStagePause::Draw(void)
{
    ;
};


//
// *********************************************************************************
//


/*static*/ CMenuStagePause* CMenuStagePause::Instance(void)
{
    static CMenuStagePause s_MenuStagePause;
    return &s_MenuStagePause;
};


void CMenuStagePause::Start(void* param)
{
    CGaugeInformation::DispInit();
    
    CGameSound::PlaySE(SDCODE_SE(4099));
    CGameSound::Pause();

    EnableStickToDirButton(true);
};


void CMenuStagePause::Stop(void)
{
    EnableStickToDirButton(false);
};


bool CMenuStagePause::Update(void)
{
    bool bResult = true;
    
    if (CScreenFade::IsDrawing())
        return bResult;

    CGaugeInformation::PausePeriod();

    CGameStage* pStage = CGameStage::GetCurrent();
    ASSERT(pStage);

    CGaugeInformation::PAUSESTATUS status = CGaugeInformation::GetPauseStatus();
    switch (status)
    {
    case CGaugeInformation::PAUSESTATUS_RUN:
        {
            bResult = false;
        }
        break;
        
    case CGaugeInformation::PAUSESTATUS_BACK:
        {
            ;
        }
        break;

    case CGaugeInformation::PAUSESTATUS_RET_WORLD:
        {
            if (pStage->SetResult(CGameStage::RESULT_RET_STAGESEL))
            {                
                CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_EXIT);
                CGameData::PlayResult().SetExitSub(CGamePlayResult::EXITSUB_TO_AREASEL);
            };
        }
        break;

    case CGaugeInformation::PAUSESTATUS_RET_TITLE:
        {
            if (pStage->SetResult(CGameStage::RESULT_RET_TITLE))
            {
                CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_EXIT);
                CGameData::PlayResult().SetExitSub(CGamePlayResult::EXITSUB_TO_TITLE);
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (bResult)
    {
        CGameSound::Resume();
        CGameSound::PlaySE(SDCODE_SE(4097));
    };

    return bResult;
};


void CMenuStagePause::Draw(void)
{
    CGaugeInformation::PauseDraw();
};


//
// *********************************************************************************
//


/*static*/ const CTutorialStagePause::TUTORIALINFO CTutorialStagePause::m_aTutorialMessageInfo [] =
{
    { GAMETEXT_EMPTY,       SEGROUPID::VALUE(-1)    },
    { GAMETEXT_HELP_DLG_1,  SEGROUPID::VALUE(0xA8)  },
    { GAMETEXT_HELP_DLG_2,  SEGROUPID::VALUE(0xA9)  },
    { GAMETEXT_HELP_DLG_3,  SEGROUPID::VALUE(0xAA)  },
    { GAMETEXT_HELP_DLG_4,  SEGROUPID::VALUE(0xAC)  },
    { GAMETEXT_HELP_DLG_5,  SEGROUPID::VALUE(0xAD)  },
    { GAMETEXT_HELP_DLG_6,  SEGROUPID::VALUE(0xAE)  },
    { GAMETEXT_HELP_DLG_7,  SEGROUPID::VALUE(0xB2)  },
    { GAMETEXT_HELP_DLG_8,  SEGROUPID::VALUE(0xB4)  },
    { GAMETEXT_HELP_DLG_9,  SEGROUPID::VALUE(0xB6)  },
    { GAMETEXT_HELP_DLG_10, SEGROUPID::VALUE(0xB8)  },
};


/*static*/ CTutorialStagePause* CTutorialStagePause::Instance(void)
{
    static CTutorialStagePause s_TutorialStagePause;
    return &s_TutorialStagePause;
};


void CTutorialStagePause::Start(void* param)
{
    int32 tutorialNo = reinterpret_cast<int32>(param);
    ASSERT(tutorialNo >= 0);
    ASSERT(tutorialNo < COUNT_OF(m_aTutorialMessageInfo));    

    CVoiceManager::SetVoice(SEGROUPID::VALUE(m_aTutorialMessageInfo[tutorialNo].m_nSeGroup), true);

    const wchar* pwszText = CGameText::GetText(GAMETEXT(m_aTutorialMessageInfo[tutorialNo].m_nTextID));

    m_pMsgWnd = new CMessageWindow(CMessageWindow::COLOR_TUTORIAL);
    m_pMsgWnd->Set(0.0f, 0.0f, 480.0f, 180.0f);
    m_pMsgWnd->SetOpenAction(true);
    m_pMsgWnd->SetText(pwszText, 16.0f, { 0xFF, 0xFF, 0xFF, 0xFF });
    m_pMsgWnd->Open();

    m_fTimer = 1.0f;

    m_bComplete = false;
};


void CTutorialStagePause::Stop(void)
{
    if (m_pMsgWnd)
    {
        if (m_pMsgWnd->IsOpen())
            m_pMsgWnd->InputAction();
        
        delete m_pMsgWnd;
        m_pMsgWnd = nullptr;
    };
};


bool CTutorialStagePause::Update(void)
{    
    if (CScreenFade::IsDrawing())
        return true;

    if (m_fTimer > 0.0f)
    {
        m_fTimer -= CGameProperty::GetElapsedTime();
        return false;
    };

	if (!m_bComplete)
	{
		int32 iController = CGameData::Attribute().GetVirtualPad();
		if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
		{
			m_bComplete = true;
			if (m_pMsgWnd->IsOpen())
				m_pMsgWnd->InputAction();
			m_pMsgWnd->Close();
		};
	}
	else
	{
		if (!m_pMsgWnd->IsOpen())
			return true;
	};

    return false;
};


void CTutorialStagePause::Draw(void)
{
    ;
};