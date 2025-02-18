#include "ProcessInfo.hpp"
#include "ProcessDispatcher.hpp"


CProcessInfo::CProcessInfo(void)
: m_iLabel(PROCESSTYPES::LABEL_EOL)
, m_iPriority(PROCESSTYPES::PRIORITY_HIGH)
, m_state(PROCESSTYPES::STATE_NONE)
, m_pDispatcher(nullptr)
{
    ;
};


CProcessInfo::~CProcessInfo(void)
{
    ;
};


void CProcessInfo::Set(int32 iLabel, int32 iPriority, PROCESSTYPES::STATE state)
{
    ASSERT(iLabel >= PROCESSTYPES::LABEL_TOP);
    ASSERT(iLabel < PROCESSTYPES::LABEL_EOL);

    ASSERT(iPriority >= PROCESSTYPES::PRIORITY_MIN);
    ASSERT(iPriority <= PROCESSTYPES::PRIORITY_MAX);

    m_iLabel    = iLabel;
    m_iPriority = iPriority;
    m_state     = state;
};


void CProcessInfo::Set(CProcessDispatcher& dispatcher)
{
    m_pDispatcher = &dispatcher;
};


CProcess& CProcessInfo::Process(int32 iLabel)
{
    return m_pDispatcher->Process(iLabel);
};


bool CProcessInfo::IsProcessExist(int32 iLabel)
{
    return m_pDispatcher->IsProcessExist(iLabel);
};


PROCESSTYPES::STATE CProcessInfo::State(void) const
{
    return m_state;
};


int32 CProcessInfo::Priority(void) const
{
    return m_iPriority;
};


int32 CProcessInfo::Label(void) const
{
    return m_iLabel;
};