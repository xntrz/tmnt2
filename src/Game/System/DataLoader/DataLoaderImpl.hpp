#pragma once


class IDataLoaderImpl
{
public:
    virtual ~IDataLoaderImpl(void) {};
    virtual void Eval(void* pBufferOrg, uint32 sizeBuffer, const void* fid) = 0;
};