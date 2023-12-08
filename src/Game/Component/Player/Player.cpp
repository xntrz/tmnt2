#include "Player.hpp"
#include "PlayerCharacter.hpp"
#include "Manipulator.hpp"

#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "System/Common/Controller.hpp"


static const char* s_apszPlayerClassNameTable[] =
{
    "player_1",
    "player_2",
    "player_3",
    "player_4",
};


/*static*/ CPlayer* CPlayer::New(int32 nNo, int32 nControllerNo)
{
    ASSERT(nNo >= 0 && nNo < GAMETYPES::PLAYERS_MAX);

    CPlayer* pPlayer = new CPlayer(s_apszPlayerClassNameTable[nNo], nNo, nControllerNo);
    ASSERT(pPlayer);

    return pPlayer;
};


/*static*/ void CPlayer::Delete(CPlayer* pPlayer)
{
    ASSERT(pPlayer);
	delete pPlayer;
};


CPlayer::CPlayer(const char* pszName, int32 nNo, int32 nControllerNo)
: CGameObject(pszName, GAMEOBJECTTYPE::PLAYER)
, m_nNo(nNo)
, m_nControllerNo(nControllerNo)
, m_nNumCharacter(0)
, m_pCurrentCharacterInfo(nullptr)
, m_bSleep(false)
{
    for (int32 i = 0; i < COUNT_OF(m_aCharacterInfo); ++i)
    {
        m_aCharacterInfo[i].m_pCharacter = nullptr;
        m_aCharacterInfo[i].m_pManipulator = nullptr;
        m_aCharacterInfo[i].m_nIndex = i;
    };
};


CPlayer::~CPlayer(void)
{
    for (int32 i = 0; i < m_nNumCharacter; ++i)
    {
        CHARACTERINFO* pCharacterInfo = &m_aCharacterInfo[i];

        ASSERT(pCharacterInfo->m_pManipulator);
        ASSERT(pCharacterInfo->m_pCharacter);

        CManipulator::Delete(pCharacterInfo->m_pManipulator);
        pCharacterInfo->m_pManipulator = nullptr;
        
        CPlayerCharacter::Delete(pCharacterInfo->m_pCharacter);
        pCharacterInfo->m_pCharacter = nullptr;
    };
};


void CPlayer::Run(void)
{
    if (m_bSleep)
        return;
    
    if (!IsEnableCharacterChanging())
        return;

    if (m_nControllerNo >= IGamepad::Max())
        return;

	uint32 uDigitalTrigger = CController::GetDigitalTrigger(m_nControllerNo);

    if (IGamepad::CheckFunction(uDigitalTrigger, IGamepad::FUNCTION_SWITCH_CHR))
    {
        ChangeNextCharacter();
    };
};


void CPlayer::AddCharacter(PLAYERID::VALUE idChr, GAMETYPES::COSTUME costume)
{
    ASSERT(m_nNumCharacter >= 0 && m_nNumCharacter < COUNT_OF(m_aCharacterInfo));
    ASSERT(idChr >= PLAYERID::ID_START && idChr < PLAYERID::ID_MAX);

    CHARACTERINFO* pCharacterInfo = &m_aCharacterInfo[m_nNumCharacter];

    pCharacterInfo->m_pCharacter = CPlayerCharacter::New(idChr, costume);
    ASSERT(pCharacterInfo->m_pCharacter);
    pCharacterInfo->m_pCharacter->SetPlayerNo(m_nNo);
    pCharacterInfo->m_pCharacter->SetPadID(m_nControllerNo);

    pCharacterInfo->m_pManipulator = CManipulator::New(pCharacterInfo->m_pCharacter, m_nControllerNo);
    ASSERT(pCharacterInfo->m_pManipulator);

    if (m_pCurrentCharacterInfo)
    {
        SetSleepCharacter(pCharacterInfo);
    }
    else
    {
        SetAwakeCharacter(pCharacterInfo);
        m_pCurrentCharacterInfo = pCharacterInfo;
    };

    ++m_nNumCharacter;
};


void CPlayer::SetCurrentCharacter(PLAYERID::VALUE idChr, bool bPlayChangeEffect)
{
    CHARACTERINFO* pCharacterInfo = SearchCharacter(idChr);
    
    ASSERT(pCharacterInfo);
    ASSERT(m_pCurrentCharacterInfo);

    CPlayerCharacter* pBeforeCharacter = m_pCurrentCharacterInfo->m_pCharacter;
    CPlayerCharacter* pAfterCharacter = pCharacterInfo->m_pCharacter;
    
    SetSleepCharacter(m_pCurrentCharacterInfo);
    SetAwakeCharacter(pCharacterInfo);

    pBeforeCharacter->OnDetach(pCharacterInfo->m_pCharacter);
    pAfterCharacter->OnAttach(pBeforeCharacter, bPlayChangeEffect);

    m_pCurrentCharacterInfo = pCharacterInfo;
};


void CPlayer::GenerateItemEffect(ITEMID::VALUE idItem)
{
    ASSERT(m_pCurrentCharacterInfo);
    
    m_pCurrentCharacterInfo->m_pCharacter->GenerateItemEffect(idItem);
};


CPlayerCharacter* CPlayer::GetCharacter(int32 nIndex) const
{
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(m_aCharacterInfo));
    ASSERT(nIndex >= 0 && nIndex < m_nNumCharacter);
    
    return m_aCharacterInfo[nIndex].m_pCharacter;
};


