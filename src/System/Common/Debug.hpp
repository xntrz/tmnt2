#pragma once


class CDebug
{
public:    
    static void(*Output)(const char* fname, int32 fline, const char* format, ...);
    static void(*Fatal)(const char* reason, ...);

public:
    static void Initialize(void);
    static void Terminate(void);
    static void StartRwDebug(void);
    static void StopRwDebug(void);
    static void SupressRwDebugAssert(bool bSupress);
    static void Assert(const char* expression, const char* fname, int32 fline);
    static void Assert(const char* expression, const char* fname, int32 fline, const char* format, ...);
};

#ifdef ASSERT
#undef ASSERT
#endif

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define FATAL(fmt, ...) CDebug::Fatal(fmt, ##__VA_ARGS__)

#ifdef _DEBUG

#define ASSERT(expression, ...)                                             \
    do                                                                      \
    {                                                                       \
        if (!(expression))                                                  \
        {                                                                   \
	        CDebug::Assert(#expression, __FILE__, __LINE__, ##__VA_ARGS__); \
        };                                                                  \
    } while(0)

#define OUTPUT(format, ...)     CDebug::Output(__FILENAME__, __LINE__, format, ##__VA_ARGS__)

#else

#define ASSERT(expression, ...) ((void)0)
#define OUTPUT(format, ...)     ((void)0)

#endif