#pragma once

#include "AdxFileData.hpp"
#include "FileTypes.hpp"


enum FILE_STAT
{
    FILE_STAT_NOREAD = 0,
    FILE_STAT_READING,
    FILE_STAT_READEND,
    FILE_STAT_ERROR,
};


class CFileAccess : public CAdxFileAccess
{
public:
    static bool IsExist(const char* pszFilename, bool bCheckID = true);

    CFileAccess(void);
    virtual ~CFileAccess(void);
    virtual bool Open(int32 id);
    virtual bool Open(const char* pszFilename);
    virtual FILE_STAT SyncStat(void);
    virtual void ReleaseBuff(void);
    FILE_STAT GetStat(void) const;
    bool IsReadEnd(void) const;
    void SetAwait(void);

private:
    FILE_STAT m_stat;
};


class CFileInfo : public CFileAccess
{
public:
    CFileInfo(void);
    virtual ~CFileInfo(void);
    virtual bool Open(int32 id) override;
    virtual FILE_STAT SyncStat(void) override;
    virtual void ReleaseBuff(void) override;

private:
    uint32 m_uRefCount;
};


class CFileAccessFname : public CFileAccess
{
public:
    CFileAccessFname(const char* pszFilename);
    virtual ~CFileAccessFname(void);
    virtual bool Open(int32 id = -1) override;
    
private:
    char m_szFilename[FILETYPES::FILE_NAME_MAX];
};