#include "NexusStageSequence.hpp"
#include "NexusStageSeqState.hpp"

#include "Game/Component/GameData/GameData.hpp"


/*static*/ CProcess* CNexusStageSequence::Instance(void)
{
    return new CNexusStageSequence;
};


CNexusStageSequence::CNexusStageSequence(void)
{
    ;
};


CNexusStageSequence::~CNexusStageSequence(void)
{
    ;
};


bool CNexusStageSequence::OnAttach(const void* pParam)
{
    CGameData::Attribute().SetGamemode(GAMETYPES::GAMEMODE_NEXUS);

    CStageBaseSequence::OnAttach(pParam);

    RegisterStateObject(STATE_LOAD, new CLoadStageSeqState(CGameData::PlayParam().GetStage()), true);
    if (CGameData::PlayParam().IsFirstStage())
        RegisterStateObject(STATE_INTRO, new CIntroStageSeqState, true);
    RegisterStateObject(STATE_PLAY, new CPlayNexusStageSeqState, true);

    ChangeState(STATE_LOAD);
    
    return true;
};


void CNexusStageSequence::OnDetach(void)
{
    CStageBaseSequence::OnDetach();
    CGameData::Attribute().SetGamemode(GAMETYPES::GAMEMODE_NORMAL);
};