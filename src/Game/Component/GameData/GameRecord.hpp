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
        uint32                   m_uTimestampInit;
        CItemRecord::RAWDATA     m_item;
        CAntiqueRecord::RAWDATA  m_antique;
        CDatabaseRecord::RAWDATA m_database;
        CNexusRecord::RAWDATA    m_nexus;
        CAreaRecord::RAWDATA     m_area;
        CSecretRecord::RAWDATA   m_secret;
    };

    static_assert(std::is_pod<RAWDATA>::value, "game RAWDATA should be POD type");
    
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
    int32 GetAchievedPoint(void) const;

    inline CItemRecord& Item(void)          { return m_item; };
    inline CAntiqueRecord& Antique(void)    { return m_antique; };
    inline CDatabaseRecord& Database(void)  { return m_database; };
    inline CNexusRecord& Nexus(void)        { return m_nexus; };
    inline CAreaRecord& Area(void)          { return m_area; };
    inline CSecretRecord& Secret(void)      { return m_secret; };

    inline const CItemRecord& Item(void) const          { return m_item; };
    inline const CAntiqueRecord& Antique(void) const    { return m_antique; };
    inline const CDatabaseRecord& Database(void) const  { return m_database; };
    inline const CNexusRecord& Nexus(void) const        { return m_nexus; };
    inline const CAreaRecord& Area(void) const          { return m_area; };
    inline const CSecretRecord& Secret(void) const      { return m_secret; };

private:
    uint32          m_uTimestampInit;
    CItemRecord     m_item;
    CAntiqueRecord  m_antique;
    CDatabaseRecord m_database;
    CNexusRecord    m_nexus;
    CAreaRecord     m_area;
    CSecretRecord   m_secret;
};