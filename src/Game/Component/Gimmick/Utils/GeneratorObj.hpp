#pragma once

#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/GimmickData.hpp"

#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/Component/Enemy/EnemyTypes.hpp"


class CEnemy;


class CGenEnemyManipTable
{
public:
    struct MANIP
    {
        enum STEP
        {
            STEP_NONE = 0,
            STEP_ENEMY_HIDE,
            STEP_ENEMY_APPEAR,
            STEP_ENEMY_IDLE,
            STEP_ENEMY_DEAD,
        };

        MANIP(void);
        bool Generate(ENEMYID::VALUE id, int32 nPattern, const RwV3d& rvPos, float fRotY, float fRange, ENEMYTYPES::STATUS status);
        void Show(void);
        void Activate(void);
        void Deactivate(void);
        void Kill(void);
        bool IsOutOfSight(void);
        bool IsAlive(void);
        CEnemy* GetEnemy(void);
        bool IsActive(void) const;

    private:
        STEP m_step;
        int32 m_nActiveNum;
        uint32 m_hEnemy;
    };
    
public:
    CGenEnemyManipTable(void);
    ~CGenEnemyManipTable(void);
    void Setup(int32 nTableSize);
    void Cleanup(void);
    void KillOutOfSightEnemy(void);
    int32 GetSize(void) const;
    MANIP* GetManipulator(int32 index) const;

private:
    int32 m_nTableSize;
    MANIP* m_pTableTop;
};


class CEnemySetData
{
public:
    struct ENEMYDATA
    {
        ENEMYID::VALUE m_id;
        int32 m_nPattern;
    };
    
public:
    static int32 ConvertEnemyPatternFromMGD(int32 nInternalPattern);
    
    CEnemySetData(void);
    void Init(const CGimmickData::ENEMYSET_DATA& rSetData);
    const ENEMYDATA& GetEnemyData(int32 nndex);

private:
    int32 m_nArraySize;
    ENEMYDATA m_aEnemyData[CGimmickData::ENEMY_PATTERN_NUM];
};


class CEnemyGeneratorBase
{
public:    
    enum STATE
    {
        STATE_NONE = 0,
        STATE_IDLE,
        STATE_GENERATE_ENEMY,
        STATE_END,
        STATE_HIDE_ENEMY,
        STATE_APPEAR_ENEMY,
    };
    
protected:
    struct GENERATEPOINT
    {
        RwV3d m_vPos;
        float m_fRotY;

        void Init(GIMMICKPARAM::GIMMICK_BASIC* pParam);
        void Clear(void);

        inline GENERATEPOINT(void) { Clear(); };
        inline GENERATEPOINT(const RwV3d& rvPos, float fRotY) : m_vPos(rvPos), m_fRotY(fRotY) {};
    };

    struct INITPARAM
    {
        bool m_bAutoActivate;
        int32 m_nTotalEnemy;
        int32 m_nAliveEnemyMax;
        int32 m_nEnemyPerAppear;
        float m_fAppearInterval;
        float m_fActivateDistance;
        float m_fEnemyRange;
        CEnemyGeneratorBase::GENERATEPOINT m_origin;
    };

public:
    CEnemyGeneratorBase(void);
    virtual ~CEnemyGeneratorBase(void);
    virtual void PreRunState(float dt);
    virtual void RunState(float dt);
    virtual bool IsActive(void) const;
    virtual bool OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo) = 0;
    STATE Run(float dt);
    void Start(void);
    void Stop(void);
    void Finish(void);
    int32 CountAliveEnemy(void) const;
    int32 CountDeadEnemy(void) const;
    bool HandleQuery(CGimmickQuery* pQuery) const;
    bool HandleEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype);
    void Setup(const INITPARAM& rParam);
    void Cleanup(void);
    void ChangeState(STATE eState);
    void CalcWorldPoint(GENERATEPOINT& rWorldPoint, const GENERATEPOINT rOffsetPoint) const;

    inline STATE GetState(void) const { return m_state; };
    inline int32 GetTotalEnemyNum(void) const { return m_nTotalEnemy; };
    
private:
    int32 generate(int32 nGenerateNum);
    int32 calcGenerateNum(int32 nRequestNum) const;
    void suspend(void);
    void resume(void);

protected:
    STATE m_state;
    int32 m_nSuspendCount;
    int32 m_nAppearedEnemy;
    float m_fRemainTime;
    int32 m_nTotalEnemy;
    int32 m_nAliveEnemyMax;
    int32 m_nEnemyPerAppear;
    float m_fAppearInterval;
    float m_fActivateDistance;
    float m_fEnemyRange;
    bool m_bAutoActivate;
    GENERATEPOINT m_origin;
    CGenEnemyManipTable m_enemyManipTable;
};


class CSettingEnemyGenerator final : public CEnemyGeneratorBase
{
private:
    struct INFO
    {
        ENEMYID::VALUE m_id;
        int32 m_nPattern;
        GENERATEPOINT m_point;
    };

public:
    CSettingEnemyGenerator(void);
    virtual ~CSettingEnemyGenerator(void);
    virtual bool OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo) override;
    void Init(const RwV3d& rvPos, float fRotY, int32 nEnemyNum, float fEnemyRange, bool bAutoActivate);
    void Init(GIMMICKPARAM::GIMMICK_ENEMY_PLACE* pParam, int32 nEnemyNum, bool bAutoActivate);
    void SetGenerateInfo(int32 nIndex, const RwV3d& rvPos, float fRotY, ENEMYID::VALUE idEnemy, int32 nPattern);
    void ActivateEnemy(void);

private:
    void initTable(ENEMYID::VALUE idEnemy, int32 nPattern, int32 nEnemyNum);

private:
    int32 m_nGenerateNum;
    INFO* m_pGenerateInfoTable;
};


class CSingleEnemyGenerator final : public CEnemyGeneratorBase
{
public:
    CSingleEnemyGenerator(void);
    virtual ~CSingleEnemyGenerator(void);
    virtual bool OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo) override;
    void Init(GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_SINGLE* pParam, bool bAutoActivate);
    
private:
    GENERATEPOINT m_genpoint;
    ENEMYID::VALUE m_id;
    int32 m_nPattern;
};


class CMultipleEnemyGenerator : public CEnemyGeneratorBase
{
public:
    CMultipleEnemyGenerator(void);
    virtual ~CMultipleEnemyGenerator(void);
    virtual bool OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo) override;
    void Init(GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE* pParam, bool bAutoActivate);

    inline void SetGeneratePoint(const RwV3d* pvPos, float fRotY) { m_genpoint.m_vPos = *pvPos;  m_genpoint.m_fRotY = fRotY; };
    
protected:
    GENERATEPOINT m_genpoint;
    CEnemySetData m_enemySetData;
};


class CDoorEnemyGenerator final : public CMultipleEnemyGenerator
{
public:
    CDoorEnemyGenerator(void);
    virtual ~CDoorEnemyGenerator(void);
    virtual void PreRunState(float dt) override;
    virtual bool OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo) override;
    void ShowEnemy(void);
};