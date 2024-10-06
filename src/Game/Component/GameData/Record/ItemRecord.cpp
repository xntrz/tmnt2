#include "ItemRecord.hpp"


static const uint32 ALL_CRY_TAKEN_MASK = 0x3FFFFFFF;


static_assert(~ALL_CRY_TAKEN_MASK == 0xC0000000, "idb -> CItemRecord::IsValid_4858A0");


static const uint32 ALL_ITEM_TAKEN_MASK = (1 << static_cast<uint32>(ITEMID::ID_HEAL_SMALL))
                                        | (1 << static_cast<uint32>(ITEMID::ID_HEAL_FULL))
                                        | (1 << static_cast<uint32>(ITEMID::ID_MISSILE))
                                        | (1 << static_cast<uint32>(ITEMID::ID_CRY_RED))
                                        | (1 << static_cast<uint32>(ITEMID::ID_CRY_GREEN))
                                        | (1 << static_cast<uint32>(ITEMID::ID_CRY_WHITE))
                                        | (1 << static_cast<uint32>(ITEMID::ID_CRY_ORANGE))
                                        | (1 << static_cast<uint32>(ITEMID::ID_ANTIQUE))
                                        | (1 << static_cast<uint32>(ITEMID::ID_INVINCIBLE))
                                        | (1 << static_cast<uint32>(ITEMID::ID_COMEBACK))
                                        | (1 << static_cast<uint32>(ITEMID::ID_DON_LASER));


static_assert(~ALL_ITEM_TAKEN_MASK == 0xFFFC03F1, "idb -> CItemRecord::IsValid_4858A0");


static const AREAID::VALUE s_aCrystalRedExistsArea[] =
{
    AREAID::ID_AREA01, AREAID::ID_AREA02, AREAID::ID_AREA09, AREAID::ID_AREA10,
    AREAID::ID_AREA11, AREAID::ID_AREA12, AREAID::ID_AREA14, AREAID::ID_AREA15,
    AREAID::ID_AREA17, AREAID::ID_AREA18, AREAID::ID_AREA19, AREAID::ID_AREA20,
    AREAID::ID_AREA21, AREAID::ID_AREA23, AREAID::ID_AREA24, AREAID::ID_AREA25,
    AREAID::ID_AREA29, AREAID::ID_AREA30, AREAID::ID_AREA33, AREAID::ID_AREA36,
    AREAID::ID_AREA37, AREAID::ID_AREA38, AREAID::ID_AREA45, AREAID::ID_AREA26,
    AREAID::ID_AREA04, AREAID::ID_AREA05, AREAID::ID_AREA06, AREAID::ID_AREA22,
    AREAID::ID_AREA60_A, AREAID::ID_AREA60_C,
};


static const AREAID::VALUE s_aCrystalGreenExistsArea[] =
{
    AREAID::ID_AREA01, AREAID::ID_AREA02, AREAID::ID_AREA09, AREAID::ID_AREA10,
    AREAID::ID_AREA11, AREAID::ID_AREA12, AREAID::ID_AREA14, AREAID::ID_AREA17,
    AREAID::ID_AREA18, AREAID::ID_AREA19, AREAID::ID_AREA20, AREAID::ID_AREA21,
    AREAID::ID_AREA23, AREAID::ID_AREA24, AREAID::ID_AREA25, AREAID::ID_AREA28,
    AREAID::ID_AREA29, AREAID::ID_AREA30, AREAID::ID_AREA31, AREAID::ID_AREA38,
    AREAID::ID_AREA39, AREAID::ID_AREA50, AREAID::ID_AREA56, AREAID::ID_AREA26,
    AREAID::ID_AREA05, AREAID::ID_AREA44, AREAID::ID_AREA34, AREAID::ID_AREA35,
    AREAID::ID_AREA32, AREAID::ID_AREA60_A,
};


