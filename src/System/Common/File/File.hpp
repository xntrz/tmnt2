#pragma once


class CFileAccess;


class CFile
{
public:
    enum STATUS
    {
        STATUS_NOREAD = 0,
        STATUS_READING,
        STATUS_READEND,
        STATUS_ERROR,
    };

public:
    CFile(int32 nLabel);
    virtual ~CFile(void);
    virtual bool Open(int32 id);
    virtual bool Open(const char* pszName);
    virtual void Close(void);
    virtual void* Data(void) const;
    virtual uint32 Size(void) const;
    virtual STATUS Status(void);

protected:
    int32 m_nLabel;
    STATUS m_status;
    CFileAccess* m_pAccessData;
};