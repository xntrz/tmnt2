#pragma once


class CDataLoader
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static bool IsLoadEnd(void);
    static void Regist(int32 iFileID);
    static void Regist(const char* pszFilename);
};