#include "GamePlayResult.hpp"
#include "GameData.hpp"
#include "GameTime.hpp"

#include "Game/Component/GameMain/SecretInfo.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"


template<class T>
static inline int32 evaluateValue(T value, const T* aTable, int32 nTableSize)
{
    int32 iResult = 0;

    if (aTable[0] >= aTable[nTableSize - 1])
    {
        for (int32 i = 0; i < nTableSize; ++i)
        {
            if (value > aTable[i])
                break;

            ++iResult;
        };
    }
    else
    {
        for (int32 i = 0; i < nTableSize; ++i)
        {
            if (value < aTable[i])
                break;

            ++iResult;
        };
    };

    return iResult;
};


/*static*/ const float CGamePlayResult::m_afLifeEvalTable[] =
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
    return evaluateValue(nValue, anTable, nTableSize);
};


/*static*/ int32 CGamePlayResult::EvaluateFloat(float fValue, const float* afTable, int32 nTableSize)
{
    return evaluateValue(fValue, afTable, nTableSize);
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
    m_baserank          = GAMETYPES::CLEARRANK_NONE;
    m_bTakenRedCry      = false;
    m_bTakenGreenCry    = false;
    m_bTakenOrangeCry   = false;
    m_bTakenWhiteCry    = false;
    m_bTakenAntique     = false;
    m_bTakenComeback    = false;
    m_cleartime.Clear();
    m_nRemainingHP      = 0;
    m_nMaxHP            = 0;
    m_nTakenItemCount   = 0;
    m_nTotalItemCount   = 0;
    m_arearank          = GAMETYPES::CLEARRANK_NONE;
    m_nRidePrizeNo      = -1;

    for (int32 i = 0; i < COUNT_OF(m_aStageNode); ++i)
    {
        m_aStageNode[i].m_mvp = 0;
        m_aStageNode[i].m_cleartime.Clear();
    };

    m_aPlayerID[0] = PLAYERID::ID_MAX;
    m_aPlayerID[1] = PLAYERID::ID_MAX;
    m_aPlayerID[2] = PLAYERID::ID_MAX;
    m_aPlayerID[3] = PLAYERID::ID_MAX;

    m_CharaResult.Clear();
    m_StageResult.Clear();
    m_RideResult.Clear();

    m_aClearEvalTable[0] = 0;
    m_aClearEvalTable[1] = 0;
    m_aClearEvalTable[2] = 0;
    m_aClearEvalTable[3] = 0;
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
    if (m_result == AREARESULT_GAMECLEAR)
        return (CGameData::PlayParam().GetArea() == AREAID::ID_AREA58);
    
    return false;
};


bool CGamePlayResult::IsNexusEnding(void) const
{
    if (m_result == AREARESULT_GAMECLEAR)
        return (CGameData::PlayParam().GetArea() == AREAID::ID_AREA60_D);

    return false;
};


bool CGamePlayResult::IsEnding(void) const
{
    return (IsScenarioEnding() || IsNexusEnding());
};


void CGamePlayResult::InitForArea(void)
{
    Clear();

	GAMETYPES::DIFFICULTY difficulty = CGameData::Option().Play().GetDifficulty();
    switch (difficulty)
    {
    case GAMETYPES::DIFFICULTY_EASY:
        m_baserank = GAMETYPES::CLEARRANK_E;
        break;

    case GAMETYPES::DIFFICULTY_NORMAL:
        m_baserank = GAMETYPES::CLEARRANK_D;
        break;

    case GAMETYPES::DIFFICULTY_HARD:
        m_baserank = GAMETYPES::CLEARRANK_C;
        break;

    default:
        ASSERT(false);
        break;
    };

    int32 charaInfoNum = CGameData::PlayParam().GetCharaInfoNum();
    ASSERT(charaInfoNum >= 0);
    ASSERT(charaInfoNum <= COUNT_OF(m_aPlayerID));
    
    for (int32 i = 0; i < charaInfoNum; ++i)
    {
        const CGamePlayParam::CHARAINFO& charaInfo = CGameData::PlayParam().CharaInfo(i);
        m_aPlayerID[i] = charaInfo.m_CharacterID;
    };

    int32 playerCtxNum = CGameData::PlayParam().GetPlayerNum();
    if (playerCtxNum > 0)
    {
        CGamePlayParam::PLAYERCONTEXT& playerCtx = CGameData::PlayParam().PlayerContext(0);
        m_nMaxHP = playerCtx.m_iHPMax;
    };
};


