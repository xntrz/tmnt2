#include "ProcessDispatcher.hpp"
#include "Process.hpp"
#include "ProcessMail.hpp"
#include "ProcessMemory.hpp"


class CProcessDispatcher::CAccesser
{
private:
	struct OBJECT
	{
		static const OBJECT EMPTY;

		int32 m_iLabel;
		PROCESSTYPES::STATE m_state;
		int32 m_flags;
		CProcess* m_pProcess;
	};

public:
	CAccesser(void);
	virtual ~CAccesser(void);
	bool Insert(int32 iPriority, int32 iLabel, CProcess* pProcess);
	bool Erase(int32 iLabel);
	bool FindFirst(void);
	bool FindNext(void);	
	bool StrideNext(void);
	bool Set(OBJECT& object);
	bool Get(OBJECT& object);
	bool SetState(PROCESSTYPES::STATE state);
	bool SetFlags(int32 flags);
	PROCESSTYPES::STATE GetState(void);
	int32 GetFlags(void);
	CProcess* GetProcess(void);
	int32 GetLabel(void);
	int32 GetPriority(void);
	bool Search(int32 iLabel);
	bool IsExist(int32 iLabel) const;
	CProcess* Refer(int32 iLabel) const;
	int32 EnumerateMax(void) const;
	void SetTerminationPhase(void);

private:
	OBJECT m_apObjects[PROCESSTYPES::PRIORITY_MAX][PROCESSTYPES::PRIORITY_CAPACITY];
	int32 m_aiNumRunning[PROCESSTYPES::PRIORITY_MAX];
	int32 m_numTotalRunning;
	int32 m_currentPriority;
	int32 m_currentObject;
	bool m_bTerminationPhase;
};


class CProcessDispatcher::CList
{
public:
	CList(const PROCESSTYPES::PROCESS* pProcessList);
	virtual ~CList(void);
	bool Search(int32 iLabel, PROCESSTYPES::PROCESS& result) const;
	bool IsProperList(void) const;

private:
	const PROCESSTYPES::PROCESS* m_pProcessList;
};


/*static*/ const CProcessDispatcher::CAccesser::OBJECT CProcessDispatcher::CAccesser::OBJECT::EMPTY =
{
	PROCESSTYPES::LABEL_EOL,
	PROCESSTYPES::STATE_INVALID,
	PROCESSTYPES::ACCESS_NONE, nullptr
};


CProcessDispatcher::CAccesser::CAccesser(void)
: m_numTotalRunning(0)
, m_currentPriority(0)
, m_currentObject(0)
, m_bTerminationPhase(false)
{
	for (int32 i = 0; i < COUNT_OF(m_apObjects); ++i)
	{
		m_aiNumRunning[i] = 0;
		
		for (int32 j = 0; j < COUNT_OF(m_apObjects[0]); ++j)
			m_apObjects[i][j] = OBJECT::EMPTY;
	};
};


CProcessDispatcher::CAccesser::~CAccesser(void)
{
	;
};


bool CProcessDispatcher::CAccesser::Insert(int32 iPriority, int32 iLabel, CProcess* pProcess)
{
	ASSERT(iPriority >= PROCESSTYPES::PRIORITY_MIN && iPriority < PROCESSTYPES::PRIORITY_MAX);
	ASSERT((iLabel >= PROCESSTYPES::LABEL_TOP) && (iLabel <= PROCESSTYPES::LABEL_EOL));

	for (int32 i = 0; i < COUNT_OF(m_apObjects[0]); ++i)
	{
		OBJECT& object = m_apObjects[iPriority][i];
		if (object.m_state == PROCESSTYPES::STATE_INVALID)
		{
			object.m_flags 		= PROCESSTYPES::ACCESS_NONE;
			object.m_iLabel 	= iLabel;
			object.m_pProcess 	= pProcess;
			object.m_state 		= PROCESSTYPES::STATE_START;

			++m_aiNumRunning[iPriority];
			++m_numTotalRunning;

			return true;
		};
	};

	return false;
};


