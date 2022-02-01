#pragma once


class CRwFileSystem;


class CRwFileManager
{
public:
    static bool Initialize(void);
    static void Terminate(void);
    static void Regist(CRwFileSystem* pRwFileSystem, bool bSetAsDefault = false);
    static void Remove(CRwFileSystem* pRwFileSystem);
};