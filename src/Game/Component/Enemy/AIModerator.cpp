#include "AIModerator.hpp"
#include "EnemyCharacter.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"

#include "Game/Component/GameMain/GameEnemy.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"


CAIModerator::CAIModerator(CEnemyCharacter* pEnemyChr)
: m_fTimer(0.0f)
, m_pEnemyChr(pEnemyChr)
{
    if (pEnemyChr->IsFrequencyParameterValid())
    {
        m_AIFreqParam.Initialize(pEnemyChr->FrequencyParameter(),
                                 pEnemyChr->CharacterParameter().m_iFrequencyMax,
                                 GAMETYPES::DIFFICULTY_NUM);
    };

    ThinkOrder().SetOrder(CAIThinkOrder::ORDER_NOTHING);

    m_AIThinkOrderOld = ThinkOrder();
};


CAIModerator::~CAIModerator(void)
{
    ;
};


void CAIModerator::Draw(void)
{
    ;
};


CAIThinkOrder& CAIModerator::ThinkOrder(void)
{
    return m_pEnemyChr->AIThinkOrder();
};


CEnemyCharacter& CAIModerator::EnemyCharacter(void)
{
    return *m_pEnemyChr;
};


const CEnemyCharacter& CAIModerator::EnemyCharacter(void) const
{
    return *m_pEnemyChr;
};


const ENEMYTYPES::CHARACTERISTIC& CAIModerator::Characteristic(void)
{
    return m_pEnemyChr->AICharacteristic();
};


CAIFrequencyParam& CAIModerator::AIFreqParam(void)
{
    return m_AIFreqParam;
};


const CAIFrequencyParam& CAIModerator::AIFreqParam(void) const
{
    return m_AIFreqParam;
};