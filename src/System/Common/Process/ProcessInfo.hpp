#pragma once

#include "ProcessTypes.hpp"


class CProcessDispatcher;
class CProcess;


class CProcessInfo
{
public:
    CProcessInfo(void);
    ~CProcessInfo(void);
    virtual void Set(int32 iLabel, int32 iPriority, PROCESSTYPES::STATE state);
    virtual void Set(CProcessDispatcher& dispatcher);
    virtual CProcess& Process(int32 iLabel);
    virtual bool IsProcessExist(int32 iLabel);
    virtual PROCESSTYPES::STATE State(void) const;
    virtual int32 Priority(void) const;
    virtual int32 Label(void) const;	

private:
    int32 m_iLabel;
    int32 m_iPriority;
    PROCESSTYPES::STATE m_state;
    CProcessDispatcher* m_pDispatcher;
};