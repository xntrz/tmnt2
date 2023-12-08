#include "Sequence.hpp"
#include "ProcessMail.hpp"


class CSequence::CPrivate
{
public:
	struct MESSAGE
	{
		enum TYPE
		{
			TYPE_NONE = 0,
			TYPE_JUMP,
			TYPE_CALL,
			TYPE_RET,
			TYPE_KILL,
			TYPE_MAX,
		};

		int32 m_id;
		const void* m_param;
		TYPE m_type;
		int32 m_iLabelParent;
		int32 m_iLabelChild;
		int32 m_iLabelPrev;
	};

	typedef MESSAGE* PMESSAGE;

	static bool m_bMemCheck;
	static MESSAGE m_SeqMsg;

	int32 MESSAGE_ID;
	int32 m_iLabelParent;
	int32 m_iLabelChild;
	int32 m_iLabelPrev;
};


/*static*/ bool CSequence::CPrivate::m_bMemCheck;


/*static*/ CSequence::CPrivate::MESSAGE CSequence::CPrivate::m_SeqMsg =
{
	/*m_id*/			0,
	/*m_param*/			nullptr,
	/*m_type*/			CSequence::CPrivate::MESSAGE::TYPE_NONE,
	/*m_iLabelParent*/	PROCESSTYPES::LABEL_EOL,
	/*m_iLabelChild*/	PROCESSTYPES::LABEL_EOL,
	/*m_iLabelPrev*/	PROCESSTYPES::LABEL_EOL,
};


/*static*/ int32 CSequence::m_iLabelCurrently = PROCESSTYPES::LABEL_EOL;


/*static*/ int32 CSequence::GetCurrently(void)
{
	return m_iLabelCurrently;
};


CSequence::CSequence(void)
: m_pPrivate(nullptr)
{
	m_pPrivate = new CPrivate;
	ASSERT(m_pPrivate);
	
	m_pPrivate->MESSAGE_ID 		= 'SEQM';
	m_pPrivate->m_iLabelParent	= PROCESSTYPES::LABEL_EOL;
	m_pPrivate->m_iLabelChild 	= PROCESSTYPES::LABEL_EOL;
	m_pPrivate->m_iLabelPrev	= PROCESSTYPES::LABEL_EOL;
};


CSequence::~CSequence(void)
{
	if (m_pPrivate)
	{
		delete m_pPrivate;
		m_pPrivate = nullptr;
	};
};


bool CSequence::Attach(void)
{
	ASSERT(Info().State() == PROCESSTYPES::STATE_START);

	bool bResult = false;
	const void* param = nullptr;
	
	PROCESSTYPES::MAIL mail;
	if (Mail().Recv(mail) && (mail.m_type == PROCESSTYPES::MAIL::TYPE_MSG))
	{	
		CPrivate::MESSAGE* pMsg = CPrivate::PMESSAGE(mail.m_param);
		ASSERT(pMsg->m_id == Private().MESSAGE_ID);

		switch (pMsg->m_type)
		{
		case CPrivate::MESSAGE::TYPE_JUMP:
		case CPrivate::MESSAGE::TYPE_CALL:
			{
				param = pMsg->m_param;

				Private().m_iLabelParent= pMsg->m_iLabelParent;
				Private().m_iLabelChild	= pMsg->m_iLabelChild;
				Private().m_iLabelPrev	= pMsg->m_iLabelPrev;			
			}
			break;

		default:
			ASSERT(false);
			return false;
		};
	};

	m_iLabelCurrently = Info().Label();
	
	return OnAttach(param);
};


void CSequence::Detach(void)
{
	OnDetach();
};


void CSequence::Move(void)
{
	PROCESSTYPES::MAIL mail;

	if (Info().State() == PROCESSTYPES::STATE_RESUME
		&& Mail().Recv(mail)
		&& (mail.m_type == PROCESSTYPES::MAIL::TYPE_MSG))
	{
		CPrivate::MESSAGE* pMsg = CPrivate::PMESSAGE(mail.m_param);
		ASSERT(pMsg->m_id == Private().MESSAGE_ID);

		switch (pMsg->m_type)
		{
		case CPrivate::MESSAGE::TYPE_RET:
			{
				Private().m_iLabelChild = PROCESSTYPES::LABEL_EOL;
				
				m_iLabelCurrently = Info().Label();
				
				OnMove(true, pMsg->m_param);
				DisposeMessage();
			}
			break;

		case CPrivate::MESSAGE::TYPE_KILL:
			{
				Kill(pMsg->m_iLabelParent, pMsg->m_param);
			}			
			break;

		default:
			ASSERT(false);
			break;
		};
	}
	else
	{
		OnMove(false, nullptr);
		DisposeMessage();
	};
};