bool CProcessDispatcher::CAccesser::Erase(int32 iLabel)
{
	ASSERT(iLabel >= PROCESSTYPES::LABEL_TOP && iLabel <= PROCESSTYPES::LABEL_EOL);

	bool bResult = false;
	int32 prevPriority = m_currentPriority;
	int32 prevObject = m_currentObject;

	if (Search(iLabel))
	{
		OBJECT object = OBJECT::EMPTY;
		Set(object);

		ASSERT(m_aiNumRunning[m_currentPriority] > 0);
		ASSERT(m_numTotalRunning > 0);
		
		--m_aiNumRunning[m_currentPriority];
		--m_numTotalRunning;
		
		bResult = true;
	};

	m_currentPriority = prevPriority;
	m_currentObject = prevObject;

	return bResult;
};


bool CProcessDispatcher::CAccesser::FindFirst(void)
{
	m_currentPriority = (m_bTerminationPhase ? COUNT_OF(m_apObjects) - 1 : 0);
	m_currentObject = (m_bTerminationPhase ? COUNT_OF(m_apObjects[0]) - 1 : 0);

	OBJECT object = OBJECT::EMPTY;
	return Get(object);
};


bool CProcessDispatcher::CAccesser::FindNext(void)
{
	OBJECT object = OBJECT::EMPTY;
	if (Get(object))
	{
		if (!m_bTerminationPhase)
			++m_currentObject;
		else
			--m_currentObject;
		
		return Get(object);
	};

	return false;
};


bool CProcessDispatcher::CAccesser::StrideNext(void)
{
	if (!m_bTerminationPhase)
	{
		for (; m_currentPriority < COUNT_OF(m_apObjects); ++m_currentPriority)
		{
			for (; m_currentObject < COUNT_OF(m_apObjects[0]); ++m_currentObject)
			{
				if (m_apObjects[m_currentPriority][m_currentObject].m_state > PROCESSTYPES::STATE_INVALID)
					return true;
			};

			m_currentObject = 0;
		};
	}
	else
	{
		for (; m_currentPriority >= 0; --m_currentPriority)
		{
			for (; m_currentObject >= 0; --m_currentObject)
			{
				if (m_apObjects[m_currentPriority][m_currentObject].m_state > PROCESSTYPES::STATE_INVALID)
					return true;
			};

			m_currentObject = COUNT_OF(m_apObjects[0]);
		};
	};

	return false;
};


bool CProcessDispatcher::CAccesser::Set(OBJECT& object)
{
	if (StrideNext())
	{
		m_apObjects[m_currentPriority][m_currentObject] = object;
		return true;
	};

	return false;
};


bool CProcessDispatcher::CAccesser::Get(OBJECT& object)
{
	if (StrideNext())
	{
		object = m_apObjects[m_currentPriority][m_currentObject];
		return true;
	};

	return false;
};


bool CProcessDispatcher::CAccesser::SetState(PROCESSTYPES::STATE state)
{
	OBJECT object = OBJECT::EMPTY;
	Get(object);
	object.m_state = state;
	return Set(object);
};


bool CProcessDispatcher::CAccesser::SetFlags(int32 flags)
{
	OBJECT object = OBJECT::EMPTY;
	Get(object);
	object.m_flags = flags;
	return Set(object);
};


PROCESSTYPES::STATE CProcessDispatcher::CAccesser::GetState(void)
{
	OBJECT object = OBJECT::EMPTY;
	Get(object);
	return object.m_state;
};


int32 CProcessDispatcher::CAccesser::GetFlags(void)
{
	OBJECT object = OBJECT::EMPTY;
	Get(object);
	return object.m_flags;
};


CProcess* CProcessDispatcher::CAccesser::GetProcess(void)
{
	OBJECT object = OBJECT::EMPTY;
	Get(object);
	return object.m_pProcess;
};


int32 CProcessDispatcher::CAccesser::GetLabel(void)
{
	OBJECT object = OBJECT::EMPTY;
	Get(object);
	return object.m_iLabel;
};


int32 CProcessDispatcher::CAccesser::GetPriority(void)
{
	return m_currentPriority;
};


bool CProcessDispatcher::CAccesser::Search(int32 iLabel)
{
	for (m_currentPriority = 0; m_currentPriority < COUNT_OF(m_apObjects); ++m_currentPriority)
	{
		for (m_currentObject = 0; m_currentObject < COUNT_OF(m_apObjects[0]); ++m_currentObject)
		{
			OBJECT& object = m_apObjects[m_currentPriority][m_currentObject];
			if (object.m_iLabel == iLabel && object.m_state > PROCESSTYPES::STATE_INVALID)
				return true;
		};
	};

	return false;
};


