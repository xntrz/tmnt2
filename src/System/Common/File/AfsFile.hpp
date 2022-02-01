#pragma once

#include "File.hpp"


class CAfsFile : public CFile
{
public:
    CAfsFile(int32 nLabel);
    virtual ~CAfsFile(void);
};


class CAfsFileID : public CAfsFile
{
public:
    CAfsFileID(void);
    virtual ~CAfsFileID(void);
};


class CAfsFileISO : public CAfsFile
{
public:
    CAfsFileISO(void);
    virtual ~CAfsFileISO(void);
    virtual bool Open(const char* pszName) override;
};