static const AREAID::VALUE s_aCrystalOrangeExistsArea[] =
{
    AREAID::ID_AREA01, AREAID::ID_AREA10, AREAID::ID_AREA11, AREAID::ID_AREA12,
    AREAID::ID_AREA15, AREAID::ID_AREA17, AREAID::ID_AREA18, AREAID::ID_AREA19,
    AREAID::ID_AREA20, AREAID::ID_AREA21, AREAID::ID_AREA24, AREAID::ID_AREA25,
    AREAID::ID_AREA28, AREAID::ID_AREA29, AREAID::ID_AREA30, AREAID::ID_AREA33,
    AREAID::ID_AREA39, AREAID::ID_AREA40, AREAID::ID_AREA45, AREAID::ID_AREA47,
    AREAID::ID_AREA48, AREAID::ID_AREA52, AREAID::ID_AREA55, AREAID::ID_AREA26,
    AREAID::ID_AREA34, AREAID::ID_AREA35, AREAID::ID_AREA04, AREAID::ID_AREA05,
    AREAID::ID_AREA41, AREAID::ID_AREA13,
};


static const AREAID::VALUE s_aCrystalWhiteExistsArea[] =
{
    AREAID::ID_AREA01, AREAID::ID_AREA09, AREAID::ID_AREA10, AREAID::ID_AREA11,
    AREAID::ID_AREA14, AREAID::ID_AREA15, AREAID::ID_AREA17, AREAID::ID_AREA20,
    AREAID::ID_AREA21, AREAID::ID_AREA23, AREAID::ID_AREA24, AREAID::ID_AREA28,
    AREAID::ID_AREA33, AREAID::ID_AREA36, AREAID::ID_AREA37, AREAID::ID_AREA39,
    AREAID::ID_AREA45, AREAID::ID_AREA47, AREAID::ID_AREA49, AREAID::ID_AREA52,
    AREAID::ID_AREA53, AREAID::ID_AREA54, AREAID::ID_AREA27, AREAID::ID_AREA04,
    AREAID::ID_AREA05, AREAID::ID_AREA41, AREAID::ID_AREA42, AREAID::ID_AREA43,
    AREAID::ID_AREA46, AREAID::ID_AREA60_B,
};


CItemRecord::CItemRecord(void)
{
    ;
};


CItemRecord::~CItemRecord(void)
{
    ;
};


void CItemRecord::SetDefault(void)
{
    m_uCrystalRedFlag = 0;
    m_uCrystalGreenFlag = 0;
    m_uCrystalOrangeFlag = 0;
    m_uCrystalWhiteFlag = 0;
    m_uItemTakenFlag = 0;
    m_iComebackNum = 0;
};


bool CItemRecord::IsValid(void) const
{
    if (m_iComebackNum < 0 ||
        m_iComebackNum > 1)
    {
        OUTPUT(" %s is failed: comeback num\n", __FUNCTION__);
        return false;
    };

    if (FLAG_TEST_ANY(m_uCrystalRedFlag, ~ALL_CRY_TAKEN_MASK))
    {
        OUTPUT(" %s is failed: red cry flag\n", __FUNCTION__);
        return false;
    };

    if (FLAG_TEST_ANY(m_uCrystalGreenFlag, ~ALL_CRY_TAKEN_MASK))
    {
        OUTPUT(" %s is failed: green cry flag\n", __FUNCTION__);
        return false;
    };

    if (FLAG_TEST_ANY(m_uCrystalOrangeFlag, ~ALL_CRY_TAKEN_MASK))
    {
        OUTPUT(" %s is failed: orange cry flag\n", __FUNCTION__);
        return false;
    };

    if (FLAG_TEST_ANY(m_uCrystalWhiteFlag, ~ALL_CRY_TAKEN_MASK))
    {
        OUTPUT(" %s is failed: white cry flag\n", __FUNCTION__);
        return false;
    };

    if (FLAG_TEST_ANY(m_uItemTakenFlag, ~ALL_ITEM_TAKEN_MASK))
    {
        OUTPUT(" %s is failed: item taken flag\n", __FUNCTION__);
        return false;
    };

    OUTPUT(" %s ...OK!\n", __FUNCTION__);
    return true;
};


void CItemRecord::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_uCrystalRedFlag = m_uCrystalRedFlag;
    rRawData.m_uCrystalGreenFlag = m_uCrystalGreenFlag;
    rRawData.m_uCrystalOrangeFlag = m_uCrystalOrangeFlag;
    rRawData.m_uCrystalWhiteFlag = m_uCrystalWhiteFlag;
    rRawData.m_uItemTakenFlag = m_uItemTakenFlag;
    rRawData.m_iComebackNum = m_iComebackNum;
};


