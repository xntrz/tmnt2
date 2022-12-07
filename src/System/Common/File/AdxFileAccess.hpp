#pragma once

#include "FileAccess.hpp"


class CAdxFileAccess final : public CFileAccess, public CListNode<CAdxFileAccess>
{
public:
    static bool IsExists(const char* pszFilename);
    
    CAdxFileAccess(void);
    virtual ~CAdxFileAccess(void);
    virtual bool Read(const char* pszName) override;
    virtual bool Read(int32 Id) override;
    virtual void Clear(void) override;
    virtual void Sync(void) override;

private:
    bool ReadNw(void);

private:
    uint32 m_uReferenceCount;
};