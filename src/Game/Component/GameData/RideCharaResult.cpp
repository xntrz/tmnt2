#include "RideCharaResult.hpp"
#include "GamePlayResult.hpp"
#include "GameData.hpp"


/*static*/ int32 CRideCharaResult::m_anCoinEvalTable[] =
{
    500,
    1500,
    2500,
    4000,
    6000,
    8000,
};


/*static*/ int32 CRideCharaResult::m_anControlEvalTable[] =
{
    112,
    137,
    152,
    162,
    167,
    170,
};


/*static*/ int32 CRideCharaResult::m_anShotEvalTable[] =
{
    1,
    6,
    11,
    16,
    21,
    26,
};


/*static*/ float CRideCharaResult::m_afTrickEvalTable[] =
{
    1.0f,
    20.0f,
    35.0f,
    50.0f,
    70.0f,
    80.0f,
};


/*static*/ int32 CRideCharaResult::m_anPersonalEvalTable[] =
{
    3,
    6,
    9,
    12,
    15,
    18,
};


/*static*/ int32 CRideCharaResult::NODE::CompareEvaluation(const NODE& NodeA, const NODE& NodeB)
{
    //
    //  Personal
    //
    if (NodeA.m_nPersonalPoint < NodeB.m_nPersonalPoint)
        return -1;

    if (NodeA.m_nPersonalPoint > NodeB.m_nPersonalPoint)
        return 1;

    //
    //  Control
    //
    if (NodeA.m_nControlEval < NodeB.m_nControlEval)
        return -1;

    if (NodeA.m_nControlEval > NodeB.m_nControlEval)
        return 1;
    
    //
    //  Shot
    //
    if (NodeA.m_nShotEval < NodeB.m_nShotEval)
        return -1;

    if (NodeA.m_nShotEval > NodeB.m_nShotEval)
        return 1;
    
    //
    //  Trick
    //
    if (NodeA.m_nTrickEval < NodeB.m_nTrickEval)
        return -1;

    if (NodeA.m_nTrickEval > NodeB.m_nTrickEval)
        return 1;
    
    //
    //  Coin
    //
    if (NodeA.m_nCoinEval < NodeB.m_nCoinEval)
        return -1;

    if (NodeA.m_nCoinEval > NodeB.m_nCoinEval)
        return 1;
    
    return 0;
};


void CRideCharaResult::NODE::Clear(void)
{
    m_nScoreGoldCount   = 0;
    m_nScoreSilverCount = 0;
    m_nCoinPoint        = 0;
    m_nCoinEval         = 0;
    m_nCrashCount       = 0;
    m_fPlaytimeSeconds  = 0.0f;
    m_nControlEval      = 0;
    m_nJumpCount        = 0;
    m_nTrickCount       = 0;
    m_fTrickRatio       = 0.0f;
    m_nTrickEval        = 0;
    m_nShotCount        = 0;
    m_nShotEval         = 0;
    m_nPersonalPoint    = 0;
    m_nPersonalEval     = 0;
};


