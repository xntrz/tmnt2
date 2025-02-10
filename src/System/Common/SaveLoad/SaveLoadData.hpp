#pragma once


class CSaveLoadDataBase
{
protected:
    struct HEADER
    {
        int32  m_iVersion;
        uint32 m_uSize;
        uint32 m_uChecksum;
    };
    
public:
    CSaveLoadDataBase(void);
    virtual ~CSaveLoadDataBase(void);
    virtual void Initialize(const void* pData, uint32 uDataSize);
    virtual void Initialize(void);
    virtual void Terminate(void);
    virtual void Update(void);
    virtual bool IsValid(void) const;
    void SetData(int32 iVersion, const void* pData, uint32 uDataSize);
    void* GetData(void) const;
    uint32 GetSize(void) const;
    int32 GetVersion(void) const;
    uint32 GetCheckSum(void) const;
    bool IsValidVersion(void) const;
    bool IsValidCheckSum(void) const;

protected:
    HEADER& Header(void) const;

protected:
    uint8* m_pData;
    uint32 m_uSize;
    int32  m_iVersion;
};