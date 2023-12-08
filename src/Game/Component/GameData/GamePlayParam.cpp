#include "GamePlayParam.hpp"
#include "GameData.hpp"

#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/SecretInfo.hpp"


/*static*/ CGamePlayParam::PRIZEINFO CGamePlayParam::PRIZEINFO::DUMMY;


/*static*/ int32 CGamePlayParam::m_anPlayerHPMax[] =
{
    0,
    300,
    500,
    700,
    900,
};


/*static*/ int32 CGamePlayParam::m_anRidePrizePoint[] =
{
    10000,
    8000,
    6000,
    4000,
};


/*static*/ int32 CGamePlayParam::m_anShurikenMax[] =
{
    10,
    10,
    30,
    30,
};


void CGamePlayParam::CHARAINFO::Init(void)
{
    m_Costume       = GAMETYPES::COSTUME_NONE;
    m_CharacterID   = PLAYERID::ID_MAX;
    m_iPlayerNo     = -1;
};


void CGamePlayParam::PLAYERCONTEXT::Init(int32 no)
{
    m_iNo           = no;
    m_iHP           = 0;
    m_iHPMax        = 0;
    m_iDamage       = 0;
    m_iShurikenNum  = 0;
    m_iShurikenMax  = 0;
    m_CharacterID   = PLAYERID::ID_MAX;
    m_iCharacterNum = 0;
};


void CGamePlayParam::PLAYERCONTEXT::AddChara(PLAYERID::VALUE idChara)
{
    if (!m_iCharacterNum++)
        m_CharacterID = idChara;
};


bool CGamePlayParam::PLAYERCONTEXT::IsPlaying(void) const
{
    return (m_iCharacterNum > 0);
};


void CGamePlayParam::PRIZEINFO::Init(void)
{
    m_PrizeType     = GAMETYPES::PRIZE_NONE;
    m_iPointsNum    = 0;
    m_bTaken        = false;
};


CGamePlayParam::CGamePlayParam(void)
{
    ;
};


CGamePlayParam::~CGamePlayParam(void)
{
    ;
};


void CGamePlayParam::Clear(void)
{
    ClearPlayer();
    ClearArea();
};


void CGamePlayParam::InitForArea(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aPlayerContext); ++i)
    {
        if (!m_aPlayerContext[i].IsPlaying())
            continue;

        m_aPlayerContext[i].m_iHP           = m_anPlayerHPMax[m_numPlayer];
        m_aPlayerContext[i].m_iHPMax        = m_anPlayerHPMax[m_numPlayer];
        m_aPlayerContext[i].m_iShurikenMax  = m_anShurikenMax[CGameData::Record().Secret().GetAerialLevel()];
        m_aPlayerContext[i].m_iDamage       = 0;
    };

    initPrizeInfo();
};


void CGamePlayParam::InitForStage(void)
{
    ;
};


void CGamePlayParam::ClearArea(void)
{
    m_StageID = STAGEID::ID_NONE;
    m_AreaID = AREAID::ID_NONE;
    m_iStageIndex = 0;
    m_playmode = AREAPLAYMODE_SERIES;
    m_numPrize = 0;
    
    for (int32 i = 0; i < COUNT_OF(m_aPrizeInfo); ++i)
        m_aPrizeInfo[i].Init();
};


void CGamePlayParam::SetAreaPlaymode(AREAPLAYMODE playmode)
{
    m_playmode = playmode;
};


void CGamePlayParam::SetStartArea(AREAID::VALUE idArea, int32 iStageIndex)
{
    m_AreaID        = idArea;
    m_iStageIndex   = iStageIndex;
    m_StageID       = CAreaInfo::GetStageID(m_AreaID, m_iStageIndex);
};


bool CGamePlayParam::NextStage(void)
{
    if (m_playmode == AREAPLAYMODE_ONE_STAGE)
        return false;

    if (++m_iStageIndex >= CAreaInfo::GetStageNum(m_AreaID))
        return false;

    m_StageID = CAreaInfo::GetStageID(m_AreaID, m_iStageIndex);
    
    return true;
};