void CRideCharaResult::NODE::AddRideAction(GAMETYPES::RIDEACT rideact)
{
    switch (rideact)
    {
    case GAMETYPES::RIDEACT_SCORE_SILVER:
        ++m_nScoreSilverCount;
        break;

    case GAMETYPES::RIDEACT_SCORE_GOLD:
        ++m_nScoreGoldCount;
        break;

    case GAMETYPES::RIDEACT_JUMP:
        ++m_nJumpCount;
        break;

    case GAMETYPES::RIDEACT_TRICK:
        ++m_nTrickCount;
        break;

    case GAMETYPES::RIDEACT_WALL_CRASH:
        ++m_nCrashCount;
        break;

    case GAMETYPES::RIDEACT_KILL:
        ++m_nShotCount;
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CRideCharaResult::NODE::Evaluate(MODE mode)
{
    m_nCoinEval = EvaluateCoin();
    m_nControlEval = EvaluateControl();

    switch (mode)
    {
    case MODE_TRICKRACE:
        m_nTrickEval = EvaluateTrick();
        m_nShotEval = 0;
        break;

    case MODE_SHOTRACE:
        m_nTrickEval = 0;
        m_nShotEval = EvaluateShot();
        break;

    default:
        ASSERT(false);
        break;
    };

    m_nPersonalPoint = (m_nCoinEval + m_nControlEval + m_nTrickEval + m_nShotEval);
    m_nPersonalEval = CGamePlayResult::EvaluateInt(m_nPersonalPoint, m_anPersonalEvalTable, COUNT_OF(m_anPersonalEvalTable));
};


int32 CRideCharaResult::NODE::EvaluateCoin(void)
{
    m_nCoinPoint = (500 * m_nScoreGoldCount) + (100 * m_nScoreSilverCount);
    return CGamePlayResult::EvaluateInt(m_nCoinPoint, m_anCoinEvalTable, COUNT_OF(m_anCoinEvalTable));
};


int32 CRideCharaResult::NODE::EvaluateControl(void)
{
    return CGamePlayResult::EvaluateInt(int32(m_fPlaytimeSeconds) - m_nCrashCount, m_anControlEvalTable, COUNT_OF(m_anControlEvalTable));
};


int32 CRideCharaResult::NODE::EvaluateTrick(void)
{    
	if (m_nJumpCount)
		m_fTrickRatio = float((100 * m_nTrickCount) / m_nJumpCount);
    else
        m_fTrickRatio = 0.0f;

    return CGamePlayResult::EvaluateFloat(m_fTrickRatio, m_afTrickEvalTable, COUNT_OF(m_afTrickEvalTable));
};


int32 CRideCharaResult::NODE::EvaluateShot(void)
{
    return CGamePlayResult::EvaluateInt(m_nShotCount, m_anShotEvalTable, COUNT_OF(m_anShotEvalTable));
};


/*static*/ CRideCharaResult::MODE CRideCharaResult::GetMode(void)
{
    AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
    
    if ((idArea == AREAID::ID_AREA22) || (idArea == AREAID::ID_AREA32))
        return MODE_SHOTRACE;
    else
        return MODE_TRICKRACE;
};


CRideCharaResult::CRideCharaResult(void)
{
    Clear();
};


CRideCharaResult::~CRideCharaResult(void)
{
    ;
};


void CRideCharaResult::Clear(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aNode); ++i)
        m_aNode[i].Clear();

    m_nMVP              = 0;
    m_nTotalGoldCoin    = 0;
    m_nTotalSilverCoin  = 0;
    m_nTotalCoinPoint   = 0;
};


void CRideCharaResult::SetCharaPlaytime(int32 nIndex, float fPlaytime)
{
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(m_aNode));

    m_aNode[nIndex].m_fPlaytimeSeconds = fPlaytime;
};


void CRideCharaResult::AddRideAction(int32 nIndex, GAMETYPES::RIDEACT rideact)
{
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(m_aNode));

    m_aNode[nIndex].AddRideAction(rideact);
};


void CRideCharaResult::Evaluate(void)
{
    MODE mode = GetMode();
    
    for (int32 i = 0; i < COUNT_OF(m_aNode); ++i)
        m_aNode[i].Evaluate(mode);

    m_nMVP = 0;

    for (int32 i = 1; i < COUNT_OF(m_aNode); ++i)
    {
        if (NODE::CompareEvaluation(m_aNode[m_nMVP], m_aNode[i]) < 0)
            m_nMVP = i;
    };

    m_nTotalGoldCoin = 0;
    m_nTotalSilverCoin = 0;
    m_nTotalCoinPoint = 0;

    for (int32 i = 0; i < COUNT_OF(m_aNode); ++i)
    {
        m_nTotalGoldCoin    += m_aNode[i].m_nScoreGoldCount;
        m_nTotalSilverCoin  += m_aNode[i].m_nScoreSilverCount;
        m_nTotalCoinPoint   += m_aNode[i].m_nCoinPoint;
    };
};


int32 CRideCharaResult::GetMVP(void) const
{
    return m_nMVP;
};


int32 CRideCharaResult::GetTotalGoldCoin(void) const
{
    return m_nTotalGoldCoin;
};


int32 CRideCharaResult::GetTotalSilverCoin(void) const
{
    return m_nTotalSilverCoin;
};


int32 CRideCharaResult::GetTotalCoinPoint(void) const
{
    return m_nTotalCoinPoint;
};


const CRideCharaResult::NODE& CRideCharaResult::Chara(int32 nIndex) const
{
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(m_aNode));

    if (nIndex >= 0 && nIndex < COUNT_OF(m_aNode))
        return m_aNode[nIndex];
    else
        return m_aNode[0];
};

