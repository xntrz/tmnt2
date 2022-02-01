#pragma once

#include "Process.hpp"


class CSequence : public CProcess
{
private:
	class CPrivate
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

		static MESSAGE m_SeqMsg;

		uint32 MESSAGE_ID;
		int32 m_iLabelParent;
		int32 m_iLabelChild;
		int32 m_iLabelPrev;
	};

public:
	CSequence(void);
	virtual ~CSequence(void);
	virtual bool Attach(void) override;
	virtual void Detach(void) override;
	virtual void Move(void) override;
	virtual void Draw(void) const override;
	virtual bool Call(int32 iLabel, bool bKeepDraw = false, const void* param = nullptr);
	virtual bool Jump(int32 iLabel, const void* param = nullptr);
	virtual bool Kill(int32 iLabel, const void* param = nullptr);
	virtual bool Ret(const void* param = nullptr);
	virtual bool OnAttach(const void* param = nullptr) = 0;
	virtual void OnDetach(void) = 0;
	virtual void OnMove(bool bResume = false, const void* param = nullptr) = 0;
	virtual void OnDraw(void) const = 0;
	int32 Child(void) const;
	int32 Parent(void) const;
	void DisposeMessage(void);
	static int32 GetCurrently(void);

private:
	const CPrivate& Private(void) const;

private:
	static int32 m_iLabelCurrently;
	CPrivate m_private;
};