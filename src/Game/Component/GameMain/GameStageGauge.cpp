#include "GameStageGauge.hpp"

#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/EventCheckGimmick.hpp"
#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEnemy.hpp"
#include "Game/Component/GameMain/GameGimmick.hpp"
#include "Game/Component/Gauge/GaugeInformation.hpp"
#include "Game/Component/Gauge/GaugeStatus.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


void CBossExGauge::Start(void)
{
    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();

    for (int32 i = 0; i < COUNT_OF(m_aBossId); ++i)
    {
        m_aBossId[i] = CStageInfo::GetGaugeEnemyID(stageId, i);
        if (m_aBossId[i])
            ++m_nBossNum;

        m_ahBoss[i] = 0;
    };
};


void CBossExGauge::Stop(void)
{
    for (int32 i = 0; i < m_nBossNum; ++i)
        CGaugeInformation::BossGaugeEnable(false, i, ENEMYID::ID_DUMMY);
};


void CBossExGauge::Update(void)
{
    for (int32 i = 0; i < m_nBossNum; ++i)
    {
        if (!m_aBossId[i])
            continue;
        
        if (m_ahBoss[i])
        {
            if (CGameObjectManager::GetObject(m_ahBoss[i]))
                continue;        // already set and alive so just continue
            else
                m_ahBoss[i] = 0; // already set and dead try to find new boss with this id
        };

        m_ahBoss[i] = findBoss(i);
        if (!m_ahBoss[i])
            continue;

        CGameObject* pObj = CGameObjectManager::GetObject(m_ahBoss[i]);
        if (pObj)
        {
            ASSERT(pObj->GetType() == GAMEOBJECTTYPE::ENEMY);
            CEnemy* pEnemy = static_cast<CEnemy*>(pObj);

			CGaugeInformation::BossGaugeEnable(true, i, m_aBossId[i]);

            int32 hpMax = pEnemy->GetHPMax();
            CGaugeInformation::BossGaugeSet(i, hpMax);
        };
    };
};


void CBossExGauge::HandleEnemyDamaged(CEnemy* pEnemy, int32 nRemainHP)
{
    ASSERT(pEnemy);
    
    ENEMYID::VALUE enemyId = pEnemy->Character().GetID();
    for (int32 i = 0; i < BOSS_MAX; ++i)
    {
        if (isBoss(enemyId, i))
            CGaugeInformation::BossGaugeSet(i, nRemainHP);
    };
};


bool CBossExGauge::isBoss(ENEMYID::VALUE enemyId, int32 no) const
{
    ENEMYID::VALUE bossId = getStageBoss(no);
    if (!bossId)
        return false;

    /* ninja rats case - boss is first rat "A" */
    if ((bossId >= ENEMYID::ID_NINJA_RATS_A) &&
        (bossId <= ENEMYID::ID_NINJA_RATS_D))
    {
        return (bossId == ENEMYID::ID_NINJA_RATS_A);
    };    

    return (bossId == enemyId);
};


ENEMYID::VALUE CBossExGauge::getStageBoss(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < COUNT_OF(m_aBossId));

    return m_aBossId[no];
};


uint32 CBossExGauge::findBoss(int32 no) const
{
    ENEMYID::VALUE enemyId = getStageBoss(no);
    if (enemyId)
    {
        int32 enemyCount = CGameProperty::GetEnemyMax();
        for (int32 i = 0; i < enemyCount; ++i)
        {
            CEnemy* pEnemy = CGameProperty::GetEnemy(i);
            if (pEnemy && (pEnemy->Character().GetID() == enemyId))
                return pEnemy->GetHandle();
        };
    };

    return 0;
};


//
// *********************************************************************************
//


void CTimerExGauge::Start(void)
{
    m_hObj        = 0;
    m_fTimeNow    = 0.0f;
    m_fTimeTarget = 0.0f;

    CGaugeStatus::TimerDispEnable(true);

    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();
    switch (stageId)
    {
    case STAGEID::ID_ST11J:
    case STAGEID::ID_ST15N:
    case STAGEID::ID_ST20F:
    case STAGEID::ID_ST39N:
    case STAGEID::ID_ST40OB:
    case STAGEID::ID_ST45N:
        {
            CGimmick* pGimmick = CGimmickManager::Find("GMETIME_A000");
            if (pGimmick)
            {
                ASSERT(pGimmick->GetID() == GIMMICKID::ID_E_TIME);
                CTimeCheckGimmick* pTimeCheckGimmick = static_cast<CTimeCheckGimmick*>(pGimmick);

                m_hObj        = pGimmick->GetHandle();
                m_fTimeTarget = pTimeCheckGimmick->GetTargetTime();
                m_fTimeNow    = pTimeCheckGimmick->GetRemainTime();

                CGaugeStatus::TimerDispEnable(true);
            };
        }
        break;

    default:
        break;
    };
};


