#include "CharaResult.hpp"
#include "GamePlayResult.hpp"


/*static*/ const CCharaResult::TECHINFO CCharaResult::m_aTechInfo[] =
{
    { 0,    TECHINFO::TYPE_NONE     },  //  NONE
    { 30,   TECHINFO::TYPE_OFFENSE  },  //  AABBC
    { 15,   TECHINFO::TYPE_OFFENSE  },  //  B CHARGE PHASE 3
    { 10,   TECHINFO::TYPE_OFFENSE  },  //  AABB
    { 8,    TECHINFO::TYPE_OFFENSE  },  //  THROW_BACK
    { 6,    TECHINFO::TYPE_OFFENSE  },  //  AAC
    { 5,    TECHINFO::TYPE_OFFENSE  },  //  B CHARGE PHASE 2
    { 4,    TECHINFO::TYPE_OFFENSE  },  //  AAB
    { 3,    TECHINFO::TYPE_OFFENSE  },  //  B CHARGE PHASE 1
    { 3,    TECHINFO::TYPE_OFFENSE  },  //  RUN ATTACK
    { 3,    TECHINFO::TYPE_OFFENSE  },  //  THROW
    { 2,    TECHINFO::TYPE_OFFENSE  },  //  AA
    { 2,    TECHINFO::TYPE_OFFENSE  },  //  B CHARGE PHASE NONE
    { 1,    TECHINFO::TYPE_OFFENSE  },  //  KNIFE
    { 1,    TECHINFO::TYPE_OFFENSE  },  //  A
    { 1,    TECHINFO::TYPE_OFFENSE  },  //  DASH
    { 20,   TECHINFO::TYPE_DEFENCE  },  //  GUARD IMPACT
    { 15,   TECHINFO::TYPE_DEFENCE  },  //  AABC (all play chara have some status effect here)
    { 150,  TECHINFO::TYPE_AERIAL   },  //  AABBB
    { 50,   TECHINFO::TYPE_AERIAL   },  //  COMBINATION
    { 15,   TECHINFO::TYPE_AERIAL   },  //  KNIFE JUMP
    { 10,   TECHINFO::TYPE_AERIAL   },  //  JUMP
    { 5,    TECHINFO::TYPE_AERIAL   },  //  JUMP WALL
    { 5,    TECHINFO::TYPE_AERIAL   },  //  JUMP DOUBLE
    { -1,   TECHINFO::TYPE_DEFENCE  },  //  DAMAGED
    { -5,   TECHINFO::TYPE_AERIAL   },  //  FALLEN
};


/*static*/ const float CCharaResult::m_afOffenseEvalTable[] =
{
    1.5f,
    2.0f,
    3.0f,
    5.0f,
};


/*static*/ const int32 CCharaResult::m_anDefenseEvalTable[] =
{
    1,
    26,
    51,
    100,
};


/*static*/ const int32 CCharaResult::m_anAerialEvalTable[] =
{
    50,
    100,
    200,
    400,
};


/*static*/ const int32 CCharaResult::m_anPersonalEvalTable[] =
{
    2,
    5,
    8,
    11,
};


/*static*/ int32 CCharaResult::NODE::CompareEvaluation(const NODE& NodeA, const NODE& NodeB)
{
    //
    //  Personal
    //
    if (NodeA.m_nPersonalPoint < NodeB.m_nPersonalPoint)
        return -1;

    if (NodeA.m_nPersonalPoint > NodeB.m_nPersonalPoint)
        return 1;

    //
    //  Offense
    //
    if (NodeA.m_nOffenseEval < NodeB.m_nOffenseEval)
        return -1;

    if (NodeA.m_nOffenseEval > NodeB.m_nOffenseEval)
        return 1;

    //
    //  Defense
    //
    if (NodeA.m_nDefenseEval < NodeB.m_nDefenseEval)
        return -1;

    if (NodeA.m_nDefenseEval > NodeB.m_nDefenseEval)
        return 1;

    //
    //  Aerial
    //
    if (NodeA.m_nAerialEval < NodeB.m_nAerialEval)
        return -1;

    if (NodeA.m_nAerialEval > NodeB.m_nAerialEval)
        return 1;

    return 0;
};


void CCharaResult::NODE::Clear(void)
{
    m_nOffensePoint     = 0;
    m_nOffenseCount     = 0;
    m_fOffenseRatio     = 0.0f;
    m_nOffenseEval      = 0;
    m_nDefensePoint     = 0;
    m_nDefenseEval      = 0;
    m_nAerialPoint      = 0;
    m_nAerialEval       = 0;
    m_nPersonalPoint    = 0;
    m_nPersonalEval     = 0;
    m_nDefenseCount     = 0;
    m_nJumpPoint        = 0;
};


