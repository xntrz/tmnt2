#include "PlayDemoSeqState.hpp"
#include "PlayDemoSequence.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/PadStream.hpp"
#include "Game/System/Misc/Gamepad.hpp"


void CPlayDemoStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
	pSeq->Stage().AddStageObjects();

	bool bBlink = false;
	pSeq->Stage().AddPlayers(bBlink);

    CMapCamera* pMapCamera = pSeq->Stage().GetMapCamera();
    ASSERT(pMapCamera);

    pMapCamera->SetCameraMode(CMapCamera::MODE_AUTOCHANGE);
    if (CGameProperty::GetPlayerNum() > 1)
        pMapCamera->SetPathMode(CMapCamera::PATHMODE_MULTIPLAYER);
    else
        pMapCamera->SetPathMode(CMapCamera::PATHMODE_SINGLEPLAYER);

    CScreenFade::BlackIn();
    m_fTime = 0.0f;
    m_step = STEP_PLAY;
};


void CPlayDemoStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    ;
};


bool CPlayDemoStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    switch (m_step)
    {
    case STEP_PLAY:
        {
            CGameStage& stage = pSeq->Stage();

            if (CPadStreamSwitch::m_mode == CPadStream::MODE_RECORD)
            {
                if (!CPadStreamSwitch::m_bEnd)
                    m_fTime += CGameProperty::GetElapsedTime();
                else
                    m_step = STEP_PLAYEND;
            }
            else
            {
				m_fTime += CGameProperty::GetElapsedTime();

                bool bIsStageEnd    = (stage.GetResult() != CGameStage::RESULT_NONE);
                bool bIsPadStreamEnd= (CPadStreamSwitch::m_bEnd);
                bool bIsTimeout     = (m_fTime > 58.0f);
                bool bIsUserStopRq  = (IPad::GetDigitalTrigger(IPad::CONTROLLER_UNLOCKED_ON_VIRTUAL, IPad::DIGITAL_OK));

                if (bIsStageEnd || bIsPadStreamEnd || bIsTimeout || bIsUserStopRq)
                    m_step = STEP_PLAYEND;
            };
        }
        break;

    case STEP_PLAYEND:
        {
            CScreenFade::BlackOut();
            m_step = STEP_FADEIN;
        }
        break;

    case STEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
        break;

    default:
        break;
    };

    return (m_step >= STEP_END);
};