#include "AIModerator.hpp"
#include "EnemyCharacter.hpp"


CAIModerator::CAIModerator(CEnemyCharacter& rEnemyCharacter)
: m_fTimer(0.0f)
, m_rEnemyCharacter(rEnemyCharacter)
{
    if (m_rEnemyCharacter.FrequencyParameter())
    {
        m_AIFreqParam.Initialize(
            m_rEnemyCharacter.FrequencyParameter(),
            m_rEnemyCharacter.CharacterParameter().m_iFrequencyMax,
            3
        );
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
    return m_rEnemyCharacter.AIThinkOrder();
};


CEnemyCharacter& CAIModerator::EnemyCharacter(void)
{
    return m_rEnemyCharacter;
};


ENEMYTYPES::CHARACTERISTIC& CAIModerator::Characteristic(void)
{
    return m_rEnemyCharacter.AICharacteristic();
};