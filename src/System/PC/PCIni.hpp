#pragma once


class CIni
{
public:
	typedef bool(*PFN_KEYENUMCALLBACK)(const char* key, const char* value, void* param);

public:
	static void* New(void);
	static void* Open(const char* buffer, int32 bufferSize);
	static bool Save(void* hIni, char* buffer, int32* bufferSize);
	static void Close(void* hIni);
	static bool SegmentNew(void* hIni, const char* segment);
	static bool SegmentSearch(void* hIni, const char* segment);
	static bool SegmentDelete(void* hIni, const char* segment);
	static bool KeyNew(void* hIni, const char* segment, const char* key);
	static bool KeySearch(void* hIni, const char* segment, const char* key);
	static bool KeyDelete(void* hIni, const char* segment, const char* key);
	static void KeyEnum(void* hIni, PFN_KEYENUMCALLBACK pfnEnumCB, void* param = nullptr);
	static bool KeyWriteInt(void* hIni, const char* segment, const char* key, int32 value);
	static bool KeyWriteUInt(void* hIni, const char* segment, const char* key, uint32 value);
	static bool KeyWriteHex(void* hIni, const char* segment, const char* key, uint32 value);
	static bool KeyWriteFloat(void* hIni, const char* segment, const char* key, float value);
	static bool KeyWriteDouble(void* hIni, const char* segment, const char* key, double value);
	static bool KeyWriteBool(void* hIni, const char* segment, const char* key, bool value);
	static bool KeyWriteString(void* hIni, const char* segment, const char* key, const char* buffer, int32 sizeBuffer);
	static bool KeyWriteString(void* hIni, const char* segment, const char* key, const char* value);
	static bool KeyReadInt(void* hIni, const char* segment, const char* key, int32* value);
	static bool KeyReadUInt(void* hIni, const char* segment, const char* key, uint32* value);
	static bool KeyReadHex(void* hIni, const char* segment, const char* key, uint32* value);
	static bool KeyReadFloat(void* hIni, const char* segment, const char* key, float* value);
	static bool KeyReadDouble(void* hIni, const char* segment, const char* key, double* value);
	static bool KeyReadBool(void* hIni, const char* segment, const char* key, bool* value);
	static bool KeyReadString(void* hIni, const char* segment, const char* key, char* buffer, int32 sizeBuffer);
	static bool KeyReadString(void* hIni, const char* segment, const char* key, const char** value);
};