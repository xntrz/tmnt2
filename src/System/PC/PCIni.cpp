#include "PCIni.hpp"

#include <list>

#define assert ASSERT


static bool IsHexString(const char* str)
{
	if (str[0] == '0' &&
		str[1] == 'x')
	{
		while (*str)
		{
			char ch = *str++;

			bool bResult =	(ch >= '0' && ch <= '9') ||
							(ch >= 'a' && ch <= 'f') ||
							(ch >= 'A' && ch <= 'F');

			if (!bResult)
				return false;
		};
		
		return true;
	};

	return false;
};


class CIniContainer
{
public:
	static const char* SEGMENT_NULL;

	struct KEY
	{
		std::string m_name;
		std::string m_value;
	};

	struct SEGMENT
	{
		std::string m_name;
		std::list<KEY> m_listKey;
	};

public:
	CIniContainer(void)
	: m_pCurrentSegment(nullptr)
	, m_pCurrentKey(nullptr)
	{	
		SegmentNew(SEGMENT_NULL);
	};


	bool SegmentNew(const char* segment)
	{	
		if (SegmentSearch(segment))
			return false;

		m_listSegment.push_back({ segment, {} });
		m_pCurrentSegment = &m_listSegment.back();
		
		return true;
	};


	SEGMENT* SegmentSearch(const char* segment)
	{
		if (!segment)
			segment = SEGMENT_NULL;
		
		if (m_pCurrentSegment && m_pCurrentSegment->m_name == segment)
			return m_pCurrentSegment;

		auto it = m_listSegment.begin();
		while (it != m_listSegment.end())
		{
			if ((*it).m_name == segment)
			{
				m_pCurrentSegment = &*it;
				return &*it;
			};

			++it;
		};

		return nullptr;
	};


	bool SegmentDelete(const char* segment)
	{	
		if (!SegmentSearch(segment))
			return false;

		if (!segment || (segment && std::string(segment) == SEGMENT_NULL))
			return false;

		auto it = m_listSegment.begin();
		while (it != m_listSegment.end())
		{
			if ((*it).m_name == segment)
			{
				m_pCurrentSegment = nullptr;
				m_pCurrentKey = nullptr;
				
				m_listSegment.erase(it);
				return true;
			};

			++it;
		};

		return false;
	};


	bool KeyNew(const char* segment, const char* key)
	{
		if (KeySearch(segment, key))
			return false;

		m_pCurrentSegment->m_listKey.push_back({ key, "" });
		m_pCurrentKey = &m_pCurrentSegment->m_listKey.back();

		return true;
	};


	KEY* KeySearch(const char* segment, const char* key)
	{
		SEGMENT* pSegment = SegmentSearch(segment);
		if (!pSegment)
			return nullptr;			

		auto it = pSegment->m_listKey.begin();
		while (it != pSegment->m_listKey.end())
		{
			if ((*it).m_name == key)
			{
				m_pCurrentKey = &*it;
				return &*it;
			};

			++it;
		};

		return nullptr;
	};


	bool KeyDelete(const char* segment, const char* key)
	{
		if (!KeySearch(segment, key))
			return false;

		auto it = m_pCurrentSegment->m_listKey.begin();
		while (it != m_pCurrentSegment->m_listKey.end())
		{
			if ((*it).m_name == key)
			{
				m_pCurrentKey = nullptr;

				m_pCurrentSegment->m_listKey.erase(it);
				
				return true;
			};
			
			++it;
		};

		return false;
	};


	void KeyEnum(CIni::PFN_KEYENUMCALLBACK pfnEnumCB, void* param = nullptr)
	{
		for (auto& itA : m_listSegment)
		{
			for (auto& itB : itA.m_listKey)
			{
				if (pfnEnumCB(itB.m_name.c_str(), itB.m_value.c_str(), param))
					return;
			};
		};
	};


	std::string& KeyValue(const char* segment, const char* key)
	{
		return m_pCurrentKey->m_value;
	};