bool CProcessDispatcher::CAccesser::IsExist(int32 iLabel) const
{
	for (int32 i = 0; i < COUNT_OF(m_apObjects); ++i)
	{
		for (int32 j = 0; j < COUNT_OF(m_apObjects[0]); ++j)
		{
			if (m_apObjects[i][j].m_iLabel == iLabel)
				return true;
		};
	};

	return false;
};


CProcess* CProcessDispatcher::CAccesser::Refer(int32 iLabel) const
{
	for (int32 i = 0; i < COUNT_OF(m_apObjects); ++i)
	{
		for (int32 j = 0; j < COUNT_OF(m_apObjects[0]); ++j)
		{
			const OBJECT& object = m_apObjects[i][j];
			if (object.m_iLabel == iLabel && object.m_state > PROCESSTYPES::STATE_INVALID)
			{
				ASSERT(object.m_pProcess);
				return object.m_pProcess;
			};
		};
	};

	return nullptr;
};


int32 CProcessDispatcher::CAccesser::EnumerateMax(void) const
{
	return m_numTotalRunning;
};


void CProcessDispatcher::CAccesser::SetTerminationPhase(void)
{
	m_bTerminationPhase = true;
};


CProcessDispatcher::CList::CList(const PROCESSTYPES::PROCESS* pProcessList)
: m_pProcessList(pProcessList)
{
	ASSERT(m_pProcessList);
};


CProcessDispatcher::CList::~CList(void)
{
	;
};


bool CProcessDispatcher::CList::Search(int32 iLabel, PROCESSTYPES::PROCESS& result) const
{
	const PROCESSTYPES::PROCESS* pProcess = m_pProcessList;
	while (pProcess->m_iLabel != PROCESSTYPES::LABEL_EOL)
	{
		if (pProcess->m_iLabel == iLabel)
		{
			result = *pProcess;
			return true;
		};

		++pProcess;
	};

	return false;
};


bool CProcessDispatcher::CList::IsProperList(void) const
{
	const PROCESSTYPES::PROCESS* pProcess = m_pProcessList;
	while (pProcess->m_iLabel != PROCESSTYPES::LABEL_EOL)
	{
		if (pProcess->m_iPriority < PROCESSTYPES::PRIORITY_MIN ||
			pProcess->m_iPriority >= PROCESSTYPES::PRIORITY_MAX)// ||
			//!pProcess->m_pfnInstance)
			return false;

		++pProcess;
	};

	return true;
};


/*static*/ CProcessMail* CProcessDispatcher::m_pMailSvc = nullptr;


/*static*/ bool CProcessDispatcher::SendMail(PROCESSTYPES::MAIL& mail)
{
	ASSERT(m_pMailSvc);
	return m_pMailSvc->Send(mail);
};


/*static*/ bool CProcessDispatcher::AttachProcess(int32 iLabel)
{
	PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;

	mail.m_type 	= PROCESSTYPES::MAIL::TYPE_ATTACH;
	mail.m_iLabel 	= iLabel;

	return CProcessDispatcher::SendMail(mail);
};


/*static*/ bool CProcessDispatcher::DetachProcess(int32 iLabel)
{
	PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;

	mail.m_type 	= PROCESSTYPES::MAIL::TYPE_DETACH;
	mail.m_iLabel 	= iLabel;

	return CProcessDispatcher::SendMail(mail);
};


CProcessDispatcher::CProcessDispatcher(const PROCESSTYPES::PROCESS* pProcessList)
: m_pMail(nullptr)
, m_pMemory(nullptr)
, m_pAccesser(nullptr)
, m_pList(nullptr)
{
	m_pMail 	= new CProcessMail;
	m_pMemory 	= new CProcessMemory;
	m_pAccesser = new CAccesser;
	m_pList 	= new CList(pProcessList);
	
	ASSERT(m_pMail);
	ASSERT(m_pMemory);
	ASSERT(m_pAccesser);
	ASSERT(m_pList);

	ASSERT(!m_pMailSvc);
	m_pMailSvc = m_pMail;
};


CProcessDispatcher::~CProcessDispatcher(void)
{
	ASSERT(m_pMailSvc);
	ASSERT(m_pMailSvc == m_pMail);
	m_pMailSvc = nullptr;

	if (m_pList)
	{
		delete m_pList;
		m_pList = nullptr;
	};

	if (m_pAccesser)
	{
		delete m_pAccesser;
		m_pAccesser = nullptr;
	};

	if (m_pMemory)
	{
		delete m_pMemory;
		m_pMemory = nullptr;
	};

	if (m_pMail)
	{
		delete m_pMail;
		m_pMail = nullptr;
	};
};


