#include "AreaRecord.hpp"

#include "Game/Component/GameData/GameTime.hpp"


static const uint32 ALL_CLEAR_ROOT_MASK =
    BIT(CAreaRecord::CLEAR_ROOT_A) |
    BIT(CAreaRecord::CLEAR_ROOT_B) |
    BIT(CAreaRecord::CLEAR_ROOT_C);

static const uint32 CLEARTIME_RANK_E = GAMETIME_HMS(6, 20, 00);
static const uint32 CLEARTIME_RANK_D = GAMETIME_HMS(5, 01, 00);
static const uint32 CLEARTIME_RANK_C = GAMETIME_HMS(4, 01, 00);
static const uint32 CLEARTIME_RANK_B = GAMETIME_HMS(3, 01, 00);
static const uint32 CLEARTIME_RANK_A = GAMETIME_HMS(2, 31, 00);
static const uint32 CLEARTIME_RANK_S = GAMETIME_HMS(2, 11, 00);
static const uint32 CLEARTIME_RANK_SS = 0;

static const int32 POINT_BORDERLINE[] =
{
    0,  //  none
    0,  //  e
    1,  //  d
    27, //  c
    80, //  b
    133,//  a
    186,//  s
    239,//  ss
};

static const int32 POINT_PER_RANK[] =
{
    0,  //  none
    0,  //  e
    1,  //  d
    2,  //  c
    3,  //  b
    4,  //  a
    5,  //  s
    6,  //  ss
};

static_assert(COUNT_OF(POINT_BORDERLINE) == GAMETYPES::CLEARRANK_NUM, "update me");
static_assert(COUNT_OF(POINT_PER_RANK) == GAMETYPES::CLEARRANK_NUM, "update me");


CAreaRecord::CAreaRecord(void)
{
    ;
};


CAreaRecord::~CAreaRecord(void)
{
    ;
};


void CAreaRecord::SetDefault(void)
{
    m_AreaNow = AREAID::ID_NONE;
    m_AreaSelected = AREAID::ID_AREA01;
    
    for (int32 i = 0; i < COUNT_OF(m_aNodeArea); ++i)
    {
        m_aNodeArea[i].m_clearrank  = GAMETYPES::CLEARRANK_NONE;
        m_aNodeArea[i].m_cleartime  = 0;
        m_aNodeArea[i].m_rootflag   = 0;
        m_aNodeArea[i].m_state      = STATE_NONE;
    };
};


bool CAreaRecord::IsValid(void) const
{
    if (m_AreaNow < AREAID::ID_NONE && m_AreaNow >= AREAID::SELECTABLEMAX)
    {
        OUTPUT("[GAME] %s is failed: area now\n", __FUNCTION__);
        return false;
    };

    if (m_AreaSelected < AREAID::ID_NONE && m_AreaSelected >= AREAID::SELECTABLEMAX)
    {
        OUTPUT("[GAME] %s is failed: area selected\n", __FUNCTION__);
        return false;
    };

    for (int32 i = 0; i < COUNT_OF(m_aNodeArea); ++i)
    {
        if (m_aNodeArea[i].m_state < 0 ||
            m_aNodeArea[i].m_state > STATE_CLEAR)
        {
            OUTPUT("[GAME] %s is failed: area node state\n", __FUNCTION__);
            return false;
        };

        if (m_aNodeArea[i].m_clearrank < GAMETYPES::CLEARRANK_NONE ||
            m_aNodeArea[i].m_clearrank > GAMETYPES::CLEARRANK_SS)
        {
            OUTPUT("[GAME] %s is failed: area node clearrank\n", __FUNCTION__);
            return false;
        };

        if (IS_FLAG_SET_ANY(m_aNodeArea[i].m_rootflag, ~ALL_CLEAR_ROOT_MASK))
        {
            OUTPUT("[GAME] %s is failed: area node root flag\n", __FUNCTION__);
            return false;
        };
    };

    OUTPUT("[GAME] %s ...OK!\n", __FUNCTION__);
    return true;
};


