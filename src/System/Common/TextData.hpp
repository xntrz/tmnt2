#pragma once


class CTextData
{
private:
    struct HEADER;

public:    
    static const wchar* EMPTYTEXT;
    
public:    
    static int32 StrLen(const wchar* pString);
    static void StrCpy(wchar* pDestString, const wchar* pSrcString);
    static void StrCat(wchar* pDestString, const wchar* pSrcString);
    static void Sprintf(wchar* pDest, const wchar* pFormat, ...);
    static void ToMultibyte(char* dst, int32 dstlen, const wchar* src);
    
    CTextData(void);
    ~CTextData(void);
    void Read(const void* pBuffer, uint32 uBufferSize);
    void Clear(void);
    int32 GetTextNum(void) const;
    const wchar* GetText(int32 id) const;

private:
    void* m_pRaw;
    HEADER* m_pHeader;
};