void CItemRecord::Restore(const RAWDATA& rRawData)
{
    m_uCrystalRedFlag = rRawData.m_uCrystalRedFlag;
    m_uCrystalGreenFlag = rRawData.m_uCrystalGreenFlag;
    m_uCrystalOrangeFlag = rRawData.m_uCrystalOrangeFlag;
    m_uCrystalWhiteFlag = rRawData.m_uCrystalWhiteFlag;
    m_uItemTakenFlag = rRawData.m_uItemTakenFlag;
    m_iComebackNum = rRawData.m_iComebackNum;
};


void CItemRecord::SetItemTaken(ITEMID::VALUE idItem)
{
    ASSERT(idItem > ITEMID::ID_NONE && idItem <= ITEMID::ID_MAX);

    FLAG_SET(m_uItemTakenFlag, BIT(idItem));
};


void CItemRecord::SetAreaCrystalTaken(GAMETYPES::CRYSTALTYPE crytype, AREAID::VALUE idArea)
{
    int32 iCryNo = 0;
    
    if (getAreaCrystalInfo(crytype, idArea, &iCryNo))
        setCrystalFlag(crytype, iCryNo, true);
};


void CItemRecord::SetComebackTaken(void)
{
    m_iComebackNum = Clamp(++m_iComebackNum, 0, 1);
    ASSERT(m_iComebackNum >= 0 && m_iComebackNum <= 1);
};


void CItemRecord::SetComebackUsed(void)
{
    ASSERT(m_iComebackNum > 0);
    
    if (m_iComebackNum)
        --m_iComebackNum;    
};


bool CItemRecord::IsItemTakenYet(ITEMID::VALUE idItem) const
{
    ASSERT(idItem > ITEMID::ID_NONE && idItem <= ITEMID::ID_MAX);

    uint32 flag = (1 << static_cast<uint32>(idItem));
    
    return FLAG_TEST(m_uItemTakenFlag, flag);
};


int32 CItemRecord::GetCrystalNum(GAMETYPES::CRYSTALTYPE crytype) const
{
    int32 iResult = 0;
    uint32 uCryFlag = 0;
    
    switch (crytype)
    {
    case GAMETYPES::CRYSTALTYPE_RED:
        uCryFlag = m_uCrystalRedFlag;
        break;

    case GAMETYPES::CRYSTALTYPE_GREEN:
        uCryFlag = m_uCrystalGreenFlag;
        break;

    case GAMETYPES::CRYSTALTYPE_ORANGE:
        uCryFlag = m_uCrystalOrangeFlag;
        break;

    case GAMETYPES::CRYSTALTYPE_WHITE:
        uCryFlag = m_uCrystalWhiteFlag;
        break;

    default:
        ASSERT(false);
        break;
    };

    ASSERT(getCrystalMax(crytype) <= BITSOF(uCryFlag));
    
    for (int32 i = 0; i < getCrystalMax(crytype); i++)
    {
        uint32 flag = (1 << static_cast<uint32>(i));
        if (FLAG_TEST(uCryFlag, flag))
            ++iResult;
    };

    return iResult;
};


GAMETYPES::CRYSTALTYPE CItemRecord::FindAreaCrystal(AREAID::VALUE idArea) const
{
    static const GAMETYPES::CRYSTALTYPE s_aCryType[GAMETYPES::CRYSTALTYPE_NUM] =
    {
        GAMETYPES::CRYSTALTYPE_RED,
        GAMETYPES::CRYSTALTYPE_GREEN,
        GAMETYPES::CRYSTALTYPE_WHITE,
        GAMETYPES::CRYSTALTYPE_ORANGE,
    };

    for (int32 i = 0; i < COUNT_OF(s_aCryType); ++i)
    {
        int32 iCryNo = 0;
        if (getAreaCrystalInfo(s_aCryType[i], idArea, &iCryNo))
            return s_aCryType[i];
    };

    return GAMETYPES::CRYSTALTYPE_NONE;
};


bool CItemRecord::IsAreaCrystalTaken(GAMETYPES::CRYSTALTYPE crytype, AREAID::VALUE idArea) const
{
    int32 iCryNo = 0;
    
    if (getAreaCrystalInfo(crytype, idArea, &iCryNo))
        return getCrystalFlag(crytype, iCryNo);
    else
        return false;    
};


