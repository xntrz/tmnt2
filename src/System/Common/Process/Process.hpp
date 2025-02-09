#pragma once

#include "ProcessTypes.hpp"
#include "ProcessInfo.hpp"


class CProcessDispatcher;
class CProcessMail;
class CProcessMemory;


class CProcess
{
public:
	class CResources
	{
	public:
		CResources(void);
		~CResources(void);
		void Attach(CProcessDispatcher& dispatcher);
		void Attach(CProcessMail& mail);
		void Attach(CProcessMemory& memory);
		CProcessMail& Mail(void);
		CProcessMemory& Memory(void);
		CProcessInfo& Info(void);

	private:
		CProcessInfo m_info;
		CProcessMemory* m_pMemory;
		CProcessMail* m_pMail;
		CProcessDispatcher* m_pDispatcher;
	};

public:
	CProcess(void);
	virtual ~CProcess(void);
	virtual bool Attach(void) = 0;
	virtual void Detach(void) = 0;
	virtual void Move(void) = 0;
	virtual void Draw(void) const = 0;
	CProcessInfo& Info(void);
	CProcessMemory& Memory(void);
	CProcessMail& Mail(void);
	CResources& Resources(void);
    const char* ClassName(void) const;

private:
	CResources m_resources;
};