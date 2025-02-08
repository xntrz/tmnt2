#include "TitleSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/MenuSound.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/File/FileID.hpp"


/*static*/ CTitleSequence::MENUITEMINFO CTitleSequence::m_aMenuItemInfoTable[] =
{
	{ MENUITEMID_GAME_NEW,		GAMETEXT(0x232),	CTitleSequence::NEXT_SEQUENCE_GAME_NEW,			true,	true,	},
	{ MENUITEMID_GAME_CONTINUE,	GAMETEXT(0x233),	CTitleSequence::NEXT_SEQUENCE_GAME_CONTINUE,	true,	true,	},
	{ MENUITEMID_OPTIONS,		GAMETEXT(0xD),		CTitleSequence::NEXT_SEQUENCE_OPTIONS, 			true,	true,	},
	{ MENUITEMID_QUIT,			GAMETEXT(0xE),		CTitleSequence::NEXT_SEQUENCE_QUIT,				true,	true,	},
	{ MENUITEMID_ARCADE,		GAMETEXT(0x234),	CTitleSequence::NEXT_SEQUENCE_ARCADE,			false,	false,	},
};


/*static*/ CProcess* CTitleSequence::Instance(void)
{
    return new CTitleSequence;
};


CTitleSequence::CTitleSequence(void)
: CAnim2DSequence("Title")
, m_NextSequence(NEXT_SEQUENCE_QUIT)
, m_phase(PHASE_NONE)
, m_iCurrentSelect(0)
, m_fTimer(0.0f)
, m_Dialog(CDialog::COLOR_ERROR, CDialog::STATUS_YES, CController::CONTROLLER_UNLOCKED_ON_VIRTUAL)
{
    ;
};


CTitleSequence::~CTitleSequence(void)
{
    ;
};


bool CTitleSequence::OnAttach(const void* pParam)
{
	m_NextSequence = NEXT_SEQUENCE_QUIT;
	m_phase = PHASE_NONE;
	m_iCurrentSelect = 0;
	m_fTimer = 0.0f;
	
	SetMenuItem();
	CAnim2DSequence::m_bDisplayLoading = false;
		
	UnlockAllControllers();
	CGameData::Attribute().SetVirtualPad(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);

#ifdef BUILD_TRIAL
	CTimeoutProcess::Enable(this, true);
	CTimeoutProcess::Start(this);
#endif

	bool bResult = CAnim2DSequence::OnAttach(FILEID::ID_TITLE);
	CDataLoader::Regist(FILEID::ID_TITLE2);
	return bResult;
};


void CTitleSequence::OnDetach(void)
{
	CGameSound::FadeOut(CGameSound::FADESPEED_NORMAL);
	CAnim2DSequence::OnDetach();
};


void CTitleSequence::OnMove(bool bRet, const void* pReturnValue)
{
	switch (m_animstep)
	{
	case ANIMSTEP_FADEIN:
		{
			if (CScreenFade::IsFading())
				break;
			
			CGameSound::PlayBGM(SDCODE_BGM(0x3020));
			m_pAnimation2D->FlashUnlockKeyEnable(true);
		}
		break;

	case ANIMSTEP_DRAW:
		{
			if (bRet)
			{
				SetMenuItem();
				CScreenFade::BlackIn(0.0f);
			};

			switch (m_phase)
			{
			case PHASE_NONE:
				m_phase = PHASE_START;
				break;

			case PHASE_START:
				CheckPressStart();
				UpdateTimers();
				UpdateDemo();
				break;

			case PHASE_CHOICE:
				UpdateMenu();
				UpdateTimers();
				UpdateDemo();
				break;

			case PHASE_CHOICE_WARNING:
				UpdateNewGameWarning();
				break;

			default:
				ASSERT(false);
				break;
			};
		}
		break;

	case ANIMSTEP_END:
		{
			if (bRet)
			{
				CConfigure::SetLaunchMode(TYPEDEF::CONFIG_LAUNCH_ARCADE);
                Ret(reinterpret_cast<const void*>(PROCESSTYPES::LABEL_EOL));
			}
			else
			{
				Branch();
			};
		}
        break;

    default:
        break;
    };

	CAnim2DSequence::OnMove(bRet, pReturnValue);
};


