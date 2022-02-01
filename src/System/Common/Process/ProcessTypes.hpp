#pragma once


class CProcess;


namespace PROCESSTYPES
{
	static const int32 LABEL_TOP			= 0;
	static const int32 LABEL_EOL			= 0x7FFFFFFF;
	static const int32 LABEL_PREV			= 0x7FFFFFFE;

	static const int32 PRIORITY_MIN			= 0;
	static const int32 PRIORITY_HIGH		= PRIORITY_MIN;
	static const int32 PRIORITY_MEDIUM		= 4;
	static const int32 PRIORITY_LOW			= 7;
	static const int32 PRIORITY_MAX			= 8;
	static const int32 PRIORITY_CAPACITY	= 16;

	static const int32 ACCESS_NONE			= 0x0;
	static const int32 ACCESS_RUN			= 0x1;
	static const int32 ACCESS_DRAW			= 0x2;
	static const int32 ACCESS_ALL			= ACCESS_RUN | ACCESS_DRAW;

	typedef CProcess*(*PFN_PROCESSINSTANCE)(void);

	struct PROCESS
	{
		const char* m_pszName;
		int32 m_iLabel;
		int32 m_iPriority;
		PFN_PROCESSINSTANCE m_pfnInstance;
	};

	enum STATE
	{
		STATE_INVALID,
		STATE_START,
		STATE_RUN,
		STATE_PAUSE,
		STATE_RESUME,
	};

	struct MAIL
	{
		enum TYPE
		{
			TYPE_NONE = 0,
			TYPE_MSG,
			TYPE_ATTACH,
			TYPE_DETACH,
			TYPE_PAUSE,
			TYPE_RESUME,
			TYPE_RUN_ENABLE,
			TYPE_DRAW_ENABLE,
			TYPE_RUN_DISABLE,
			TYPE_DRAW_DISABLE,
			TYPE_EOL,
		};

		TYPE m_type;
		int32 m_iLabel;
		const void* m_param;

		static const MAIL EMPTY;
	};
};