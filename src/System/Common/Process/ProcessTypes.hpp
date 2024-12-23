#pragma once


class CProcess;


namespace PROCESSTYPES
{
	static const int32 LABEL_TOP 			= 0;
	static const int32 LABEL_EOL 			= ~(1 << 31);
	static const int32 LABEL_SEQ_PREV		=  (1 << 31);
	static const int32 LABEL_SEQ_DEC 	    = -1;
	static const int32 LABEL_SEQ_INC 	    = -2;

	static const int32 PRIORITY_MIN 		= 0;
	static const int32 PRIORITY_MAX			= 8;	
	static const int32 PRIORITY_HIGH		= PRIORITY_MIN;
	static const int32 PRIORITY_LOW			= PRIORITY_MAX - 1;
    static const int32 PRIORITY_NORMAL      = (PRIORITY_MAX + PRIORITY_MIN) / 2;
	
	static const int32 ACCESS_NONE			= 0;
	static const int32 ACCESS_MOVE			= (1 << 0);
	static const int32 ACCESS_DRAW			= (1 << 1);
	static const int32 ACCESS_ALL			= ACCESS_MOVE | ACCESS_DRAW;

	struct PROCESS
	{
		int32 m_iLabel;
		int32 m_iPriority;
		CProcess* (*m_pfnInstance)(void);
	};

	enum STATE
	{
		STATE_NONE = 0,
		STATE_START,
		STATE_RUN,
		STATE_PAUSE,
		STATE_RESUME,
		
		STATE_NUM,
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
			TYPE_MOVE_ENABLE,
			TYPE_DRAW_ENABLE,
			TYPE_MOVE_DISABLE,
			TYPE_DRAW_DISABLE,
			TYPE_EXIT,

			TYPE_MAX,
		};

		TYPE m_type;
		int32 m_iLabel;
		const void* m_param;

		inline MAIL(void): m_type(TYPE_NONE), m_iLabel(LABEL_EOL), m_param(nullptr) {};
	};
};