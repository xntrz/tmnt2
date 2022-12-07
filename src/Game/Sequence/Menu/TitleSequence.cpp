#include "TitleSequence.hpp"

#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/GameText.hpp"
#include "Game/System/2d/MenuSound.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/File/FileID.hpp"


namespace MENUITEMID
{
	enum VALUE
	{
		ID_GAME_NEW,
		ID_GAME_CONTINUE,
		ID_OPTIONS,
		ID_QUIT,
		ID_ARCADE,
	};
};


/*static*/ CTitleSequence::MENUITEMINFO CTitleSequence::m_aMenuItemInfoTable[] =
{
	{ MENUITEMID::ID_GAME_NEW, 		GAMETEXT::VALUE(0x232), CTitleSequence::NEXT_SEQUENCE_GAME_NEW, 		true, 	true, 	},
	{ MENUITEMID::ID_GAME_CONTINUE, GAMETEXT::VALUE(0x233), CTitleSequence::NEXT_SEQUENCE_GAME_CONTINUE, 	true, 	true, 	},
	{ MENUITEMID::ID_OPTIONS, 		GAMETEXT::VALUE(0xD), 	CTitleSequence::NEXT_SEQUENCE_OPTIONS, 			true, 	true, 	},
	{ MENUITEMID::ID_QUIT, 			GAMETEXT::VALUE(0xE), 	CTitleSequence::NEXT_SEQUENCE_QUIT, 			true, 	true, 	},
	{ MENUITEMID::ID_ARCADE, 		GAMETEXT::VALUE(0x234), CTitleSequence::NEXT_SEQUENCE_ARCADE, 			false, 	false, 	},
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
, m_Dialog(CDialog::COLOR_ERROR, CDialog::STATUS_NO, CController::CONTROLLER_UNLOCKED_ON_VIRTUAL)
{
    ;
};


CTitleSequence::~CTitleSequence(void)
{
    ;
};


bool CTitleSequence::OnAttach(const void* param)
{
	m_NextSequence = NEXT_SEQUENCE_QUIT;
	m_phase = PHASE_NONE;
	m_iCurrentSelect = 0;
	m_fTimer = 0.0f;

#ifdef _TARGET_PC
	m_aMenuItemInfoTable[MENUITEMID::ID_QUIT].m_bEnabled = true;
	m_aMenuItemInfoTable[MENUITEMID::ID_QUIT].m_bVisible = true;
#else
	if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_ARCADEGAME))
	{
		m_aMenuItemInfoTable[MENUITEMID::ID_ARCADE].m_bEnabled = true;
		m_aMenuItemInfoTable[MENUITEMID::ID_ARCADE].m_bVisible = true;
	};
#endif	
	m_aMenuItemInfoTable[MENUITEMID::ID_GAME_CONTINUE].m_bEnabled = (!CGameData::IsNewGame());
	
	CAnim2DSequence::m_bDisplayLoading = false;
		
	CController::UnlockAllControllers();
	CGameData::Attribute().SetVirtualPad(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);
	
	bool bResult = CAnim2DSequence::OnAttach(FILEID::ID_TITLE);
	CDataLoader::Regist(FILEID::ID_TITLE2);
	return bResult;
};


void CTitleSequence::OnDetach(void)
{
	CGameSound::FadeOut(CGameSound::FADESPEED_NORMAL);
	CAnim2DSequence::OnDetach();
};


void CTitleSequence::OnMove(bool bRet, const void* param)
{
	if (bRet)
	{
		CController::UnlockAllControllers();
		CGameData::Attribute().SetVirtualPad(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);
	};

	CAnim2DSequence::OnMove(bRet, param);

	if (m_step != STEP_DRAW)
		return;

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
};