void CCharaResult::NODE::AddTechnicalAction(PLAYERID::VALUE idPlayer, GAMETYPES::TECACT tecact)
{
    static_assert(COUNT_OF(m_aTechInfo) == GAMETYPES::TECACT_MAX, "update me");

    const TECHINFO* pTechInfo = &m_aTechInfo[tecact];

    switch (pTechInfo->m_type)
    {
    case TECHINFO::TYPE_OFFENSE:
        ++m_nOffenseCount;
        m_nOffensePoint += pTechInfo->m_nPoints;
        break;

    case TECHINFO::TYPE_DEFENCE:
        m_nDefensePoint += pTechInfo->m_nPoints;        
        break;

    case TECHINFO::TYPE_AERIAL:
        if ((idPlayer == PLAYERID::ID_MIC) && (tecact == GAMETYPES::TECACT_JUMP))
            ++m_nAerialPoint;
        else
            m_nAerialPoint += pTechInfo->m_nPoints;        
        break;

    default:
        ASSERT(false);
        break;
    };

    if ((tecact > GAMETYPES::TECACT_NONE) &&
        (tecact < GAMETYPES::TECACT_JUMP_WALL))
    {
        ++m_nDefensePoint;
        ++m_nDefenseCount;
    };

    if (tecact == GAMETYPES::TECACT_JUMP)
        m_nJumpPoint += pTechInfo->m_nPoints;
};


void CCharaResult::NODE::Evaluate(void)
{
    m_nOffenseEval = EvaluateOffenseTech();
    m_nDefenseEval = EvaluateDefenseTech();
    m_nAerialEval  = EvaluateAerialTech();

    m_nPersonalPoint = (m_nOffenseEval + m_nDefenseEval + m_nAerialEval);
    m_nPersonalEval = CGamePlayResult::EvaluateInt(m_nPersonalPoint, m_anPersonalEvalTable, COUNT_OF(m_anPersonalEvalTable));
};


int32 CCharaResult::NODE::EvaluateAerialTech(void)
{
    return CGamePlayResult::EvaluateInt(m_nAerialPoint, m_anAerialEvalTable, COUNT_OF(m_anAerialEvalTable));
};


int32 CCharaResult::NODE::EvaluateDefenseTech(void)
{
    return CGamePlayResult::EvaluateInt(m_nDefensePoint, m_anDefenseEvalTable, COUNT_OF(m_anDefenseEvalTable));
};


int32 CCharaResult::NODE::EvaluateOffenseTech(void)
{
    m_fOffenseRatio = 0.0f;
    
    if (m_nOffenseCount)
        m_fOffenseRatio = (static_cast<float>(m_nOffensePoint) / static_cast<float>(m_nOffenseCount));
    
    return CGamePlayResult::EvaluateFloat(m_fOffenseRatio, m_afOffenseEvalTable, COUNT_OF(m_afOffenseEvalTable));
};


CCharaResult::CCharaResult(void)
{
    Clear();
};


CCharaResult::~CCharaResult(void)
{
    ;
};


void CCharaResult::Clear(void)
{
    m_nMVP = 0;
    
    for (int32 i = 0; i < COUNT_OF(m_aNode); ++i)
        m_aNode[i].Clear();
};


void CCharaResult::AddTechnicalAction(int32 nIndex, PLAYERID::VALUE idPlayer, GAMETYPES::TECACT tecact)
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < COUNT_OF(m_aNode));

    m_aNode[nIndex].AddTechnicalAction(idPlayer, tecact);
};


void CCharaResult::Evaluate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aNode); ++i)
        m_aNode[i].Evaluate();

    m_nMVP = 0;

    for (int32 i = 1; i < COUNT_OF(m_aNode); ++i)
    {
        if (NODE::CompareEvaluation(m_aNode[m_nMVP], m_aNode[i]) < 0)
            m_nMVP = i;
    };
};


int32 CCharaResult::GetMVP(void) const
{
    return m_nMVP;
};


const CCharaResult::NODE& CCharaResult::Chara(int32 nIndex) const
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < COUNT_OF(m_aNode));

    if ((nIndex >= 0) && (nIndex < COUNT_OF(m_aNode)))
        return m_aNode[nIndex];

    return m_aNode[0];
};