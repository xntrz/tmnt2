#pragma once

#include "ProcessTypes.hpp"


class CProcessMemory;
class CProcessMail;


class CProcessDispatcher
{
private:
	class CAccesser;
	class CList;

public:
	static bool SendMail(PROCESSTYPES::MAIL& mail);
	static bool AttachProcess(int32 iLabel);
	static bool DetachProcess(int32 iLabel);

	CProcessDispatcher(const PROCESSTYPES::PROCESS* pProcessList);
	virtual ~CProcessDispatcher(void);
	bool Start(void);
	void Stop(void);
	bool Add(int32 iLabel);
	bool Dispose(int32 iLabel);
	void Dispose(void);
	void Move(void);
	void Draw(void) const;
	void Dispatch(void);
	void DispatchState(void);
	void DispatchMail(void);
	bool IsEnded(void) const;
	bool IsProcessExist(int32 iLabel) const;
	CProcess& Process(int32 iLabel) const;	
	CAccesser& Accesser(void) const;
	CList& List(void) const;

private:
	static CProcessMail* m_pMailSvc;
	CProcessMail* m_pMail;
	CProcessMemory* m_pMemory;
	CAccesser* m_pAccesser;
	CList* m_pList;
};