#pragma once


namespace SYSTEXT
{
    enum VALUE
    {

    };
};


class CSystemText
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const void* pBuffer, uint32 uBufferSize);
    static const wchar* GetText(SYSTEXT::VALUE id);
    static int32 GetTextNum(void);
    static const wchar* EmptyText(void);
};