#include "GamePlayResult.hpp"
#include "GameData.hpp"
#include "GameTime.hpp"

#include "Game/Component/GameMain/SecretInfo.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"


/*static*/ const float CGamePlayResult::m_afLifeEvalTable [] =
{
    20.0f,
    50.0f,
    70.0f,
    85.0f,
};


/*static*/ const float CGamePlayResult::m_afItemEvalTable[] = 
{
    1.0f,
    30.0f,
    50.0f,
    70.0f,
};


/*static*/ const int32 CGamePlayResult::m_anNormalAreaEvalTable[] = 
{
    6,
    10,
    16,
    22,    
};


/*static*/ const int32 CGamePlayResult::m_anRideAreaEvalTable[] = 
{
    1000,
    3000,
    5000,
    7000,
    9000,
    10000,
};


/*static*/ int32 CGamePlayResult::EvaluateInt(int32 nValue, const int32* anTable, int32 nTableSize)
{
    int32 iResult = 0;
    
    if (anTable[0] >= anTable[nTableSize - 1])
    {
        for (int32 i = 0; i < nTableSize; ++i)
        {
            if (nValue > anTable[i])
                break;

			++iResult;            
        };
    }
    else
    {
        for (int32 i = 0; i < nTableSize; ++i)
        {
            if (nValue < anTable[i])
                break;

			++iResult;            
        };
    };

    return iResult;
};


/*static*/ int32 CGamePlayResult::EvaluateFloat(float fValue, const float* afTable, int32 nTableSize)
{
    int32 iResult = 0;

    if (afTable[0] >= afTable[nTableSize - 1])
    {
        for (int32 i = 0; i < nTableSize; ++i)
        {
            if (fValue > afTable[i])
                break;

			++iResult;            
        };
    }
    else
    {
        for (int32 i = 0; i < nTableSize; ++i)
        {
            if (fValue < afTable[i])
                break;

			++iResult;            
        };
    };

    return iResult;
};


CGamePlayResult::CGamePlayResult(void)
{
    Clear();
};


CGamePlayResult::~CGamePlayResult(void)
{
    ;
};


void CGamePlayResult::Clear(void)
{
    m_type              = GAMETYPES::RESULTTYPE_NORMAL;
    m_result            = AREARESULT_NONE;
    m_clearsub          = CLEARSUB_A;
    m_exitsub           = EXITSUB_NONE;
    m_baserankNormal    = GAMETYPES::CLEARRANK_NONE;
    m_baserankRide      = GAMETYPES::CLEARRANK_NONE;
    m_bTakenRedCry      = false;
    m_bTakenGreenCry    = false;
    m_bTakenOrangeCry   = false;
    m_bTakenWhiteCry    = false;
    m_bTakenAntique     = false;
    m_bTakenComeback    = false;
    m_cleartimeTotal.Clear();
    m_nRemainingHP      = 0;
    m_nMaxHP            = 0;
    m_nTakenItemCount   = 0;
    m_nTotalItemCount   = 0;
    m_totalrank         = GAMETYPES::CLEARRANK_NONE;
    m_nRidePrizeNo      = -1;

    for (int32 i = 0; i < COUNT_OF(m_aNodeStage); ++i)
    {
        m_aNodeStage[i].m_nMVP = 0;
        m_aNodeStage[i].m_cleartime.Clear();
    };

    m_aPlayerID[0] = PLAYERID::ID_INVALID;
    m_aPlayerID[1] = PLAYERID::ID_INVALID;
    m_aPlayerID[2] = PLAYERID::ID_INVALID;
    m_aPlayerID[3] = PLAYERID::ID_INVALID;

    m_CharaResultNormal.Clear();
    m_CharaResultRide.Clear();
    
    m_auCleartimeEvalTable[0] = 0;
    m_auCleartimeEvalTable[1] = 0;
    m_auCleartimeEvalTable[2] = 0;
    m_auCleartimeEvalTable[3] = 0;
};


void CGamePlayResult::SetAreaResult(AREARESULT arearesult)
{
    m_result = arearesult;
};


