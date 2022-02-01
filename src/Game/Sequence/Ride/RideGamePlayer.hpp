#pragma once

#include "Game/Component/GameMain/GamePlayer.hpp"


class CRidePlayer;


class CRideGamePlayer : public IGamePlayer
{
public:
    static CRideGamePlayer& Dummy(void);
    
    CRideGamePlayer(void);
    virtual ~CRideGamePlayer(void);
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
    CRidePlayer& Player(void) const;
    void CreatePlayer(int32 no);
    void CreatePlayerEx(int32 no, int32 gamepadNo);
    void DestroyPlayer(void);
    void AddPlayerCharacter(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);

private:
    int32 m_nNo;
    uint32 m_hObj;
    CRidePlayer* m_pPlayer;
    RwV3d m_vPosition;
    RwV3d m_vLastPosition;
};