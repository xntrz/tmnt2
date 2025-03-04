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
        m_pData = new uint8[m_uSize];

        std::memcpy(m_pData, pData, static_cast<size_t>(m_uSize));
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
    m_pData = new uint8[m_uSize];

    if (m_pData)
    {
        std::memcpy(m_pData + sizeof(HEADER), pData, static_cast<size_t>(uDataSize));

        HEADER* pHeader = reinterpret_cast<HEADER*>(m_pData);
        
        pHeader->m_iVersion  = m_iVersion;
        pHeader->m_uSize     = m_uSize;
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

    uint8* pData = (m_pData + sizeof(HEADER));
    uint32 uResult = 0;
    uint32 uSize = Header().m_uSize - sizeof(HEADER);

    for (uint32 i = 0; i < uSize; ++i)
        uResult += pData[i];
    
    return (uResult & 0xFF);
};


bool CSaveLoadDataBase::IsValidVersion(void) const
{
    return (Header().m_iVersion == m_iVersion);
};


bool CSaveLoadDataBase::IsValidCheckSum(void) const
{
    uint32 FileChecksum = Header().m_uChecksum;
    uint32 CalcChecksum = GetCheckSum();

    return (FileChecksum == CalcChecksum);
};


CSaveLoadDataBase::HEADER& CSaveLoadDataBase::Header(void) const
{
    ASSERT(m_pData);

    return *reinterpret_cast<HEADER*>(m_pData);
};