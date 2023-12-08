#pragma once

#include "System/Common/File/FileAccess.hpp"


class CPCPhyFileAccess : public CFileAccess
{
public:
    static bool IsExists(const char* pszFilename);
    
    inline CPCPhyFileAccess(void) {};
    virtual ~CPCPhyFileAccess(void) {};
    virtual bool Open(const char* pszFilename) override;
    virtual void Clear(void) override;
};