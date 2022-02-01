#include "GamePlayer.hpp"
#include "GameProperty.hpp"
#include "GameEvent.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/Player.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Misc/Gamepad.hpp"


CGamePlayerSharedProperty::CGamePlayerSharedProperty(void)
: m_nHP(0)
, m_nHPMax(0)
{
    ;
};


CGamePlayerSharedProperty::~CGamePlayerSharedProperty(void)
{
    ;
};


void CGamePlayerSharedProperty::Reset(void)
{
    m_nHP = 0;
    m_nHPMax = 0;
};


int32 CGamePlayerSharedProperty::AddHP(int32 nHP)
{
    m_nHP = Math::Clamp(m_nHP + nHP, 0, m_nHPMax);
    return m_nHP;
};


void CGamePlayerSharedProperty::SetHP(int32 nHP)
{
    m_nHP = nHP;
};


void CGamePlayerSharedProperty::SetHPMax(int32 nHPMax)
{
    m_nHPMax = nHPMax;
};


int32 CGamePlayerSharedProperty::GetHP(void) const
{
    return m_nHP;
};


int32 CGamePlayerSharedProperty::GetHPMax(void) const
{
    return m_nHPMax;
};


/*static*/ CGamePlayerSharedProperty CGamePlayerProperty::m_SharedProperty;


CGamePlayerProperty::CGamePlayerProperty(void)
: m_nTotalDamage(0)
, m_nShuriken(0)
, m_nShurikenMax(0)
{
    ;
};


CGamePlayerProperty::~CGamePlayerProperty(void)
{
    ;
};


void CGamePlayerProperty::Setup(bool bResetShared)
{
    if (bResetShared)
        SharedProperty().Reset();

    m_nTotalDamage = 0;
    m_nShuriken = 0;
    m_nShurikenMax = 0;
};


void CGamePlayerProperty::Cleanup(void)
{
    ;
};


CGamePlayerSharedProperty& CGamePlayerProperty::SharedProperty(void) const
{
    return m_SharedProperty;
};


int32 CGamePlayerProperty::AddHP(int32 nHP)
{
    if (nHP < 0)
        m_nTotalDamage -= nHP;

    return SharedProperty().AddHP(nHP);
};


void CGamePlayerProperty::SetHP(int32 nHP)
{
    SharedProperty().SetHP(nHP);
};


void CGamePlayerProperty::SetHPMax(int32 nHPMax)
{
    SharedProperty().SetHPMax(nHPMax);
};


void CGamePlayerProperty::SetDamage(int32 nDmg)
{
    m_nTotalDamage = nDmg;
};


void CGamePlayerProperty::AddShurikenNum(int32 nShurikenNum)
{
    m_nShuriken = Math::Clamp(m_nShuriken + nShurikenNum, 0, m_nShurikenMax);
};


void CGamePlayerProperty::SetShurikenNum(int32 nShurikenNum)
{
    m_nShuriken = nShurikenNum;
};


void CGamePlayerProperty::SetShurikenMax(int32 nShurikenMax)
{
    m_nShurikenMax = nShurikenMax;
};


int32 CGamePlayerProperty::GetHP(void) const
{
    return SharedProperty().GetHP();
};


int32 CGamePlayerProperty::GetHPMax(void) const
{
    return SharedProperty().GetHPMax();
};


int32 CGamePlayerProperty::GetDamage(void) const
{
    return m_nTotalDamage;
};


int32 CGamePlayerProperty::GetShurikenNum(void) const
{
    return m_nShuriken;
};


int32 CGamePlayerProperty::GetShurikenMax(void) const
{
    return m_nShurikenMax;
};


/*static*/ CGamePlayer& CGamePlayer::Dummy(void)
{
    static CGamePlayer s_DummyGameplayer;
    
    return s_DummyGameplayer;
};


/*static*/ void CGamePlayer::SetDamageNegation(bool bSet)
{
    if (bSet)
    {
        ++m_nDamageNegationCount;
    }
    else
    {
        if (m_nDamageNegationCount > 0)
            --m_nDamageNegationCount;
    };
};


