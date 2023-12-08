#pragma once


class CFileAccess
{
public:
    friend class CFileManager;

    enum STAT
    {
        STAT_NOREAD = 0,
        STAT_PENDING,
        STAT_READING,
        STAT_READEND,
        STAT_ERROR
    };

public:
    CFileAccess(void);
    virtual ~CFileAccess(void);
    virtual bool Open(const char* name);
    virtual bool Open(int32 id);
    virtual void Clear(void);
    virtual void Sync(void);
    virtual STAT Stat(void) const;
    virtual void* Data(void) const;
    virtual uint32 Size(void) const;

protected:
    STAT m_stat;
    void* m_pBuffer;
    uint32 m_uBufferSize;
};