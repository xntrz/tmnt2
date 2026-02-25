#pragma once

#include "cri_adxf.h"


class CAdxFileAccess
{
public:
    static bool IsExist(const char* pszFilename);
    static const char* GetLastFilename(void);
    
    CAdxFileAccess(void);
    virtual ~CAdxFileAccess(void);
    bool Open(int32 id);
    bool Open(const char* pszFilename);
    void Read(void);
    void Close(void);
    void ReleaseBuff(void);
    int32 GetStat(void) const;
    uint8* GetData(void);
    const uint8* GetData(void) const;
    uint32 GetSize(void) const;
    
private:
    static char m_szLastFilename[ADXF_FNAME_MAX];
    static ADXF m_adxf;
    uint8* m_pBuff;
    uint32 m_uSize;
};