#pragma once

#include "GameTextID.hpp"


class CGameText
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const void* pBuffer, uint32 uBufferSize);
    static int32 GetTextNum(void);
    static const wchar* GetText(GAMETEXT id);
    static const wchar* EmptyText(void);
};