void CGamePlayResult::InitForStage(void)
{
    m_StageResult.Clear();
};


void CGamePlayResult::TermForStage(void)
{
    m_StageResult.Evaluate();

    STAGENODE* pStageNode = getCurrentStage();
    ASSERT(pStageNode);

    if (pStageNode)
        pStageNode->m_mvp = m_StageResult.GetMVP();
};


void CGamePlayResult::Evaluate(void)
{
    m_CharaResult.Evaluate();

    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
    {
        m_RideResult.Evaluate();
        
        int32 nPoint = m_RideResult.GetTotalCoinPoint();
        int32 nEval = EvaluateInt(nPoint, m_anRideAreaEvalTable, COUNT_OF(m_anRideAreaEvalTable));
        m_arearank = calcRideClearRank(nEval);
    }
    else
    {
        AREAID::VALUE areaId = CGameData::PlayParam().GetArea();
        initClearTimeEvalTable(areaId);

        int32 nEvalTime = evaluateClearTime();
        int32 nEvalHP   = evaluateRemainingHP();
        int32 nEvalItem = evaluateItem();
        int32 nEvalMVP  = m_CharaResult.Chara(m_CharaResult.GetMVP()).m_nPersonalPoint;
        int32 nEvalTotal= (nEvalTime + nEvalHP + nEvalItem + nEvalMVP);

        int32 nEval = EvaluateInt(nEvalTotal, m_anNormalAreaEvalTable, COUNT_OF(m_anNormalAreaEvalTable));

        m_arearank = calcClearRank(nEval);
    };
};


void CGamePlayResult::TakePrize(AREAID::VALUE idArea)
{
    switch (m_type)
    {
    case GAMETYPES::RESULTTYPE_RIDE:
        {
            for (int32 i = 0; i < CGameData::PlayParam().GetPrizeInfoNum(); ++i)
            {
                const CGamePlayParam::PRIZEINFO& prizeinfo = CGameData::PlayParam().PrizeInfo(i);

                if ((prizeinfo.m_iPointsNum <= m_RideResult.GetTotalCoinPoint()) && !prizeinfo.m_bTaken)
                {
                    m_nRidePrizeNo = i;
                    takeOnePrize(idArea, prizeinfo.m_PrizeType);
                    break;
                };
            };
        }
        break;

    case GAMETYPES::RESULTTYPE_NEXUS:
        {
            for (int32 i = 0; i < CGameData::PlayParam().GetPrizeInfoNum(); ++i)
                takeOnePrize(idArea, CGameData::PlayParam().PrizeInfo(i).m_PrizeType);
        }
        break;

    default:
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

            CGameData::Record().Area().UpdateAreaClearTime(idArea, m_cleartime);
        }
        break;

    case GAMETYPES::RESULTTYPE_NEXUS:
        if (m_result == AREARESULT_GAMECLEAR)
        {
            GAMETYPES::NEXUSID idNexus = CAreaInfo::GetNexusID(CGameData::PlayParam().GetArea());
            ASSERT(idNexus != GAMETYPES::NEXUSID_NUM);

            CGameData::Record().Nexus().SetTournamentState(idNexus, CNexusRecord::STATE_CLEAR);
            CGameData::Record().Nexus().UpdateTournamentClearTime(idNexus, m_cleartime);
        };
        break;

    default:
        break;
    };
};


