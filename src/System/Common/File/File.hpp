#pragma once

#include "FileData.hpp"


class CFileManager;


class CBaseFile
{
public:
    static void Initialize(CFileManager* pFileManager);
    static void Terminate(void);
    static bool LoadPartition(int32 ptid, const char* fname, void* dir, void* ptinfo);

    CBaseFile(void);
    virtual ~CBaseFile(void);
    uint8* GetData(void);
    const uint8* GetData(void) const;
    uint32 GetSize(void) const;
    FILE_STAT GetStat(void) const;

protected:
    static CFileManager* m_pFileManager;
    CFileAccess* m_pAccessData;
};


class CIDFile : virtual public CBaseFile
{
public:
    CIDFile(void);
    virtual ~CIDFile(void);
    void OpenRequest(int32 id);
    void Close(void);
};


class CISOFile : virtual public CBaseFile
{
public:
    CISOFile(void);
    virtual ~CISOFile(void);
    void OpenRequest(const char* pszFilename);
    void Close(void);
};


class CFile : public CIDFile,
              public CISOFile
{
public:
    enum TYPE
    {
        TYPE_UNKNOWN = -1,
        TYPE_ID = 0,
        TYPE_ISO,
    };

public:
    CFile(void);
    CFile(int32 id);
    virtual ~CFile(void);
    void OpenRequest(int32 id);
    void OpenRequest(const char* pszFilename, bool bSearchID = true);
    bool Open(const char* pszFilename);
    void Close(void);
    TYPE GetType(void) const;

private:
    TYPE m_type;
};


