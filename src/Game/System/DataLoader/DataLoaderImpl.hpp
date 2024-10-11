#pragma once


class IDataLoaderImpl
{
public:
    virtual ~IDataLoaderImpl(void) {};
    virtual void Eval(void* pBufferOrg, uint32 sizeBuffer) = 0;
};