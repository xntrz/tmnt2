#pragma once

#include "AIThinkOrder.hpp"
#include "AIFrequencyParam.hpp"
#include "EnemyTypes.hpp"


class CEnemyCharacter;


class CAIModerator
{
public:
    CAIModerator(CEnemyCharacter* pEnemyChr);
    virtual ~CAIModerator(void);
    virtual void Run(void) = 0;
    virtual void Draw(void);
    CAIThinkOrder& ThinkOrder(void);
    CEnemyCharacter& EnemyCharacter(void);
    const ENEMYTYPES::CHARACTERISTIC& Characteristic(void);
    CAIFrequencyParam& AIFreqParam(void);
    const CAIFrequencyParam& AIFreqParam(void) const;

protected:
    float m_fTimer;
    CAIThinkOrder m_AIThinkOrderOld;
    CEnemyCharacter* m_pEnemyChr;
    CAIFrequencyParam m_AIFreqParam;
};