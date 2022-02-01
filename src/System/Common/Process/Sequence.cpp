#include "Sequence.hpp"
#include "ProcessMail.hpp"


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


CSequence::CSequence(void)
{
	m_private.MESSAGE_ID	= 'SEQM';
	m_private.m_iLabelParent= PROCESSTYPES::LABEL_EOL;
	m_private.m_iLabelChild = PROCESSTYPES::LABEL_EOL;
	m_private.m_iLabelPrev	= PROCESSTYPES::LABEL_EOL;
};


CSequence::~CSequence(void)
{
	;
};


bool CSequence::Attach(void)
{
	ASSERT(Info().State() == PROCESSTYPES::STATE_START);

	PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;
	if (Mail().Recv(mail))
	{
		CPrivate::MESSAGE* pMsg = CPrivate::PMESSAGE(mail.m_param);
		ASSERT(pMsg->m_id == Private().MESSAGE_ID);
		switch (pMsg->m_type)
		{
		case CPrivate::MESSAGE::TYPE_JUMP:
		case CPrivate::MESSAGE::TYPE_CALL:
			m_private.m_iLabelParent= pMsg->m_iLabelParent;
			m_private.m_iLabelChild = pMsg->m_iLabelChild;
			m_private.m_iLabelPrev	= pMsg->m_iLabelPrev;
			m_iLabelCurrently = Info().Label();
			return OnAttach(pMsg->m_param);

		default:
			ASSERT(false);
			return false;
		};
	}
	else
	{
		m_iLabelCurrently = Info().Label();
		return OnAttach();
	};
};


void CSequence::Detach(void)
{
	OnDetach();
};


void CSequence::Move(void)
{
	PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;

	if (Info().State() == PROCESSTYPES::STATE_RESUME && Mail().Recv(mail))
	{
		CPrivate::MESSAGE* pMsg = CPrivate::PMESSAGE(mail.m_param);
		ASSERT(pMsg->m_id == Private().MESSAGE_ID);
		switch (pMsg->m_type)
		{
		case CPrivate::MESSAGE::TYPE_RET:
			m_private.m_iLabelChild = PROCESSTYPES::LABEL_EOL;
			m_iLabelCurrently = Info().Label();
			OnMove(true, pMsg->m_param);
			DisposeMessage();
			break;

		case CPrivate::MESSAGE::TYPE_KILL:
			Kill(pMsg->m_iLabelParent, pMsg->m_param);
			break;

		default:
			ASSERT(false);
			break;
		};
	}
	else
	{
		OnMove();
		DisposeMessage();
	};
};


void CSequence::Draw(void) const
{
	OnDraw();
};


bool CSequence::Call(int32 iLabel, bool bKeepDraw, const void* param)
{
	CPrivate::m_SeqMsg.m_id				= Private().MESSAGE_ID;
	CPrivate::m_SeqMsg.m_param			= param;
	CPrivate::m_SeqMsg.m_type			= CPrivate::MESSAGE::TYPE_CALL;
	CPrivate::m_SeqMsg.m_iLabelParent	= Info().Label();
	CPrivate::m_SeqMsg.m_iLabelChild	= PROCESSTYPES::LABEL_EOL;
	CPrivate::m_SeqMsg.m_iLabelPrev		= PROCESSTYPES::LABEL_EOL;

	m_private.m_iLabelChild = iLabel;

	if (!Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_PAUSE))
		return false;
	
	if (bKeepDraw)
		Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DRAW_ENABLE);

	if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_ATTACH))
		return false;

	if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_MSG, &CPrivate::m_SeqMsg))
		return false;

	return true;
};


bool CSequence::Jump(int32 iLabel, const void* param)
{
	if (iLabel == PROCESSTYPES::LABEL_PREV)
		iLabel = Private().m_iLabelPrev;

	CPrivate::m_SeqMsg.m_id				= Private().MESSAGE_ID;
	CPrivate::m_SeqMsg.m_param			= param;
	CPrivate::m_SeqMsg.m_type			= CPrivate::MESSAGE::TYPE_JUMP;
	CPrivate::m_SeqMsg.m_iLabelParent	= Private().m_iLabelParent;
	CPrivate::m_SeqMsg.m_iLabelChild	= Private().m_iLabelChild;
	CPrivate::m_SeqMsg.m_iLabelPrev		= Info().Label();

	if (!Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DETACH))
		return false;

	if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_ATTACH))
		return false;

	if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_MSG, &CPrivate::m_SeqMsg))
		return false;

	return true;
};


bool CSequence::Kill(int32 iLabel, const void* param)
{
	if (iLabel == Private().m_iLabelParent)
		return Ret(param);

	CPrivate::m_SeqMsg.m_id				= Private().MESSAGE_ID;
	CPrivate::m_SeqMsg.m_param			= param;
	CPrivate::m_SeqMsg.m_type			= CPrivate::MESSAGE::TYPE_KILL;
	CPrivate::m_SeqMsg.m_iLabelParent	= iLabel;
	CPrivate::m_SeqMsg.m_iLabelChild	= PROCESSTYPES::LABEL_EOL;
	CPrivate::m_SeqMsg.m_iLabelPrev		= PROCESSTYPES::LABEL_EOL;

	if (Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DETACH))
	{
		if (Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_RESUME))
			Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_MSG, &CPrivate::m_SeqMsg);

		return true;
	};

	return false;
};


bool CSequence::Ret(const void* param)
{
	CPrivate::m_SeqMsg.m_id				= Private().MESSAGE_ID;
	CPrivate::m_SeqMsg.m_param			= param;
	CPrivate::m_SeqMsg.m_type			= CPrivate::MESSAGE::TYPE_RET;
	CPrivate::m_SeqMsg.m_iLabelParent	= PROCESSTYPES::LABEL_EOL;
	CPrivate::m_SeqMsg.m_iLabelChild	= PROCESSTYPES::LABEL_EOL;
	CPrivate::m_SeqMsg.m_iLabelPrev		= PROCESSTYPES::LABEL_EOL;

	if (Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DETACH))
	{
		if (Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_RESUME))
			Mail().Send(Private().m_iLabelParent, PROCESSTYPES::MAIL::TYPE_MSG, &CPrivate::m_SeqMsg);

		return true;
	};

	return false;
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
	PROCESSTYPES::MAIL mail = PROCESSTYPES::MAIL::EMPTY;
	while (Mail().Recv(mail))
		;	
};


/*static*/ int32 CSequence::GetCurrently(void)
{
	return m_iLabelCurrently;
};


const CSequence::CPrivate& CSequence::Private(void) const
{
	return m_private;
};