void CAreaRecord::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_AreaSelected = m_AreaSelected;

    std::memset(rRawData.m_aNodeArea, 0x00, sizeof(rRawData.m_aNodeArea));
    
    for (int32 i = 0; i < COUNT_OF(m_aNodeArea); ++i)
    {
        rRawData.m_aNodeArea[i].m_state = m_aNodeArea[i].m_state;
        
        if (i < AREAID::NORMALMAX)
        {
            rRawData.m_aNodeArea[i].m_clearrank = m_aNodeArea[i].m_clearrank;
            rRawData.m_aNodeArea[i].m_cleartime = m_aNodeArea[i].m_cleartime;
            rRawData.m_aNodeArea[i].m_rootflag  = m_aNodeArea[i].m_rootflag;
        };
    };
};


void CAreaRecord::Restore(const RAWDATA& rRawData)
{
    m_AreaSelected = rRawData.m_AreaSelected;

    std::memset(m_aNodeArea, 0x00, sizeof(m_aNodeArea));

    for (int32 i = 0; i < COUNT_OF(m_aNodeArea); ++i)
    {
        m_aNodeArea[i].m_state = rRawData.m_aNodeArea[i].m_state;

        if (i < AREAID::NORMALMAX)
        {
            m_aNodeArea[i].m_clearrank  = rRawData.m_aNodeArea[i].m_clearrank;
            m_aNodeArea[i].m_cleartime  = rRawData.m_aNodeArea[i].m_cleartime;
            m_aNodeArea[i].m_rootflag   = rRawData.m_aNodeArea[i].m_rootflag;
        };
    };
};


void CAreaRecord::OnNewGame(void)
{
    SetDefault();
};


bool CAreaRecord::IsNewGame(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_aNodeArea); ++i)
    {
        if (i == AREAID::ID_NONE ||
            i == AREAID::ID_AREA03)
            continue;

        if (m_aNodeArea[i].m_state >= STATE_CLEAR)
            return false;
    };

    return true;
};


