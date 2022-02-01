#include "RideGamePlayer.hpp"
#include "RidePlayer.hpp"
#include "RideCharacter.hpp"

#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


/*static*/ CRideGamePlayer& CRideGamePlayer::Dummy(void)
{
    static CRideGamePlayer s_DummyRideGameplayer;
    return s_DummyRideGameplayer;
};


CRideGamePlayer::CRideGamePlayer(void)
: m_nNo(0)
, m_hObj(0)
, m_pPlayer(nullptr)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vLastPosition(Math::VECTOR3_ZERO)
{
    ;
};


CRideGamePlayer::~CRideGamePlayer(void)
{
    DestroyPlayer();
};


void CRideGamePlayer::GenerateItemEffect(ITEMID::VALUE idItem)
{
    ASSERT(false);
};


void CRideGamePlayer::Awake(void)
{
    if (IsAlive())
        Player().Awake();
};


void CRideGamePlayer::Sleep(void)
{
    if (IsAlive())
        Player().Sleep();
};


bool CRideGamePlayer::IsIncludedCharacter(PLAYERID::VALUE idCharacter) const
{
    if (IsAlive())
        return Player().IsIncludedCharacter(idCharacter);
    else
        return false;
};


bool CRideGamePlayer::IsAlive(void) const
{
    CGameObject* pObject = CGameObjectManager::GetObject(m_hObj);
    if (!pObject)
        return false;

    ASSERT(pObject == m_pPlayer);

    return true;
};


int32 CRideGamePlayer::GetPlayerNo(void) const
{
    return m_nNo;
};


CPlayerCharacter* CRideGamePlayer::GetCharacter(int32 nIndex) const
{
    return Player().GetCharacter(nIndex);
};


CPlayerCharacter* CRideGamePlayer::GetCurrentCharacter(void) const
{
    return Player().GetCurrentCharacter();
};


PLAYERID::VALUE CRideGamePlayer::GetCharacterID(int32 nIndex) const
{
    if (IsAlive())
        return Player().GetCharacter(nIndex)->GetID();
    else
        return PLAYERID::ID_INVALID;
};


PLAYERID::VALUE CRideGamePlayer::GetCurrentCharacterID(void) const
{
    if (IsAlive())
        return Player().GetCurrentCharacterID();
    else
        return PLAYERID::ID_INVALID;
};


void CRideGamePlayer::GetPosition(RwV3d* pvPosition)
{
    ASSERT(pvPosition);

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


void CRideGamePlayer::GetLastPosition(RwV3d* pvPosition)
{
    ASSERT(false);
};


float CRideGamePlayer::GetRotY(void) const
{
    ASSERT(false);
    return 0.0f;
};


PLAYERTYPES::STATUS CRideGamePlayer::GetStatus(void) const
{
    if (IsAlive())
        return GetCurrentCharacter()->GetStatus();
    else
        return PLAYERTYPES::STATUS_INVALID;
};


int32 CRideGamePlayer::GetCharacterNum(void) const
{
    if (IsAlive())
        return Player().GetCharacterNum();
    else
        return 0;
};


int32 CRideGamePlayer::GetHPMax(void) const
{
    return 0;
};


int32 CRideGamePlayer::GetHP(void)
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


int32 CRideGamePlayer::AddHP(int32 iHP)
{
    ASSERT(false);
	return 0;
};


void CRideGamePlayer::AddShurikenNum(int32 iShuriken)
{
    ASSERT(false);
};


void CRideGamePlayer::InvokeDeathFloor(void)
{
    if (IsAlive())
    {
        CGameObjectManager::SendMessage(
            Player().GetCurrentCharacter(),
            CHARACTERTYPES::MESSAGEID_DEATHFLOOR
        );
    };
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


void CRideGamePlayer::Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect)
{
    ASSERT(false);
};


int32 CRideGamePlayer::GetPadID(void) const
{
    if (IsAlive())
        return Player().GetPadID();
    else
        return CGamepad::Invalid();
};


int32 CRideGamePlayer::GetScore(int32 scorekind) const
{
    ASSERT(m_pPlayer);
    ASSERT(scorekind >= 0 && scorekind < RIDETYPES::SCOREKINDNUM);
    return m_pPlayer->GetScore(RIDETYPES::SCOREKIND(scorekind));
};


CRidePlayer& CRideGamePlayer::Player(void) const
{
    ASSERT(m_pPlayer);
    return *m_pPlayer;
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
    ASSERT(pPlayer);

    m_pPlayer = pPlayer;
    m_hObj = pPlayer->GetHandle();
    m_nNo = no;
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