void CGamePlayParam::SetStage(STAGEID::VALUE idStage)
{
    m_StageID       = idStage;
    m_AreaID        = CStageInfo::GetAreaID(m_StageID);
    m_iStageIndex   = CAreaInfo::IndexOfStage(m_AreaID, m_StageID);
    
    if (m_iStageIndex < 0)
        m_iStageIndex = CAreaInfo::GetStageNum(m_AreaID);    
};


void CGamePlayParam::ClearPlayer(void)
{
    m_numCharaInfo = 0;
    m_numPlayer = 0;
    
    for (int32 i = 0; i < COUNT_OF(m_aCharaInfo); ++i)
        m_aCharaInfo[i].Init();

    for (int32 i = 0; i < COUNT_OF(m_aPlayerContext); ++i)
        m_aPlayerContext[i].Init(i);
};


void CGamePlayParam::AddPlayerCharacter(int32 iPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    ASSERT(m_numCharaInfo < COUNT_OF(m_aCharaInfo));

    if (m_numCharaInfo >= COUNT_OF(m_aCharaInfo))
        return;

    int32 no = m_numCharaInfo;
    
    m_aCharaInfo[no].m_iPlayerNo = iPlayerNo;
    m_aCharaInfo[no].m_CharacterID = idPlayer;
    m_aCharaInfo[no].m_Costume = costume;
    ++m_numCharaInfo;

    m_aPlayerContext[iPlayerNo].AddChara(idPlayer);
    if (m_aPlayerContext[iPlayerNo].m_iCharacterNum == 1)
        ++m_numPlayer;
};


CGamePlayParam::AREAPLAYMODE CGamePlayParam::GetAreaPlaymode(void) const
{
    return m_playmode;
};


AREAID::VALUE CGamePlayParam::GetArea(void) const
{
    return m_AreaID;
};


int32 CGamePlayParam::GetStageIndex(void) const
{
    return m_iStageIndex;
};


bool CGamePlayParam::IsFirstStage(void) const
{
    return (m_iStageIndex == 0);
};


bool CGamePlayParam::IsLastStage(void) const
{
    int32 iStageNum = CAreaInfo::GetStageNum(m_AreaID) - 1;
    
    if (iStageNum < 0)
        iStageNum = 0;
    
    return (m_iStageIndex == iStageNum);
};


STAGEID::VALUE CGamePlayParam::GetStage(void) const
{
    return m_StageID;
};


GAMETYPES::STAGEMODE CGamePlayParam::GetStageMode(void) const
{
    return CStageInfo::GetMode(m_StageID);
};


int32 CGamePlayParam::GetPlayerNum(void) const
{
    return m_numPlayer;
};


int32 CGamePlayParam::GetCharaInfoNum(void) const
{
    return m_numCharaInfo;
};


int32 CGamePlayParam::GetPrizeInfoNum(void) const
{
    return m_numPrize;
};


const CGamePlayParam::PRIZEINFO& CGamePlayParam::PrizeInfo(int32 iPrizeIndex) const
{
    if (iPrizeIndex >= 0 && iPrizeIndex < COUNT_OF(m_aPrizeInfo))
        return m_aPrizeInfo[iPrizeIndex];
    else
        return PRIZEINFO::DUMMY;
};


const CGamePlayParam::CHARAINFO& CGamePlayParam::CharaInfo(int32 iCharainfoIndex) const
{
    if (iCharainfoIndex >= 0 && iCharainfoIndex < COUNT_OF(m_aCharaInfo))
        return m_aCharaInfo[iCharainfoIndex];
    else
        return m_aCharaInfo[0];
};


