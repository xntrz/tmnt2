#pragma once

#include "GimmickTypes.hpp"


class CGimmick;


class CGimmickQuery
{
public:
    inline CGimmickQuery(GIMMICKTYPES::QUERY querytype) :m_querytype(querytype) {};
    virtual ~CGimmickQuery(void) {};
    virtual void Default(const CGimmick* pGimmick) {};

    inline GIMMICKTYPES::QUERY GetType(void) const { return m_querytype; };

private:
    GIMMICKTYPES::QUERY m_querytype;
};


class CStateGimmickQuery : public CGimmickQuery
{
public:
    inline CStateGimmickQuery(GIMMICKTYPES::QUERY querytype) : CGimmickQuery(querytype), m_nState(0), m_nTarget(0) {};
    virtual ~CStateGimmickQuery(void) {};

public:    
    int32 m_nState;
    int32 m_nTarget;
};


class CCounterGimmickQuery : public CGimmickQuery
{
public:
    inline CCounterGimmickQuery(GIMMICKTYPES::QUERY querytype) : CGimmickQuery(querytype), m_nCounter(0) {};
    virtual ~CCounterGimmickQuery(void) {};

public:
    int32 m_nCounter;
};