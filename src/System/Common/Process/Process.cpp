#include "Process.hpp"


CProcess::CResources::CResources(void)
: m_pMemory(nullptr)
, m_pMail(nullptr)
, m_pDispatcher(nullptr)
{
	;
};


CProcess::CResources::~CResources(void)
{
	;
};


void CProcess::CResources::Attach(CProcessDispatcher& dispatcher)
{
	m_pDispatcher = &dispatcher;
};


void CProcess::CResources::Attach(CProcessMail& mail)
{
	m_pMail = &mail;
};


void CProcess::CResources::Attach(CProcessMemory& memory)
{
	m_pMemory = &memory;
};


CProcessMail& CProcess::CResources::Mail(void)
{
	return *m_pMail;
};


CProcessMemory& CProcess::CResources::Memory(void)
{
	return *m_pMemory;
};


CProcessInfo& CProcess::CResources::Info(void)
{
	return m_info;
};


CProcess::CProcess(void)
{
	;
};


CProcess::~CProcess(void)
{
	;
};


CProcessInfo& CProcess::Info(void)
{
	return m_resources.Info();
};


CProcessMemory& CProcess::Memory(void)
{
	return m_resources.Memory();
};


CProcessMail& CProcess::Mail(void)
{
	return m_resources.Mail();
};


CProcess::CResources& CProcess::Resources(void)
{
	return m_resources;
};


const char* CProcess::ClassName(void) const
{
    return "";
};