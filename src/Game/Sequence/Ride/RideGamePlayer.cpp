#include "RideGamePlayer.hpp"
#include "RidePlayer.hpp"
#include "RideCharacter.hpp"

#include "Game/System/GameObject/GameObjectManager.hpp"


CRideGamePlayer::CRideGamePlayer(void)
: m_nRefCount(0)
, m_nNo(-1)
, m_pPlayer(nullptr)
, m_vPosition(Math::VECTOR3_ZERO)
{
    ;
};


CRideGamePlayer::~CRideGamePlayer(void)
{
    DestroyPlayer();
};


void CRideGamePlayer::AddRef(void)
{
    ++m_nRefCount;
};


void CRideGamePlayer::Release(void)
{
    ASSERT(m_nRefCount > 0);
    --m_nRefCount;
};


bool CRideGamePlayer::IsAlive(void) const
{
    return (m_pPlayer != nullptr);
};


void CRideGamePlayer::GetPosition(RwV3d* pvPosition) const
{
    if (IsAlive())
    {
        Player().GetPosition(&m_vPosition);
        *pvPosition = m_vPosition;
    }
    else
    {
        *pvPosition = Math::VECTOR3_ZERO;
    };
};


void CRideGamePlayer::GetLastPosition(RwV3d* pvPosition) const
{
    ASSERT(false);
};


float CRideGamePlayer::GetRotY(void) const
{
    ASSERT(false);
	return 0.0f;
};


int32 CRideGamePlayer::GetPlayerNo(void) const
{
    return m_nNo;
};


CPlayer* CRideGamePlayer::GetPlayer(void) const
{
	return m_pPlayer;
};


CPlayerCharacter* CRideGamePlayer::GetCharacter(int32 nIndex) const
{
    return (IsAlive() ? Player().GetCharacter(nIndex) : nullptr);
};


CPlayerCharacter* CRideGamePlayer::GetCurrentCharacter(void) const
{
    ASSERT(false);
    return nullptr;
};


PLAYERID::VALUE CRideGamePlayer::GetCharacterID(int32 nIndex) const
{
    return (IsAlive() ? Player().GetCharacterID(nIndex) : PLAYERID::ID_MAX);
};


PLAYERID::VALUE CRideGamePlayer::GetCurrentCharacterID(void) const
{
    return (IsAlive() ? Player().GetCurrentCharacterID() : PLAYERID::ID_MAX);
};


int32 CRideGamePlayer::GetCharacterNum(void) const
{
    return (IsAlive() ? Player().GetCharacterNum() : 0);
};


PLAYERTYPES::STATUS CRideGamePlayer::GetStatus(void) const
{
    return (IsAlive() ? Player().GetCurrentCharacter()->GetStatus() : PLAYERTYPES::STATUS_INVALID);
};


int32 CRideGamePlayer::GetHPMax(void) const
{
    return 0;
};


int32 CRideGamePlayer::GetHP(void) const
{
    return 0;
};


int32 CRideGamePlayer::GetDamage(void) const
{
    return 0;
};


int32 CRideGamePlayer::GetShurikenMax(void) const
{
    return 0;
};


int32 CRideGamePlayer::GetShurikenNum(void) const
{
    return 0;
};


void CRideGamePlayer::Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect)
{
    (void)pvPosition;
    (void)fDirection;
    (void)bProtect;

    ASSERT(false);
};


int32 CRideGamePlayer::AddHP(int32 iHP)
{
    (void)iHP;

    ASSERT(false);
    return 0;
};


void CRideGamePlayer::AddShurikenNum(int32 iShuriken)
{
    (void)iShuriken;
    
    ASSERT(false);
};


void CRideGamePlayer::InvokeDeathFloor(void)
{
    if (IsAlive())
        CGameObjectManager::SendMessage(Player().GetCurrentCharacter(), CHARACTERTYPES::MESSAGEID_DEATHFLOOR);
};


void CRideGamePlayer::SaveContext(CGamePlayParam::PLAYERCONTEXT& rContext) const
{
    ASSERT(m_pPlayer);

    rContext.m_CharacterID = GetCurrentCharacterID();
};


void CRideGamePlayer::LoadContext(const CGamePlayParam::PLAYERCONTEXT& rContext)
{
    ASSERT(m_pPlayer);

    m_pPlayer->SetCurrentCharacter(rContext.m_CharacterID, false);
};


void CRideGamePlayer::CreatePlayer(int32 no)
{
    CreatePlayerEx(no, no);
};


void CRideGamePlayer::CreatePlayerEx(int32 no, int32 gamepadNo)
{
    ASSERT(!m_pPlayer);
    ASSERT(no >= 0 && no < GAMETYPES::PLAYERS_MAX);

    CRidePlayer* pPlayer = CRidePlayer::New(no, gamepadNo);
    if (pPlayer)
    {
        m_pPlayer = pPlayer;
        m_nNo = no;

        AddRef();
    };
};


void CRideGamePlayer::DestroyPlayer(void)
{
    if (m_pPlayer)
    {
        CPlayer::Delete(m_pPlayer);
        m_pPlayer = nullptr;
    };
};


void CRideGamePlayer::AddPlayerCharacter(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    if (IsAlive())
    {
        Player().AddCharacter(idPlayer, costume);
        Player().SetCurrentCharacter(idPlayer, false);
    };
};


CRidePlayer& CRideGamePlayer::Player(void) const
{
    ASSERT(m_pPlayer);
    return *m_pPlayer;
};