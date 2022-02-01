#pragma once

#include "System/Common/File/FileAccess.hpp"


class CPCPhysicalFileAccess :
    public CFileAccess,
    public CListNode<CPCPhysicalFileAccess>
{
public:
    static bool IsExists(const char* path);
    
    CPCPhysicalFileAccess(void);
    virtual ~CPCPhysicalFileAccess(void);
    virtual bool Read(const char* pszName) override;
    virtual bool Read(int32 nID) override;
    virtual void Clear(void) override;
    bool Write(const char* pszName, const char* data, int32 datalen);

private:
    void* m_hFile;
};


class CPCResFileAccess :
    public CFileAccess,
    public CListNode<CPCResFileAccess>
{
public:
    CPCResFileAccess(void);
    virtual ~CPCResFileAccess(void);
    virtual bool Read(const char* pszName) override;
    virtual bool Read(int32 nID) override;
    virtual void Clear(void) override;    

private:
    void* m_hData;
};