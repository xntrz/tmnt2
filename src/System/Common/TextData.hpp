#pragma once


class CTextData
{
public:
    struct HEADER;
    
    static const wchar* EMPTYTEXT;
    static const int32 MAXCOUNT = 1024;

public:
    static const wchar* tostr(int32 value);
    static int32 strncmp(const wchar* pString1, const wchar* pString2, int32 maxCount);
    static int32 strnlen(const wchar* pString, int32 maxCount);
    static int32 strncpy(wchar* pDestString, const wchar* pSrcString, int32 maxCount);
    static wchar* strncat(wchar* pDestString, const wchar* pSrcString, int32 maxCount);
    static int32 StrCmp(const wchar* pString1, const wchar* pString2);
    static int32 StrLen(const wchar* pString);
    static wchar* StrCpy(wchar* pDestString, const wchar* pSrcString);
    static wchar* StrCat(wchar* pDestString, const wchar* pSrcString);
    static int32 Sprintf(wchar* pDest, const wchar* pFormat, ...);
    static void ToMultibyte(char* dst, size_t dstMaxCount, const wchar* src);

    CTextData(void);
    ~CTextData(void);
    void Read(const void* pBuffer, uint32 uBufferSize);
    void Clear(void);
    int32 GetTextNum(void) const;
    const wchar* GetText(int32 id) const;

private:
    static wchar m_aFixText[];
    uint8*       m_pRaw;
    HEADER*      m_pHeader;
};