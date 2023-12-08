#pragma once

#include "ProcessTypes.hpp"


class CProcessMemory;
class CProcessMail;


class CProcessDispatcher final
{
private:
	class CAccesser;
	class CList;

public:
	CProcessDispatcher(const PROCESSTYPES::PROCESS* pProcessList);
	~CProcessDispatcher(void);
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
	CProcessMail* m_pMail;
	CProcessMemory* m_pMemory;
	CAccesser* m_pAccesser;
	CList* m_pList;
};