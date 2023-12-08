#include "RideStageSequence.hpp"
#include "RideStageSeqState.hpp"
#include "RideStage.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ CProcess* CRideStageSequence::Instance(void)
{
    return new CRideStageSequence;
};


CRideStageSequence::CRideStageSequence(void)
{
    ;
};


CRideStageSequence::~CRideStageSequence(void)
{
    ;
};


bool CRideStageSequence::OnAttach(const void* pParam)
{
    if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST22R ||
        CGameData::PlayParam().GetStage() == STAGEID::ID_ST32R)
        CRideStage::m_bSpace = true;
    else
        CRideStage::m_bSpace = false;

    CStageBaseSequence::OnAttach(pParam);

    RegisterStateObject(STATE_LOAD, new CLoadRideStageSeqState(CGameData::PlayParam().GetStage()), true);
    RegisterStateObject(STATE_INTRO, new CIntroStageSeqState, true);
    RegisterStateObject(STATE_PLAY, new CPlayRideStageSeqState, true);
    ChangeState(STATE_LOAD);

    CRideStage::Initialize();

    return true;
};


void CRideStageSequence::OnDetach(void)
{
    CRideStage::Terminate();
    
    CStageBaseSequence::OnDetach();
};


void CRideStageSequence::OnDraw(void) const
{
    CStageBaseSequence::OnDraw();

    if (GetState() == STATE_PLAY)
        CRideStage::Draw();
};


void CRideStageSequence::OnStateDetached(STATE state)
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
            CGameSound::PlayBGM(CStageInfo::GetBgmNo(CGameData::PlayParam().GetStage()));
            ChangeState(STATE_INTRO);
        }
        break;

    case STATE_INTRO:
        {
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