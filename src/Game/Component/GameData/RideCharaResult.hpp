#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CRideCharaResult
{
public:
    enum MODE
    {
        MODE_TRICKRACE = 0,
        MODE_SHOTRACE,
    };

    struct NODE
    {
        static int32 CompareEvaluation(const NODE& NodeA, const NODE& NodeB);

        void Clear(void);
        void AddRideAction(GAMETYPES::RIDEACT rideact);
        void Evaluate(MODE mode);        
        int32 EvaluateCoin(void);
        int32 EvaluateControl(void);
        int32 EvaluateTrick(void);
        int32 EvaluateShot(void);

        int32 m_nScoreGoldCount;
        int32 m_nScoreSilverCount;
        int32 m_nCoinPoint;
        int32 m_nCoinEval;
        int32 m_nCrashCount;
        float m_fPlaytimeSeconds;
        int32 m_nControlEval;
        int32 m_nJumpCount;
        int32 m_nTrickCount;
        float m_fTrickRatio;
        int32 m_nTrickEval;
        int32 m_nKillCount;
        int32 m_nKillEval;
        int32 m_nPersonalPoint;
        int32 m_nPersonalEval;
    };

public:
    static MODE GetMode(void);
    
    CRideCharaResult(void);
    ~CRideCharaResult(void);
    void Clear(void);
    void SetCharaPlaytime(int32 nIndex, float fPlaytime);
    void AddRideAction(int32 nIndex, GAMETYPES::RIDEACT rideact);
    void Evaluate(void);
    int32 GetMVP(void) const;
    int32 GetTotalGoldCoin(void) const;
    int32 GetTotalSilverCoin(void) const;
    int32 GetTotalCoinPoint(void) const;    
    const NODE& Chara(int32 nIndex) const;

private:
    static int32 m_anCoinEvalTable[];
    static int32 m_anControlEvalTable[];
    static int32 m_anShotEvalTable[];
    static float m_afTrickEvalTable[];
    static int32 m_anPersonalEvalTable[];

    int32 m_nMVP;
    int32 m_nTotalGoldCoin;
    int32 m_nTotalSilverCoin;
    int32 m_nTotalCoinPoint;
    NODE m_aNode[GAMETYPES::PLAYERS_MAX];
};