CGamePlayResult::AREARESULT CGamePlayResult::GetAreaResult(void) const
{
    return m_result;
};


void CGamePlayResult::SetClearSub(CLEARSUB clearsub)
{
    m_clearsub = clearsub;
};


void CGamePlayResult::SetExitSub(EXITSUB exitsub)
{
    m_exitsub = exitsub;
};


CGamePlayResult::EXITSUB CGamePlayResult::GetExitSub(void) const
{
    return m_exitsub;
};


void CGamePlayResult::SetResultType(GAMETYPES::RESULTTYPE resulttype)
{
    m_type = resulttype;
};


GAMETYPES::RESULTTYPE CGamePlayResult::GetResultType(void) const
{
    return m_type;
};


bool CGamePlayResult::IsScenarioEnding(void) const
{
    if (m_result == AREARESULT_CLEAR)
        return CGameData::PlayParam().GetArea() == AREAID::ID_AREA58;
    else
        return false;
};


bool CGamePlayResult::IsNexusEnding(void) const
{
    if (m_result == AREARESULT_CLEAR)
        return CGameData::PlayParam().GetArea() == AREAID::ID_AREA60_D;
    else
        return false;
};


bool CGamePlayResult::IsEnding(void) const
{
    return (IsScenarioEnding() || IsNexusEnding());
};


void CGamePlayResult::InitForArea(void)
{
    Clear();

    switch (CGameData::Option().Play().GetDifficulty())
    {
    case GAMETYPES::DIFFICULTY_EASY:
        m_baserankNormal = GAMETYPES::CLEARRANK_E;
        m_baserankRide = GAMETYPES::CLEARRANK_E;
        break;

    case GAMETYPES::DIFFICULTY_NORMAL:
        m_baserankNormal = GAMETYPES::CLEARRANK_D;
        m_baserankRide = GAMETYPES::CLEARRANK_E;
        break;

    case GAMETYPES::DIFFICULTY_HARD:
        m_baserankNormal = GAMETYPES::CLEARRANK_C;
        m_baserankRide = GAMETYPES::CLEARRANK_E;
        break;

    default:
        ASSERT(false);
        break;
    };

    for (int32 i = 0; i < COUNT_OF(m_aPlayerID); ++i)
        m_aPlayerID[i] = CGameData::PlayParam().CharaInfo(i).m_CharacterID;

    m_nMaxHP = CGameData::PlayParam().PlayerContext(0).m_iHPMax;
};


void CGamePlayResult::InitForStage(void)
{
    ;
};


void CGamePlayResult::TermForStage(void)
{
    getCurrentStage()->m_nMVP = GetMVP();
};


void CGamePlayResult::Evaluate(void)
{
    m_CharaResultNormal.Evaluate();

    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
    {
        m_CharaResultRide.Evaluate();
        m_totalrank = calcRideClearRank(m_CharaResultRide.GetTotalCoinPoint());
    }
    else
    {
        initClearTimeEvalTable(CGameData::PlayParam().GetArea());
        int32 nEvalTime = evaluateClearTime();
        int32 nEvalHP = evaluateRemainingHP();
        int32 nEvalItem = evaluateItem();
        int32 nEvalMVP = m_CharaResultNormal.Chara(m_CharaResultNormal.GetMVP()).m_nPersonalPoint;
        int32 nEvalTotal = (nEvalTime + nEvalHP + nEvalItem + nEvalMVP);
        m_totalrank = calcClearRank(nEvalTotal);
    };
};


void CGamePlayResult::TakePrize(AREAID::VALUE idArea)
{
    switch (m_type)
    {
    case GAMETYPES::RESULTTYPE_RIDE:
        for (int32 i = 0; i < CGameData::PlayParam().GetPrizeInfoNum(); ++i)
        {
            const CGamePlayParam::PRIZEINFO& prizeinfo = CGameData::PlayParam().PrizeInfo(i);
            
            if (prizeinfo.m_iPointsNum <= m_CharaResultRide.GetTotalCoinPoint() &&
                !prizeinfo.m_bTaken)
            {
                m_nRidePrizeNo = i;
                takeOnePrize(idArea, prizeinfo.m_PrizeType);
                break;
            };
        };
        break;

    case GAMETYPES::RESULTTYPE_NEXUS:
        for (int32 i = 0; i < CGameData::PlayParam().GetPrizeInfoNum(); ++i)
            takeOnePrize(idArea, CGameData::PlayParam().PrizeInfo(i).m_PrizeType);
        break;
    };
};


