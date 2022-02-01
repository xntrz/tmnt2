#pragma once

#include "Record/AntiqueRecord.hpp"
#include "Record/AreaRecord.hpp"
#include "Record/DatabaseRecord.hpp"
#include "Record/ItemRecord.hpp"
#include "Record/NexusRecord.hpp"
#include "Record/SecretRecord.hpp"


class CGameRecord
{
public:
    struct RAWDATA
    {
        uint32 m_uTimestampInit;
        CAntiqueRecord::RAWDATA m_antique;
        CAreaRecord::RAWDATA m_area;
        CDatabaseRecord::RAWDATA m_database;
        CItemRecord::RAWDATA m_item;
        CNexusRecord::RAWDATA m_nexus;
        CSecretRecord::RAWDATA m_secret;
    };
    
public:
    CGameRecord(void);
    ~CGameRecord(void);
    void Initialize(void);
    void Terminate(void);
    void SetDefault(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData, bool bNotSavedFlag);
    uint32 GetInitDate(void) const;
    uint32 GetAchievedPoint(void);

    CAntiqueRecord& Antique(void);
    CAreaRecord& Area(void);
    CDatabaseRecord& Database(void);
    CItemRecord& Item(void);
    CNexusRecord& Nexus(void);
    CSecretRecord& Secret(void);

private:
    uint32 m_uTimestampInit;
    CAntiqueRecord m_antique;
    CAreaRecord m_area;
    CDatabaseRecord m_database;
    CItemRecord m_item;
    CNexusRecord m_nexus;
    CSecretRecord m_secret;
};