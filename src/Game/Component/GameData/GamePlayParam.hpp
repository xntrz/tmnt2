#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameMain/AreaID.hpp"
#include "Game/Component/GameMain/StageID.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CGamePlayParam
{
public:
    enum AREAPLAYMODE
    {
        AREAPLAYMODE_SERIES = 0,
        AREAPLAYMODE_ONE_STAGE,        
    };
    
    struct CHARAINFO
    {
        void Init(void);

        PLAYERID::VALUE m_CharacterID;
        GAMETYPES::COSTUME m_Costume;
        int32 m_iPlayerNo;
    };

    struct PLAYERCONTEXT
    {
        void Init(int32 no);
        void AddChara(PLAYERID::VALUE idChara);
        bool IsPlaying(void) const;

        int32 m_iNo;
        int32 m_iHP;
        int32 m_iHPMax;
        int32 m_iDamage;
        int32 m_iShurikenNum;
        int32 m_iShurikenMax;
        PLAYERID::VALUE m_CharacterID;
        int32 m_iCharacterNum;
    };

    struct PRIZEINFO
    {
        static PRIZEINFO DUMMY;
        
        void Init(void);

        GAMETYPES::PRIZE m_PrizeType;
        int32 m_iPointsNum;
        bool m_bTaken;
    };
    
public:
    CGamePlayParam(void);
    ~CGamePlayParam(void);
    void Clear(void);
    void InitForArea(void);
    void InitForStage(void);
    void ClearArea(void);
    void SetAreaPlaymode(AREAPLAYMODE playmode);
    void SetStartArea(AREAID::VALUE idArea, int32 iStageIndex);
    bool NextStage(void);
    void SetStage(STAGEID::VALUE idStage);
    void ClearPlayer(void);
    void AddPlayerCharacter(int32 iPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume = GAMETYPES::COSTUME_NONE);
    AREAPLAYMODE GetAreaPlaymode(void) const;
    AREAID::VALUE GetArea(void) const;
    int32 GetStageIndex(void) const;
    bool IsFirstStage(void) const;
    bool IsLastStage(void) const;
    STAGEID::VALUE GetStage(void) const;
    GAMETYPES::STAGEMODE GetStageMode(void) const;
    int32 GetPlayerNum(void) const;
    int32 GetCharaInfoNum(void) const;
    int32 GetPrizeInfoNum(void) const;
    const PRIZEINFO& PrizeInfo(int32 prizeIndex) const;
    const CHARAINFO& CharaInfo(int32 charainfoIndex) const;
    PLAYERCONTEXT& PlayerContext(int32 playerIndex);
    bool IsSpaceRide(void) const;

private:
    bool isPrizeAlreadyTaken(AREAID::VALUE idArea, GAMETYPES::PRIZE prize) const;
    void initPrizeInfo(void);
    GAMETYPES::PRIZE* getPrizeArray(AREAID::VALUE idArea) const;

private:
    static int32 m_anPlayerHPMax[];
    static int32 m_anRidePrizePoint[];
    static int32 m_anShurikenMax[];
    
    AREAPLAYMODE m_playmode;
    AREAID::VALUE m_AreaID;
    STAGEID::VALUE m_StageID;
    int32 m_iStageIndex;
    int32 m_numCharaInfo;
    CHARAINFO m_aCharaInfo[GAMETYPES::PLAYERS_MAX];
    int32 m_numPlayer;
    PLAYERCONTEXT m_aPlayerContext[GAMETYPES::PLAYERS_MAX];
    int32 m_numPrize;
    PRIZEINFO m_aPrizeInfo[GAMETYPES::PRIZE_MAX];
};