void CGamePlayResult::SetStageClearSecond(float fSeconds)
{
    CGameTime cleartime(static_cast<uint32>(fSeconds));

    STAGENODE* pStageNode = getCurrentStage();
    ASSERT(pStageNode);

    if (pStageNode)
        pStageNode->m_cleartime = cleartime;
    
    m_cleartime.Add(cleartime);
};


void CGamePlayResult::SetRemainedHP(int32 iRemainHP)
{
    m_nRemainingHP = iRemainHP;
};


void CGamePlayResult::SetRideCharaPlaySecond(PLAYERID::VALUE idPlayer, float fSeconds)
{
    int32 nIndex = IndexOfChara(idPlayer);
    m_RideResult.SetCharaPlaytime(nIndex, fSeconds);
};


void CGamePlayResult::AddTotalItemCount(void)
{
    ++m_nTotalItemCount;
};


void CGamePlayResult::AddTakenItemCount(ITEMID::VALUE idItem)
{
    ASSERT(idItem > ITEMID::ID_NONE);
    ASSERT(idItem < ITEMID::ID_MAX);

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
#if !defined(_DEBUG)
        ASSERT(!m_bTakenComeback);
#endif /* !_DEBUG */        
        m_bTakenComeback = true;
        break;

    default:
        break;
    };
};


void CGamePlayResult::AddTechnicalAction(PLAYERID::VALUE idPlayer, GAMETYPES::TECACT tecact)
{
    ASSERT(tecact >= 0);
    ASSERT(tecact < GAMETYPES::TECACT_MAX);

    if ((tecact >= 0) && (tecact < GAMETYPES::TECACT_MAX))
    {
        int32 nIndex = IndexOfChara(idPlayer);
        m_CharaResult.AddTechnicalAction(nIndex, idPlayer, tecact);
        m_StageResult.AddTechnicalAction(nIndex, idPlayer, tecact);
    };    
};


void CGamePlayResult::AddRideAction(PLAYERID::VALUE idPlayer, GAMETYPES::RIDEACT rideact)
{
    ASSERT(rideact >= 0);
    ASSERT(rideact < GAMETYPES::RIDEACT_MAX);

    int32 nIndex = IndexOfChara(idPlayer);
    m_RideResult.AddRideAction(nIndex, rideact);
};


PLAYERID::VALUE CGamePlayResult::GetPlayerCharacter(int32 nIndex) const
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < COUNT_OF(m_aPlayerID));

    return m_aPlayerID[nIndex];
};


