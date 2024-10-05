#pragma once


class CFileAccess;


class CFile
{
public:
    enum STAT
    {
        STAT_NOREAD = 0,
        STAT_READING,
        STAT_READEND,
        STAT_ERROR,
    };

public:
    CFile(void);
    virtual ~CFile(void);
    virtual bool Open(int32 nType, void* pTypeData);
    virtual void Close(void);
    virtual void* Data(void) const;
    virtual uint32 Size(void) const;
    virtual STAT Stat(void);

protected:
    STAT m_stat;
    CFileAccess* m_pAccessData;
};