#include "ListFile.hpp"


/*static*/ CListFile::CParam& CListFile::CParam::Dummy(void)
{
    static CParam s_dummy;
    return s_dummy;
};


CListFile::CParam::CParam(void)
: m_type(TYPE_INT)
, m_value()
{
    m_value.string[0] = '\0';
};


CListFile::CParam::~CParam(void)
{
    ;
};


void CListFile::CParam::Parse(const char* pszParam, uint32 paramLen)
{
    char szParam[sizeof(m_value.string) * 2];
    std::strcpy(szParam, pszParam);

    removeSpaces(szParam, std::strlen(szParam));
    ASSERT(std::strlen(szParam) < sizeof(m_value.string));

    std::strcpy(m_value.string, szParam);

    if (isNumber(m_value.string))
    {
        m_type = TYPE_INT;
        m_value.integer = std::atoi(m_value.string);
    }
    else
    {
        m_type = TYPE_STR;
    };
};


CListFile::CParam::TYPE CListFile::CParam::GetType(void) const
{
    return m_type;
};


int32 CListFile::CParam::AsInt(void) const
{
    return m_value.integer;
};


const char* CListFile::CParam::AsStr(void) const
{
    return m_value.string;
};


bool CListFile::CParam::isNumber(const char* pszStr) const
{
    int32 num = -1;
    char extra = '\0';

    return (std::sscanf(pszStr, "%d%c", &num, &extra) == 1);
};


void CListFile::CParam::removeSpaces(char* pszStr, uint32 strLen) const
{
    char* ptr = pszStr;

    for (uint32 i = 0; i < strLen; ++i)
    {
        if (pszStr[i] != ' ')
            *ptr++ = pszStr[i];
    };

    *ptr = '\0';
};


//
// *********************************************************************************
//


/*static*/ CListFile::CPath& CListFile::CPath::Dummy(void)
{
    static CPath s_dummy;
    return s_dummy;
};


CListFile::CPath::CPath(void)
: m_szPath()
, m_szFExt()
, m_szFName()
, m_aParam()
, m_nParamNum(0)
{
    m_szPath[0] = '\0';
    m_szFExt[0] = '\0';
    m_szFName[0] = '\0';
};


CListFile::CPath::~CPath(void)
{
    ;
};


void CListFile::CPath::Parse(const char* pszPath, uint32 pathLen)
{
    m_nParamNum = 0;

    char szPath[PATH_MAX_LEN];
    std::strcpy(szPath, pszPath);

    char* params = std::strchr(szPath, ':');
    if (params)
    {
        *params++ = '\0';

        char* param = params;
        char* ptr = param;
        char* end = (szPath + pathLen);

        while (ptr < end)
        {
            ptr = std::strchr(ptr, ',');
            ptr = (ptr ? ptr : end);

            *ptr++ = '\0';

            ASSERT(m_nParamNum < COUNT_OF(m_aParam));
            m_aParam[m_nParamNum++].Parse(param, std::strlen(param));

            param = ptr;
        };
    };

    std::strcpy(m_szPath, szPath);

    char* fext = std::strrchr(szPath, '.');
    std::strcpy(m_szFExt, (fext ? fext : ""));

    if (fext)
        *fext = '\0';

    const char* fname = std::strrchr(szPath, '/');
    if (!fname)
        fname = std::strrchr(szPath, '\\');
    fname = fname ? (fname + 1) : szPath;

    std::strcpy(m_szFName, fname);
};


bool CListFile::CPath::IsValid(void) const
{
    if (m_szFExt[0] == '\0')
        return false;

    if (m_szPath[0] == '\0')
        return false;

    return true;
};


bool CListFile::CPath::IsList(void) const
{
    return (!std::strcmp(m_szFExt, ".list") ? true : false);
};


int32 CListFile::CPath::GetParamNum(void) const
{
    return m_nParamNum;
};


const CListFile::CParam& CListFile::CPath::GetParam(int32 no) const
{
    if ((no >= 0) && (no < m_nParamNum))
        return m_aParam[no];

    return CParam::Dummy();
};


const char* CListFile::CPath::GetStr(void) const
{
    return m_szPath;
};


const char* CListFile::CPath::GetFileExt(void) const
{
    return m_szFExt;
};


const char* CListFile::CPath::GetFileName(void) const
{
    return m_szFName;
};


//
// *********************************************************************************
//


CListFile::CListFile(void)
: m_aPaths(nullptr)
, m_nPathNum(0)
{
    ;
};


CListFile::~CListFile(void)
{
    if (m_aPaths)
    {
        delete[] m_aPaths;
        m_aPaths = nullptr;
        m_nPathNum = 0;
    };
};


void CListFile::Parse(const char* pBuff, uint32 uBuffSize)
{
    uint32 len = 0;
    char* buf = createTmpBuf(pBuff, uBuffSize, &len);
    if (buf)
    {
        int32 lines = parseBuf(buf, len, true);
        m_aPaths = new CPath[lines];
        m_nPathNum = lines;

        parseBuf(buf, len, false);
        destroyTmpBuf(buf);
    };
};


int32 CListFile::GetPathNum(void) const
{
    return m_nPathNum;
};


const CListFile::CPath& CListFile::SearchPath(const char* pszPath) const
{
    for (int32 i = 0; i < m_nPathNum; ++i)
    {
        if (!std::strcmp(m_aPaths[i].GetStr(), pszPath))
            return m_aPaths[i];
    };

    return CPath::Dummy();
};


const CListFile::CPath& CListFile::GetPath(int32 no) const
{
    if ((no >= 0) && (no < m_nPathNum))
        return m_aPaths[no];

    return CPath::Dummy();
};


char* CListFile::createTmpBuf(const char* pBuff, uint32 uBuffSize, uint32* len)
{
    char* tmp = new char[uBuffSize + 1];
    char* dst = tmp;
    uint32 size = 0;

    // '\r\n' to '\n'
    for (uint32 i = 0; i < uBuffSize; ++i)
    {
        if (pBuff[i] != '\r')
        {
            *dst++ = pBuff[i];
            ++size;
        };
    };

    *dst = '\0';
    *len = size;

    return tmp;
};


void CListFile::destroyTmpBuf(char* tmp)
{
    delete[] tmp;
};


int32 CListFile::parseBuf(const char* buf, uint32 len, bool bCountLines)
{
    int32 lineNum = 0;
    char szPath[PATH_MAX_LEN * 2]; // x2 space for params

    char szFormat[32];
    std::snprintf(szFormat, sizeof(szFormat), "%%%zu[^\n]%%n", sizeof(szPath) - 1);

    const char* ptr = buf;
    const char* end = buf + len;
    while (ptr < end)
    {
        int32 cbWritten = 0;
        int32 ret = std::sscanf(ptr, szFormat, szPath, &cbWritten);

        if (ret == 1)
        {
            if (!bCountLines)
                m_aPaths[lineNum].Parse(szPath, cbWritten);

            ++lineNum;
        };

        ptr += (cbWritten + 1);
    };

    return lineNum;
};