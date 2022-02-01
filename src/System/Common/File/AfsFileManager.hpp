#pragma once

#include "FileManager.hpp"


class CAfsFileAccess;
class CAfsRwFileSystem;


class CAfsFileManager : public CFileManager
{
public:
    static const int32 AFSACCESS_MAX = 32;
    
    enum
    {
        LABELAFS_ID = 0,
        LABELAFS_NAME,

        LABELAFS_EXTEND,        
    };
    
public:
    CAfsFileManager(void);
    virtual ~CAfsFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Sync(void) override;
    virtual void Reset(void) override;
    virtual CFileAccess* AllocRequest(int32 nID, int32 nLabel) override;
    virtual CFileAccess* AllocRequest(const char* pszName, int32 nLabel) override;
    virtual bool MountPartition(int32 iPartitionID, const char* pszArchive);
    virtual void UnmountPartition(int32 iPartitionID);
    virtual void GarbageCollection(void);

private:
    CAfsRwFileSystem* m_pAfsRwFS;
    CAfsFileAccess* m_paAfsFileAccess;
    CList<CAfsFileAccess> m_listAfsAccessPool;
    CList<CAfsFileAccess> m_listAfsAccessAlloc;
    uint32 m_uPtLoadMask;
};