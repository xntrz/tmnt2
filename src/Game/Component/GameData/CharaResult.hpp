#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CCharaResult
{
public:
    struct TECHINFO
    {
        enum TYPE
        {
            TYPE_NONE = 0,
            TYPE_OFFENSE,
            TYPE_DEFENCE,
            TYPE_AERIAL,
        };

        int32 m_nPoints;
        TYPE m_type;
    };
    
    struct NODE
    {
        static int32 CompareEvaluation(const NODE& NodeA, const NODE& NodeB);
        
        void Clear(void);
        void AddTechnicalAction(PLAYERID::VALUE idPlayer, GAMETYPES::TECACT tecact);
        void Evaluate(void);
        int32 EvaluateAerialTech(void);
        int32 EvaluateDefenseTech(void);
        int32 EvaluateOffenseTech(void);

        int32 m_nOffensePoint;
        int32 m_nOffenseCount;
        float m_fOffenseRatio;
        int32 m_nOffenseEval;
        
        int32 m_nDefensePoint;
        int32 m_nDefenseEval;
        
        int32 m_nAerialPoint;
        int32 m_nAerialEval;
        
        int32 m_nPersonalPoint;
        int32 m_nPersonalEval;

        //int32 m_nDefenseCount;  // unused in eval
        //int32 m_nJumpPoint;     // unused in eval
    };
    
public:
    CCharaResult(void);
    ~CCharaResult(void);
    void Clear(void);
    void AddTechnicalAction(int32 nIndex, PLAYERID::VALUE idPlayer, GAMETYPES::TECACT tecact);
    void Evaluate(void);
    int32 GetMVP(void) const;
    const NODE& Chara(int32 nIndex) const;

private:
    static const TECHINFO m_aTechInfo[];
    static const float m_afOffenseEvalTable[];
    static const int32 m_anDefenseEvalTable[];
    static const int32 m_anAerialEvalTable[];
    static const int32 m_anPersonalEvalTable[];

    int32 m_nMVP;
    NODE m_aNode[GAMETYPES::CHARACTER_MAX];
};