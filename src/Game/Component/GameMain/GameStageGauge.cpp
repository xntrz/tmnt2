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
    STAGEID::VALUE IdStage = CGameData::PlayParam().GetStage();
    
    m_IdBoss0 = CStageInfo::GetGaugeEnemyID(IdStage, 0);
    m_IdBoss1 = CStageInfo::GetGaugeEnemyID(IdStage, 1);
    
    if (m_IdBoss0)
        ++m_nBossNum;
    
    if (m_IdBoss1)
        ++m_nBossNum;

    m_hBoss0 = 0;
    m_hBoss1 = 0;

    for (int32 i = 0; i < m_nBossNum; ++i)
    {
        ENEMYID::VALUE IdEnemy = getStageBoss(i);
        if (IdEnemy)
            CGaugeInformation::BossGaugeEnable(true, i, IdEnemy);
    };
};


void CBossExGauge::Stop(void)
{
    for (int32 i = 0; i < m_nBossNum; ++i)
        CGaugeInformation::BossGaugeEnable(false, i, ENEMYID::ID_DUMMY);
};


void CBossExGauge::Update(void)
{
    if (!m_hBoss0)
    {
        m_hBoss0 = findBoss(0);
        if (m_hBoss0)
        {
            CGameObject* pObject = CGameObjectManager::GetObject(m_hBoss0);
            if (pObject)
            {
                ASSERT(pObject->GetType() == GAMEOBJECTTYPE::ENEMY);
                CEnemy* pEnemy = (CEnemy*)pObject;
                
                CGaugeInformation::BossGaugeSet(0, pEnemy->GetHPMax());
            };
        };
    };

    if (m_IdBoss1 && (!m_hBoss1))
    {
        m_hBoss1 = findBoss(1);
        if (m_hBoss1)
        {
            CGameObject* pObject = CGameObjectManager::GetObject(m_hBoss1);
            if (pObject)
            {
                ASSERT(pObject->GetType() == GAMEOBJECTTYPE::ENEMY);
                CEnemy* pEnemy = (CEnemy*)pObject;

                CGaugeInformation::BossGaugeSet(1, pEnemy->GetHPMax());
            };
        };
    };
};


void CBossExGauge::HandleEnemyDamaged(CEnemy* pEnemy, int32 nRemainHP)
{
    ASSERT(pEnemy);
    
    ENEMYID::VALUE IdEnemy = pEnemy->EnemyCharacter().GetID();
    for (int32 i = 0; i < 2; ++i)
    {
        if (isBoss(IdEnemy, i))
            CGaugeInformation::BossGaugeSet(i, nRemainHP);
    };
};


bool CBossExGauge::isBoss(ENEMYID::VALUE IdEnemy, int32 No)
{
   
    return false;
};


ENEMYID::VALUE CBossExGauge::getStageBoss(int32 No) const
{
    return ENEMYID::ID_BLOOD_SUCKER;
};


uint32 CBossExGauge::findBoss(int32 No) const
{
    ENEMYID::VALUE IdEnemy = getStageBoss(No);
    if (IdEnemy)
    {
        int32 EnemyCnt = CGameProperty::GetEnemyMax();
        for (int32 i = 0; i < EnemyCnt; ++i)
        {
			if (CGameProperty::Enemy(i).IsAlive())
			{
				if (CGameProperty::Enemy(i).Enemy().EnemyCharacter().GetID() == IdEnemy)
					return CGameProperty::Enemy(i).Enemy().GetHandle();
			};
        };
    };

    return 0;
};


void CTimerExGauge::Start(void)
{
    m_hObj = 0;
    m_fTimeNow = 0.0f;
    m_fTimeTarget = 0.0f;
    CGaugeStatus::TimerDispEnable(true);
    
    STAGEID::VALUE IdStage = CGameData::PlayParam().GetStage();
    switch (IdStage)
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
                
                m_hObj = pGimmick->GetHandle();
                m_fTimeTarget = ((CTimeCheckGimmick*)pGimmick)->GetTargetTime();
                m_fTimeNow = ((CTimeCheckGimmick*)pGimmick)->GetRemainTime();

                CGaugeStatus::TimerDispEnable(true);
            };
        }
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
        CGimmick* pGimmick = (CGimmick*)pObject;

        ASSERT(pGimmick->GetID() == GIMMICKID::ID_E_TIME);
        m_fTimeNow = ((CTimeCheckGimmick*)pGimmick)->GetRemainTime();

        CGaugeStatus::TimerDispSet(0, m_fTimeNow);
    };
};


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
    int32 UtromCnt = 0;
    
    int32 GimmickCnt = CGameProperty::GetGimmickMax();
    for (int32 i = 0; i < GimmickCnt; ++i)
    {
		if (CGameProperty::Gimmick(i).IsAlive())
		{
			if (CGameProperty::Gimmick(i).GetGimmick()->GetID() == GIMMICKID::ID_N_UTROM)
				++UtromCnt;
		};
    };

    CGaugeStatus::CounterDispSet(1, UtromCnt);
};


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
            CEnemy* pEnemy = (CEnemy*)pObject;

            CGaugeStatus::ExGaugeDispSet(1, CGaugeStatus::EX_GAUGE_ZAKO_FUGITOID_LIVE, pEnemy->GetHP());
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
                CEnemy* pEnemy = (CEnemy*)pObject;

                CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_FUGITOID_LIVE, pEnemy->GetHPMax());
            };            
        };
    };
};


uint32 CLifeExGauge::findFugitoid(void)
{
    int32 EnemyCnt = CGameProperty::GetEnemyMax();
    for (int32 i = 0; i < EnemyCnt; ++i)
    {
		if (CGameProperty::Enemy(i).IsAlive())
		{
			if (CGameProperty::Enemy(i).Enemy().EnemyCharacter().GetID() == ENEMYID::ID_FUGITOID)
				return CGameProperty::Enemy(i).Enemy().GetHandle();
		};
    };

    return 0;
};

#include "System/Common/Controller.hpp"
void CThreeRaceExGauge::Start(void)
{
    m_nNumFoot = 15;
    m_nNumPDG = 15;
    m_nNumMafia = 15;

    CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT, m_nNumFoot);
    CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, m_nNumPDG);
    CGaugeStatus::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA, m_nNumMafia);

    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT, true);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, true);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA, true);
};


void CThreeRaceExGauge::Stop(void)
{
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT, false);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, false);
    CGaugeStatus::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA, false);
};


void CThreeRaceExGauge::Update(void)
{
    CGaugeStatus::ExGaugeDispSet(1, CGaugeStatus::EX_GAUGE_ZAKO_COUNT_FOOT, m_nNumFoot);
    CGaugeStatus::ExGaugeDispSet(2, CGaugeStatus::EX_GAUGE_ZAKO_COUNT_BANDIT, m_nNumPDG);
    CGaugeStatus::ExGaugeDispSet(3, CGaugeStatus::EX_GAUGE_ZAKO_COUNT_MAFIA, m_nNumMafia);
};


void CThreeRaceExGauge::HandleEnemyDestroyed(CEnemy* pEnemy)
{
    ASSERT(pEnemy);

    switch (pEnemy->EnemyCharacter().GetID())
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
    };
};