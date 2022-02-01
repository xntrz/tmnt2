#pragma once

#include "GimmickTypes.hpp"


class CGimmick;


class CGimmickQuery
{
public:
    CGimmickQuery(GIMMICKTYPES::QUERY querytype);
    virtual ~CGimmickQuery(void);
    virtual void Default(const CGimmick* pGimmick);
    GIMMICKTYPES::QUERY GetType(void) const;

private:
    GIMMICKTYPES::QUERY m_querytype;
};


class CStateGimmickQuery : public CGimmickQuery
{
public:
    CStateGimmickQuery(GIMMICKTYPES::QUERY querytype);
    virtual ~CStateGimmickQuery(void);

public:    
    int32 m_nState;
    int32 m_nTarget;
};


class CCounterGimmickQuery : public CGimmickQuery
{
public:
    CCounterGimmickQuery(GIMMICKTYPES::QUERY querytype);
    virtual ~CCounterGimmickQuery(void);

public:
    int32 m_nCounter;
};