void CTitleSequence::OnDraw(void) const
{
	if ((m_phase == PHASE_CHOICE) && (m_animstep == ANIMSTEP_DRAW))
		m_pAnimation2D->SetCenterAllStrings();
	
	CAnim2DSequence::OnDraw();

    if (!((m_phase == PHASE_CHOICE) ||
          (m_phase == PHASE_CHOICE_WARNING)))
        return;

	if (CSystem2D::BeginScene())
	{
		const float fOffsetY = 80.0f;
		const float fHeight = 20.0f;
		float x = 0.0f;
		float y = fOffsetY;

		for (int32 i = 0; i < COUNT_OF(m_aMenuItemInfoTable); ++i)
		{
			const MENUITEMINFO* pMenuItemInfo = &m_aMenuItemInfoTable[i];
			
			if (!pMenuItemInfo->m_bVisible)
				continue;

			CGameFont::SetHeight(fHeight);

			if (m_iCurrentSelect == i)
			{
				CGameFont::SetRGBA(
					uint8(127.0f - Math::Cos(m_fTimer * 4.0f) * 127.0f),
					255,
					uint8(127.0f - Math::Cos(m_fTimer * 4.0f) * 127.0f),
					255
				);
			}
			else if (pMenuItemInfo->m_bEnabled)
			{
				CGameFont::SetRGBA(255, 255, 0, 255);
			}
			else
			{
				CGameFont::SetRGBA(96, 96, 0, 255);
			};

			const wchar* pwszText = CGameText::GetText(GAMETEXT(pMenuItemInfo->m_iStringID));
			x = CGameFont::GetStringWidth(pwszText, fHeight) * -0.5f;

			CGameFont::SetHeight(fHeight);
			CGameFont::Show(pwszText, x, y);

			y += fHeight;
			y += 10.0f;
		};

		CSystem2D::EndScene();
	};
};


void CTitleSequence::CheckPressStart(void)
{
	if (m_pAnimation2D->CheckMessageGetURL("start"))
	{
		CGameSound::FadeOut(CGameSound::FADESPEED_NORMAL);
		m_pAnimation2D->FlashUnlockKeyEnable(false);
		m_fTimer = 0.0f;
		m_phase = PHASE_CHOICE;
	};
};


void CTitleSequence::UpdateTimers(void)
{
	m_fTimer += CScreen::TimerStride();
};


void CTitleSequence::OpenNewGameWarning(void)
{
	ASSERT(m_phase != PHASE_CHOICE_WARNING);
	ASSERT(!m_Dialog.IsOpen());
	ASSERT(!m_Dialog.IsActive());

	m_Dialog.SetColor(CDialog::COLOR_ERROR);
    m_Dialog.SetStatus(CDialog::STATUS_NO);
	m_Dialog.Set(0.0f, 0.0f, CSprite::m_fVirtualScreenW, 180.0f);
	m_Dialog.SetText(
		CSystemText::GetText(SYSTEXT(121)),
		CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f),
		{ 0xFF, 0xFF, 0xFF, 0xFF }
	);
	m_Dialog.Open();

	m_fTimer = 0.0f;
	m_phase = PHASE_CHOICE_WARNING;
};


void CTitleSequence::UpdateNewGameWarning(void)
{
	ASSERT(m_phase == PHASE_CHOICE_WARNING);

	if (m_Dialog.IsOpen())
		return;

	switch (m_Dialog.GetStatus())
	{
	case CDialog::STATUS_YES:
		m_NextSequence = m_aMenuItemInfoTable[m_iCurrentSelect].m_nextseq;		
		BeginFadeout();
		break;

	case CDialog::STATUS_NO:
		m_phase = PHASE_CHOICE;
		break;

	default:
		ASSERT(false);
		break;
	};
};


