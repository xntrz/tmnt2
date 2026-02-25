#pragma once

#include "File.hpp"


class CStdFile final : public CFile
{
public:
    static bool exist(const char* pszFilename);

    CStdFile(void);
    virtual ~CStdFile(void);
    bool open(const char* pszFilename);
    void close(void);
    uint32 read(void* buff, uint32 size);
    bool eof(void) const;
    uint32 seek(uint32 offset, int32 type);
    uint32 tell(void) const;

private:
    uint32 m_uPosition;
};