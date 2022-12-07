#pragma once

#include "System/Common/File/FileAccess.hpp"


class CPCPhyFileAccess :
    public CFileAccess,
    public CListNode<CPCPhyFileAccess>
{
public:
    static bool IsExists(const char* path);
    
    CPCPhyFileAccess(void);
    virtual ~CPCPhyFileAccess(void);
    virtual bool Read(const char* pszName) override;
    virtual bool Read(int32 nID) override;
    virtual void Clear(void) override;
    bool Write(const char* pszName, const char* data, int32 datalen);

private:
    void* m_hFile;
};


class CPCRcFileAccess :
    public CFileAccess,
    public CListNode<CPCRcFileAccess>
{
public:
    CPCRcFileAccess(void);
    virtual ~CPCRcFileAccess(void);
    virtual bool Read(const char* pszName) override;
    virtual bool Read(int32 nID) override;
    virtual void Clear(void) override;    

private:
    void* m_hData;
};