int32 CGamePlayResult::IndexOfChara(PLAYERID::VALUE idPlayer) const
{
    for (int32 i = 0; i < COUNT_OF(m_aPlayerID); ++i)
    {
        if (m_aPlayerID[i] == idPlayer)
            return i;
    };

    ASSERT(false);

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
    return calcClearRank(m_CharaResult.Chara(nIndex).m_nOffenseEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetDefenceTechRank(int32 nIndex) const
{
    return calcClearRank(m_CharaResult.Chara(nIndex).m_nDefenseEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetAerialTechRank(int32 nIndex) const
{
    return calcClearRank(m_CharaResult.Chara(nIndex).m_nAerialEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetPersonalRank(int32 nIndex) const
{
    if (m_type == GAMETYPES::RESULTTYPE_RIDE)
        return calcRideClearRank(m_RideResult.Chara(nIndex).m_nPersonalEval);
    else
        return calcClearRank(m_CharaResult.Chara(nIndex).m_nPersonalEval);
};


int32 CGamePlayResult::GetMVP(void) const
{
    if (m_type == GAMETYPES::RESULTTYPE_RIDE)
        return m_RideResult.GetMVP();
    else
        return m_CharaResult.GetMVP();
};


GAMETYPES::CLEARRANK CGamePlayResult::GetTotalRank(void) const
{
    return m_arearank;
};


CGameTime CGamePlayResult::GetCleartimeTotal(void) const
{
    return CGameTime(m_cleartime);
};


float CGamePlayResult::GetRemainedHPRatio(void) const
{
    ASSERT(m_nMaxHP > 0);

    if (m_nMaxHP)
        return (100.0f * static_cast<float>(m_nRemainingHP)) / static_cast<float>(m_nMaxHP);
    
    return 100.0f;
};


float CGamePlayResult::GetItemTakeRatio(void) const
{
    if (m_nTotalItemCount)
        return (100.0f * static_cast<float>(m_nTakenItemCount)) / static_cast<float>(m_nTotalItemCount);
    
    return 100.0f;
};


int32 CGamePlayResult::GetRideGoldCoin(void) const
{
    return m_RideResult.GetTotalGoldCoin();
};


int32 CGamePlayResult::GetRideSilverCoin(void) const
{
    return m_RideResult.GetTotalSilverCoin();
};


int32 CGamePlayResult::GetRideCoinPoint(void) const
{
    return m_RideResult.GetTotalCoinPoint();
};


int32 CGamePlayResult::GetRideTakenPrizeNo(void) const
{
    return m_nRidePrizeNo;
};


GAMETYPES::CLEARRANK CGamePlayResult::GetCoinRank(int32 nIndex) const
{
    return calcRideClearRank(m_RideResult.Chara(nIndex).m_nCoinEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetControlRank(int32 nIndex) const
{
    return calcRideClearRank(m_RideResult.Chara(nIndex).m_nControlEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetTrickRank(int32 nIndex) const
{
    return calcRideClearRank(m_RideResult.Chara(nIndex).m_nTrickEval);
};


GAMETYPES::CLEARRANK CGamePlayResult::GetShotRank(int32 nIndex) const
{
    return calcRideClearRank(m_RideResult.Chara(nIndex).m_nShotEval);
};


int32 CGamePlayResult::GetStageMVP(int32 nIndex) const
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < COUNT_OF(m_aStageNode));

    return m_aStageNode[nIndex].m_mvp;
};


CGameTime CGamePlayResult::GetStageCleartime(int32 nIndex) const
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < COUNT_OF(m_aStageNode));

    return CGameTime(m_aStageNode[nIndex].m_cleartime);
};


void CGamePlayResult::initClearTimeEvalTable(AREAID::VALUE idArea)
{
    uint32 uCleartimeBest = CAreaInfo::GetBestRankClearTime(idArea);
    uint32 uCleartimeWorst = CAreaInfo::GetWorstRankClearTime(idArea);

    m_aClearEvalTable[0] = uCleartimeWorst;
    m_aClearEvalTable[1] = (uCleartimeBest + 2 * uCleartimeWorst) / 3;
    m_aClearEvalTable[2] = (uCleartimeWorst + 2 * uCleartimeBest) / 3;
    m_aClearEvalTable[3] = uCleartimeBest;
};


GAMETYPES::CLEARRANK CGamePlayResult::calcClearRank(int32 nEval) const
{
    return GAMETYPES::CLEARRANK(nEval + m_baserank);
};


GAMETYPES::CLEARRANK CGamePlayResult::calcRideClearRank(int32 nEval) const
{
    return GAMETYPES::CLEARRANK(nEval + 1);
};


int32 CGamePlayResult::evaluateClearTime(void) const
{
    uint32 uTotalSec = m_cleartime.GetTotalSecond();

    return evaluateValue(uTotalSec, m_aClearEvalTable, COUNT_OF(m_aClearEvalTable));
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


CGamePlayResult::STAGENODE* CGamePlayResult::getCurrentStage(void)
{
    int32 nStageIndex = CGameData::PlayParam().GetStageIndex();
    
    ASSERT(nStageIndex >= 0);
    ASSERT(nStageIndex < COUNT_OF(m_aStageNode));

    if ((nStageIndex >= 0) && (nStageIndex < COUNT_OF(m_aStageNode)))
        return &m_aStageNode[nStageIndex];
    
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

            default:
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


