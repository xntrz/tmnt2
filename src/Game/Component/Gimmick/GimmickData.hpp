#pragma once

#include "GimmickParam.hpp"


class CGimmickData
{
public:
    static const int32 ENEMY_PATTERN_NUM = 8;

    struct GIMMICKDATA : public GIMMICKPARAM::GIMMICK_BASIC
    {
        
    };


    struct MGD_HEADER
    {
        char m_szIdentity[4];
        char m_szName[16];
        int32 m_nOffsetDataPos;
        int32 m_nOffsetEnemySetDataPos;
    };

    CHECK_SIZE(MGD_HEADER, 0x1C);

    //
    //  map
    //
    struct MAPGIMMICK_DATA
    {
        int32 m_nSize;
        char* m_pData;
    };

    struct MAPGIMMICK_DATA_LIST
    {
        int32 m_nGimmickDataNum;
        MAPGIMMICK_DATA m_aGimmickData[];
    };

    //
    //  enemy
    //
    struct ENEMY_KIND
    {
        int16 m_id;
        int16 m_pattern;
    };

    struct ENEMYSET_DATA
    {
        int32 m_nEnemyKindNum;
        ENEMY_KIND m_aEnemyKind[ENEMY_PATTERN_NUM];
    };

    struct ENEMYSET_DATA_LIST
    {
        int32 m_nEnemySetDataNum;
        ENEMYSET_DATA m_aEnemySadData[];
    };

    CHECK_SIZE(ENEMY_KIND, 0x4);
    CHECK_SIZE(ENEMYSET_DATA, 0x24);

public:
    CGimmickData(void);
    ~CGimmickData(void);
    void Read(const void* pBuffer, uint32 uBufferSize);
    void PostRead(void);
    void Cleanup(void);
    void CreateGimmicks(void);
    MGD_HEADER* GetMGDHeader(void) const;
    MAPGIMMICK_DATA_LIST* GetGimmickDataList(void) const;
    GIMMICKDATA* GetGimmickData(int32 no) const;
    int32 GetGimmickDataNum(void) const;
    ENEMYSET_DATA_LIST* GetEnemySetDataList(void) const;
    ENEMYSET_DATA* GetEnemySetData(int32 no) const;

private:
    char* m_pMGDBuffer;
};


class CGimmickDataManager
{
public:
    enum MGD_DATA_KIND
    {
        KIND_NORMAL = 0,
        KIND_EFFECT,
        
        KINDNUM,
    };

public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const char* pszName, const void* pBuffer, uint32 uBufferSize);
    static void CreateGimmicks(MGD_DATA_KIND kind);
    static const CGimmickData::ENEMYSET_DATA* GetEnemySetData(int32 no, MGD_DATA_KIND kind);
};