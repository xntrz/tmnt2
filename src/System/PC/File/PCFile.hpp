#pragma once

#include "System/Common/File/File.hpp"


class CPCFile : public CFile
{
public:
    inline CPCFile(void) {};
    virtual ~CPCFile(void) {};
    virtual bool Open(const char* fname);
};