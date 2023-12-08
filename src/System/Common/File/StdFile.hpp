#pragma once

#include "AdxFile.hpp"


class CStdFile final : public CAdxFileISO
{
public:
    static bool IsExists(const char* pszFilename);

    CStdFile(void);
    virtual ~CStdFile(void);
    virtual bool Open(const char* pszFilename) override;
    virtual void Close(void) override;
    virtual uint32 Read(void* buff, uint32 size);
    virtual bool IsEof(void);
    virtual uint32 Seek(uint32 offset, int32 type);
    virtual bool IsOpen(void);

private:
    uint32 m_uPosition;
};