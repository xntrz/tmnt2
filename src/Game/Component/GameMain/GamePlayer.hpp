#pragma once

#include "GameTypes.hpp"
#include "ItemID.hpp"
#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameData/GamePlayParam.hpp"


class CPlayer;
class CPlayerCharacter;


class CGamePlayerSharedProperty final
{
public:
    CGamePlayerSharedProperty(void);
    ~CGamePlayerSharedProperty(void);
    void Reset(void);
    int32 AddHP(int32 nHP);
    void SetHP(int32 nHP);
    void SetHPMax(int32 nHPMax);
    int32 GetHP(void) const;
    int32 GetHPMax(void) const;

private:
    int32 m_nHP;
    int32 m_nHPMax;
};


class CGamePlayerProperty
{
public:
    CGamePlayerProperty(void);
    ~CGamePlayerProperty(void);
    void Setup(bool bResetShared);
    void Cleanup(void);
    CGamePlayerSharedProperty& SharedProperty(void) const;
    int32 AddHP(int32 nHP);
    void SetHP(int32 nHP);
    void SetHPMax(int32 nHPMax);
    void SetDamage(int32 nDmg);
    void AddShurikenNum(int32 nShurikenNum);
    void SetShurikenNum(int32 nShurikenNum);
    void SetShurikenMax(int32 nShurikenMax);
    int32 GetHP(void) const;
    int32 GetHPMax(void) const;
    int32 GetDamage(void) const;
    int32 GetShurikenNum(void) const;
    int32 GetShurikenMax(void) const;
    
private:
    static CGamePlayerSharedProperty m_SharedProperty;
    int32 m_nTotalDamage;
    int32 m_nShuriken;
    int32 m_nShurikenMax;
};


class IGamePlayer
{
public:
    virtual void GenerateItemEffect(ITEMID::VALUE idItem) = 0;
    virtual void Awake(void) = 0;
    virtual void Sleep(void) = 0;
    virtual bool IsIncludedCharacter(PLAYERID::VALUE idCharacter) const = 0;
    virtual bool IsAlive(void) const = 0;
    virtual int32 GetPlayerNo(void) const = 0;
    virtual CPlayerCharacter* GetCharacter(int32 nIndex) const = 0;
    virtual CPlayerCharacter* GetCurrentCharacter(void) const = 0;
    virtual PLAYERID::VALUE GetCharacterID(int32 nIndex) const = 0;
    virtual PLAYERID::VALUE GetCurrentCharacterID(void) const = 0;
    virtual void GetPosition(RwV3d* pvPosition) = 0;
    virtual void GetLastPosition(RwV3d* pvPosition) = 0;
    virtual float GetRotY(void) const = 0;
    virtual PLAYERTYPES::STATUS GetStatus(void) const = 0;
    virtual int32 GetCharacterNum(void) const = 0;
    virtual int32 GetHPMax(void) const = 0;
    virtual int32 GetHP(void) = 0;
    virtual int32 GetDamage(void) const = 0;
    virtual int32 GetShurikenMax(void) const = 0;
    virtual int32 GetShurikenNum(void) const = 0;
    virtual int32 AddHP(int32 iHP) = 0;
    virtual void AddShurikenNum(int32 iShuriken) = 0;
    virtual void InvokeDeathFloor(void) = 0;
    virtual void SaveContext(CGamePlayParam::PLAYERCONTEXT& rContext) const = 0;
    virtual void LoadContext(const CGamePlayParam::PLAYERCONTEXT& rContext) = 0;
    virtual void Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect) = 0;
    virtual int32 GetPadID(void) const = 0;
    virtual int32 GetScore(int32 scorekind = -1) const = 0;
};


class CGamePlayer : public IGamePlayer
{
public:
    static CGamePlayer& Dummy(void);
    static void SetDamageNegation(bool bSet);
    static void ClearDamageNegation(void);
    
    CGamePlayer(void);
    virtual ~CGamePlayer(void);
    virtual void GenerateItemEffect(ITEMID::VALUE idItem) override;
    virtual void Awake(void) override;
    virtual void Sleep(void) override;
    virtual bool IsIncludedCharacter(PLAYERID::VALUE idCharacter) const override;
    virtual bool IsAlive(void) const override;
    virtual int32 GetPlayerNo(void) const override;
    virtual CPlayerCharacter* GetCharacter(int32 nIndex) const override;
    virtual CPlayerCharacter* GetCurrentCharacter(void) const override;
    virtual PLAYERID::VALUE GetCharacterID(int32 nIndex) const override;
    virtual PLAYERID::VALUE GetCurrentCharacterID(void) const override;
    virtual void GetPosition(RwV3d* pvPosition) override;
    virtual void GetLastPosition(RwV3d* pvPosition) override;
    virtual float GetRotY(void) const override;
    virtual PLAYERTYPES::STATUS GetStatus(void) const override;
    virtual int32 GetCharacterNum(void) const override;
    virtual int32 GetHPMax(void) const override;
    virtual int32 GetHP(void) override;
    virtual int32 GetDamage(void) const override;
    virtual int32 GetShurikenMax(void) const override;
    virtual int32 GetShurikenNum(void) const override;
    virtual int32 AddHP(int32 iHP) override;
    virtual void AddShurikenNum(int32 iShuriken) override;
    virtual void InvokeDeathFloor(void) override;
    virtual void SaveContext(CGamePlayParam::PLAYERCONTEXT& rContext) const override;
    virtual void LoadContext(const CGamePlayParam::PLAYERCONTEXT& rContext) override;
    virtual void Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect) override;
    virtual int32 GetPadID(void) const override;
    virtual int32 GetScore(int32 scorekind) const override;
    CPlayer& Player(void) const;
    void CreatePlayer(int32 no);
    void CreatePlayerEx(int32 no, int32 gamepadNo);
    void DestroyPlayer(void);
    void AddPlayerCharacter(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    bool IsInvincible(void) const;

private:
    static int32 m_nDamageNegationCount;
    int32 m_nRefCount;
    int32 m_nNo;
    uint32 m_hObj;
    CPlayer* m_pPlayer;
    CGamePlayerProperty m_property;
    RwV3d m_vPosition;
    RwV3d m_vLastPosition;
};