void CTitleSequence::UpdateMenu(void)
{
	int32 iController = CGameData::Attribute().GetVirtualPad();
	
	if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_OK);
		m_fTimer = 0.0f;

		if (m_iCurrentSelect == MENUITEMID_GAME_NEW						&&
			m_aMenuItemInfoTable[MENUITEMID_GAME_CONTINUE].m_bVisible	&&
			m_aMenuItemInfoTable[MENUITEMID_GAME_CONTINUE].m_bEnabled)
		{
			OpenNewGameWarning();
		}
		else
		{
			m_NextSequence = m_aMenuItemInfoTable[m_iCurrentSelect].m_nextseq;
			BeginFadeout();
		};
	}
	else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_CANCEL);

		int32 iSelect = GetSelectByItemIndex(MENUITEMID_QUIT);
		if (m_iCurrentSelect == iSelect)
		{
			m_NextSequence = NEXT_SEQUENCE_QUIT;
			BeginFadeout();
		}
		else
		{
			m_iCurrentSelect = iSelect;
		};
	}
	else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_SELECT);
		
		m_fTimer = 0.0f;
		
		do
		{
			m_iCurrentSelect = InvClamp(--m_iCurrentSelect, 0, COUNT_OF(m_aMenuItemInfoTable) - 1);
		} while (!m_aMenuItemInfoTable[m_iCurrentSelect].m_bEnabled);
	}
	else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_SELECT);

		m_fTimer = 0.0f;
		
		do
		{
			m_iCurrentSelect = InvClamp(++m_iCurrentSelect, 0, COUNT_OF(m_aMenuItemInfoTable) - 1);
		} while (!m_aMenuItemInfoTable[m_iCurrentSelect].m_bEnabled);
	};
};


void CTitleSequence::UpdateDemo(void)
{
	float fElapsedTimeForDemo = (m_phase == PHASE_START ? 30.0f : 15.0f);
	
	if (m_fTimer > fElapsedTimeForDemo)
	{
		m_NextSequence = NEXT_SEQUENCE_DEMO;
		BeginFadeout();
	};
};


int32 CTitleSequence::GetSelectByItemIndex(int32 iItemIndex) const
{
	for (int32 i = 0; i < COUNT_OF(m_aMenuItemInfoTable); ++i)
	{
		if (m_aMenuItemInfoTable[i].m_iIndex == iItemIndex)
			return i;
	};

	return -1;
};


void CTitleSequence::SetMenuItem(void)
{
#ifdef TARGET_PC
	m_aMenuItemInfoTable[MENUITEMID_QUIT].m_bEnabled = true;
	m_aMenuItemInfoTable[MENUITEMID_QUIT].m_bVisible = true;
#else
	if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_ARCADEGAME))
	{
		m_aMenuItemInfoTable[MENUITEMID_ARCADE].m_bEnabled = true;
		m_aMenuItemInfoTable[MENUITEMID_ARCADE].m_bVisible = true;
	};
#endif	
	m_aMenuItemInfoTable[MENUITEMID_GAME_CONTINUE].m_bEnabled = (!CGameData::IsNewGame());
};


void CTitleSequence::Branch(void)
{
	switch (m_NextSequence)
	{
	case NEXT_SEQUENCE_GAME_NEW:
		CGameData::OnNewGame();
		Ret();
		break;

	case NEXT_SEQUENCE_GAME_CONTINUE:
		Ret();
		break;

	case NEXT_SEQUENCE_DEMO:
		Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_PLAYDEMO));
		break;

	case NEXT_SEQUENCE_OPTIONS:
		Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_OPTIONS));
		break;

	case NEXT_SEQUENCE_ARCADE:
	case NEXT_SEQUENCE_QUIT:
		Ret(reinterpret_cast<const void*>(PROCESSTYPES::LABEL_EOL));
		break;

	default:
		ASSERT(false);
		break;
	};
};