bool CProcessDispatcher::Start(void)
{
	if (List().IsProperList())
		return Add(PROCESSTYPES::LABEL_TOP);

	return false;
};


void CProcessDispatcher::Stop(void)
{
	Dispose();
};


bool CProcessDispatcher::Add(int32 iLabel)
{
	PROCESSTYPES::PROCESS process;

	if (!List().Search(iLabel, process))
		return false;

	if (Accesser().Search(iLabel))
		return false;

	ASSERT(process.m_pfnInstance, "Process \"%s\" invalid or not implemented", process.m_pszName);

	CProcess* pProcess = process.m_pfnInstance();	
	pProcess->Resources().Attach(*this);
	pProcess->Resources().Attach(*m_pMail);
	pProcess->Resources().Attach(*m_pMemory);

	if (Accesser().Insert(process.m_iPriority, process.m_iLabel, pProcess) &&
		Accesser().Search(process.m_iLabel))
	{
		Accesser().SetState(PROCESSTYPES::STATE_START);
		Accesser().SetFlags(PROCESSTYPES::ACCESS_NONE);

		pProcess->Resources().Info().Set(*this);
		pProcess->Resources().Info().Set(Accesser().GetLabel(), Accesser().GetPriority(), Accesser().GetState());

		return true;
	}
	else
	{
		delete pProcess;
		return false;
	};
};


bool CProcessDispatcher::Dispose(int32 iLabel)
{
	if (!Accesser().Search(iLabel))
		return false;

	CProcess* pProcess = Accesser().GetProcess();
	ASSERT(pProcess);
	pProcess->Detach();
	Accesser().Erase(iLabel);
	delete pProcess;

	return true;
};


void CProcessDispatcher::Dispose(void)
{
	Accesser().SetTerminationPhase();
	
	while (Accesser().FindFirst())
	{
		CProcess* pProcess = Accesser().GetProcess();
		ASSERT(pProcess);
		pProcess->Detach();
		Accesser().Erase(Accesser().GetLabel());
		delete pProcess;
	};
};


void CProcessDispatcher::Move(void)
{
	for (bool bResult = Accesser().FindFirst(); bResult; bResult = Accesser().FindNext())
	{
		CProcess* pProcess = Accesser().GetProcess();
		ASSERT(pProcess);
		pProcess->Info().Set(Accesser().GetLabel(), Accesser().GetPriority(), Accesser().GetState());
		pProcess->Mail().SetCurrent(Accesser().GetLabel());

		switch (Accesser().GetState())
		{
		case PROCESSTYPES::STATE_START:
			if (!pProcess->Attach())
				ASSERT(false);
			break;

		case PROCESSTYPES::STATE_RESUME:
		case PROCESSTYPES::STATE_RUN:
		case PROCESSTYPES::STATE_PAUSE:
			if (Accesser().GetFlags() & PROCESSTYPES::ACCESS_RUN)
				pProcess->Move();
			break;

		case PROCESSTYPES::STATE_INVALID:
		default:
			ASSERT(false);
			break;
		};
	};
};


void CProcessDispatcher::Draw(void) const
{
	for (bool bResult = Accesser().FindFirst(); bResult; bResult = Accesser().FindNext())
	{
		CProcess* pProcess = Accesser().GetProcess();
		ASSERT(pProcess);
		pProcess->Info().Set(Accesser().GetLabel(), Accesser().GetPriority(), Accesser().GetState());
		pProcess->Mail().SetCurrent(Accesser().GetLabel());

		switch (Accesser().GetState())
		{
		case PROCESSTYPES::STATE_RUN:
			if (Accesser().GetFlags() & PROCESSTYPES::ACCESS_DRAW)
				pProcess->Draw();
			break;
		};
	};
};


void CProcessDispatcher::Dispatch(void)
{
	DispatchState();
	DispatchMail();
};


