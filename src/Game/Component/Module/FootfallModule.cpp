#include "FootfallModule.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Sound/GameSound.hpp"


CFootfallModule::CFootfallModule(CCharacter* pCharacter)
: IModule(MODULETYPE::FOOTFALL)
, m_pCharacter(pCharacter)
{
    ASSERT(m_pCharacter);
};


CFootfallModule::~CFootfallModule(void)
{
    ;
};


void CFootfallModule::Run(void)
{
    if (TimingCheck())
    {
        //
        //  TODO CGameSound::PlayWalkSE
        //
    };
};


void CFootfallModule::Draw(void)
{
    ;
};


bool CFootfallModule::TimingCheck(void)
{
    return false;
};


CPlayerFootfallModule::CPlayerFootfallModule(CPlayerCharacter* pPlayerChr)
: CFootfallModule(pPlayerChr)
, m_pPlayerChr(pPlayerChr)
{
    ASSERT(m_pPlayerChr);
};


CPlayerFootfallModule::~CPlayerFootfallModule(void)
{
    ;
};


bool CPlayerFootfallModule::TimingCheck(void)
{
    return (m_pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_RUN &&
            m_pPlayerChr->IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_TIMING));
};