/*static*/ void CGamePlayer::ClearDamageNegation(void)
{
    m_nDamageNegationCount = 0;
};


/*static*/ int32 CGamePlayer::m_nDamageNegationCount = 0;


CGamePlayer::CGamePlayer(void)
: m_nRefCount(0)
, m_nNo(-1)
, m_hObj(0)
, m_pPlayer(nullptr)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vLastPosition(Math::VECTOR3_ZERO)
{
    ;
};


CGamePlayer::~CGamePlayer(void)
{
    DestroyPlayer();
};


void CGamePlayer::GenerateItemEffect(ITEMID::VALUE idItem)
{
    if (IsAlive())
        Player().GenerateItemEffect(idItem);
};


void CGamePlayer::Awake(void)
{
    if (IsAlive())
        Player().Awake();    
};


void CGamePlayer::Sleep(void)
{
    if (IsAlive())
        Player().Sleep();
};


bool CGamePlayer::IsIncludedCharacter(PLAYERID::VALUE idCharacter) const
{
    if (IsAlive())
        return Player().IsIncludedCharacter(idCharacter);
    else
        return false;
};


bool CGamePlayer::IsAlive(void) const
{
    CGameObject* pObject = CGameObjectManager::GetObject(m_hObj);
    if (!pObject)
        return false;

    ASSERT(pObject == m_pPlayer);

    return true;
};


int32 CGamePlayer::GetPlayerNo(void) const
{
    return m_nNo;
};


CPlayerCharacter* CGamePlayer::GetCharacter(int32 nIndex) const
{
    return Player().GetCharacter(nIndex);
};


CPlayerCharacter* CGamePlayer::GetCurrentCharacter(void) const
{
    return Player().GetCurrentCharacter();
};


PLAYERID::VALUE CGamePlayer::GetCharacterID(int32 nIndex) const
{
    if (IsAlive())
        return Player().GetCharacter(nIndex)->GetID();
    else
        return PLAYERID::ID_INVALID;
};


PLAYERID::VALUE CGamePlayer::GetCurrentCharacterID(void) const
{
    if (IsAlive())
        return Player().GetCurrentCharacterID();
    else
        return PLAYERID::ID_INVALID;
};


void CGamePlayer::GetPosition(RwV3d* pvPosition)
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


void CGamePlayer::GetLastPosition(RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    if (IsAlive())
    {
        CPlayerCharacter* pCurrentPlayerChr = m_pPlayer->GetCurrentCharacter();
        ASSERT(pCurrentPlayerChr);

        pCurrentPlayerChr->GetReplacePosition(&m_vLastPosition);
        *pvPosition = m_vLastPosition;
    }
    else
    {
        *pvPosition = Math::VECTOR3_ZERO;
    };
};


float CGamePlayer::GetRotY(void) const
{
    if (IsAlive())
        return GetCurrentCharacter()->GetDirection();
    else
        return 0.0f;
};


PLAYERTYPES::STATUS CGamePlayer::GetStatus(void) const
{
    if (IsAlive())
        return GetCurrentCharacter()->GetStatus();
    else
        return PLAYERTYPES::STATUS_INVALID;
};


int32 CGamePlayer::GetCharacterNum(void) const
{
    if (IsAlive())
        return Player().GetCharacterNum();
    else
        return 0;
};


int32 CGamePlayer::GetHPMax(void) const
{
    if (IsAlive())
        return m_property.GetHPMax();
    else
        return 0;
};


int32 CGamePlayer::GetHP(void)
{
    if (IsAlive())
        return m_property.GetHP();
    else
        return 0;
};


int32 CGamePlayer::GetDamage(void) const
{
    if (IsAlive())
        return m_property.GetDamage();
    else
        return 0;
};


int32 CGamePlayer::GetShurikenMax(void) const
{
    if (IsAlive())
        return m_property.GetShurikenMax();
    else
        return 0;
};


