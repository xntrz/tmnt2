#include "PlayDemoSeqState.hpp"
#include "PlayDemoSequence.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/PadStream.hpp"
#include "System/Common/Controller.hpp"


void CPlayDemoStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    pSeq->Stage().AddPlayers();

    CMapCamera* pMapCamera = pSeq->Stage().GetMapCamera();
    ASSERT(pMapCamera);

    pMapCamera->SetCameraMode(CMapCamera::MODE_AUTOCHANGE);
    if (CGameProperty::GetPlayerNum() > 1)
        pMapCamera->SetPathMode(CMapCamera::PATHMODE_MULTIPLAYER);
    else
        pMapCamera->SetPathMode(CMapCamera::PATHMODE_SINGLEPLAYER);

    CScreenFade::StartOut();
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

				if (stage.GetResult() ||
					CPadStreamSwitch::m_bEnd ||
					m_fTime >= 58.0f ||
					CController::GetDigitalTrigger(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::DIGITAL_OK))
				{
                    m_step = STEP_PLAYEND;
				};
            };
        }
        break;

    case STEP_PLAYEND:
        {
            CScreenFade::StartIn();
            m_step = STEP_FADEIN;
        }
        break;

    case STEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
        break;
    };

    return (m_step >= STEP_END);
};