#include "PCIniVar.hpp"
#include "PCIni.hpp"


CIniVar::exception::exception(const std::string msg)
: m_msg(msg)
{
    ;
};


CIniVar::exception::~exception(void)
{
    ;
};


char const* CIniVar::exception::what(void) const
{
    return m_msg.c_str();
};


CIniVar::key::key(CIniVar& ini)
: m_ini(ini)
{
    ;
};


CIniVar::key::~key(void)
{
    ;
};


void CIniVar::key::remove(void)
{
    if (m_ini.check_access(access_write))
    {
        if (!CIni::KeyDelete(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey))
            write_error("delete key");
    }
    else
    {
        write_error("no access for deleting");
    };
};


void CIniVar::key::operator=(bool value)
{
    if (!CIni::KeyWriteBool(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, value))
        write_error("bool");
};


void CIniVar::key::operator=(float value)
{
    if (!CIni::KeyWriteFloat(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, value))
        write_error("float");
};


void CIniVar::key::operator=(int32 value)
{
    if (!CIni::KeyWriteInt(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, value))
        write_error("int");
};


void CIniVar::key::operator=(uint32 value)
{
    if (!CIni::KeyWriteUInt(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, value))
        write_error("uint");
};


void CIniVar::key::operator=(string_buffer strbuff)
{
    if (!CIni::KeyWriteString(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, strbuff.m_pBuffer, strbuff.m_uBufferSize))
        write_error("str_size");
};


void CIniVar::key::operator=(const char* value)
{
    if (!CIni::KeyWriteString(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, value))
        write_error("str_raw");
};


CIniVar::key::operator bool(void) const
{
    bool value = false;
    if (!CIni::KeyReadBool(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, &value))
        read_error("bool");
    return value;
};


CIniVar::key::operator float(void) const
{
    float value = 0.0f;
    if (!CIni::KeyReadFloat(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, &value))
        read_error("float");
    return value;
};


CIniVar::key::operator int32(void) const
{
    int32 value = 0;
    if (!CIni::KeyReadInt(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, &value))
        read_error("int");
    return value;
};


CIniVar::key::operator uint32(void) const
{
    uint32 value = 0;
    if (!CIni::KeyReadUInt(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, &value))
        read_error("uint");
    return value;
};


CIniVar::key::operator const char*(void) const
{
    const char* ptr = nullptr;
    if (!CIni::KeyReadString(m_ini.m_handle, m_ini.m_pszCurSegment, m_ini.m_pszCurKey, &ptr))
        read_error("str");
    return ptr;
};


void CIniVar::key::write_error(const char* msg) const
{
    throw exception(
        std::string("error while writing key \'")
        + m_ini.m_pszCurKey
        + "\' in segment \'"
        + m_ini.m_pszCurSegment
        + "\', msg: "
        + msg
    );
};


void CIniVar::key::read_error(const char* msg) const
{
    throw exception(
        std::string("error while reading key \'")
        + m_ini.m_pszCurKey
        + "\' in segment \'"
        + m_ini.m_pszCurSegment
        + "\', msg: "
        + msg
    );
};


CIniVar::segment::segment(CIniVar& ini)
: m_ini(ini)
{
    ;
};


CIniVar::segment::~segment(void) 
{
    ;
};


void CIniVar::segment::remove(void)
{
    if (m_ini.check_access(access_write))
    {
        if (!CIni::SegmentDelete(m_ini.m_handle, m_ini.m_pszCurSegment))
            throw exception(std::string("failed to delete segment ") + m_ini.m_pszCurSegment);
    }
	else
	{
		throw exception(std::string("no access for deleting segment ") + m_ini.m_pszCurSegment);
	};
};


CIniVar::key CIniVar::segment::operator[](const char* name)
{
    if (!CIni::KeySearch(m_ini.m_handle, m_ini.m_pszCurSegment, name))
    {
        if (m_ini.check_access(access_write))
        {
            if (!CIni::KeyNew(m_ini.m_handle, m_ini.m_pszCurSegment, name))
                throw exception(std::string("failed to create key: ") + name);
        }
        else
        {
            throw exception(std::string("key not found: ") + name);
        };
    };

    m_ini.m_pszCurKey = name;

    return key(m_ini);
};


bool CIniVar::segment::exist(const char* name)
{
    return CIni::KeySearch(m_ini.m_handle, m_ini.m_pszCurSegment, name);
};


CIniVar::CIniVar(uint32 access)
: m_handle(CIni::New())
, m_access(access)
{
    if (!m_handle)
        throw exception(std::string("failed to create new ini"));
};


CIniVar::CIniVar(const char* buffer, int32 bufferSize, uint32 access)
: m_handle(CIni::Open(buffer, bufferSize))
, m_access(access)
, m_pszCurKey(nullptr)
, m_pszCurSegment(nullptr)
{
    if (!m_handle)
        throw exception(std::string("failed to open ini"));
};


CIniVar::~CIniVar(void) 
{
    if (m_handle)
    {
        CIni::Close(m_handle);
        m_handle = nullptr;
    };
};


std::string CIniVar::save(void) 
{
    int32 bufferSize = 1024;
    std::string buffer;
    buffer.resize(bufferSize);
    
    if (!CIni::Save(m_handle, &buffer[0], &bufferSize))
    {
        buffer.resize(bufferSize);
        if (!CIni::Save(m_handle, &buffer[0], &bufferSize))
            throw exception("failed to save ini file");
    };

    return buffer;
};


bool CIniVar::exist(const char* name)
{
    return CIni::SegmentSearch(m_handle, name);
};


CIniVar::segment CIniVar::operator[](const char* name)
{    
	m_pszCurKey = m_pszCurSegment = nullptr;

    if (!CIni::SegmentSearch(m_handle, name))
    {
        if (check_access(access_write))
            CIni::SegmentNew(m_handle, name);
        else
            throw exception(std::string("segment not found: ") + name);
    };

    m_pszCurSegment = name;

    return segment(*this);
};


bool CIniVar::check_access(uint32 access) const
{
    return IS_FLAG_SET(m_access, access);
};