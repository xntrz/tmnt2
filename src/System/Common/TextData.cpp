#include "TextData.hpp"


struct CTextData::HEADER
{
    uint32  m_uSize;
    char    m_szLanguage[8];
    int32   m_numStrings;
    uint32  m_aOffsets[];
};


/*static*/ const wchar* CTextData::EMPTYTEXT = UTEXT("");
/*static*/ wchar CTextData::m_aFixText[10];


/*static*/ const wchar*
CTextData::tostr(int32 value)
{
    static wchar s_wszWord[16];
    s_wszWord[0] = 0;

    char szBuff[32];
    int32 len = std::sprintf(szBuff, "%d", value);
    ASSERT(len < 16);

    int32 i;
    for (i = 0; i < len; ++i)
        s_wszWord[i] = szBuff[i];
    
    s_wszWord[i] = 0;

    return s_wszWord;
};


/*static*/ int32
CTextData::strncmp(const wchar* pString1,
                   const wchar* pString2,
                   int32 maxCount)
{
    ASSERT(pString1 != nullptr);
    ASSERT(pString2 != nullptr);

    int32 count = 0;

    wchar wch1 = *pString1;
    wchar wch2 = *pString2;

    if (wch1 != wch2)
        return wch1 - wch2;

    while (wch1 || wch2)
    {
        ++pString1;
        ++pString2;

        if (++count >= maxCount)
            break;

        wch1 = *pString1;
        wch2 = *pString2;

        if (wch1 != wch2)
            return wch1 - wch2;
    };

    return 0;
};


/*static*/ int32
CTextData::strnlen(const wchar* pString, int32 maxCount)
{
    ASSERT(pString != nullptr);

    int32 len = 0;
    int32 count = 0;

    while (*pString)
    {
        ++pString;
        ++len;

        if (++count >= maxCount)
            return len;
    };

    return len;
};


/*static*/ int32
CTextData::strncpy(wchar* pDestString, const wchar* pSrcString, int32 maxCount)
{
    ASSERT(pDestString != nullptr);
    ASSERT(pSrcString != nullptr);

    int32 srcLen = strnlen(pSrcString, MAXCOUNT);
    if (srcLen <= maxCount)
    {
        if (srcLen > 0)
            std::memcpy(pDestString, pSrcString, sizeof(wchar) * srcLen);

        pDestString[srcLen] = 0;
    }
    else
    {
        std::memcpy(pDestString, pSrcString, sizeof(wchar) * maxCount);
    };

    return srcLen;
};


/*static*/ wchar*
CTextData::strncat(wchar* pDestString, const wchar* pSrcString, int32 maxCount)
{
    int32 dstLen = strnlen(pDestString, MAXCOUNT);
    strncpy(&pDestString[dstLen], pSrcString, MAXCOUNT);

    return pDestString;
};


/*static*/ int32 CTextData::StrCmp(const wchar* pString1, const wchar* pString2)
{
#if defined(TARGET_PC)
    return std::wcscmp(pString1, pString2);
#elif defined(TARGET_WEB)
    return strncmp(pString1, pString2, MAXCOUNT);
#else
#error Not implemented for current target
#endif
};


/*static*/ int32 CTextData::StrLen(const wchar* pString)
{
#if defined(TARGET_PC)
    return static_cast<int32>(std::wcslen(pString));
#elif defined(TARGET_WEB)
    return strnlen(pString, MAXCOUNT);
#else
#error Not implemented for current target
#endif
};


/*static*/ wchar* CTextData::StrCpy(wchar* pDestString, const wchar* pSrcString)
{
#if defined(TARGET_PC)
    return std::wcscpy(pDestString, pSrcString);
#elif defined(TARGET_WEB)
    strncpy(pDestString, pSrcString, MAXCOUNT);
    return pDestString;
#else
#error Not implemented for current target
#endif
};


/*static*/ wchar* CTextData::StrCat(wchar* pDestString, const wchar* pSrcString)
{
#if defined(TARGET_PC)
    return std::wcscat(pDestString, pSrcString);
#elif defined(TARGET_WEB)
    return strncat(pDestString, pSrcString, MAXCOUNT);
#else
#error Not implemented for current target
#endif
};


/*static*/ int32 CTextData::Sprintf(wchar* pBuffer, const wchar* pFormat, ...)
{
#if defined(TARGET_PC)
    va_list vl;
    va_start(vl, pFormat);
    int32 ret = std::vswprintf(pBuffer, static_cast<size_t>(MAXCOUNT), pFormat, vl);
    va_end(vl);

    return ret;
#elif defined(TARGET_WEB)
    ASSERT(pBuffer != nullptr);
    ASSERT(pFormat != nullptr);

    int32 nRemainLen = MAXCOUNT;

    va_list vl;
    va_start(vl, pFormat);

    while (*pFormat)
    {
        wchar wch = *pFormat++;
        if (wch == UCHAR('%'))
        {
            const wchar* arg = nullptr;

            wch = *pFormat++;
            switch (wch)
            {
            case UCHAR('s'):
                {
                    arg = va_arg(vl, const wchar*);
                }
                break;

            case UCHAR('d'):
                {
                    int32 value = va_arg(vl, int32);
                    arg = tostr(value);            
                }
                break;

            case UCHAR('0'):
            case UCHAR('1'):
            case UCHAR('2'):
            case UCHAR('3'):
            case UCHAR('4'):
            case UCHAR('5'):
            case UCHAR('6'):
            case UCHAR('7'):
            case UCHAR('8'):
            case UCHAR('9'):
                {
                    arg = &m_aFixText[wch - UCHAR('0')];
                }
                break;

            case UCHAR('c'):
                {
                    int32 value = va_arg(vl, int32);
                    *pBuffer = static_cast<wchar>(value);

                    --nRemainLen;
                    ++pBuffer;
                }
                break;

            case UCHAR('%'):
                {
                    *pBuffer = wch;

                    --nRemainLen;
                    ++pBuffer;
                }
                break;

            default:
                break;
            };

            if (arg != nullptr)
            {
                int32 len = strncpy(pBuffer, arg, nRemainLen);
                nRemainLen -= len;
                pBuffer += len;
            };
        }
        else
        {
            *pBuffer = wch;
            
            --nRemainLen;
            ++pBuffer;
        };

        ASSERT(nRemainLen > 0);
    };

    *pBuffer = 0;

    return (MAXCOUNT - nRemainLen);
#else
#error Not implemented for current target
#endif
};


/*static*/ void CTextData::ToMultibyte(char* dst, size_t dstMaxCount, const wchar* src)
{
#if defined(TARGET_PC)
    std::wcstombs(dst, src, dstMaxCount);
#elif defined(TARGET_WEB)
    size_t count = 0;
    while (*src)
    {
        *dst = static_cast<char>(*src);

        if (++count >= dstMaxCount)
            break;

        ++dst;
        ++src;
    };

    *dst = 0;
#else
#error Not implemented for current target
#endif
};


CTextData::CTextData(void)
: m_pRaw(nullptr)
, m_pHeader(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_aFixText); ++i)
        m_aFixText[i] = UCHAR(' ');
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

    std::memcpy(m_pRaw, pBuffer, uBufferSize);
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