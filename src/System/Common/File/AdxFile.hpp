#pragma once

#include "File.hpp"


class CAdxFile : public CFile
{
public:
    CAdxFile(int32 nLabel);
    virtual ~CAdxFile(void);
};


class CAdxFileID : public CAdxFile
{
public:
    CAdxFileID(void);
    virtual ~CAdxFileID(void);
};


class CAdxFileISO : public CAdxFile
{
public:
    CAdxFileISO(void);
    virtual ~CAdxFileISO(void);
    virtual bool Open(const char* pszName) override;
};