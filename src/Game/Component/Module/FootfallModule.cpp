#include "FootfallModule.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
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
        SE_WALK_PARAM param;
        param.Type = m_pCharacter->GetCharacterType();
        
        if (m_pCharacter->GetCharacterType() == CCharacter::TYPE_ENEMY)
            param.Id = static_cast<CCharacterCompositor*>(m_pCharacter)->GetID();
        else            
            param.Id = static_cast<CPlayerCharacter*>(m_pCharacter)->GetID();
        
        param.MoveType = SEMOVETYPE_RUN;
        param.GroundType = m_pCharacter->GetCollisionGround()->m_attribute;

        m_pCharacter->GetPosition(&param.Pos);

        CGameSound::PlayWalkSE(&param);    
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


//
// *********************************************************************************
//


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
    return ((m_pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_RUN) &&
             m_pPlayerChr->TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING));
};