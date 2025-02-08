#pragma once

#include "PCFileAccess.hpp"
#include "PCRwFileSystem.hpp"

#include "System/Common/File/AdxFileManager.hpp"

#include <list>


class CPCFileManager final : public CAdxFileManager
{
public:
    enum FILETYPE
    {
        FILETYPE_PC = FILETYPE_EXTEND,
    };

public:
    static bool IsExist(const char* pszFilename);

    CPCFileManager(void);
    virtual ~CPCFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Sync(void) override;
    virtual void Error(const char* pszDescription) override;
    virtual CFileAccess* AllocRequest(int32 nType, void* pTypeData) override;
    virtual bool SetupFileSystem(void) override;
    virtual void ShutdownFileSystem(void) override;

private:
    void SetAfsPath(void);
    void SetAfsOverrideFlag(void);
    void SetCwd(void);
    void PhyAccessInit(void);
    void PhyAccessTerm(void);
    void PhyAccessSync(void);
    CPCPhyFileAccess* PhyAccessAlloc(const char* pszFilename);

private:
    CPCRwFileSystem m_rwFileSystemPC;
    CPCPhyFileAccess m_aPhyFAccessPool[32];
    std::list<CPCPhyFileAccess*> m_listPhyFAccessFree;
    std::list<CPCPhyFileAccess*> m_listPhyFAccessAlloc;
    void* m_adxfic;
    bool m_bAfsOverrideFlag;
    char m_szAfsPath[256];
};