bool CItemRecord::IsComebackProcessed(void) const
{
    return (m_iComebackNum > 0);
};


void CItemRecord::DebugSetCryTaken(GAMETYPES::CRYSTALTYPE crytype, int32 iCryNo, bool bTaken)
{
    //
    //  Used only for debug tests
    //
    
#ifdef _DEBUG
    setCrystalFlag(crytype, iCryNo, bTaken);
#endif    
};


int32 CItemRecord::getCrystalMax(GAMETYPES::CRYSTALTYPE crytype) const
{
    switch (crytype)
    {
    case GAMETYPES::CRYSTALTYPE_RED:
        return 30;

    case GAMETYPES::CRYSTALTYPE_GREEN:
        return 30;

    case GAMETYPES::CRYSTALTYPE_ORANGE:
        return 30;

    case GAMETYPES::CRYSTALTYPE_WHITE:
        return 30;

    default:
        return 0;
    };
};


bool CItemRecord::getAreaCrystalInfo(GAMETYPES::CRYSTALTYPE crytype, AREAID::VALUE idArea, int32* piCryNo) const
{
    const AREAID::VALUE* paCrystalAreaExists = nullptr;
    
    switch (crytype)
    {
    case GAMETYPES::CRYSTALTYPE_RED:
        paCrystalAreaExists = s_aCrystalRedExistsArea;
        break;

    case GAMETYPES::CRYSTALTYPE_GREEN:
        paCrystalAreaExists = s_aCrystalGreenExistsArea;
        break;

    case GAMETYPES::CRYSTALTYPE_ORANGE:
        paCrystalAreaExists = s_aCrystalOrangeExistsArea;
        break;

    case GAMETYPES::CRYSTALTYPE_WHITE:
        paCrystalAreaExists = s_aCrystalWhiteExistsArea;
        break;

    default:
        ASSERT(false);
        break;
    };

    if (paCrystalAreaExists)
    {
        for (int32 i = 0; i < getCrystalMax(crytype); ++i)
        {
            if (paCrystalAreaExists[i] == idArea)
            {
                ASSERT(piCryNo);
                *piCryNo = i;
                
                return true;
            };
        };
    };

    return false;
};


bool CItemRecord::getCrystalFlag(GAMETYPES::CRYSTALTYPE crytype, int32 no) const
{
    bool bResult = false;
    
    if (no >= 0 && no < getCrystalMax(crytype))
    {
        uint32 flag = (1 << static_cast<uint32>(no));

        switch (crytype)
        {
        case GAMETYPES::CRYSTALTYPE_RED:
            bResult = FLAG_TEST(m_uCrystalRedFlag, flag);
            break;

        case GAMETYPES::CRYSTALTYPE_GREEN:
            bResult = FLAG_TEST(m_uCrystalGreenFlag, flag);
            break;

        case GAMETYPES::CRYSTALTYPE_ORANGE:
            bResult = FLAG_TEST(m_uCrystalOrangeFlag, flag);
            break;

        case GAMETYPES::CRYSTALTYPE_WHITE:
            bResult = FLAG_TEST(m_uCrystalWhiteFlag, flag);
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    return bResult;
};


void CItemRecord::setCrystalFlag(GAMETYPES::CRYSTALTYPE crytype, int32 no, bool bProcessed)
{    
    if (no >= 0 && no < getCrystalMax(crytype))
    {
        uint32* puFlag = nullptr;
        
        switch (crytype)
        {
        case GAMETYPES::CRYSTALTYPE_RED:
            puFlag = &m_uCrystalRedFlag;
            break;

        case GAMETYPES::CRYSTALTYPE_GREEN:
            puFlag = &m_uCrystalGreenFlag;
            break;

        case GAMETYPES::CRYSTALTYPE_ORANGE:
            puFlag = &m_uCrystalOrangeFlag;
            break;

        case GAMETYPES::CRYSTALTYPE_WHITE:
            puFlag = &m_uCrystalWhiteFlag;
            break;

        default:
            ASSERT(false);
            break;
        };

        if (puFlag)
        {
            uint32 flag = (1 << static_cast<uint32>(no));

            if (bProcessed)
                FLAG_SET(*puFlag, flag);
            else
                FLAG_CLEAR(*puFlag, flag);
        };
    };
};



