#pragma once

#include "PCRwFileSystem.hpp"

#include "System/Common/File/AdxFileManager.hpp"


class CPCFileManager : public CAdxFileManager
{
public:
    CPCFileManager(void);
    virtual ~CPCFileManager(void);
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void Error(const char* pszDescription) override;
    virtual bool SetupFileSystem(void) override;
    virtual void ShutdownFileSystem(void) override;
    void SetAfsPath(void);
    void SetCwd(void);

private:
    CPCRwFileSystem m_rwFileSystemPC;
    void*           m_adxfic;
    char            m_szAfsPath[256];
};


class CPCFileManagerReadLog final : public CPCFileManager
{
public:
    CPCFileManagerReadLog(void);
    virtual ~CPCFileManagerReadLog(void) {};
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    virtual void ReadEnd(CFileAccess* pData) override;

private:
    FILE* m_readLog;
};