	void WriteSegments(std::string& buffer, bool bWriteSegmentName) const
	{
		for (auto& segment : m_listSegment)
		{
			if (bWriteSegmentName && segment.m_name != SEGMENT_NULL)
			{
				buffer += '[';
				buffer += segment.m_name;
				buffer += ']';
				buffer += '\n';
			};
			
			for (auto& key : segment.m_listKey)
			{
				buffer += key.m_name;
				buffer += " = ";
				buffer += key.m_value;
				buffer += '\n';
			};

			if (bWriteSegmentName)
				buffer += '\n';
		};
	};


private:
	std::list<SEGMENT> m_listSegment;
	SEGMENT* m_pCurrentSegment;
	KEY* m_pCurrentKey;
};


/*static*/ const char* CIniContainer::SEGMENT_NULL = "NULL";


class CIniReader
{
private:
	static const int32 WRITEBUF_MAX = 256;

	
	enum STATE
	{
		STATE_NONE,
		STATE_IDLE,
		STATE_KEY,
		STATE_VALUE,
		STATE_SEGMENT,
		STATE_COMMENT,

		STATE_MAX,
	};


public:
	CIniReader(CIniContainer& container)
	: m_container(container)
	, m_state(STATE_NONE)
	, m_nChrSkip(0)
	, m_bBegin(false)
	{
		m_CurrentSegment.reserve(256);
		m_buffer.reserve(256);
	};

	
	bool Import(const char* buffer, int32 bufferSize)
	{
		if (!buffer || !bufferSize)
			return false;

		onBegin();

		char* pBuffer = new char[bufferSize + 1];
		memcpy(pBuffer, buffer, bufferSize);

		if (bufferSize >= 1)
		{
			if (pBuffer[bufferSize - 1] != '\n')
				pBuffer[bufferSize] = '\n';			
		};

		for (int32 i = 0; i < bufferSize + 1; i++)
		{
			char ch = pBuffer[i];

			switch (currentState())
			{
			case STATE::STATE_IDLE:
				onIdle(ch);
				break;

			case STATE::STATE_KEY:
				onKey(ch);
				break;

			case STATE::STATE_VALUE:
				onValue(ch);
				break;

			case STATE::STATE_SEGMENT:
				onSegment(ch);
				break;

			case STATE::STATE_COMMENT:
				onComment(ch);
				break;
			};
		};

		if (pBuffer)
		{
			delete[] pBuffer;
			pBuffer = nullptr;
		};

		onEnd();

		return true;
	};

	
	bool Export(char* buffer, int32* bufferSize)
	{
		if (!buffer || !bufferSize)
			return false;

		std::string strBuffer;
		strBuffer.reserve(4096);

		m_container.WriteSegments(strBuffer, true);

		printf("%s\n", strBuffer.c_str());

		if (*bufferSize < int32(strBuffer.size()))
		{
			*bufferSize = strBuffer.size();
			return false;
		};

		*bufferSize = strBuffer.size();
		memcpy(&buffer[0], &strBuffer[0], strBuffer.size());

		return true;
	};

	
private:
	void onBegin(void)
	{
		changeState(STATE_IDLE);
	};


	void onIdle(char ch)
	{
		if (ch == '[')
		{
			clearWriteBuffer();
			changeState(STATE_SEGMENT);
		}
		else if (ch == ';')
		{
			changeState(STATE_COMMENT);
		}
		else if (isAlphabet(ch) || isDigit(ch))
		{
			clearWriteBuffer();
			changeState(STATE_KEY);
			writeChr(ch);
		};
	};


	void onSegment(char ch)
	{
		if (isAlphabet(ch) || isDigit(ch))
		{
			writeChr(ch);
		}
		else if (ch == ']')
		{
			writeChr('\0');

			m_container.SegmentNew(&m_buffer[0]);
			m_CurrentSegment = m_buffer;

			clearWriteBuffer();
			changeState(STATE_IDLE);
		};
	};


	void onKey(char ch)
	{
		if (ch != '=')
		{
			writeChr(ch);
		}
		else
		{
			m_buffer[m_buffer.size() - m_nChrSkip] = '\0';

			if (m_CurrentSegment.empty())
				m_container.KeyNew(nullptr, &m_buffer[0]);
			else				
				m_container.KeyNew(&m_CurrentSegment[0], &m_buffer[0]);

			m_CurrentKey = m_buffer;
			
			clearWriteBuffer();
			changeState(STATE_VALUE);
		};
	};