void CGamePlayResult::ApplyToRecord(void)
{
    AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
    
    if (m_bTakenRedCry)
    {
        CGameData::Record().Item().SetAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_RED, idArea);
        m_bTakenRedCry = false;
    };

    if (m_bTakenGreenCry)
    {
        CGameData::Record().Item().SetAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_GREEN, idArea);
        m_bTakenGreenCry = false;
    };

    if (m_bTakenWhiteCry)
    {
        CGameData::Record().Item().SetAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_WHITE, idArea);
        m_bTakenWhiteCry = false;
    };

    if (m_bTakenOrangeCry)
    {
        CGameData::Record().Item().SetAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_ORANGE, idArea);
        m_bTakenOrangeCry = false;
    };

    if (m_bTakenAntique)
    {
        ANTIQUEID::VALUE idAntique = CAreaInfo::GetAntiqueID(idArea);
        CGameData::Record().Antique().SetAntiqueTaken(idAntique);
        m_bTakenAntique = false;
    };

    if (m_bTakenComeback)
    {
        CGameData::Record().Item().SetComebackTaken();
        m_bTakenComeback = false;
    };

    switch (m_type)
    {
    case GAMETYPES::RESULTTYPE_NORMAL:
    case GAMETYPES::RESULTTYPE_RIDE:
        {
            AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
            
            switch (m_clearsub)
            {
            case CLEARSUB_A:
                CGameData::Record().Area().SetAreaCleared(idArea, CAreaRecord::CLEAR_ROOT_A);
                break;

            case CLEARSUB_B:
                CGameData::Record().Area().SetAreaCleared(idArea, CAreaRecord::CLEAR_ROOT_B);
                break;

            default:
                ASSERT(false);
                break;
            };

            if (CGameData::Record().Area().GetAreaClearRank(idArea) < GetTotalRank())
            {
                CGameData::Record().Area().SetAreaRank(idArea, GetTotalRank());
                CGameData::Record().Area().SetNowClearArea();
            };

            CGameData::Record().Area().UpdateAreaClearTime(idArea, m_cleartimeTotal);
        }
        break;

    case GAMETYPES::RESULTTYPE_NEXUS:
        if (m_result == AREARESULT_CLEAR)
        {
            GAMETYPES::NEXUSID idNexus = CAreaInfo::GetNexusID(CGameData::PlayParam().GetArea());
            ASSERT(idNexus != GAMETYPES::NEXUSID_NUM);

            CGameData::Record().Nexus().SetTournamentState(idNexus, CNexusRecord::STATE_CLEAR);
            CGameData::Record().Nexus().UpdateTournamentClearTime(idNexus, m_cleartimeTotal);
        };
        break;
    };
};


void CGamePlayResult::SetStageclearSecond(float fSeconds)
{
    getCurrentStage()->m_cleartime = CGameTime(uint32(fSeconds));
    m_cleartimeTotal.Add(CGameTime(uint32(fSeconds)));
};


void CGamePlayResult::SetRemainedHP(int32 iRemainHP)
{
    m_nRemainingHP = iRemainHP;
};


void CGamePlayResult::SetRideCharaPlaySecond(PLAYERID::VALUE idPlayer, float fSeconds)
{
    int32 nIndex = IndexOfChara(idPlayer);

    ASSERT(nIndex >= 0 && nIndex < GAMETYPES::CHARACTER_MAX);

    if (nIndex < -1)
        return;

    m_CharaResultRide.SetCharaPlaytime(nIndex, fSeconds);
};


void CGamePlayResult::AddTotalItemCount(void)
{
    ++m_nTotalItemCount;
};


