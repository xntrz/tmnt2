#pragma once


enum SYSTEXT {};


class CSystemText
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const void* pBuffer, uint32 uBufferSize);
    static int32 GetTextNum(void);
    static const wchar* GetText(SYSTEXT id);
    static const wchar* EmptyText(void);
};