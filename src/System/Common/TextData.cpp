#include "TextData.hpp"


struct CTextData::HEADER
{
    uint32  m_uSize;
    char    m_szLanguage[8];
    int32   m_numStrings;
    uint32  m_aOffsets[];
};


/*static*/ const wchar* CTextData::EMPTYTEXT = UTEXT("");


/*static*/ int32 CTextData::StrCmp(const wchar* pString1, const wchar* pString2)
{
#ifdef TARGET_PC
    return std::wcscmp(pString1, pString2);
#else
#error Not implemented for current target
#endif
};


/*static*/ int32 CTextData::StrLen(const wchar* pString)
{
#ifdef TARGET_PC
    return std::wcslen(pString);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::StrCpy(wchar* pDestString, const wchar* pSrcString)
{
#ifdef TARGET_PC
    std::wcscpy(pDestString, pSrcString);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::StrCat(wchar* pDestString, const wchar* pSrcString)
{
#ifdef TARGET_PC
    std::wcscat(pDestString, pSrcString);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::Sprintf(wchar* pDest, const wchar* pFormat, ...)
{
#ifdef TARGET_PC
    va_list vl;
    va_start(vl, pFormat);
    std::vswprintf(pDest, pFormat, vl);
    va_end(vl);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::ToMultibyte(char* dst, size_t dstlen, const wchar* src)
{
#ifdef TARGET_PC
    std::wcstombs(dst, src, dstlen);
#else
#error Not implemented for current target
#endif
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
    m_pHeader = reinterpret_cast<HEADER*>(m_pRaw);
    
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
    const wchar* pwszResult = EMPTYTEXT;

    if ((id >= 0) && (id <= m_pHeader->m_numStrings))
        pwszResult = reinterpret_cast<const wchar*>(m_pRaw + m_pHeader->m_aOffsets[id]);

    return pwszResult;
};