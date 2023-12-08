#include "PlayerChangeVoiceModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerUtil.hpp"
#include "Game/System/Sound/MessageManager.hpp"


CPlayerChangeVoiceModule::CPlayerChangeVoiceModule(CPlayerCharacter* pPlayerChr)
: IModule(MODULETYPE::PLAYER_CHNG_VOICE)
, m_pPlayerChr(pPlayerChr)
, m_fInitTStamp(0.0f)
, m_fChangeVoxRqTime(0.0f)
, m_fComboVoxRqTime(0.0f)
, m_fRandVoxRqTime(0.0f)
, m_bChangeVoxRq(false)
, m_bComboVoxRq(false)
{
    ASSERT(m_pPlayerChr);

    m_fInitTStamp = CGameProperty::GetTotalElapsedTime();
    m_fChangeVoxRqTime = m_fInitTStamp;
    m_fRandVoxRqTime = Math::RandFloatRange(150.0f, 200.0f);
};


CPlayerChangeVoiceModule::~CPlayerChangeVoiceModule(void)
{
    ;
};


void CPlayerChangeVoiceModule::Run(void)
{
    //
    //  update change voice
    //
    if (GetTime() < 0.1f)
        m_bChangeVoxRq = false;

    if (m_bChangeVoxRq && (GetTime() >= m_fChangeVoxRqTime))
    {
        PlayChangeVoice();
        m_bChangeVoxRq = false;
    };

    //
    //  update rand voice
    //
    if (GetTime() >= m_fRandVoxRqTime)
    {
        CMessageManager::Request(SEGROUPID::VALUE(1), m_pPlayerChr->GetID());
        m_fRandVoxRqTime += Math::RandFloatRange(150.0f, 200.0f);
    }
    else
    {
        if (m_pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_IDLE)
            m_fRandVoxRqTime += CGameProperty::GetElapsedTime();
    };

    //
    //  update combo voice
    //
    if (m_bComboVoxRq && (GetTime() >= m_fComboVoxRqTime))
    {
        PlayerUtil::CallVoiceOfComboHit(m_pPlayerChr);
        m_bComboVoxRq = false;
    };
};


void CPlayerChangeVoiceModule::Draw(void)
{
    ;
};


void CPlayerChangeVoiceModule::PlayChangeVoice(void)
{
    switch (m_pPlayerChr->GetID())
    {
    case PLAYERID::ID_LEO:
        CMessageManager::Request(SEGROUPID::VALUE(27));
        break;

    case PLAYERID::ID_RAP:
        CMessageManager::Request(SEGROUPID::VALUE(28));
        break;

    case PLAYERID::ID_MIC:
        CMessageManager::Request(SEGROUPID::VALUE(29));
        break;

    case PLAYERID::ID_DON:
        CMessageManager::Request(SEGROUPID::VALUE(30));
        break;

    case PLAYERID::ID_SPL:
        CMessageManager::Request(SEGROUPID::VALUE(33));
        break;

    case PLAYERID::ID_CAS:
        CMessageManager::Request(SEGROUPID::VALUE(31));
        break;

    case PLAYERID::ID_KAR:
        CMessageManager::Request(SEGROUPID::VALUE(32));
        break;

    case PLAYERID::ID_SLA:
        CMessageManager::Request(SEGROUPID::VALUE(34));
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CPlayerChangeVoiceModule::Start(void)
{
    m_bComboVoxRq = false;
    
    m_bChangeVoxRq = true;
    m_fChangeVoxRqTime = (GetTime() + 0.8f);

    m_fRandVoxRqTime = (GetTime() + Math::RandFloatRange(150.0f, 200.0f));
};


void CPlayerChangeVoiceModule::SetComboWall(void)
{
    m_bComboVoxRq = true;
    m_fComboVoxRqTime = (GetTime() + 1.3f);
};


float CPlayerChangeVoiceModule::GetTime(void) const
{
    return (CGameProperty::GetTotalElapsedTime() - m_fInitTStamp);
};