#include "GamePlayer.hpp"
#include "GameProperty.hpp"
#include "GameEvent.hpp"
#include "GamePlayerContainer.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/Player.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


CGamePlayerSharedProperty::CGamePlayerSharedProperty(void)
: m_bDynamicCreation(false)
, m_nRefCnt(0)
, m_nHP(0)
, m_nHPMax(0)
{
    ;
};


CGamePlayerSharedProperty::~CGamePlayerSharedProperty(void)
{
    ;
};


void CGamePlayerSharedProperty::AddRef(void)
{
    ++m_nRefCnt;
};


void CGamePlayerSharedProperty::Release(void)
{
    if (!--m_nRefCnt)
    {
        if (m_bDynamicCreation)
            delete this;
    };
};


void CGamePlayerSharedProperty::Reset(void)
{
    m_nHP = 0;
    m_nHPMax = 0;
};


int32 CGamePlayerSharedProperty::AddHP(int32 nHP)
{
    m_nHP = Clamp(m_nHP + nHP, 0, m_nHPMax);
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


//
// *********************************************************************************
//


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
    SharedProperty().AddRef();
    
    if (bResetShared)
        SharedProperty().Reset();

    m_nTotalDamage = 0;
    m_nShuriken = 0;
    m_nShurikenMax = 0;
};