	void onValue(char ch)
	{
		if (ch != '\n' &&
			ch != '\r' &&
			ch != ';')
		{
			writeChr(ch);
		}
		else
		{
			m_buffer[m_buffer.size() - m_nChrSkip] = '\0';

			if(m_CurrentSegment.empty())
				m_container.KeyValue(nullptr, &m_CurrentKey[0]) = m_buffer;
			else
				m_container.KeyValue(&m_CurrentSegment[0], &m_CurrentKey[0]) = m_buffer;

			if (ch == ';')
				changeState(STATE_COMMENT);
			else
				changeState(STATE_IDLE);
		};
	};


	void onComment(char ch)
	{
		if (ch == '\n')
			changeState(STATE_IDLE);
	};


	void onEnd(void)
	{
		;
	};


	void changeState(STATE state)
	{
		m_state = state;
	};


	STATE currentState(void) const
	{
		return m_state;
	};


	void clearWriteBuffer(void)
	{
		m_buffer.clear();		
		m_bBegin = false;
		m_nChrSkip = 0;
	};


	void writeChr(char ch)
	{		
		switch (ch)
		{
		case ' ':
		case '\t':
			if (m_bBegin)
			{
				m_buffer += ch;
				++m_nChrSkip;
			};
			break;

		default:
			m_buffer += ch;
			if (!m_bBegin)
				m_bBegin = true;
			else
				m_nChrSkip = 0;
			break;
		};
	};


	inline bool isAlphabet(char ch) const
	{
		return	(ch >= 'a' && ch <= 'z') ||
				(ch >= 'A' && ch <= 'Z');
	};


	inline bool isDigit(char ch) const
	{
		return	(ch >= '0' && ch <= '9');
	};

	
private:
	CIniContainer& m_container;
	STATE m_state;
	std::string m_CurrentSegment;
	std::string m_CurrentKey;
	std::string m_buffer;
	int32 m_nChrSkip;
	bool m_bBegin;
};


static std::string ValueToString(const char* pszFormat, ...)
{
	char szBuff[1024];	
	szBuff[0] = '\0';
	
	va_list vl;
	va_start(vl, pszFormat);

	if (!strcmp(pszFormat, "%b"))
	{
		bool value = va_arg(vl, bool);
		strcpy(szBuff, value ? "true" : "false");
	}
	else
	{
		vsprintf(szBuff, pszFormat, vl);
	};

	va_end(vl);

	return std::string(szBuff);
};


static void StringToValue(const std::string& str, const char* pszFormat, ...)
{
	va_list vl;
	va_start(vl, pszFormat);

	if (!strcmp(pszFormat, "%b"))
	{
		bool* value = va_arg(vl, bool*);
		bool bResult = false;
		if (!std::strcmp(&str[0], "true"))
			bResult = true;
		*value = bResult;
	}
	else
	{
		vsscanf(&str[0], pszFormat, vl);
	};

	va_end(vl);
};


static CIniContainer& IniContainer(void* hIni)
{
	assert(hIni);
	return *(CIniContainer*)hIni;
};


/*static*/ void* CIni::New(void)
{
	return new CIniContainer();
};


/*static*/ void* CIni::Open(const char* buffer, int32 bufferSize)
{
	CIniContainer* pIni = new CIniContainer();
	if (pIni)
	{
		CIniReader IniReader(*pIni);
		if (!IniReader.Import(buffer, bufferSize))
		{
			delete pIni;
			pIni = nullptr;
		};
	};

	return pIni;
};


/*static*/ bool CIni::Save(void* hIni, char* buffer, int32* bufferSize)
{
	assert(hIni);
	
	CIniReader IniReader(*((CIniContainer*)hIni));
	return IniReader.Export(buffer, bufferSize);	
};


/*static*/ void CIni::Close(void* hIni)
{
	assert(hIni);

	CIniContainer* pIni = (CIniContainer*)hIni;
	delete pIni;	
};


/*static*/ bool CIni::SegmentNew(void* hIni, const char* segment)
{
	return IniContainer(hIni).SegmentNew(segment);
};


/*static*/ bool CIni::SegmentSearch(void* hIni, const char* segment)
{
	return (IniContainer(hIni).SegmentSearch(segment) != nullptr);
};


/*static*/ bool CIni::SegmentDelete(void* hIni, const char* segment)
{
	return IniContainer(hIni).SegmentDelete(segment);
};


/*static*/ bool CIni::KeyNew(void* hIni, const char* segment, const char* key)
{
	return IniContainer(hIni).KeyNew(segment, key);
};


