#pragma once


class CFileManager;


class CFileAccess
{
public:
    friend CFileManager;
    
    enum STATUS
    {
        STATUS_NOREAD = 0,
        STATUS_PENDING,
        STATUS_READING,
        STATUS_READEND,
        STATUS_ERROR,
    };
    
public:
    CFileAccess(void);
    virtual ~CFileAccess(void);
    virtual bool Read(const char* pszName);
    virtual bool Read(int32 nID);
    virtual void Clear(void);
    virtual void Sync(void);
    virtual STATUS Status(void) const;
    virtual uint32 Size(void) const;
    virtual void* Data(void) const;

protected:
    void* m_pBuffer;
    uint32 m_uBufferSize;
    STATUS m_status;
};