PLAYERID::VALUE CPlayer::GetCharacterID(int32 nIndex) const
{
    CPlayerCharacter* pPlayerChr = GetCharacter(nIndex);
    if (pPlayerChr)
        return pPlayerChr->GetID();
    else
        return PLAYERID::ID_MAX;
};


CPlayerCharacter* CPlayer::GetCurrentCharacter(void) const
{
    ASSERT(m_pCurrentCharacterInfo);
    ASSERT(m_pCurrentCharacterInfo->m_pCharacter);
    
    return m_pCurrentCharacterInfo->m_pCharacter;
};


PLAYERID::VALUE CPlayer::GetCurrentCharacterID(void) const
{
    ASSERT(m_pCurrentCharacterInfo);
    ASSERT(m_pCurrentCharacterInfo->m_pCharacter);

    return m_pCurrentCharacterInfo->m_pCharacter->GetID();
};


int32 CPlayer::GetCharacterNum(void) const
{
    return m_nNumCharacter;
};


bool CPlayer::IsIncludedCharacter(PLAYERID::VALUE idChr)
{
    return (SearchCharacter(idChr) != nullptr);
};


void CPlayer::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(m_pCurrentCharacterInfo);
    ASSERT(m_pCurrentCharacterInfo->m_pCharacter);

    m_pCurrentCharacterInfo->m_pCharacter->GetBodyPosition(pvPosition);
};


void CPlayer::Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect)
{
    ASSERT(pvPosition);
    ASSERT(m_pCurrentCharacterInfo);
    ASSERT(m_pCurrentCharacterInfo->m_pCharacter);

    m_pCurrentCharacterInfo->m_pCharacter->Relocation(pvPosition, fDirection, bProtect);
};


void CPlayer::Sleep(void)
{
    if (!m_bSleep)
    {
        ASSERT(m_pCurrentCharacterInfo);
        
        SetSleepCharacter(m_pCurrentCharacterInfo);
        m_bSleep = true;
    };
};


void CPlayer::Awake(void)
{
    if (m_bSleep)
    {
        ASSERT(m_pCurrentCharacterInfo);

        SetAwakeCharacter(m_pCurrentCharacterInfo);
        m_bSleep = false;
    };
};


bool CPlayer::IsEnableCharacterChanging(void)
{
    ASSERT(m_pCurrentCharacterInfo);

    if (!m_pCurrentCharacterInfo)
        return false;

    ASSERT(m_pCurrentCharacterInfo->m_pCharacter);
    
    return m_pCurrentCharacterInfo->m_pCharacter->IsEnableCharacterChanging();
};


void CPlayer::ChangeNextCharacter(void)
{
    ASSERT(m_pCurrentCharacterInfo);
    ASSERT(m_pCurrentCharacterInfo->m_pCharacter);
    
    PLAYERID::VALUE idChrNext = GetNextPlayerID(m_pCurrentCharacterInfo);

    if (idChrNext != m_pCurrentCharacterInfo->m_pCharacter->GetID())
    {
        SetCurrentCharacter(idChrNext, true);
        ASSERT(m_pCurrentCharacterInfo->m_pCharacter);
        CGameSound::PlaySE(SDCODE_SE(4118));
    };
};


void CPlayer::SetAwakeCharacter(CHARACTERINFO* pCharacterInfo)
{
    ASSERT(pCharacterInfo);
    ASSERT(pCharacterInfo->m_pCharacter);

    CGameObjectManager::SendMessage(
        pCharacterInfo->m_pCharacter,
        GAMEOBJECTTYPES::MESSAGEID_AWAKE
    );
    
    CGameObjectManager::SendMessage(
        pCharacterInfo->m_pManipulator,
        GAMEOBJECTTYPES::MESSAGEID_AWAKE
    );
};


void CPlayer::SetSleepCharacter(CHARACTERINFO* pCharacterInfo)
{
    ASSERT(pCharacterInfo);
    ASSERT(pCharacterInfo->m_pCharacter);

    CGameObjectManager::SendMessage(
        pCharacterInfo->m_pCharacter,
        GAMEOBJECTTYPES::MESSAGEID_SLEEP
    );

    CGameObjectManager::SendMessage(
        pCharacterInfo->m_pManipulator,
        GAMEOBJECTTYPES::MESSAGEID_SLEEP
    );
};


PLAYERID::VALUE CPlayer::GetNextPlayerID(CHARACTERINFO* pCharacterInfo)
{
    ASSERT(pCharacterInfo);
    ASSERT(pCharacterInfo->m_pCharacter);

    int32 nIndex = (pCharacterInfo->m_nIndex + 1) % m_nNumCharacter;
    ASSERT(m_aCharacterInfo[nIndex].m_pCharacter);

    return m_aCharacterInfo[nIndex].m_pCharacter->GetID();
};


CPlayer::CHARACTERINFO* CPlayer::SearchCharacter(PLAYERID::VALUE idChr)
{
    for (int32 i = 0; i < m_nNumCharacter; ++i)
    {
        ASSERT(m_aCharacterInfo[i].m_pCharacter);
        
        if (m_aCharacterInfo[i].m_pCharacter->GetID() == idChr)
            return &m_aCharacterInfo[i];
    };

    return nullptr;
};


int32 CPlayer::GetPadID(void) const
{
    return m_nControllerNo;
};


int32 CPlayer::GetNo(void) const
{
    return m_nNo;
};