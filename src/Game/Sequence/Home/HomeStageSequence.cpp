#include "HomeStageSequence.hpp"
#include "Home2D.hpp"
#include "HomeCamera.hpp"
#include "HomeStageSeqState.hpp"

#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ HOMETYPES::DOORKIND CHomeStageSequence::m_eDoorKind = HOMETYPES::DOORKIND_EXIT;


/*static*/ CProcess* CHomeStageSequence::Instance(void)
{
    return new CHomeStageSequence;
};


CHomeStageSequence::CHomeStageSequence(void)
: m_pHomeCamera(nullptr)
{
    ;
};


CHomeStageSequence::~CHomeStageSequence(void)
{
    ASSERT(!m_pHomeCamera);
};


bool CHomeStageSequence::OnAttach(const void* param)
{
    CGameData::PlayParam().SetStage(STAGEID::ID_ST03S);
    CStageBaseSequence::OnAttach(param);

    m_pHomeCamera = new CHomeCamera;
    ASSERT(m_pHomeCamera);
    Stage().SetCameraUpdater(m_pHomeCamera);

    CHome2D::Initialize(m_pHomeCamera);

    RegisterStateObject(STATE_LOAD, new CLoadHomeStageSeqState, true);
    RegisterStateObject(STATE_PLAY, new CPlayHomeStageSeqState, true);
    ChangeState(STATE_LOAD);

    return true;
};


void CHomeStageSequence::OnDetach(void)
{
    m_eDoorKind = m_pHomeCamera->GetDoorKind(HOMETYPES::DOORPOSITION_FRONT);
    Stage().SetCameraUpdater(nullptr);

    CHome2D::Terminate();

    if (m_pHomeCamera)
    {
        delete m_pHomeCamera;
        m_pHomeCamera = nullptr;
    };

    CStageBaseSequence::OnDetach();
};


void CHomeStageSequence::OnMove(bool bResume, const void* param)
{
    CStageBaseSequence::OnMove(bResume, param);
};


void CHomeStageSequence::OnDraw(void) const
{
    CStageBaseSequence::OnDraw();
    if (GetState() == STATE_PLAY)
        CHome2D::Draw();
};


void CHomeStageSequence::OnStateDetached(STATE state)
{
    switch (state)
    {
    case STATE_NONE:
        {
            ChangeState(STATE_LOAD);
        }
        break;

    case STATE_LOAD:
        {
            CGameSound::PlayBGM(CStageInfo::GetBgmNo(STAGEID::ID_ST03S));
            ChangeState(STATE_PLAY);
        }
        break;

    case STATE_PLAY:
        {
            ChangeState(STATE_END);
        }
        break;
    };
};


CHomeCamera* CHomeStageSequence::GetCamera(void) const
{
    return m_pHomeCamera;
};