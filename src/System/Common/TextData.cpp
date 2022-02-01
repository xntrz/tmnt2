#include "TextData.hpp"


/*static*/ const wchar CTextData::EMPTYTEXT = 0;


struct CTextData::HEADER
{
    uint32 m_uSize;
    char m_szLanguage[8];
    int32 m_numStrings;
    uint32 m_aOffsets[];
};


CTextData::CTextData(void)
: m_pRaw(nullptr)
, m_pHeader(nullptr)
{
    ;
};


CTextData::~CTextData(void)
{
    Clear();
};


void CTextData::Read(const void* pBuffer, uint32 uBufferSize)
{
    CHECK_SIZE(HEADER, 0x10);

    ASSERT(pBuffer);
    ASSERT(uBufferSize);

    m_pRaw = new uint8[uBufferSize];
    memcpy(m_pRaw, pBuffer, uBufferSize);
    m_pHeader = (HEADER*)m_pRaw;
    
    ASSERT(m_pHeader->m_uSize);
    ASSERT(m_pHeader->m_uSize <= uBufferSize);
};


void CTextData::Clear(void)
{
    if (m_pRaw)
    {
        delete[] m_pRaw;
        m_pRaw = nullptr;
    };
};


int32 CTextData::GetTextNum(void) const
{
    return m_pHeader->m_numStrings;
};


const wchar* CTextData::GetText(int32 id) const
{
    const wchar* pwszResult = &EMPTYTEXT;

    if (id >= 0 && id <= m_pHeader->m_numStrings)
        pwszResult = (const wchar*)((uint8*)m_pRaw + m_pHeader->m_aOffsets[id]);

    return pwszResult;
};


/*static*/ int32 CTextData::StrLen(const wchar* pString)
{
#ifdef _TARGET_PC
    return std::wcslen(pString);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::StrCpy(wchar* pDestString, const wchar* pSrcString)
{
#ifdef _TARGET_PC
    std::wcscpy(pDestString, pSrcString);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::StrCat(wchar* pDestString, const wchar* pSrcString)
{
#ifdef _TARGET_PC
    std::wcscat(pDestString, pSrcString);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::Sprintf(wchar* pDest, const wchar* pFormat, ...)
{
#ifdef _TARGET_PC
    va_list vl;
    va_start(vl, pFormat);
    vswprintf(pDest, pFormat, vl);
    va_end(vl);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::ToMultibyte(char* dst, int32 dstlen, const wchar* src)
{
#ifdef _TARGET_PC
    std::wstring wbuffer(src);
    std::string buffer(wbuffer.begin(), wbuffer.end());
	ASSERT(dstlen >= int32(buffer.length() + 1), "src: %d, dst: %d", buffer.length() + 1, dstlen);
    int32 nNumCopied = buffer.copy(dst, dstlen);
    dst[nNumCopied] = '\0';
#else
#error Not implemented for current target
#endif
};