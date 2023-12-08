#pragma once

#include "FileAccess.hpp"

#include "cri_adxf.h"


class CAdxFileAccess final : public CFileAccess
{
public:
    static bool IsExists(const char* pszFilename);
    
    CAdxFileAccess(void);
    virtual ~CAdxFileAccess(void);
    virtual bool Open(const char* name) override;
    virtual bool Open(int32 id) override;
    virtual void Clear(void) override;
    virtual void Sync(void) override;

private:
    bool ReadNoWait(void);

private:
    static ADXF m_adxf;
    uint32 m_uReferenceCount;
};