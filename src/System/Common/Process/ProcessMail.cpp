#include "ProcessMail.hpp"


CProcessMail::CMailBox::CMailBox(void)
: m_size(0)
{
	Clear();
};


CProcessMail::CMailBox::~CMailBox(void)
{
	;
};


void CProcessMail::CMailBox::Clear(int32 iLabel)
{
	if (!m_size)
		return;

	for (int32 i = 0; i < COUNT_OF(m_aMailes); ++i)
	{
		if (m_aMailes[i].m_iLabel == iLabel)
		{
			m_aMailes[i] = PROCESSTYPES::MAIL();
			--m_size;
		};
	};
};


void CProcessMail::CMailBox::Clear(void)
{
	m_size = 0;
	
	for (int32 i = 0; i < COUNT_OF(m_aMailes); ++i)
		m_aMailes[i] = PROCESSTYPES::MAIL();
};


bool CProcessMail::CMailBox::Accept(PROCESSTYPES::MAIL& mail)
{
	if (!m_size)
		return false;

	for (int32 i = 0; i < COUNT_OF(m_aMailes); ++i)
	{
		if (m_aMailes[i].m_iLabel != PROCESSTYPES::LABEL_EOL)
		{
			mail = m_aMailes[i];
			m_aMailes[i] = PROCESSTYPES::MAIL();
			--m_size;
			
			return true;
		};
	};

	return false;
};


bool CProcessMail::CMailBox::Accept(int32 iReceiver, PROCESSTYPES::MAIL& mail)
{
	ASSERT(iReceiver >= PROCESSTYPES::LABEL_TOP && iReceiver < PROCESSTYPES::LABEL_EOL);

	if (!m_size)
		return false;

	for (int32 i = 0; i < COUNT_OF(m_aMailes); ++i)
	{
		if (m_aMailes[i].m_iLabel == iReceiver)
		{
			mail = m_aMailes[i];
			m_aMailes[i] = PROCESSTYPES::MAIL();
			--m_size;
			
			return true;
		};
	};

	return false;
};


bool CProcessMail::CMailBox::Post(PROCESSTYPES::MAIL& mail)
{
	if (!mail.m_type || mail.m_iLabel == PROCESSTYPES::LABEL_EOL)
		return false;

	for (int32 i = 0; i < COUNT_OF(m_aMailes); ++i)
	{
		if (m_aMailes[i].m_iLabel == PROCESSTYPES::LABEL_EOL)
		{
			m_aMailes[i] = mail;
			++m_size;
			return true;
		};
	};

	return false;
};


CProcessMail::CProcessMail(void)
: m_iCurrentReceiver(PROCESSTYPES::LABEL_EOL)
, m_pMailPost(nullptr)
, m_pMailCarrier(nullptr)
{
	m_pMailPost = new CMailBox;
	m_pMailCarrier = new CMailBox;

	ASSERT(m_pMailPost);
	ASSERT(m_pMailCarrier);
};


CProcessMail::~CProcessMail(void)
{
	if (m_pMailCarrier)
	{
		delete m_pMailCarrier;
		m_pMailCarrier = nullptr;
	};

	if (m_pMailPost)
	{
		delete m_pMailPost;
		m_pMailPost = nullptr;
	};
};


bool CProcessMail::Send(int32 iReceiver, PROCESSTYPES::MAIL::TYPE type, const void* param)
{
	PROCESSTYPES::MAIL mail;

	mail.m_iLabel	= iReceiver;
	mail.m_type		= type;
	mail.m_param	= param;

	return Send(mail);
};


bool CProcessMail::Send(PROCESSTYPES::MAIL& mail)
{
	return MailPost().Post(mail);
};


bool CProcessMail::Recv(PROCESSTYPES::MAIL& mail)
{
	return MailCarrier().Accept(m_iCurrentReceiver, mail);
};


void CProcessMail::SetCurrent(int32 iReceiver)
{
	m_iCurrentReceiver = iReceiver;
};


CProcessMail::CMailBox& CProcessMail::MailPost(void)
{
	ASSERT(m_pMailPost);
	return *m_pMailPost;
};


CProcessMail::CMailBox& CProcessMail::MailCarrier(void)
{
	ASSERT(m_pMailCarrier);
	return *m_pMailCarrier;
};


int32 CProcessMail::CurrentReceiver(void) const
{
	return m_iCurrentReceiver;
};