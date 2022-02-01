#pragma once

#include "AfsFile.hpp"


class CAfsRwFile final : protected CAfsFileISO
{
public:
    enum SEEKTYPE
    {
        SEEKTYPE_BEGIN = 0,
        SEEKTYPE_CURRENT,
        SEEKTYPE_END,

        SEEKTYPE_NUM,
    };

public:
    static bool IsExists(const char* pszFilename);
    
    CAfsRwFile(void);
    virtual ~CAfsRwFile(void);
    virtual bool Open(int32 id) override;
    virtual bool Open(const char* pszName) override;
    virtual void Close(void) override;    
    virtual uint32 Read(void* buffer, uint32 size);
    virtual bool IsEof(void);
    virtual uint32 Seek(int32 pos, SEEKTYPE type);    
    virtual bool IsOpen(void);

private:
    uint32 m_uPosition;
};