void CAreaRecord::RegistNewArea(void)
{
    if (m_AreaSelected < 0 || m_AreaSelected >= AREAID::NORMALMAX)
        return;

    if (!GetAreaState(m_AreaSelected))
        return;

    switch (m_AreaSelected)
    {
    case AREAID::ID_AREA01:
        SetAreaOpened(AREAID::ID_AREA02);
        break;
        
    case AREAID::ID_AREA02:
        SetAreaOpened(AREAID::ID_AREA04);
        SetAreaOpened(AREAID::ID_AREA09);
        SetAreaOpened(AREAID::ID_MNY_STN);
        break;
        
    case AREAID::ID_AREA04:
        SetAreaOpened(AREAID::ID_AREA05);
        break;
        
    case AREAID::ID_AREA05:
        SetAreaOpened(AREAID::ID_AREA06);
        break;
        
    case AREAID::ID_AREA09:
        SetAreaOpened(AREAID::ID_AREA10);
        break;
        
    case AREAID::ID_AREA10:
        SetAreaOpened(AREAID::ID_AREA11);
        break;
        
    case AREAID::ID_AREA11:
        if (IsAreaRootCleared(AREAID::ID_AREA11, CLEAR_ROOT_A))
        {
            SetAreaOpened(AREAID::ID_AREA12);
            SetAreaOpened(AREAID::ID_MNY_E01);
            SetAreaOpened(AREAID::ID_DHO_E01);
        };
        
        if (IsAreaRootCleared(AREAID::ID_AREA11, CLEAR_ROOT_B))
        {
            SetAreaOpened(AREAID::ID_AREA15);
            SetAreaOpened(AREAID::ID_MNY_E02);
            SetAreaOpened(AREAID::ID_DHO_E02);
        };
        break;
        
    case AREAID::ID_AREA12:
        SetAreaOpened(AREAID::ID_AREA13);
        break;
        
    case AREAID::ID_AREA13:
        SetAreaOpened(AREAID::ID_AREA14);
        break;
        
    case AREAID::ID_AREA14:
        SetAreaOpened(AREAID::ID_AREA15);
        break;
        
    case AREAID::ID_AREA15:
        SetAreaOpened(AREAID::ID_DHO_STN);
        SetAreaOpened(AREAID::ID_AREA17);
        SetAreaOpened(AREAID::ID_AREA19);
        break;
        
    case AREAID::ID_AREA17:
        SetAreaOpened(AREAID::ID_AREA18);
        break;
        
    case AREAID::ID_AREA18:
    case AREAID::ID_AREA20:
        if (GetAreaState(AREAID::ID_AREA18) == STATE_CLEAR &&
            GetAreaState(AREAID::ID_AREA20) == STATE_CLEAR)
            SetAreaOpened(AREAID::ID_AREA21);
        break;
        
    case AREAID::ID_AREA19:
        SetAreaOpened(AREAID::ID_AREA20);
        break;
        
    case AREAID::ID_AREA21:
        SetAreaOpened(AREAID::ID_AREA22);
        break;
        
    case AREAID::ID_AREA22:
        SetAreaOpened(AREAID::ID_AREA23);
        break;
        
    case AREAID::ID_AREA23:
        SetAreaOpened(AREAID::ID_AREA24);
        break;
        
    case AREAID::ID_AREA24:
        SetAreaOpened(AREAID::ID_AREA25);
        SetAreaOpened(AREAID::ID_TRI_STN);
        SetAreaOpened(AREAID::ID_DHO_E03);
        SetAreaOpened(AREAID::ID_TRI_E01);
        break;
        
    case AREAID::ID_AREA25:
        if (IsAreaRootCleared(AREAID::ID_AREA25, CLEAR_ROOT_A))
            SetAreaOpened(AREAID::ID_AREA28);
        if (IsAreaRootCleared(AREAID::ID_AREA25, CLEAR_ROOT_B))
            SetAreaOpened(AREAID::ID_AREA26);
        break;
        
    case AREAID::ID_AREA26:
        SetAreaOpened(AREAID::ID_AREA27);
        break;
        
    case AREAID::ID_AREA27:
    case AREAID::ID_AREA29:
        SetAreaOpened(AREAID::ID_AREA30);
        break;
        
    case AREAID::ID_AREA28:
        SetAreaOpened(AREAID::ID_AREA29);
        break;
        
    case AREAID::ID_AREA30:
        SetAreaOpened(AREAID::ID_AREA31);
        break;
        
    case AREAID::ID_AREA31:
        SetAreaOpened(AREAID::ID_AREA32);
        break;
        
    case AREAID::ID_AREA32:
        SetAreaOpened(AREAID::ID_AREA33);
        SetAreaOpened(AREAID::ID_JPN_STN);
        SetAreaOpened(AREAID::ID_TRI_E02);
        SetAreaOpened(AREAID::ID_JPN_E01);
        break;
        
    case AREAID::ID_AREA33:
        if (IsAreaRootCleared(AREAID::ID_AREA33, CLEAR_ROOT_A))
            SetAreaOpened(AREAID::ID_AREA36);
        if (IsAreaRootCleared(AREAID::ID_AREA33, CLEAR_ROOT_B))
            SetAreaOpened(AREAID::ID_AREA34);
        break;
        
    case AREAID::ID_AREA34:
        SetAreaOpened(AREAID::ID_AREA35);
        break;
        
    case AREAID::ID_AREA35:
    case AREAID::ID_AREA37:
        SetAreaOpened(AREAID::ID_AREA38);
        break;
        
    case AREAID::ID_AREA36:
        SetAreaOpened(AREAID::ID_AREA37);
        break;
        
    case AREAID::ID_AREA38:
        SetAreaOpened(AREAID::ID_AREA39);
        SetAreaOpened(AREAID::ID_JPN_E02);
        SetAreaOpened(AREAID::ID_MNY_E03);
        break;
        
    case AREAID::ID_AREA39:
        SetAreaOpened(AREAID::ID_AREA40);
        break;
        
    case AREAID::ID_AREA40:
        SetAreaOpened(AREAID::ID_AREA41);
        SetAreaOpened(AREAID::ID_AREA43);
        SetAreaOpened(AREAID::ID_AREA45);
        break;
        
    case AREAID::ID_AREA41:
        SetAreaOpened(AREAID::ID_AREA42);
        break;
        
    case AREAID::ID_AREA43:
        SetAreaOpened(AREAID::ID_AREA44);
        break;
        
    case AREAID::ID_AREA45:
        SetAreaOpened(AREAID::ID_AREA46);
        break;
        
    case AREAID::ID_AREA46:
        SetAreaOpened(AREAID::ID_AREA47);
        break;
        
    case AREAID::ID_AREA47:
        SetAreaOpened(AREAID::ID_AREA48);
        SetAreaOpened(AREAID::ID_MNY_E04);
        SetAreaOpened(AREAID::ID_FNY_E01);
        break;
        
    case AREAID::ID_AREA48:
        SetAreaOpened(AREAID::ID_FNY_STN);
        SetAreaOpened(AREAID::ID_AREA49);
        break;
        
    case AREAID::ID_AREA49:
        SetAreaOpened(AREAID::ID_AREA50);
        break;
        
    case AREAID::ID_AREA50:
        SetAreaOpened(AREAID::ID_AREA52);
        SetAreaOpened(AREAID::ID_MNY_E05);
        SetAreaOpened(AREAID::ID_KUR_E01);
        break;
        
    case AREAID::ID_AREA52:
        SetAreaOpened(AREAID::ID_KUR_STN);
        SetAreaOpened(AREAID::ID_AREA53);
        SetAreaOpened(AREAID::ID_AREA54);
        SetAreaOpened(AREAID::ID_AREA55);
        break;
        
    case AREAID::ID_AREA53:
    case AREAID::ID_AREA54:
    case AREAID::ID_AREA55:
        if (GetAreaState(AREAID::ID_AREA53) == STATE_CLEAR &&
            GetAreaState(AREAID::ID_AREA54) == STATE_CLEAR &&
            GetAreaState(AREAID::ID_AREA55) == STATE_CLEAR)
        {
            SetAreaOpened(AREAID::ID_AREA56);
        };
        break;
        
    case AREAID::ID_AREA56:
        SetAreaOpened(AREAID::ID_AREA57);
        break;
        
    case AREAID::ID_AREA57:
        SetAreaOpened(AREAID::ID_AREA58);
        break;
        
    default:
        break;
    };
};


