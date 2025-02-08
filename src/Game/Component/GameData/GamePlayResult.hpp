#pragma once

#include "CharaResult.hpp"
#include "RideCharaResult.hpp"
#include "GameTime.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameMain/AreaID.hpp"
#include "Game/Component/GameMain/ItemID.hpp"
#include "Game/Component/GameMain/StageID.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CGamePlayResult
{
public:
    enum AREARESULT
    {
        AREARESULT_NONE = 0,
        AREARESULT_GAMECLEAR,
        AREARESULT_GAMEOVER,
        AREARESULT_EXIT,
    };

    enum CLEARSUB
    {
        CLEARSUB_A = 0,
        CLEARSUB_B,
    };

    enum EXITSUB
    {
        EXITSUB_NONE = 0,
        EXITSUB_TO_TITLE,
        EXITSUB_TO_CHARASEL,
        EXITSUB_TO_AREASEL,
    };

    struct STAGENODE
    {
        int32 m_mvp;
        CGameTime m_cleartime;
    };

public:
    static int32 EvaluateInt(int32 nValue, const int32* anTable, int32 nTableSize);
    static int32 EvaluateFloat(float fValue, const float* afTable, int32 nTableSize);
    
    CGamePlayResult(void);
    ~CGamePlayResult(void);
    void Clear(void);
    void SetAreaResult(AREARESULT arearesult);
    AREARESULT GetAreaResult(void) const;
    void SetClearSub(CLEARSUB clearsub);
    void SetExitSub(EXITSUB exitsub);
    EXITSUB GetExitSub(void) const;
    void SetResultType(GAMETYPES::RESULTTYPE resulttype);
    GAMETYPES::RESULTTYPE GetResultType(void) const;
    bool IsScenarioEnding(void) const;
    bool IsNexusEnding(void) const;
    bool IsEnding(void) const;
    void InitForArea(void);
    void InitForStage(void);
    void TermForStage(void);
    void Evaluate(void);
    void TakePrize(AREAID::VALUE idArea);
    void ApplyToRecord(void);
    void SetStageClearSecond(float fSeconds);
    void SetRemainedHP(int32 iRemainHP);
    void SetRideCharaPlaySecond(PLAYERID::VALUE idPlayer, float fSeconds);
    void AddTotalItemCount(void);
    void AddTakenItemCount(ITEMID::VALUE idItem);
    void AddTechnicalAction(PLAYERID::VALUE idPlayer, GAMETYPES::TECACT tecact);
    void AddRideAction(PLAYERID::VALUE idPlayer, GAMETYPES::RIDEACT rideact);
    PLAYERID::VALUE GetPlayerCharacter(int32 nIndex) const;
    int32 IndexOfChara(PLAYERID::VALUE idPlayer) const;
    int32 GetTakenCrystalNum(GAMETYPES::CRYSTALTYPE crytype) const;
    bool IsAntiqueTaken(void) const;
    GAMETYPES::CLEARRANK GetOffenseTechRank(int32 nIndex) const;
    GAMETYPES::CLEARRANK GetDefenceTechRank(int32 nIndex) const;
    GAMETYPES::CLEARRANK GetAerialTechRank(int32 nIndex) const;
    GAMETYPES::CLEARRANK GetPersonalRank(int32 nIndex) const;
    int32 GetMVP(void) const;
    GAMETYPES::CLEARRANK GetTotalRank(void) const;
    CGameTime GetCleartimeTotal(void) const;
    float GetRemainedHPRatio(void) const;
    float GetItemTakeRatio(void) const;
    int32 GetRideGoldCoin(void) const;
    int32 GetRideSilverCoin(void) const;
    int32 GetRideCoinPoint(void) const;
    int32 GetRideTakenPrizeNo(void) const;
    GAMETYPES::CLEARRANK GetCoinRank(int32 nIndex) const;
    GAMETYPES::CLEARRANK GetControlRank(int32 nIndex) const;
    GAMETYPES::CLEARRANK GetTrickRank(int32 nIndex) const;
    GAMETYPES::CLEARRANK GetShotRank(int32 nIndex) const;
    int32 GetStageMVP(int32 nIndex) const;
    CGameTime GetStageCleartime(int32 nIndex) const;

private:
    void initClearTimeEvalTable(AREAID::VALUE idArea);
    GAMETYPES::CLEARRANK calcClearRank(int32 nEval) const;
    GAMETYPES::CLEARRANK calcRideClearRank(int32 nEval) const;
    int32 evaluateClearTime(void) const;
    int32 evaluateRemainingHP(void) const;
    int32 evaluateItem(void) const;
    STAGENODE* getCurrentStage(void);
    void takeOnePrize(AREAID::VALUE idArea, GAMETYPES::PRIZE prize);

private:
    static const float m_afLifeEvalTable[];
    static const float m_afItemEvalTable[];
    static const int32 m_anNormalAreaEvalTable[];
    static const int32 m_anRideAreaEvalTable[];

    GAMETYPES::RESULTTYPE m_type;
    AREARESULT m_result;
    CLEARSUB m_clearsub;
    EXITSUB m_exitsub;
    GAMETYPES::CLEARRANK m_baserank;
    bool m_bTakenRedCry;
    bool m_bTakenGreenCry;
    bool m_bTakenOrangeCry;
    bool m_bTakenWhiteCry;
    bool m_bTakenAntique;
    bool m_bTakenComeback;
    CGameTime m_cleartime;
    int32 m_nRemainingHP;
    int32 m_nMaxHP;
    int32 m_nTakenItemCount;
    int32 m_nTotalItemCount;
    GAMETYPES::CLEARRANK m_arearank;
    int32 m_nRidePrizeNo;
    STAGENODE m_aStageNode[GAMETYPES::STAGE_MAX];
    PLAYERID::VALUE m_aPlayerID[GAMETYPES::CHARACTER_MAX];
    CCharaResult m_CharaResult;
    CCharaResult m_StageResult;
    CRideCharaResult m_RideResult;
    uint32 m_aClearEvalTable[4];
};