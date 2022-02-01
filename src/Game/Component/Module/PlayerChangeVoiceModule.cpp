#include "PlayerChangeVoiceModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Sound/MessageManager.hpp"


CPlayerChangeVoiceModule::CPlayerChangeVoiceModule(CPlayerCharacter* pPlayerChr)
: IModule(MODULETYPE::PLAYER_CHNG_VOICE)
, m_pPlayerChr(pPlayerChr)
{
    ASSERT(m_pPlayerChr);
};


CPlayerChangeVoiceModule::~CPlayerChangeVoiceModule(void)
{
    ;
};


void CPlayerChangeVoiceModule::Run(void)
{
    ;
};


void CPlayerChangeVoiceModule::Draw(void)
{
    ;
};


void CPlayerChangeVoiceModule::SetComboWall(void)
{
    ;
};


void CPlayerChangeVoiceModule::PlayChangeVoice(void)
{
    ;
};


void CPlayerChangeVoiceModule::PlayRandomVoice(void)
{
    ;
};


void CPlayerChangeVoiceModule::Start(void)
{
    ;
};