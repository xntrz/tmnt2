#pragma once

#include "FileQueue.hpp"
#include "FileData.hpp"
#include "RwFileSystemManager.hpp"


class CFileManager
{
protected:
    enum STAT
    {
        STAT_READY = 0,
        STAT_BUSY,
    };

    struct READ_REQUEST
    {
        CFileAccess* pData;
        int32        id;
    };

public:
    static CFileManager& Instance(void);
    
    CFileManager(void);
    virtual ~CFileManager(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Sync(void);
    virtual void ResetData(void) = 0;
    virtual void Error(const char* pszDescription) = 0;
    virtual void ReadEnd(CFileAccess* pData);
    void ReadDataRequest(CFileAccess* pData, int32 id = -1);
    void OpenFile(CFileAccess* pData, int32 id);
    CFileAccess* GetFileInfo(int32 id);

protected:
    static CFileManager*     m_pInstance;
    STAT                     m_stat;
    int32                    m_id;
    CFileAccess*             m_pAccessData;
    CFileQueue<READ_REQUEST> m_readQueue;
    CFileInfo*               m_pInfoTable;
    CRwFileSystemManager     m_rwFileSystem;
};