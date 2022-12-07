#pragma once

#include "AIThinkOrder.hpp"
#include "AIFrequencyParam.hpp"
#include "EnemyTypes.hpp"


class CEnemyCharacter;

class CAIModerator
{
public:
    CAIModerator(CEnemyCharacter& rEnemyCharacter);
    virtual ~CAIModerator(void);
    virtual void Run(void) = 0;
    virtual void Draw(void);
    CAIThinkOrder& ThinkOrder(void);
    CEnemyCharacter& EnemyCharacter(void);
    ENEMYTYPES::CHARACTERISTIC& Characteristic(void);
    
private:
    float m_fTimer;
    CAIThinkOrder m_AIThinkOrderOld;
    CEnemyCharacter& m_rEnemyCharacter;
    CAIFrequencyParam m_AIFreqParam;
};