CGamePlayParam::PLAYERCONTEXT& CGamePlayParam::PlayerContext(int32 iPlayerIndex)
{
    ASSERT(iPlayerIndex >= 0 && iPlayerIndex < COUNT_OF(m_aPlayerContext));
    return m_aPlayerContext[iPlayerIndex];
};


bool CGamePlayParam::IsSpaceRide(void) const
{
    return (GetArea() == AREAID::ID_AREA22 ||
            GetArea() == AREAID::ID_AREA32);
};


bool CGamePlayParam::isPrizeAlreadyTaken(AREAID::VALUE idArea, GAMETYPES::PRIZE prize) const
{
    bool bResult = false;
    
    switch (prize)
    {
    case GAMETYPES::PRIZE_CRYSTAL:
        {
            GAMETYPES::CRYSTALTYPE crystal = CGameData::Record().Item().FindAreaCrystal(idArea);
            if (crystal != GAMETYPES::CRYSTALTYPE_NONE)
            {
                if (CGameData::Record().Item().IsAreaCrystalTaken(crystal, idArea))
                    bResult = true;
            };
        }        
        break;

    case GAMETYPES::PRIZE_ANTIQUE:
        {
            if (CGameData::Record().Antique().IsAreaAntiqueTaken(idArea))
                bResult = true;
        }
        break;

    case GAMETYPES::PRIZE_BONUS:
        {
            DBITEMID::VALUE idDbitem = DBITEMID::ID_NONE;
            
            switch (idArea)
            {
            case AREAID::ID_AREA06:
                idDbitem = CSecretInfo::GetDatabase(SECRETID::ID_DATABASE_PROMOTION_ARTS);
                break;

            case AREAID::ID_AREA13:
                idDbitem = CSecretInfo::GetDatabase(SECRETID::ID_DATABASE_ANIMS);
                break;

            case AREAID::ID_AREA22:
                idDbitem = CSecretInfo::GetDatabase(SECRETID::ID_DATABASE_CG);
                break;

            case AREAID::ID_AREA32:
                idDbitem = CSecretInfo::GetDatabase(SECRETID::ID_DATABASE_SECONDARY_A);
                break;

            case AREAID::ID_AREA46:
                idDbitem = CSecretInfo::GetDatabase(SECRETID::ID_DATABASE_SECONDARY_B);
                break;
            };

            if (idDbitem != DBITEMID::ID_NONE)
                bResult = CGameData::Record().Database().IsItemUnlocked(idDbitem);
        }
        break;

    case GAMETYPES::PRIZE_COMEBACK:
        bResult = CGameData::Record().Item().IsComebackProcessed();
        break;

    case GAMETYPES::PRIZE_SECRET:
        bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_SEARCH_ANTIQUE);
        break;
    };

    return bResult;
};


void CGamePlayParam::initPrizeInfo(void)
{
    GAMETYPES::PRIZE* pPrizeArray = getPrizeArray(m_AreaID);

    for (int32 i = 0; i < COUNT_OF(m_aPrizeInfo); ++i)
    {
        m_aPrizeInfo[i].m_PrizeType = pPrizeArray[i];
        
        if (pPrizeArray[i] != GAMETYPES::PRIZE_NONE)
        {
            ++m_numPrize;
            m_aPrizeInfo[i].m_bTaken = isPrizeAlreadyTaken(m_AreaID, pPrizeArray[i]);

            if (CStageInfo::GetMode(m_StageID) == GAMETYPES::STAGEMODE_RIDE)
                m_aPrizeInfo[i].m_iPointsNum = m_anRidePrizePoint[i];
            else
                m_aPrizeInfo[i].m_iPointsNum = 0;

        }
        else
        {
            m_aPrizeInfo[i].m_iPointsNum = 0;
            m_aPrizeInfo[i].m_bTaken = false;
        };
    };
};