void CGamePlayerProperty::Cleanup(void)
{
    SharedProperty().Release();
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
    m_nShuriken = Clamp(m_nShuriken + nShurikenNum, 0, m_nShurikenMax);
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


//
// *********************************************************************************
//


/*static*/ CGamePlayer CGamePlayer::m_dummy;
/*static*/ int32 CGamePlayer::m_nDamageNegationCount = 0;
/*static*/ CGamePlayerContainer* CGamePlayer::m_pContainer = nullptr;


/*static*/ CGamePlayer* CGamePlayer::Dummy(void)
{
    return &m_dummy;
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


/*static*/ void CGamePlayer::AttachContainer(CGamePlayerContainer* pContainer)
{
    m_pContainer = pContainer;
};


/*static*/ void CGamePlayer::DetachContainer(void)
{
    m_pContainer = nullptr;
};


CGamePlayer::CGamePlayer(void)
: m_nRefCount(0)
, m_nNo(-1)
, m_hObj(0)
, m_pPlayer(nullptr)
, m_property()
, m_vPosition(Math::VECTOR3_ZERO)
, m_vLastPosition(Math::VECTOR3_ZERO)
{
    ;
};


CGamePlayer::~CGamePlayer(void)
{
    DestroyPlayer();
};


void CGamePlayer::AddRef(void)
{
    ++m_nRefCount;
};


void CGamePlayer::Release(void)
{
    if (!--m_nRefCount)
    {
        if (m_pContainer)
            m_pContainer->Remove(this);
        else
            delete this;
    };
};


bool CGamePlayer::IsAlive(void) const
{
    return (CGameObjectManager::GetObject(m_hObj) != nullptr);
};


void CGamePlayer::GetPosition(RwV3d* pvPosition) const
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


void CGamePlayer::GetLastPosition(RwV3d* pvPosition) const
{
    if (IsAlive())
    {
        Player().GetCurrentCharacter()->GetReplacePosition(&m_vLastPosition);
        *pvPosition = m_vLastPosition;
    }
    else
    {
        *pvPosition = Math::VECTOR3_ZERO;
    };
};


float CGamePlayer::GetRotY(void) const
{
    return (IsAlive() ? Player().GetCurrentCharacter()->GetDirection() : 0.0f);
};


int32 CGamePlayer::GetPlayerNo(void) const
{
    return m_nNo;
};


CPlayer* CGamePlayer::GetPlayer(void) const
{
    return m_pPlayer;
};


CPlayerCharacter* CGamePlayer::GetCharacter(int32 nIndex) const
{
    return (IsAlive() ? Player().GetCharacter(nIndex) : nullptr);
};


CPlayerCharacter* CGamePlayer::GetCurrentCharacter(void) const
{
    return (IsAlive() ? Player().GetCurrentCharacter() : nullptr);
};


PLAYERID::VALUE CGamePlayer::GetCharacterID(int32 nIndex) const
{
    return (IsAlive() ? Player().GetCharacterID(nIndex) : PLAYERID::ID_MAX);
};


PLAYERID::VALUE CGamePlayer::GetCurrentCharacterID(void) const
{
    return (IsAlive() ? Player().GetCurrentCharacterID() : PLAYERID::ID_MAX);
};


int32 CGamePlayer::GetCharacterNum(void) const
{
    return (IsAlive() ? Player().GetCharacterNum() : 0);
};


PLAYERTYPES::STATUS CGamePlayer::GetStatus(void) const
{
    return (IsAlive() ? Player().GetCurrentCharacter()->GetStatus() : PLAYERTYPES::STATUS_INVALID);
};


int32 CGamePlayer::GetHPMax(void) const
{
    return (IsAlive() ? m_property.GetHPMax() : 0);
};


int32 CGamePlayer::GetHP(void) const
{
    return (IsAlive() ? m_property.GetHP() : 0);
};


int32 CGamePlayer::GetDamage(void) const
{
    return (IsAlive() ? m_property.GetDamage() : 0);
};


int32 CGamePlayer::GetShurikenMax(void) const
{
    return (IsAlive() ? m_property.GetShurikenMax() : 0);
};


int32 CGamePlayer::GetShurikenNum(void) const
{
    return (IsAlive() ? m_property.GetShurikenNum() : 0);
};


void CGamePlayer::Relocation(const RwV3d* pvPosition, float fDirection, bool bBlink)
{
    if (IsAlive())
        Player().Relocation(pvPosition, fDirection, bBlink);
};


int32 CGamePlayer::AddHP(int32 iHP)
{
    if (!IsAlive())
        return 0;

    if (!isInvincible() || (iHP > 0))
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
    };

    return 0;
};


void CGamePlayer::AddShurikenNum(int32 iShuriken)
{
    if (IsAlive())
        m_property.AddShurikenNum(iShuriken);
};


void CGamePlayer::InvokeDeathFloor(void)
{
    if (IsAlive())
        CGameObjectManager::SendMessage(Player().GetCurrentCharacter(), CHARACTERTYPES::MESSAGEID_DEATHFLOOR);
};


void CGamePlayer::SaveContext(CGamePlayParam::PLAYERCONTEXT& rContext) const
{
    rContext.m_iHP          = m_property.GetHP();
    rContext.m_iHPMax       = m_property.GetHPMax();
    rContext.m_iDamage      = m_property.GetDamage();
    rContext.m_CharacterID  = GetCurrentCharacterID();
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


void CGamePlayer::CreatePlayer(int32 no)
{
    CreatePlayerEx(no, no);
};


void CGamePlayer::CreatePlayerEx(int32 no, int32 gamepadNo)
{
    ASSERT(no >= 0);
    ASSERT(no < GAMETYPES::PLAYERS_MAX);
    ASSERT(m_pPlayer == nullptr);

    CPlayer* pPlayer = CPlayer::New(no, gamepadNo);
    if (pPlayer)
    {
        m_pPlayer = pPlayer;
        m_hObj    = pPlayer->GetHandle();
        m_nNo     = no;

        m_property.Setup(m_nNo == 0);

        AddRef();
    };
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


void CGamePlayer::AddCharacter(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    if (IsAlive())
    {
        Player().AddCharacter(idPlayer, costume);
        Player().SetCurrentCharacter(idPlayer, false);
    };
};


CPlayer& CGamePlayer::Player(void) const
{
    return *m_pPlayer;
};


bool CGamePlayer::isInvincible(void) const
{
    return (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_MIGHTYTURTLE) || (m_nDamageNegationCount > 0));
};