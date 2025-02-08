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

public:
#ifdef _DEBUG    
    /* for enemy test */
    using COMPLETE_CB = void(*)(void*, uint32, void*);

    static void RegistIntercept(COMPLETE_CB cb, void* pParam = nullptr);
    static void RemoveIntercept(COMPLETE_CB cb);
#endif /* _DEBUG */
};