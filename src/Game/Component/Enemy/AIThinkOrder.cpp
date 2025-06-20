#include "AIThinkOrder.hpp"


CAIThinkOrder::CAIThinkOrder(void)
: m_eOrder(ORDER_NOTHING)
, m_eResult(RESULT_WAITING)
{
    Clear();    
};


CAIThinkOrder::~CAIThinkOrder(void)
{
    ;
};


CAIThinkOrder::ORDER CAIThinkOrder::GetOrder(void) const
{
    return m_eOrder;
};


CAIThinkOrder::RESULT CAIThinkOrder::GetAnswer(void) const
{
    return m_eResult;
};


void CAIThinkOrder::SetAnswer(RESULT eResult)
{
    m_eResult = eResult;
};


CAIThinkOrder::ORDERNOTHING& CAIThinkOrder::OrderNothing(void)
{
    ASSERT(m_eOrder == ORDER_NOTHING);
    return m_order.m_nothing;
};


CAIThinkOrder::ORDERWAIT& CAIThinkOrder::OrderWait(void)
{
    ASSERT(m_eOrder == ORDER_WAIT);
    return m_order.m_wait;
};


CAIThinkOrder::ORDERMOVE& CAIThinkOrder::OrderMove(void)
{
    ASSERT(m_eOrder == ORDER_MOVE);
    return m_order.m_move;
};



CAIThinkOrder::ORDERRUN& CAIThinkOrder::OrderRun(void)
{
    ASSERT(m_eOrder == ORDER_RUN);
    return m_order.m_run;
};


CAIThinkOrder::ORDERATTACK& CAIThinkOrder::OrderAttack(void)
{
    ASSERT(m_eOrder == ORDER_ATTACK);
    return m_order.m_attack;
};



CAIThinkOrder::ORDERDEFENCE& CAIThinkOrder::OrderDefence(void)
{
    ASSERT(m_eOrder == ORDER_DEFENCE);
    return m_order.m_defence;
};


void CAIThinkOrder::Clear(void)
{
    m_eOrder = ORDER_NOTHING;
    m_eResult = RESULT_WAITING;
    std::memset(&m_order, 0x00, sizeof(m_order));
};


void CAIThinkOrder::SetOrder(ORDER eOrder)
{
    m_eOrder = eOrder;
};