void CSequence::Draw(void) const
{
	OnDraw();
};


bool CSequence::Call(int32 iLabel, const void* pParam, bool bDrawEnable)
{
	CPrivate::MESSAGE* pMsg = &CPrivate::m_SeqMsg;

	pMsg->m_id 			= Private().MESSAGE_ID;
	pMsg->m_param 		= pParam;
	pMsg->m_type 		= CPrivate::MESSAGE::TYPE_CALL;
	pMsg->m_iLabelParent= Info().Label();
	pMsg->m_iLabelChild = PROCESSTYPES::LABEL_EOL;
	pMsg->m_iLabelPrev 	= PROCESSTYPES::LABEL_EOL;

	Private().m_iLabelChild = iLabel;

	if (!Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_PAUSE))
		return false;

	if (bDrawEnable)
		Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DRAW_ENABLE);

	if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_ATTACH))
		return false;

	Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_MSG, pMsg);

	return true;
};


bool CSequence::Jump(int32 iLabel, const void* pParam)
{
	if (iLabel == PROCESSTYPES::LABEL_PREV)
		iLabel = Private().m_iLabelPrev;

	CPrivate::MESSAGE* pMsg = &CPrivate::m_SeqMsg;

	pMsg->m_id 			= Private().MESSAGE_ID;
	pMsg->m_param 		= pParam;
	pMsg->m_type 		= CPrivate::MESSAGE::TYPE_JUMP;
	pMsg->m_iLabelParent= Private().m_iLabelParent;
	pMsg->m_iLabelChild = Private().m_iLabelChild;
	pMsg->m_iLabelPrev 	= Info().Label();

	if (!Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DETACH))
		return false;

	if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_ATTACH))
		return false;

	Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_MSG, pMsg);

	return true;
};


bool CSequence::Kill(int32 iBackToLabel, const void* pReturnValue)
{
	CPrivate::MESSAGE* pMsg = &CPrivate::m_SeqMsg;

	pMsg->m_id 			= Private().MESSAGE_ID;
	pMsg->m_param 		= pReturnValue;
	pMsg->m_type 		= CPrivate::MESSAGE::TYPE_KILL;
	pMsg->m_iLabelParent= iBackToLabel;
	pMsg->m_iLabelChild = PROCESSTYPES::LABEL_EOL;
	pMsg->m_iLabelPrev 	= PROCESSTYPES::LABEL_EOL;

	if (iBackToLabel == Private().m_iLabelParent)
		return Ret(pReturnValue);

	if (!Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DETACH))
		return false;

	if (Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_RESUME))
		Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_MSG, pMsg);
		
	return true;
};


bool CSequence::Ret(const void* pReturnValue)
{
	CPrivate::MESSAGE* pMsg = &CPrivate::m_SeqMsg;
	
	pMsg->m_id 			= Private().MESSAGE_ID;
	pMsg->m_param 		= pReturnValue;
	pMsg->m_type 		= CPrivate::MESSAGE::TYPE_RET;
	pMsg->m_iLabelParent= PROCESSTYPES::LABEL_EOL;
	pMsg->m_iLabelChild = PROCESSTYPES::LABEL_EOL;
	pMsg->m_iLabelPrev 	= PROCESSTYPES::LABEL_EOL;

	if (!Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DETACH))
		return false;

	if (Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_RESUME))
		Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_MSG, pMsg);

	return true;
};


int32 CSequence::Child(void) const
{
	return Private().m_iLabelChild;
};


int32 CSequence::Parent(void) const
{
	return Private().m_iLabelParent;
};


void CSequence::DisposeMessage(void)
{
	PROCESSTYPES::MAIL mail;
	while (Mail().Recv(mail))
		;	
};