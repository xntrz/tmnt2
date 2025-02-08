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
    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();

    CRideStage::m_bSpace = ((stageId == STAGEID::ID_ST22R) ||
                            (stageId == STAGEID::ID_ST32R));

    CStageBaseSequence::OnAttach(pParam);

    RegisterStateObject(STATE_LOAD,  new CLoadRideStageSeqState(stageId), true);
    RegisterStateObject(STATE_INTRO, new CIntroStageSeqState, true);
    RegisterStateObject(STATE_PLAY,  new CPlayRideStageSeqState, true);
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
        ChangeState(STATE_LOAD);
        break;

    case STATE_LOAD:
        {
            STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();
            int32 bgmCode = CStageInfo::GetBgmNo(stageId);

            CGameSound::PlayBGM(bgmCode);

            ChangeState(STATE_INTRO);
        }
        break;

    case STATE_INTRO:
        ChangeState(STATE_PLAY);
        break;

    case STATE_PLAY:
        ChangeState(STATE_END);
        break;

    default:
        break;
    };
};