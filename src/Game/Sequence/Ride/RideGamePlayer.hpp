#pragma once

#include "Game/Component/GameMain/GamePlayer.hpp"


class CRidePlayer;


class CRideGamePlayer : public IGamePlayer
{
public:
    CRideGamePlayer(void);
    virtual ~CRideGamePlayer(void);
    virtual void AddRef(void) override;
    virtual void Release(void) override;
    virtual bool IsAlive(void) const override;
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual void GetLastPosition(RwV3d* pvPosition) const override;
    virtual float GetRotY(void) const override;
    virtual int32 GetPlayerNo(void) const override;
    virtual CPlayer* GetPlayer(void) const override;
    virtual CPlayerCharacter* GetCharacter(int32 nIndex) const override;
    virtual CPlayerCharacter* GetCurrentCharacter(void) const override;
    virtual PLAYERID::VALUE GetCharacterID(int32 nIndex) const override;
    virtual PLAYERID::VALUE GetCurrentCharacterID(void) const override;
    virtual int32 GetCharacterNum(void) const override;
    virtual PLAYERTYPES::STATUS GetStatus(void) const override;
    virtual int32 GetHPMax(void) const override;
    virtual int32 GetHP(void) const override;
    virtual int32 GetDamage(void) const override;
    virtual int32 GetShurikenMax(void) const override;
    virtual int32 GetShurikenNum(void) const override;
    virtual void Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect) override;
    virtual int32 AddHP(int32 iHP) override;
    virtual void AddShurikenNum(int32 iShuriken) override;
    virtual void InvokeDeathFloor(void) override;
    virtual void SaveContext(CGamePlayParam::PLAYERCONTEXT& rContext) const override;
    virtual void LoadContext(const CGamePlayParam::PLAYERCONTEXT& rContext) override;
    void CreatePlayer(int32 no);
    void CreatePlayerEx(int32 no, int32 gamepadNo);
    void DestroyPlayer(void);
    void AddPlayerCharacter(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    CRidePlayer& Player(void) const;

private:
    int32 m_nRefCount;
    int32 m_nNo;
    CRidePlayer* m_pPlayer;
    mutable RwV3d m_vPosition;
};