GAMETYPES::PRIZE* CGamePlayParam::getPrizeArray(AREAID::VALUE idArea) const
{
    static GAMETYPES::PRIZE s_aPrizeDummy[] =
    {
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
    };

    static GAMETYPES::PRIZE s_aPrizeArea06[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_ANTIQUE,
        GAMETYPES::PRIZE_COMEBACK,
        GAMETYPES::PRIZE_BONUS,
    };

    static GAMETYPES::PRIZE s_aPrizeArea13[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_ANTIQUE,
        GAMETYPES::PRIZE_COMEBACK,
        GAMETYPES::PRIZE_BONUS,
    };

    static GAMETYPES::PRIZE s_aPrizeArea22[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_ANTIQUE,
        GAMETYPES::PRIZE_COMEBACK,
        GAMETYPES::PRIZE_BONUS,
    };

    static GAMETYPES::PRIZE s_aPrizeArea32[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_ANTIQUE,
        GAMETYPES::PRIZE_COMEBACK,
        GAMETYPES::PRIZE_BONUS,
    };

    static GAMETYPES::PRIZE s_aPrizeArea46[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_ANTIQUE,
        GAMETYPES::PRIZE_COMEBACK,
        GAMETYPES::PRIZE_BONUS,
    };

    static GAMETYPES::PRIZE s_aPrizeArea60A[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_SECRET,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
    };

    static GAMETYPES::PRIZE s_aPrizeArea60B[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
    };

    static GAMETYPES::PRIZE s_aPrizeArea60C[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_ANTIQUE,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
    };


    static GAMETYPES::PRIZE s_aPrizeArea60D[] =
    {
        GAMETYPES::PRIZE_CRYSTAL,
        GAMETYPES::PRIZE_ANTIQUE,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
    };

    static GAMETYPES::PRIZE s_aPrizeNexus2nd[] =
    {
        GAMETYPES::PRIZE_COMEBACK,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
        GAMETYPES::PRIZE_NONE,
    };

    static_assert(COUNT_OF(s_aPrizeArea06) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea13) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea22) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea32) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea46) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea60A) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea60B) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea60C) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeArea60D) == GAMETYPES::PRIZE_MAX, "update me");
    static_assert(COUNT_OF(s_aPrizeNexus2nd) == GAMETYPES::PRIZE_MAX, "update me");

    GAMETYPES::PRIZE* pPrizeArray = s_aPrizeDummy;
    
    switch (idArea)
    {
    case AREAID::ID_AREA06:
        pPrizeArray = s_aPrizeArea06;
        break;
        
    case AREAID::ID_AREA13:
        pPrizeArray = s_aPrizeArea13;
        break;
        
    case AREAID::ID_AREA22:
        pPrizeArray = s_aPrizeArea22;
        break;
        
    case AREAID::ID_AREA32:
        pPrizeArray = s_aPrizeArea32;
        break;
        
    case AREAID::ID_AREA46:
        pPrizeArray = s_aPrizeArea46;
        break;
        
    case AREAID::ID_AREA60_A:
        pPrizeArray = s_aPrizeArea60A;
        break;
        
    case AREAID::ID_AREA60_B:
        pPrizeArray = s_aPrizeArea60B;
        break;
        
    case AREAID::ID_AREA60_C:
        pPrizeArray = s_aPrizeArea60D;
        break;
        
    case AREAID::ID_AREA60_D:
        pPrizeArray = s_aPrizeArea60C;
        break;
        
    default:
        pPrizeArray = s_aPrizeDummy;
        break;
    }

    if (idArea >= AREAID::NEXUSSTART && idArea < AREAID::NEXUSMAX)
    {
        GAMETYPES::NEXUSID NexusID = CAreaInfo::GetNexusID(idArea);
        ASSERT(NexusID != GAMETYPES::NEXUSID_NUM);
        
        if (CGameData::Record().Nexus().GetTournamentState(NexusID) == CNexusRecord::STATE_CLEAR)
            pPrizeArray = s_aPrizeNexus2nd;        
    };

    return pPrizeArray;
};