/*static*/ bool CIni::KeySearch(void* hIni, const char* segment, const char* key)
{
	return (IniContainer(hIni).KeySearch(segment, key) != nullptr);
};


/*static*/ bool CIni::KeyDelete(void* hIni, const char* segment, const char* key)
{
	return IniContainer(hIni).KeyDelete(segment, key);
};


/*static*/ void CIni::KeyEnum(void* hIni, PFN_KEYENUMCALLBACK pfnEnumCB, void* param)
{
	IniContainer(hIni).KeyEnum(pfnEnumCB, param);
};


/*static*/ bool CIni::KeyWriteInt(void* hIni, const char* segment, const char* key, int32 value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;	

	IniContainer(hIni).KeyValue(segment, key) = ValueToString("%d", value);
	return true;
};


/*static*/ bool CIni::KeyWriteUInt(void* hIni, const char* segment, const char* key, uint32 value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	IniContainer(hIni).KeyValue(segment, key) = ValueToString("%u", value);
	return true;
};


/*static*/ bool CIni::KeyWriteHex(void* hIni, const char* segment, const char* key, uint32 value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	IniContainer(hIni).KeyValue(segment, key) = ValueToString("0x%08x", value);
	return true;
};


/*static*/ bool CIni::KeyWriteFloat(void* hIni, const char* segment, const char* key, float value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	IniContainer(hIni).KeyValue(segment, key) = ValueToString("%f", value);
	return true;
};


/*static*/ bool CIni::KeyWriteDouble(void* hIni, const char* segment, const char* key, double value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	IniContainer(hIni).KeyValue(segment, key) = ValueToString("%lf", value);
	return true;
};


/*static*/ bool CIni::KeyWriteBool(void* hIni, const char* segment, const char* key, bool value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	IniContainer(hIni).KeyValue(segment, key) = ValueToString("%b", value);
	return true;
};


/*static*/ bool CIni::KeyWriteString(void* hIni, const char* segment, const char* key, const char* buffer, int32 bufferSize)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	IniContainer(hIni).KeyValue(segment, key) = std::string(buffer, buffer + bufferSize);
	return true;
};


/*static*/ bool CIni::KeyWriteString(void* hIni, const char* segment, const char* key, const char* buffer)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	IniContainer(hIni).KeyValue(segment, key) = std::string(buffer);
	return true;
};


/*static*/ bool CIni::KeyReadInt(void* hIni, const char* segment, const char* key, int32* value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	StringToValue(IniContainer(hIni).KeyValue(segment, key), "%d", value);
	return true;
};


/*static*/ bool CIni::KeyReadUInt(void* hIni, const char* segment, const char* key, uint32* value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	StringToValue(IniContainer(hIni).KeyValue(segment, key), "%u", value);
	return true;
};


/*static*/ bool CIni::KeyReadHex(void* hIni, const char* segment, const char* key, uint32* value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	StringToValue(IniContainer(hIni).KeyValue(segment, key), "0x%x", value);
	return true;
};


/*static*/ bool CIni::KeyReadFloat(void* hIni, const char* segment, const char* key, float* value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	StringToValue(IniContainer(hIni).KeyValue(segment, key), "%f", value);
	return true;
};


/*static*/ bool CIni::KeyReadDouble(void* hIni, const char* segment, const char* key, double* value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	StringToValue(IniContainer(hIni).KeyValue(segment, key), "%lf", value);
	return true;
};


/*static*/ bool CIni::KeyReadBool(void* hIni, const char* segment, const char* key, bool* value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	StringToValue(IniContainer(hIni).KeyValue(segment, key), "%b", value);
	return true;
};


/*static*/ bool CIni::KeyReadString(void* hIni, const char* segment, const char* key, char* buffer, int32 bufferSize)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	std::string& KeyValue = IniContainer(hIni).KeyValue(segment, key);
	assert(int32(KeyValue.size()) < bufferSize);
	strncpy(buffer, &KeyValue[0], KeyValue.size());

	return true;
};


/*static*/ bool CIni::KeyReadString(void* hIni, const char* segment, const char* key, const char** value)
{
	if (!IniContainer(hIni).KeySearch(segment, key))
		return false;

	*value = &IniContainer(hIni).KeyValue(segment, key)[0];
	return true;
};