void CTitleSequence::OnDraw(void) const
{
    if (m_phase == PHASE_CHOICE && m_step == STEP_DRAW)
		m_pAnimation2D->SetCenterAllStrings();
	
	CAnim2DSequence::OnDraw();

	if (m_phase != PHASE_CHOICE)
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

			if (m_iCurrentSelect == i)
			{
				CGameFont::m_pFont->SetRGBA(
					uint8(127.0f - Math::Cos(m_fTimer * 4.0f) * 127.0f),
					255,
					uint8(127.0f - Math::Cos(m_fTimer * 4.0f) * 127.0f),
					255
				);
			}
			else if (pMenuItemInfo->m_bEnabled)
			{
				CGameFont::m_pFont->SetRGBA(255, 255, 0, 255);
			}
			else
			{
				CGameFont::m_pFont->SetRGBA(96, 96, 0, 255);
			};

			const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(pMenuItemInfo->m_iStringID));
			x = CGameFont::GetStringWidth(pwszText, fHeight) * -0.5f;
			CGameFont::m_pFont->Show(pwszText, fHeight, x, y);
			y += fHeight;
			y += 10.0f;
		};

		CSystem2D::EndScene();
	};
};


bool CTitleSequence::OnRet(void)
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
		Ret((const void*)PROCESSTYPES::LABEL_SEQ_STAGEDEMO);
		break;

	case NEXT_SEQUENCE_OPTIONS:
		Ret((const void*)PROCESSTYPES::LABEL_SEQ_OPTION);
		break;
		
	case NEXT_SEQUENCE_ARCADE:
	case NEXT_SEQUENCE_QUIT:
		Ret((const void*)PROCESSTYPES::LABEL_EOL);
		break;

	default:
		ASSERT(false);
		break;
	};

	return true;
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
	m_Dialog.Set(0.0f, 0.0f, CSprite::m_fVirtualScreenW, 180.0f);

	m_Dialog.SetText(
		CSystemText::GetText(SYSTEXT::VALUE(121)),
		CGameFont::GetScreenSize() * 0.0044f,
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
		Ret();
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

		if (m_iCurrentSelect == MENUITEMID::ID_GAME_NEW						&&
			m_aMenuItemInfoTable[MENUITEMID::ID_GAME_CONTINUE].m_bVisible	&&
			m_aMenuItemInfoTable[MENUITEMID::ID_GAME_CONTINUE].m_bEnabled)
		{
			//
			//	If pressing new game while continue is available
			//
			
			OpenNewGameWarning();
		}
		else
		{
			m_NextSequence = m_aMenuItemInfoTable[m_iCurrentSelect].m_nextseq;
			Ret();
		};
	}
	else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_CANCEL);

		int32 iSelect = GetSelectByItemIndex(MENUITEMID::ID_QUIT);
		if (m_iCurrentSelect == iSelect)
		{
			m_NextSequence = NEXT_SEQUENCE_QUIT;
			Ret();
		}
		else
		{
			m_iCurrentSelect = iSelect;
		};
	}
	else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_UP))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_SELECT);
		
		m_fTimer = 0.0f;
		
		do
		{
			m_iCurrentSelect = Math::InvClamp(--m_iCurrentSelect, 0, COUNT_OF(m_aMenuItemInfoTable) - 1);
		} while (!m_aMenuItemInfoTable[m_iCurrentSelect].m_bEnabled);
	}
	else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_DOWN))
	{
		CMenuSound::PlaySE(CMenuSound::SOUND_ID_SELECT);

		m_fTimer = 0.0f;
		
		do
		{
			m_iCurrentSelect = Math::InvClamp(++m_iCurrentSelect, 0, COUNT_OF(m_aMenuItemInfoTable) - 1);
		} while (!m_aMenuItemInfoTable[m_iCurrentSelect].m_bEnabled);
	};
};


void CTitleSequence::UpdateDemo(void)
{
	float fElapsedTimeForDemo = (m_phase == PHASE_START ? 30.0f : 15.0f);
	
	if (m_fTimer > fElapsedTimeForDemo)
	{
		m_NextSequence = NEXT_SEQUENCE_DEMO;
		Ret();
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