void CAreaRecord::SetNowClearArea(void)
{
    m_AreaNow = m_AreaSelected;
};


AREAID::VALUE CAreaRecord::GetNowClearArea(void)
{
    AREAID::VALUE idArea = m_AreaNow;
    m_AreaNow = AREAID::ID_NONE;
    return idArea;
};


void CAreaRecord::SetAreaOpened(AREAID::VALUE idArea)
{
    if (!isSelectableArea(idArea))
        return;

    NODEAREA& nodearea = m_aNodeArea[idArea];
	if(nodearea.m_state == STATE_NONE)
		nodearea.m_state = STATE_OPEN;
};


void CAreaRecord::SetAreaCleared(AREAID::VALUE idArea, CLEAR_ROOT clearroot)
{
    if (!isSelectableArea(idArea))
        return;

    NODEAREA& nodearea = m_aNodeArea[idArea];

    nodearea.m_state = STATE_CLEAR;    
    FLAG_SET(nodearea.m_rootflag, BIT(clearroot));
    
    ASSERT(!IS_FLAG_SET_ANY(nodearea.m_rootflag, ~ALL_CLEAR_ROOT_MASK));
};


void CAreaRecord::SetAreaRank(AREAID::VALUE idArea, GAMETYPES::CLEARRANK clearrank)
{
    if (!isSelectableArea(idArea))
        return;

    NODEAREA& nodearea = m_aNodeArea[idArea];

    nodearea.m_clearrank = clearrank;
};


void CAreaRecord::UpdateAreaClearTime(AREAID::VALUE idArea, const CGameTime& cleartime)
{
    if (!isSelectableArea(idArea))
        return;

    NODEAREA& nodearea = m_aNodeArea[idArea];

    if (nodearea.m_cleartime < cleartime)
        nodearea.m_cleartime = cleartime;
};


CAreaRecord::STATE CAreaRecord::GetAreaState(AREAID::VALUE idArea) const
{
    return getNode(idArea).m_state;
};


