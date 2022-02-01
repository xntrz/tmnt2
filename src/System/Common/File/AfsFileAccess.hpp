#pragma once

#include "FileAccess.hpp"


class CAfsFileAccess final : public CFileAccess, public CListNode<CAfsFileAccess>
{
public:
    static bool IsExists(const char* pszFilename);
    
    CAfsFileAccess(void);
    virtual ~CAfsFileAccess(void);
    virtual bool Read(const char* pszName) override;
    virtual bool Read(int32 nID) override;
    virtual void Clear(void) override;
    virtual void Sync(void) override;

private:
    bool ReadCommon(int32 ptid, int32 phyid);

private:
    uint32 m_uReferenceCount;
};