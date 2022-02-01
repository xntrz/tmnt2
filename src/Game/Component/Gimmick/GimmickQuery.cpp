#include "GimmickQuery.hpp"


CGimmickQuery::CGimmickQuery(GIMMICKTYPES::QUERY querytype)
: m_querytype(querytype)
{
    ;
};


CGimmickQuery::~CGimmickQuery(void)
{
    ;
};


void CGimmickQuery::Default(const CGimmick* pGimmick)
{
    ;
};


GIMMICKTYPES::QUERY CGimmickQuery::GetType(void) const
{
    return m_querytype;
};


CStateGimmickQuery::CStateGimmickQuery(GIMMICKTYPES::QUERY querytype)
: CGimmickQuery(querytype)
, m_nState(0)
, m_nTarget(0)
{
    ;
};


CStateGimmickQuery::~CStateGimmickQuery(void)
{
    ;
};


CCounterGimmickQuery::CCounterGimmickQuery(GIMMICKTYPES::QUERY querytype)
: CGimmickQuery(querytype)
, m_nCounter(0)
{
    ;
};


CCounterGimmickQuery::~CCounterGimmickQuery(void)
{
    ;
};