void CProcessDispatcher::DispatchState(void)
{
	for (bool bResult = Accesser().FindFirst(); bResult; bResult = Accesser().FindNext())
	{
		CProcess* pProcess = Accesser().GetProcess();
		ASSERT(pProcess);
		pProcess->Info().Set(Accesser().GetLabel(), Accesser().GetPriority(), Accesser().GetState());
		pProcess->Mail().SetCurrent(Accesser().GetLabel());

		switch (Accesser().GetState())
		{
		case PROCESSTYPES::STATE_START:
		case PROCESSTYPES::STATE_RESUME:
			Accesser().SetState(PROCESSTYPES::STATE_RUN);
			Accesser().SetFlags(PROCESSTYPES::ACCESS_ALL);
			break;
		};
	};
};


void CProcessDispatcher::DispatchMail(void)
{
	PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;
	CProcessMail::CMailBox& mailpost = m_pMail->MailPost();
	CProcessMail::CMailBox& mailcarrier = m_pMail->MailCarrier();

	for (bool bResult = mailpost.Accept(mail); bResult; bResult = mailpost.Accept(mail))
	{
		if (mail.m_iLabel != PROCESSTYPES::LABEL_EOL)
		{
			switch (mail.m_type)
			{
			case PROCESSTYPES::MAIL::TYPE_MSG:
				mailcarrier.Post(mail);
				break;

			case PROCESSTYPES::MAIL::TYPE_ATTACH:
				if (!Add(mail.m_iLabel))
					ASSERT(false);
				break;

			case PROCESSTYPES::MAIL::TYPE_DETACH:
				if (!Dispose(mail.m_iLabel))
					ASSERT(false);
				mailpost.Clear(mail.m_iLabel);
				mailcarrier.Clear(mail.m_iLabel);
				break;

			case PROCESSTYPES::MAIL::TYPE_PAUSE:
				if (Accesser().Search(mail.m_iLabel))
				{
					Accesser().SetFlags(Accesser().GetFlags() & ~PROCESSTYPES::ACCESS_RUN);
					Accesser().SetFlags(Accesser().GetFlags() & ~PROCESSTYPES::ACCESS_DRAW);
					Accesser().SetState(PROCESSTYPES::STATE_PAUSE);
				};
				break;

			case PROCESSTYPES::MAIL::TYPE_RESUME:
				if (Accesser().Search(mail.m_iLabel))
				{
					Accesser().SetFlags(Accesser().GetFlags() | PROCESSTYPES::ACCESS_RUN);
					Accesser().SetFlags(Accesser().GetFlags() | PROCESSTYPES::ACCESS_DRAW);
					Accesser().SetState(PROCESSTYPES::STATE_RESUME);
				};
				break;

			case PROCESSTYPES::MAIL::TYPE_RUN_ENABLE:
				if (Accesser().Search(mail.m_iLabel))
					Accesser().SetFlags(Accesser().GetFlags() | PROCESSTYPES::ACCESS_RUN);
				break;

			case PROCESSTYPES::MAIL::TYPE_DRAW_ENABLE:
				if (Accesser().Search(mail.m_iLabel))
					Accesser().SetFlags(Accesser().GetFlags() | PROCESSTYPES::ACCESS_DRAW);
				break;

			case PROCESSTYPES::MAIL::TYPE_RUN_DISABLE:
				if (Accesser().Search(mail.m_iLabel))
					Accesser().SetFlags(Accesser().GetFlags() & ~PROCESSTYPES::ACCESS_RUN);
				break;

			case PROCESSTYPES::MAIL::TYPE_DRAW_DISABLE:
				if (Accesser().Search(mail.m_iLabel))
					Accesser().SetFlags(Accesser().GetFlags() & ~PROCESSTYPES::ACCESS_DRAW);
				break;

			default:
				ASSERT(false);
				break;
			};
		}
		else
		{
			if (mail.m_type == PROCESSTYPES::MAIL::TYPE_EOL)
			{
				Dispose();
				mailpost.Clear();
				mailcarrier.Clear();
				break;
			};
		};
	};
};


bool CProcessDispatcher::IsEnded(void) const
{
	return !Accesser().EnumerateMax();
};


bool CProcessDispatcher::IsProcessExist(int32 iLabel) const
{
	return Accesser().IsExist(iLabel);
};


CProcess& CProcessDispatcher::Process(int32 iLabel) const
{
	return *Accesser().Refer(iLabel);
};


CProcessDispatcher::CAccesser& CProcessDispatcher::Accesser(void) const
{
	ASSERT(m_pAccesser);
	return *m_pAccesser;
};


CProcessDispatcher::CList& CProcessDispatcher::List(void) const
{
	ASSERT(m_pList);
	return *m_pList;
};