void CGamePlayResult::AddTakenItemCount(ITEMID::VALUE idItem)
{
    ASSERT(idItem > ITEMID::ID_NONE && idItem < ITEMID::ID_MAX);

    if (idItem == ITEMID::ID_DON_LASER)
        return;

    ASSERT(m_nTakenItemCount < m_nTotalItemCount);
    ++m_nTakenItemCount;

    switch (idItem)
    {
    case ITEMID::ID_CRY_RED:
        ASSERT(!m_bTakenRedCry);
        m_bTakenRedCry = true;
        break;
        
    case ITEMID::ID_CRY_GREEN:
        ASSERT(!m_bTakenGreenCry);
        m_bTakenGreenCry = true;
        break;
        
    case ITEMID::ID_CRY_WHITE:
        ASSERT(!m_bTakenWhiteCry);
        m_bTakenWhiteCry = true;
        break;
        
    case ITEMID::ID_CRY_ORANGE:
        ASSERT(!m_bTakenOrangeCry);
        m_bTakenOrangeCry = true;
        break;
        
    case ITEMID::ID_ANTIQUE:
        ASSERT(!m_bTakenAntique);
        m_bTakenAntique = true;
        break;
        
    case ITEMID::ID_COMEBACK:
        ASSERT(!m_bTakenComeback);
        m_bTakenComeback = true;
        break;
    };    
};


void CGamePlayResult::AddTechnicalAction(PLAYERID::VALUE idPlayer, GAMETYPES::TECACT tecact)
{
    ASSERT(tecact >= 0 && tecact < GAMETYPES::TECACT_MAX);

    int32 nIndex = IndexOfChara(idPlayer);

    ASSERT(nIndex >= 0 && nIndex < GAMETYPES::CHARACTER_MAX);

    if (nIndex < -1)
        return;

    m_CharaResultNormal.AddTechnicalAction(nIndex, idPlayer, tecact);
};


void CGamePlayResult::AddRideAction(PLAYERID::VALUE idPlayer, GAMETYPES::RIDEACT rideact)
{
    ASSERT(rideact >= 0 && rideact < GAMETYPES::RIDEACT_MAX);
    
    int32 nIndex = IndexOfChara(idPlayer);

    ASSERT(nIndex >= 0 && nIndex < GAMETYPES::CHARACTER_MAX);
    
    if (nIndex < -1)
        return;

    m_CharaResultRide.AddRideAction(nIndex, rideact);
};


PLAYERID::VALUE CGamePlayResult::GetPlayerCharacter(int32 nIndex) const
{
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(m_aPlayerID));
    
    return m_aPlayerID[nIndex];
};


int32 CGamePlayResult::IndexOfChara(PLAYERID::VALUE idPlayer) const
{
    for (int32 i = 0; i < COUNT_OF(m_aPlayerID); ++i)
    {
        if (m_aPlayerID[i] == idPlayer)
            return i;
    };

    return -1;
};


int32 CGamePlayResult::GetTakenCrystalNum(GAMETYPES::CRYSTALTYPE crytype) const
{
    int32 iResult = 0;
    
    switch (crytype)
    {
    case GAMETYPES::CRYSTALTYPE_RED:
        iResult = (m_bTakenRedCry ? 1 : 0);
        break;
        
    case GAMETYPES::CRYSTALTYPE_GREEN:
        iResult = (m_bTakenGreenCry ? 1 : 0);
        break;
        
    case GAMETYPES::CRYSTALTYPE_WHITE:
        iResult = (m_bTakenWhiteCry ? 1 : 0);
        break;
        
    case GAMETYPES::CRYSTALTYPE_ORANGE:
        iResult = (m_bTakenOrangeCry ? 1 : 0);
        break;

    default:
        ASSERT(false);
        break;
    };

    return iResult;
};


bool CGamePlayResult::IsAntiqueTaken(void) const
{
    return m_bTakenAntique;
};


