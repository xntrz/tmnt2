#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameMain/AreaID.hpp"
#include "Game/Component/GameMain/ItemID.hpp"


class CItemRecord
{
public:
    struct RAWDATA
    {
        uint32 m_uCrystalRedFlag;
        uint32 m_uCrystalGreenFlag;
        uint32 m_uCrystalOrangeFlag;
        uint32 m_uCrystalWhiteFlag;
        uint32 m_uItemTakenFlag;
        int32 m_iComebackNum;
    };
    
public:
    CItemRecord(void);
    ~CItemRecord(void);
    void SetDefault(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetItemTaken(ITEMID::VALUE idItem);
    void SetAreaCrystalTaken(GAMETYPES::CRYSTALTYPE crytype, AREAID::VALUE idArea);
    void SetComebackTaken(void);
    void SetComebackUsed(void);
    bool IsItemTakenYet(ITEMID::VALUE idItem) const;
    int32 GetCrystalNum(GAMETYPES::CRYSTALTYPE crytype) const;
    GAMETYPES::CRYSTALTYPE FindAreaCrystal(AREAID::VALUE idArea) const;
    bool IsAreaCrystalTaken(GAMETYPES::CRYSTALTYPE crytype, AREAID::VALUE idArea) const;
    bool IsComebackProcessed(void) const;
    void DebugSetCryTaken(GAMETYPES::CRYSTALTYPE crytype, int32 iCryNo, bool bTaken);
    
private:
    int32 getCrystalMax(GAMETYPES::CRYSTALTYPE crytype) const;
    bool getAreaCrystalInfo(GAMETYPES::CRYSTALTYPE crytype, AREAID::VALUE idArea, int32* piCryNo) const;
    bool getCrystalFlag(GAMETYPES::CRYSTALTYPE crytype, int32 no) const;
    void setCrystalFlag(GAMETYPES::CRYSTALTYPE crytype, int32 no, bool bProcessed);

private:
    uint32 m_uCrystalRedFlag;
    uint32 m_uCrystalGreenFlag;
    uint32 m_uCrystalOrangeFlag;
    uint32 m_uCrystalWhiteFlag;
    uint32 m_uItemTakenFlag;
    int32 m_iComebackNum;
};