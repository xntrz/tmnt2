#pragma once

#include "rwcore.h"
#include "rt2d.h"

// rtanim.h fwd
typedef struct RtAnimAnimation  RtAnimAnimation;

// rphanim.h fwd
typedef struct RpHAnimHierarchy RpHAnimHierarchy;

// rt2danim.h fwd
typedef struct Rt2dMaestro      Rt2dMaestro;
typedef struct Rt2dAnim         Rt2dAnim;
typedef struct Rt2dAnimProps    Rt2dAnimProps;
typedef struct Rt2dMessage      Rt2dMessage;

#if (defined(TARGET_PC))
#define rwRESOURCESDEFAULTARENASIZE (4 << 20) // 4 MB
#else
#define rwRESOURCESDEFAULTARENASIZE (3 << 20) // 3 MB
#endif

#include <string>
#include <vector>
#include <cmath>
#include <cfloat> // FLT_MIN/MAX, FLT_EPSILON
#include <climits> // INT_MIN/MAX etc

#ifdef _DEBUG
#include <inttypes.h> // PRId32 etc
#endif /* _DEBUG */

#ifdef TARGET_PC

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef wchar_t             wchar;
#if defined(_MSC_VER)
typedef signed __int64      int64;
typedef unsigned __int64    uint64;
#else /* defined(_MSC_VER) */
typedef signed long long    int64;
typedef unsigned long long  uint64;
#endif /* defined(_MSC_VER) */

#define UTEXT(text)	L##text

#endif /* TARGET_PC */

#include "MemoryStd.hpp"
#include "Debug.hpp"

#define CHECK_SIZE(type, targetSize)	static_assert(sizeof(type) == targetSize, "incorrect size")
#define COUNT_OF(ptr)	            	static_cast<int32>(sizeof(ptr) / sizeof(ptr[0]))

#define ALIGN_CHECK(v, a) 				(((v) & (((a) - 1u))) == 0u)
#define ALIGN_ADJUST(v, a) 				((a) - ((v) & ((a) - 1u)))
#define ALIGN_ROUND_DOWN(v, a) 			((v) & ~((a) - 1u))
#define ALIGN_ROUND_UP(v, a) 			(((v) + ((a) - 1u)) & ~((a) - 1u))
#define ALIGN(v, a) 					(ALIGN_ROUND_UP(v, a))

#define BITSOF(var)		            	static_cast<int32>(sizeof(var) << 3)

#ifdef UINT8_MAX
#undef UINT8_MAX
#endif

#ifdef UINT16_MAX
#undef UINT16_MAX
#endif

#ifdef UINT32_MAX
#undef UINT32_MAX
#endif

#ifdef UINT64_MAX
#undef UINT64_MAX
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

template<class T>
inline T Clamp(T val, T min, T max)
{
	return (val > max ? max : (val < min ? min : val));
};


template<class T>
inline T InvClamp(T val, T min, T max)
{
	return (val > max ? min : (val < min ? max : val));
};


template<class T>
inline T Max(T a, T b)
{
	return (a > b ? a : b);
};


template<class T>
inline T Min(T a, T b)
{
	return (a < b ? a : b);
};


namespace TYPEDEF
{
	enum CONFIG_TV
	{
		CONFIG_TV_NTSC = 0,
		CONFIG_TV_PAL,
	};
	
	enum CONFIG_LANG
	{
		CONFIG_LANG_ENGLISH = 0,
		CONFIG_LANG_GERMAN,
		CONFIG_LANG_FRENCH,
		CONFIG_LANG_SPANISH,
        CONFIG_LANG_ITALIAN,

        CONFIG_LANG_NUM,
    };

	enum CONFIG_LAUNCH
	{
		CONFIG_LAUNCH_NORMAL = 0,
		CONFIG_LAUNCH_ARCADE,
		CONFIG_LAUNCH_TVCHANGE,
		CONFIG_LAUNCH_DASHBOARD,
	};

	static const float 	DEFAULT_SCREEN_WIDTH 	= 640.0f;
	static const float 	DEFAULT_SCREEN_HEIGHT	= 480.0f;
	static const float 	DEFAULT_ASPECTRATIO 	= (DEFAULT_SCREEN_WIDTH / DEFAULT_SCREEN_HEIGHT);
	static const float 	DEFAULT_VIEWWINDOW 		= (0.5f);
	static const float 	DEFAULT_CLIP_FAR 		= 100.0f;
	static const float 	DEFAULT_CLIP_NEAR 		= 0.1f;

	static const float 	VSCR_W 		= 640.0f;			// virtual screen w
	static const float 	VSCR_H 		= 448.0f;			// virtual screen h
	static const float 	VSCR_X 		= -(VSCR_W * 0.5f);	// virtual screen x
	static const float 	VSCR_Y		= -(VSCR_H * 0.5f);	// virtual screen y

    static const float  FLOAT_MIN = FLT_MIN;
    static const float  FLOAT_MAX = FLT_MAX;

    static const uint8  UINT8_MIN  = 0u;
    static const uint8  UINT8_MAX  = UCHAR_MAX;
    static const uint16 UINT16_MIN = 0u;
    static const uint16 UINT16_MAX = USHRT_MAX;
    static const uint32 UINT32_MIN = 0u;
    static const uint32 UINT32_MAX = UINT_MAX;
    static const uint64 UINT64_MIN = 0ull;
    static const uint64 UINT64_MAX = ULLONG_MAX;

    static const int8   SINT8_MIN  = SCHAR_MIN;
    static const int8   SINT8_MAX  = SCHAR_MAX;
    static const int16  SINT16_MIN = SHRT_MIN;
    static const int16  SINT16_MAX = SHRT_MAX;
    static const int32  SINT32_MIN = INT_MIN;
    static const int32  SINT32_MAX = INT_MAX;
    static const int64  SINT64_MIN = LLONG_MIN;
    static const int64  SINT64_MAX = LLONG_MAX;
};