bool CAreaRecord::IsAreaRootCleared(AREAID::VALUE idArea, CLEAR_ROOT clearroot) const
{
    if (!isSelectableArea(idArea))
        return false;

    if (m_aNodeArea[idArea].m_state != STATE_CLEAR)
        return false;

    return IS_FLAG_SET(m_aNodeArea[idArea].m_rootflag, BIT(clearroot));
};


GAMETYPES::CLEARRANK CAreaRecord::GetAreaClearRank(AREAID::VALUE idArea) const
{
	if (isSelectableArea(idArea))
		return getNode(idArea).m_clearrank;
	else
		return GAMETYPES::CLEARRANK_NONE;
};


const CGameTime& CAreaRecord::GetAreaClearTime(AREAID::VALUE idArea) const
{
	return getNode(idArea).m_cleartime;
};


void CAreaRecord::SetCurrentSelectedArea(AREAID::VALUE idArea)
{
    m_AreaSelected = idArea;
};


AREAID::VALUE CAreaRecord::GetCurrentSelectedArea(void) const
{
    return m_AreaSelected;
};


int32 CAreaRecord::CountRankedArea(GAMETYPES::CLEARRANK clearrank) const
{
    int32 iResult = 0;

    for (int32 i = 0; i < AREAID::NORMALMAX; ++i)
    {
        if (i != AREAID::ID_NONE &&
            i != AREAID::ID_AREA03 &&
            GetAreaClearRank(AREAID::VALUE(i)) == clearrank)
        {
            ++iResult;
        };
    };

    return iResult;
};


GAMETYPES::CLEARRANK CAreaRecord::CalcTotalClearRank(void) const
{
    int32 iResult = GAMETYPES::CLEARRANK_NONE;
    int32 iTotalPoints = 0;
    
    for (int32 i = 0; i < AREAID::NORMALMAX; ++i)
    {
        ASSERT(m_aNodeArea[i].m_clearrank < COUNT_OF(POINT_PER_RANK));
        ASSERT(m_aNodeArea[i].m_clearrank >= GAMETYPES::CLEARRANK_NONE &&
               m_aNodeArea[i].m_clearrank < GAMETYPES::CLEARRANK_NUM);
        
        iTotalPoints += POINT_PER_RANK[m_aNodeArea[i].m_clearrank];
    };

	for (int32 i = GAMETYPES::CLEARRANK_SS; i > 0; --i)
	{
		if (iTotalPoints >= POINT_BORDERLINE[i])
		{
			iResult = i;
			break;
		};
	};

    return GAMETYPES::CLEARRANK(iResult);
};


void CAreaRecord::CalcTotalClearTime(CGameTime& totalCleartime) const
{
    totalCleartime.Clear();
    
    for (int32 i = 0; i < AREAID::NORMALMAX; ++i)     
        totalCleartime.Add(CGameTime(m_aNodeArea[i].m_cleartime));
};


GAMETYPES::CLEARRANK CAreaRecord::CalcTotalClearTimeRank(void) const
{
    CGameTime totalCleartime;
    CalcTotalClearTime(totalCleartime);

    uint32 cleartime = totalCleartime.GetTotalSecond();

    if (cleartime >= CLEARTIME_RANK_E)
        return GAMETYPES::CLEARRANK_E;
    else if (cleartime >= CLEARTIME_RANK_D)
        return GAMETYPES::CLEARRANK_D;
    else if (cleartime >= CLEARTIME_RANK_C)
        return GAMETYPES::CLEARRANK_C;
    else if (cleartime >= CLEARTIME_RANK_B)
        return GAMETYPES::CLEARRANK_B;
    else if (cleartime >= CLEARTIME_RANK_A)
        return GAMETYPES::CLEARRANK_A;
    else if (cleartime >= CLEARTIME_RANK_S)
        return GAMETYPES::CLEARRANK_S;

    return GAMETYPES::CLEARRANK_SS;
};


const CAreaRecord::NODEAREA& CAreaRecord::getNode(AREAID::VALUE idArea) const
{
    ASSERT(isSelectableArea(idArea));

    return m_aNodeArea[idArea];
};


bool CAreaRecord::isSelectableArea(AREAID::VALUE idArea) const
{
    return (idArea >= AREAID::ID_NONE && idArea < AREAID::SELECTABLEMAX);
};