GAMETYPES::CLEARRANK CGamePlayResult::GetOffenseTechRank(int32 nIndex) const
{
    return GAMETYPES::CLEARRANK(m_baserankNormal + m_CharaResultNormal.Chara(nIndex).m_nOffenseEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetDefenceTechRank(int32 nIndex) const
{
    return GAMETYPES::CLEARRANK(m_baserankNormal + m_CharaResultNormal.Chara(nIndex).m_nDefenseEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetAerialTechRank(int32 nIndex) const
{
    return GAMETYPES::CLEARRANK(m_baserankNormal + m_CharaResultNormal.Chara(nIndex).m_nAerialEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetPersonalRank(int32 nIndex) const
{
    if (m_type == GAMETYPES::RESULTTYPE_RIDE)
        return GAMETYPES::CLEARRANK(m_baserankRide + m_CharaResultRide.Chara(nIndex).m_nPersonalEval);
    else
        return GAMETYPES::CLEARRANK(m_baserankNormal + m_CharaResultNormal.Chara(nIndex).m_nPersonalEval);
};


int32 CGamePlayResult::GetMVP(void) const
{
    if (m_type == GAMETYPES::RESULTTYPE_RIDE)
        return m_CharaResultRide.GetMVP();
    else
        return m_CharaResultNormal.GetMVP();
};


GAMETYPES::CLEARRANK CGamePlayResult::GetTotalRank(void) const
{
    return m_totalrank;
};


const CGameTime& CGamePlayResult::GetCleartimeTotal(void) const
{
    return m_cleartimeTotal;
};


float CGamePlayResult::GetRemainedHPRatio(void) const
{
    ASSERT(m_nMaxHP > 0);

    if (m_nMaxHP)
        return float((100 * m_nRemainingHP) / m_nMaxHP);
    else
        return 100.0f;
};


float CGamePlayResult::GetItemTakeRatio(void) const
{
    if (m_nTotalItemCount)
        return float((100 * m_nTakenItemCount) / m_nTotalItemCount);
    else
        return 100.0f;
};


int32 CGamePlayResult::GetRideGoldCoin(void) const
{
    return m_CharaResultRide.GetTotalGoldCoin();
};


int32 CGamePlayResult::GetRideSilverCoin(void) const
{
    return m_CharaResultRide.GetTotalSilverCoin();
};


int32 CGamePlayResult::GetRideCoinPoint(void) const
{
    return m_CharaResultRide.GetTotalCoinPoint();
};


int32 CGamePlayResult::GetRideTakenPrizeNo(void) const
{
    return m_nRidePrizeNo;
};


GAMETYPES::CLEARRANK CGamePlayResult::GetCoinRank(int32 nIndex) const
{
    return GAMETYPES::CLEARRANK(m_baserankRide + m_CharaResultRide.Chara(nIndex).m_nCoinEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetControlRank(int32 nIndex) const
{
    return GAMETYPES::CLEARRANK(m_baserankRide + m_CharaResultRide.Chara(nIndex).m_nControlEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetTrickRank(int32 nIndex) const
{
    return GAMETYPES::CLEARRANK(m_baserankRide + m_CharaResultRide.Chara(nIndex).m_nTrickEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetShotRank(int32 nIndex) const
{
    return GAMETYPES::CLEARRANK(m_baserankRide + m_CharaResultRide.Chara(nIndex).m_nKillEval);
};


int32 CGamePlayResult::GetStageMVP(int32 nStage) const
{
    ASSERT(nStage >= 0 && nStage < COUNT_OF(m_aNodeStage));

    return m_aNodeStage[nStage].m_nMVP;
};


const CGameTime& CGamePlayResult::GetStageCleartime(int32 nStage) const
{
    ASSERT(nStage >= 0 && nStage < COUNT_OF(m_aNodeStage));

    return m_aNodeStage[nStage].m_cleartime;
};


void CGamePlayResult::initClearTimeEvalTable(AREAID::VALUE idArea)
{
    uint32 uCleartimeBest = CAreaInfo::GetBestRankClearTime(idArea);
    uint32 uCleartimeWorst = CAreaInfo::GetWorstRankClearTime(idArea);

    m_auCleartimeEvalTable[0] = uCleartimeWorst;
    m_auCleartimeEvalTable[1] = (uCleartimeBest + 2 * uCleartimeWorst) / 3;
    m_auCleartimeEvalTable[2] = (uCleartimeWorst + 2 * uCleartimeBest) / 3;
    m_auCleartimeEvalTable[3] = uCleartimeBest;
};


GAMETYPES::CLEARRANK CGamePlayResult::calcClearRank(int32 nEval) const
{
    return GAMETYPES::CLEARRANK(m_baserankNormal + EvaluateInt(nEval, m_anNormalAreaEvalTable, COUNT_OF(m_anNormalAreaEvalTable)));
};


GAMETYPES::CLEARRANK CGamePlayResult::calcRideClearRank(int32 nEval) const
{
    return GAMETYPES::CLEARRANK(m_baserankRide + EvaluateInt(nEval, m_anRideAreaEvalTable, COUNT_OF(m_anRideAreaEvalTable)));
};


int32 CGamePlayResult::evaluateClearTime(void) const
{
    return EvaluateInt(m_cleartimeTotal.GetTotalSecond(), (int32*)m_auCleartimeEvalTable, COUNT_OF(m_auCleartimeEvalTable));
};


int32 CGamePlayResult::evaluateRemainingHP(void) const
{
    float fRemainingHpRatio = GetRemainedHPRatio();

    return EvaluateFloat(fRemainingHpRatio, m_afLifeEvalTable, COUNT_OF(m_afLifeEvalTable));
};


int32 CGamePlayResult::evaluateItem(void) const
{
    float fItemTakeRatio = GetItemTakeRatio();

    return EvaluateFloat(fItemTakeRatio, m_afItemEvalTable, COUNT_OF(m_afItemEvalTable));
};


CGamePlayResult::NODESTAGE* CGamePlayResult::getCurrentStage(void)
{
    int32 nStageIndex = CGameData::PlayParam().GetStageIndex();
    
    ASSERT(nStageIndex >= 0 && nStageIndex < COUNT_OF(m_aNodeStage));
    
    if (nStageIndex >= 0 && nStageIndex < COUNT_OF(m_aNodeStage))
        return &m_aNodeStage[nStageIndex];
    else
        return nullptr;
};


void CGamePlayResult::takeOnePrize(AREAID::VALUE idArea, GAMETYPES::PRIZE prize)
{
    switch (prize)
    {
    case GAMETYPES::PRIZE_CRYSTAL:
        {
            GAMETYPES::CRYSTALTYPE cry = CGameData::Record().Item().FindAreaCrystal(idArea);
            
            switch (cry)
            {
            case GAMETYPES::CRYSTALTYPE_RED:
                ASSERT(!m_bTakenRedCry);
                m_bTakenRedCry = true;
                ++m_nTakenItemCount;
                break;
                
            case GAMETYPES::CRYSTALTYPE_GREEN:
                ASSERT(!m_bTakenGreenCry);
                m_bTakenGreenCry = true;
                ++m_nTakenItemCount;
                break;
                
            case GAMETYPES::CRYSTALTYPE_WHITE:
                ASSERT(!m_bTakenWhiteCry);
                m_bTakenWhiteCry = true;
                ++m_nTakenItemCount;
                break;
                
            case GAMETYPES::CRYSTALTYPE_ORANGE:
                ASSERT(!m_bTakenOrangeCry);
                m_bTakenOrangeCry = true;
                ++m_nTakenItemCount;
                break;
            };
        }
        break;

    case GAMETYPES::PRIZE_ANTIQUE:
        ASSERT(!m_bTakenAntique);
        m_bTakenAntique = true;
        ++m_nTakenItemCount;
        break;

    case GAMETYPES::PRIZE_COMEBACK:
        ASSERT(!m_bTakenComeback);
        m_bTakenComeback = true;
        ++m_nTakenItemCount;
        break;

    case GAMETYPES::PRIZE_BONUS:
        CSecretInfo::CheckGetBonusMaterials(idArea);
        break;

    case GAMETYPES::PRIZE_SECRET:
        CGameData::Record().Secret().UnlockSecret(SECRETID::ID_SEARCH_ANTIQUE);
        break;

    default:
        ASSERT(false);
        break;
    };
};


