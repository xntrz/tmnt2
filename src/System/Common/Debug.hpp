#pragma once

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)


class CDebug
{
public:    
    static void(*CallbackOutput)(const char* fname, int32 fline, const char* format, ...);
    static void(*CallbackFatal)(const char* reason, ...);
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void StartRwDebug(void);
    static void StopRwDebug(void);
    static void Assert(const char* expression, const char* fname, int32 fline);
    static void Assert(const char* expression, const char* fname, int32 fline, const char* format, ...);
};


#ifdef ASSERT
#undef ASSERT
#endif

#ifdef _DEBUG

#define DBGBREAK            \
    do                      \
    {                       \
        __asm{ int 0x3 };   \
    } while (0)

#define ASSERT(expression, ...)                                             \
    do                                                                      \
    {                                                                       \
        if (!(expression))                                                  \
        {                                                                   \
	        CDebug::Assert(#expression, __FILE__, __LINE__, ##__VA_ARGS__); \
        };                                                                  \
    } while(0)


#define OUTPUT(format, ...)     CDebug::CallbackOutput(__FILENAME__, __LINE__, format, ##__VA_ARGS__)

#else

#define DBGBREAK

#define ASSERT(expression, ...) ((void)0)

#define OUTPUT(format, ...)     ((void)0)
#define OUTPUTLN(format, ...)   ((void)0)

#endif