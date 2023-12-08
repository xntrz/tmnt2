#pragma once

#include "File.hpp"


class CAdxFileID : public CFile
{
public:
    inline CAdxFileID(void) {};
    virtual ~CAdxFileID(void) {};
    virtual bool Open(int32 fid);
};


class CAdxFileISO : public CFile
{
public:
    inline CAdxFileISO(void) {};
    virtual ~CAdxFileISO(void) {};
    virtual bool Open(const char* fname);
};