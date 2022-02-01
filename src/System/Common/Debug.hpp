#pragma once

class CDebug
{
public:    
    static void(*Output)(const char* format, ...);
    static void(*Fatal)(const char* reason);
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void StartRWDebug(void);
    static void StopRWDebug(void);
    static void Assert(const char* expression, const char* fname, int32 fline);
    static void Assert(const char* expression, const char* fname, int32 fline, const char* format, ...);
};

#ifdef ASSERT
#undef ASSERT
#endif

#if defined (_DEBUG)

#ifdef _TARGET_PC
#define DBGBREAK            \
    do                      \
    {                       \
        __asm{ int 0x3 };   \
    } while (0)
#else
#define DBGBREAK
#endif

#define ASSERT(expression, ...)                                             \
    {                                                                       \
        if (!(expression))                                                  \
        {                                                                   \
	        CDebug::Assert(#expression, __FILE__, __LINE__, ##__VA_ARGS__); \
        };                                                                  \
    }

#define OUTPUT(format, ...)     CDebug::Output(format, ##__VA_ARGS__)

#else

#define DBGBREAK

#define ASSERT(expression, ...) ((void)0)

#define OUTPUT(format, ...)     ((void)0)

#endif