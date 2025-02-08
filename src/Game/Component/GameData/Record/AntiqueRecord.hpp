#pragma once

#include "Game/Component/GameMain/AntiqueID.hpp"
#include "Game/Component/GameMain/AreaID.hpp"


class CTimeObj;


class CAntiqueRecord
{
public:
    enum SHOPSTATE
    {
        SHOPSTATE_NONE = 0,
        SHOPSTATE_OPEN,
        SHOPSTATE_NORMAL,
        SHOPSTATE_COMPLETED,
    };

    enum STATE : uint8
    {
        STATE_NONE = 0,
        STATE_UNIDENTIFIED,
        STATE_IDENTIFIED,
    };

    struct NODEANTIQUE
    {
        STATE  m_state;
        uint32 m_takenDate;
    };

    struct RAWDATA
    {
        SHOPSTATE m_shopstate;
        STATE     m_abAntiqueState[ANTIQUEID::ID_MAX];
        uint32    m_auAntiqueTakenDate[ANTIQUEID::ID_MAX];
    };

public:
    CAntiqueRecord(void);
    ~CAntiqueRecord(void);
    void SetDefault(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetShopState(SHOPSTATE shopstate);
    SHOPSTATE GetShopState(void) const;
    bool IsNewAntiqueExisted(void) const;
    int32 CountUnidentifiedAntique(void) const;
    bool CheckJustCompleted(void);
    void SetAntiqueTaken(ANTIQUEID::VALUE idAntique);
    void SetAntiqueIdentified(ANTIQUEID::VALUE idAntique);
    bool IsAntiqueTaken(ANTIQUEID::VALUE idAntique) const;
    bool IsAntiqueIdentified(ANTIQUEID::VALUE idAntique) const;
    void GetAntiqueTakenDate(ANTIQUEID::VALUE idAntique, CTimeObj& date);
    bool IsAreaAntiqueTaken(AREAID::VALUE idArea) const;

private:
    NODEANTIQUE& getAntiqueNode(ANTIQUEID::VALUE idAntique);
    const NODEANTIQUE& getAntiqueNode(ANTIQUEID::VALUE idAntique) const;

private:
    SHOPSTATE m_shopstate;
    NODEANTIQUE m_aNodeAntique[ANTIQUEID::ID_MAX];
};