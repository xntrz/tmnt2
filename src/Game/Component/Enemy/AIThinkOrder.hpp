#pragma once


class CAIThinkOrder
{
public:
    enum RESULT
    {
        RESULT_WAITING = 0,
        RESULT_ACCEPT,
        RESULT_REFUSE,
    };

    enum ORDER
    {
        ORDER_NOTHING = 0,
        ORDER_WAIT,
        ORDER_MOVE,
        ORDER_RUN,
        ORDER_ATTACK,
        ORDER_DEFENCE,
        
        ORDER_MAX,
    };

    struct ORDERATTACK
    {
        int32 m_iAttackType;
        RwV3d m_vAt;
        int32 m_iPlayerNo;
    };

    struct ORDERDEFENCE
    {
        int32 m_iDefenceType;
    };

    struct ORDERMOVE
    {
        int32 m_iMoveType;
        RwV3d m_vAt;
        int32 m_iPlayerNo;
        float m_fMoveTimer;        
    };

    struct ORDERNOTHING
    {
        void* dummy;
    };

    struct ORDERRUN
    {
        RwV3d m_vAt;
    };

    struct ORDERWAIT
    {
        int32 m_iWaitType;
        RwV3d m_vAt;
        int32 m_iPlayerNo;
        float m_fWaitTimer;
    };

public:
    CAIThinkOrder(void);
    ~CAIThinkOrder(void);
    ORDER GetOrder(void) const;
    RESULT GetAnswer(void) const;
    void SetAnswer(RESULT eResult);
    ORDERNOTHING& OrderNothing(void);
    ORDERWAIT& OrderWait(void);
    ORDERMOVE& OrderMove(void);
    ORDERRUN& OrderRun(void);
    ORDERATTACK& OrderAttack(void);
    ORDERDEFENCE& OrderDefence(void);
    void Clear(void);
    void SetOrder(ORDER eOrder);

private:
    ORDER m_eOrder;
    union
    {
        ORDERNOTHING m_nothing;
        ORDERWAIT    m_wait;
        ORDERMOVE    m_move;
        ORDERRUN     m_run;
        ORDERATTACK  m_attack;
        ORDERDEFENCE m_defence;
    } m_order;
    RESULT m_eResult;
};