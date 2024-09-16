#pragma once


template<class T, std::size_t N>
inline constexpr
std::size_t _fname_ofs(
	const T(&str)[N],
	std::size_t pos = N - 1u
)
{
	return (str[pos] == '\\') ? pos + 1u : (pos > 0u ? _fname_ofs(str, pos - 1u) : 0u);
};

template <class T, T v>
struct _fname_ofs_evaluate {
	static constexpr const T value = v;
};

#define __FILENAME__ (&__FILE__[_fname_ofs_evaluate<decltype(_fname_ofs(__FILE__)), _fname_ofs(__FILE__)>::value])

#ifdef ASSERT
#undef ASSERT
#endif /* ASSERT */

#define FATAL(fmt, ...) CDebug::Fatal(fmt, ##__VA_ARGS__)

#ifdef _DEBUG

#define ASSERT(expression, ...)                                             \
    do                                                                      \
    {                                                                       \
        if (!(expression))                                                  \
	        CDebug::Assert(#expression, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while(0)

#define OUTPUT(format, ...)     CDebug::Output(__FILENAME__, __LINE__, format, ##__VA_ARGS__)

#else /* _DEBUG */

#define ASSERT(expression, ...) ((void)0)

#define OUTPUT(format, ...)     ((void)0)

#endif /* _DEBUG */


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