#pragma once


class IDataLoaderImpl
{
public:
    virtual ~IDataLoaderImpl(void) {};
    virtual void Eval(const void* pBuffer, uint32 uBufferSize) = 0;
};