void CTimerExGauge::Stop(void)
{
    CGaugeStatus::TimerDispEnable(false);
};


void CTimerExGauge::Update(void)
{
    CGameObject* pObject = CGameObjectManager::GetObject(m_hObj);
    if (pObject)
    {
        ASSERT(pObject->GetType() == GAMEOBJECTTYPE::GIMMICK);
        CGimmick* pGimmick = static_cast<CGimmick*>(pObject);

        ASSERT(pGimmick->GetID() == GIMMICKID::ID_E_TIME);
        CTimeCheckGimmick* pTimeCheckGimmick = static_cast<CTimeCheckGimmick*>(pGimmick);

        m_fTimeNow = pTimeCheckGimmick->GetRemainTime();

        CGaugeStatus::TimerDispSet(0, m_fTimeNow);
    };
};


//
// *********************************************************************************
//


void CCounterExGauge::Start(void)
{
    CGaugeStatus::CounterDispEnable(true);
};


void CCounterExGauge::Stop(void)
{
    CGaugeStatus::CounterDispEnable(false);
};


void CCounterExGauge::Update(void)
{
    int32 utromCount = 0;
    
    int32 gimmickCount = CGameProperty::GetGimmickMax();
    for (int32 i = 0; i < gimmickCount; ++i)
    {
        CGimmick* pGimmick = CGameProperty::GetGimmick(i);
        if (pGimmick && (pGimmick->GetID() == GIMMICKID::ID_N_UTROM))
            ++utromCount;
    };

    CGaugeStatus::CounterDispSet(1, utromCount);
};


//
// *********************************************************************************
//


void CLifeExGauge::Start(void)
{
    m_hObj = 0;

    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_FUGITOID_LIVE, true);
};


void CLifeExGauge::Stop(void)
{
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_FUGITOID_LIVE, false);
};


void CLifeExGauge::Update(void)
{
    if (m_hObj)
    {
        CGameObject* pObject = CGameObjectManager::GetObject(m_hObj);
        if (pObject)
        {
            ASSERT(pObject->GetType() == GAMEOBJECTTYPE::ENEMY);
            CEnemy* pEnemy = static_cast<CEnemy*>(pObject);

            int32 hpNow = pEnemy->GetHP();
            CGaugeStatus::ExGaugeDispSet(1, CGaugeStatus::EX_GAUGE_ZAKO_FUGITOID_LIVE, hpNow);
        };
    }
    else
    {
        m_hObj = findFugitoid();
        if (m_hObj)
        {
            CGameObject* pObject = CGameObjectManager::GetObject(m_hObj);
            if (pObject)
            {
                ASSERT(pObject->GetType() == GAMEOBJECTTYPE::ENEMY);
                CEnemy* pEnemy = static_cast<CEnemy*>(pObject);

                int32 hpMax = pEnemy->GetHPMax();
                CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_FUGITOID_LIVE, hpMax);
            };            
        };
    };
};


uint32 CLifeExGauge::findFugitoid(void)
{
    int32 enemyCount = CGameProperty::GetEnemyMax();
    for (int32 i = 0; i < enemyCount; ++i)
    {
        CEnemy* pEnemy = CGameProperty::GetEnemy(i);
        if (pEnemy && (pEnemy->Character().GetID() == ENEMYID::ID_FUGITOID))
            return pEnemy->GetHandle();
    };

    return 0;
};


//
// *********************************************************************************
//


void CThreeRaceExGauge::Start(void)
{
    m_nNumFoot  = 15;
    m_nNumPDG   = 15;
    m_nNumMafia = 15;

    CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT,   m_nNumFoot);
    CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, m_nNumPDG);
    CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA,  m_nNumMafia);

    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT,   true);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, true);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA,  true);
};


void CThreeRaceExGauge::Stop(void)
{
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT,   false);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, false);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA,  false);
};


void CThreeRaceExGauge::Update(void)
{
    CGaugeStatus::ExGaugeDispSet(1, CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT,   m_nNumFoot);
    CGaugeStatus::ExGaugeDispSet(2, CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, m_nNumPDG);
    CGaugeStatus::ExGaugeDispSet(3, CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA,  m_nNumMafia);
};


void CThreeRaceExGauge::HandleEnemyDestroyed(CEnemy* pEnemy)
{
    ASSERT(pEnemy);

    switch (pEnemy->Character().GetID())
    {
    case ENEMYID::ID_FOOT_TECH_NINJA:
        {
            if (m_nNumFoot > 0)
                --m_nNumFoot;
        }
        break;

    case ENEMYID::ID_PURPLE_DRAGON_GANG:
        {
            if (m_nNumPDG > 0)
                --m_nNumPDG;
        }
        break;

    case ENEMYID::ID_MOBSTER:
    case ENEMYID::ID_LARGE_MOBSTER:
        {
            if (m_nNumMafia > 0)
                --m_nNumMafia;
        }
        break;

    default:
        break;
    };
};