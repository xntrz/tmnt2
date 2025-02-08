#pragma once

#include "PlayerID.hpp"

#include "Game/System/GameObject/GameObject.hpp"
#include "Game/Component/GameMain/ItemID.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CPlayerCharacter;
class CManipulator;


class CPlayer : public CGameObject
{
protected:
    struct CHARACTERINFO
    {
        CPlayerCharacter* m_pCharacter;
        CManipulator* m_pManipulator;
        int32 m_nIndex;
    };

public:
    static CPlayer* New(int32 nNo, int32 nControllerNo);
    static void Delete(CPlayer* pPlayer);
    
    CPlayer(const char* pszName, int32 nNo, int32 nControllerNo);
    virtual ~CPlayer(void);
    virtual void Run(void) override;
    virtual void SetCurrentCharacter(PLAYERID::VALUE idChr, bool bPlayChangeEffect);
    void AddCharacter(PLAYERID::VALUE idChr, GAMETYPES::COSTUME costume);
    void GenerateItemEffect(ITEMID::VALUE idItem);
    int32 GetCharacterNum(void) const;
    CPlayerCharacter* GetCharacter(int32 nIndex) const;
    PLAYERID::VALUE GetCharacterID(int32 nIndex) const;
    CPlayerCharacter* GetCurrentCharacter(void) const;
    PLAYERID::VALUE GetCurrentCharacterID(void) const;
    bool IsIncludedCharacter(PLAYERID::VALUE idChr);
    void GetPosition(RwV3d* pvPosition) const;
    void Relocation(const RwV3d* pvPosition, float fDirection, bool bBlink);
    void Sleep(void);
    void Awake(void);
    bool IsEnableCharacterChanging(void);
    void ChangeNextCharacter(void);
    void SetAwakeCharacter(CHARACTERINFO* pCharacterInfo);
    void SetSleepCharacter(CHARACTERINFO* pCharacterInfo);
    PLAYERID::VALUE GetNextPlayerID(CHARACTERINFO* pCharacterInfo);
    CHARACTERINFO* SearchCharacter(PLAYERID::VALUE idChr);
    int32 GetPadID(void) const;
    int32 GetNo(void) const;

protected:
    int32 m_nNo;
    int32 m_nControllerNo;
    CHARACTERINFO m_aCharacterInfo[GAMETYPES::CHARACTER_MAX];
    int32 m_nNumCharacter;
    CHARACTERINFO* m_pCurrentCharacterInfo;
    bool m_bSleep;
};