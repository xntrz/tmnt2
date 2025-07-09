#pragma once

#ifdef _MSC_VER
#pragma warning(disable : 4200) // nonstandard extension used: zero-sized array in struct/union
#pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#pragma warning(disable : 4996) // function or variable may be unsafe
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4189) // local variable is initialized but not referenced
#pragma warning(disable : 4505) // unreferenced local function has been removed
#pragma warning(disable : 5054) // operator X deprecated between enumerations of different types
#endif /* _MSC_VER */

//
//	Wrap all SD codes to this while sound subsystem is not ready
//
#define SDCODE_SE(SeCode) 		(SeCode)
#define SDCODE_BGM(BgmCode) 	(BgmCode)
#define SDCODE_VOICE(VoiceCode) (VoiceCode)

#include "rwcore.h"
#include "rpworld.h"
#include "rt2d.h"
#include "rtfsyst.h"
#include "rtpitexd.h"
#include "rt2danim.h"
#include "rtanim.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rpcollis.h"

#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>

using int8 	= int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8	= uint8_t;
using uint16= uint16_t;
using uint32= uint32_t;
using uint64= uint64_t;


#if (defined(TARGET_PC))

typedef wchar_t wchar;

#define UTEXT(text)	L##text

#elif (defined(TARGET_PS2))
typedef unsigned int wchar;
#define UTEXT(text)	text

#else

#error Current target is not set

#endif /* defined(TARGET_PC) */


#include "MemoryStd.hpp"
#include "Debug.hpp"

#define CHECK_SIZE(type, targetSize)	static_assert(sizeof(type) == targetSize, "incorrect size")
#define RWRGBALONGEX(rwrgba) 			(RWRGBALONG(rwrgba.red, rwrgba.green, rwrgba.blue, rwrgba.alpha))
#define COUNT_OF(ptr)	            	static_cast<int32>(sizeof(ptr) / sizeof(ptr[0]))

#define ALIGN_CHECK(v, a) 				(((v) & (((a) - 1u))) == 0u)
#define ALIGN_ADJUST(v, a) 				((a) - ((v) & ((a) - 1u)))
#define ALIGN_ROUND_DOWN(v, a) 			((v) & ~((a) - 1u))
#define ALIGN_ROUND_UP(v, a) 			(((v) + ((a) - 1u)) & ~((a) - 1u))
#define ALIGN(v, a) 					(ALIGN_ROUND_UP(v, a))

#define BITSOF(var)		            	(sizeof(var) << 3)

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

	static const float 	FLOAT_MIN 	= std::numeric_limits<float>::min();
	static const float 	FLOAT_MAX 	= std::numeric_limits<float>::max();

	static const uint8 	UINT8_MIN 	= std::numeric_limits<uint8>::min();
	static const uint8 	UINT8_MAX 	= std::numeric_limits<uint8>::max();
    static const uint16 UINT16_MIN 	= std::numeric_limits<uint16>::min();
	static const uint16 UINT16_MAX 	= std::numeric_limits<uint16>::max();
	static const uint32 UINT32_MIN 	= std::numeric_limits<uint32>::min();
	static const uint32 UINT32_MAX 	= std::numeric_limits<uint32>::max();
	static const uint64 UINT64_MIN 	= std::numeric_limits<uint64>::min();
	static const uint64 UINT64_MAX 	= std::numeric_limits<uint64>::max();
    
	static const int8 	SINT8_MIN 	= std::numeric_limits<int8>::min();
	static const int8 	SINT8_MAX 	= std::numeric_limits<int8>::max();
	static const int16 	SINT16_MIN 	= std::numeric_limits<int16>::min();
	static const int16 	SINT16_MAX 	= std::numeric_limits<int16>::max();
	static const int32 	SINT32_MIN 	= std::numeric_limits<int32>::min();
	static const int32 	SINT32_MAX 	= std::numeric_limits<int32>::max();
	static const int64 	SINT64_MIN 	= std::numeric_limits<int64>::min();
	static const int64 	SINT64_MAX 	= std::numeric_limits<int64>::max();
};