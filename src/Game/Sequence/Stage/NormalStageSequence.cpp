#include "NormalStageSequence.hpp"
#include "StageSeqState.hpp"

#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ CProcess* CNormalStageSequence::Instance(void)
{
    return new CNormalStageSequence;
};


CNormalStageSequence::CNormalStageSequence(void)
{
    ;
};


CNormalStageSequence::~CNormalStageSequence(void)
{
    ;
};


bool CNormalStageSequence::OnAttach(const void* param)
{
    CStageBaseSequence::OnAttach(param);

    RegisterStateObject(STATE_LOAD, new CLoadStageSeqState(CGameData::PlayParam().GetStage()), true);
    if(CGameData::PlayParam().IsFirstStage())
        RegisterStateObject(STATE_INTRO, new CIntroStageSeqState, true);    
    RegisterStateObject(STATE_PLAY, new CPlayStageSeqState, true);

    ChangeState(STATE_LOAD);

	return true;
};


void CNormalStageSequence::OnStateDetached(STATE state)
{
    switch (state)
    {
    case STATE_LOAD:
        {
            STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();

            CGameSound::PlayBGM(
                SDCODE_BGM(CStageInfo::GetBgmNo(idStage))
            );

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