int32 CGamePlayer::GetShurikenNum(void) const
{
    if (IsAlive())
        return m_property.GetShurikenNum();
    else
        return 0;
};


int32 CGamePlayer::AddHP(int32 iHP)
{
    if (!IsAlive())
        return 0;

    if (!IsInvincible() || iHP > 0)
    {
        if (m_property.AddHP(iHP) <= 0)
        {
            if (CGameData::Record().Item().IsComebackProcessed())
            {
                m_property.AddHP(m_property.GetHPMax());
                CGameData::Record().Item().SetComebackUsed();
            }
            else
            {
                CGameEvent::SetPlayerDead(m_nNo);
            };
        };

		return m_property.GetHP();
    }
	else
	{
		return 0;
	};
};


void CGamePlayer::AddShurikenNum(int32 iShuriken)
{
    if (IsAlive())
    {
        m_property.AddShurikenNum(iShuriken);
    };
};


void CGamePlayer::InvokeDeathFloor(void)
{
    if (IsAlive())
    {
        CGameObjectManager::SendMessage(
            Player().GetCurrentCharacter(),
            CHARACTERTYPES::MESSAGEID_DEATHFLOOR
        );
    };
};


void CGamePlayer::SaveContext(CGamePlayParam::PLAYERCONTEXT& rContext) const
{
    rContext.m_iHP = m_property.GetHP();
    rContext.m_iHPMax = m_property.GetHPMax();
    rContext.m_iDamage = m_property.GetDamage();
    rContext.m_CharacterID = GetCurrentCharacterID();
    rContext.m_iShurikenMax = m_property.GetShurikenMax();
    rContext.m_iShurikenNum = m_property.GetShurikenNum();
};


void CGamePlayer::LoadContext(const CGamePlayParam::PLAYERCONTEXT& rContext)
{
    m_property.SetDamage(rContext.m_iDamage);
    m_property.SetHP(rContext.m_iHP);
    m_property.SetHPMax(rContext.m_iHPMax);
    m_property.SetShurikenNum(rContext.m_iShurikenNum);
    m_property.SetShurikenMax(rContext.m_iShurikenMax);

    ASSERT(m_pPlayer);
    m_pPlayer->SetCurrentCharacter(rContext.m_CharacterID, false);
};


void CGamePlayer::Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect)
{
    if (IsAlive())
        Player().Relocation(pvPosition, fDirection, bProtect);
};


int32 CGamePlayer::GetPadID(void) const
{
    if (IsAlive())
        return Player().GetPadID();
    else
        return CGamepad::Invalid();
};


int32 CGamePlayer::GetScore(int32 scorekind) const
{
    ASSERT(false);
    return 0;
};


CPlayer& CGamePlayer::Player(void) const
{
    ASSERT(m_pPlayer);
    return *m_pPlayer;
};


void CGamePlayer::CreatePlayer(int32 no)
{
    CreatePlayerEx(no, no);
};


void CGamePlayer::CreatePlayerEx(int32 no, int32 gamepadNo)
{
    ASSERT(!m_pPlayer);
    ASSERT(no >= 0 && no < GAMETYPES::PLAYERS_MAX);
    
    CPlayer* pPlayer = CPlayer::New(no, gamepadNo);
    ASSERT(pPlayer);

    m_pPlayer = pPlayer;
    m_hObj = pPlayer->GetHandle();
    m_nNo = no;
    m_property.Setup(m_nNo == 0);
};


void CGamePlayer::DestroyPlayer(void)
{
    if (m_pPlayer)
    {
        m_property.Cleanup();
        
        CPlayer::Delete(m_pPlayer);
        m_pPlayer = nullptr;
    };
};


void CGamePlayer::AddPlayerCharacter(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    if (IsAlive())
    {
        Player().AddCharacter(idPlayer, costume);
        Player().SetCurrentCharacter(idPlayer, false);
    };
};


bool CGamePlayer::IsInvincible(void) const
{
    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_MIGHTYTURTLE))
    {
        return true;
    }
    else
    {
        return (m_nDamageNegationCount > 0);
    };
};