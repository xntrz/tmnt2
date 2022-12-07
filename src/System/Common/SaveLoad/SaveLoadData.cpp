#include "SaveLoadData.hpp"


CSaveLoadDataBase::CSaveLoadDataBase(void)
: m_pData(nullptr)
, m_uSize(0)
, m_iVersion(-1)
{
    ;
};


CSaveLoadDataBase::~CSaveLoadDataBase(void)
{
    ASSERT(!m_uSize);
    ASSERT(!m_pData);
};


void CSaveLoadDataBase::Initialize(const void* pData, uint32 uDataSize)
{
    ASSERT(pData);
    ASSERT(uDataSize);

    if (uDataSize)
    {
        m_uSize = uDataSize;
        m_pData = new char[m_uSize];
        ASSERT(m_pData);

        std::memcpy(m_pData, pData, m_uSize);
    };    
};


void CSaveLoadDataBase::Initialize(void)
{    
    m_pData = nullptr;
    m_uSize = 0;
    m_iVersion = -1;
};


void CSaveLoadDataBase::Terminate(void)
{
    if (m_pData)
    {
        delete[] m_pData;
        m_pData = nullptr;
        m_uSize = 0;
    };
    
    m_iVersion = -1;
};


void CSaveLoadDataBase::Update(void)
{
    ;
};


bool CSaveLoadDataBase::IsValid(void) const
{
    return (IsValidVersion() && IsValidCheckSum());
};


void CSaveLoadDataBase::SetData(int32 iVersion, const void* pData, uint32 uDataSize)
{
    ASSERT(pData);
    ASSERT(uDataSize);

    ASSERT(!m_uSize);
    ASSERT(!m_pData);

    m_iVersion = iVersion;
    m_uSize = uDataSize + sizeof(HEADER);
    m_pData = new char[m_uSize];

    if (m_pData)
    {
        std::memcpy(
            (uint8*)m_pData + sizeof(HEADER),
            pData,
            uDataSize
        );

        HEADER* pHeader = (HEADER*)m_pData;
        pHeader->m_iVersion = m_iVersion;
        pHeader->m_uSize = m_uSize;
        pHeader->m_uChecksum = GetCheckSum();
    };
};


void* CSaveLoadDataBase::GetData(void) const
{
    ASSERT(m_pData); 
    return m_pData;
};


uint32 CSaveLoadDataBase::GetSize(void) const
{
    ASSERT(m_uSize);
	return m_uSize;
};


int32 CSaveLoadDataBase::GetVersion(void) const
{
	ASSERT(m_iVersion);
    return m_iVersion;
};


uint32 CSaveLoadDataBase::GetCheckSum(void) const
{
    ASSERT(m_pData);
    ASSERT(m_uSize);

    uint8* pData = ((uint8*)m_pData + sizeof(HEADER));
    uint32 uResult = 0;
    uint32 uSize = Header().m_uSize - sizeof(HEADER);

    for (uint32 i = 0; i < uSize; ++i)
        uResult += pData[i];
    
	return uResult;
};


bool CSaveLoadDataBase::IsValidVersion(void) const
{
    return (Header().m_iVersion == m_iVersion);
};


bool CSaveLoadDataBase::IsValidCheckSum(void) const
{
    return (Header().m_uChecksum == GetCheckSum());
};


CSaveLoadDataBase::HEADER& CSaveLoadDataBase::Header(void) const
{
    ASSERT(m_pData);

    return *(HEADER*)m_pData;
};