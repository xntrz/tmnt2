#pragma once

#ifdef _MSC_VER
#pragma warning(disable : 4200)
#pragma warning(disable : 4996)
#endif

//
//	Wrap all SD codes to this while sound subsystem is not ready
//
#define SDCODE_SE(SeCode) 		(SeCode)
#define SDCODE_BGM(BgmCode) 	(BgmCode)
#define SDCODE_VOICE(VoiceCode) (VoiceCode)
#define MOVIECODE(MovieCode) 	(MovieCode)

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
#include <exception>
#include <string>
#include <vector>
#include <limits>

using int8 		= char;
using int16     = short;
using int32     = int;
using int64     = long long;
using uint8 	= unsigned char;
using uint16 	= unsigned short;
using uint32    = unsigned int;
using uint64 	= unsigned long long;

#ifdef _TARGET_PC
using wchar = wchar_t;
#define UTEXT(text)	L##text
#elif defined _TARGET_PS2
using wchar = unsigned int;
#define UTEXT(text)	text
#endif

#include "MemoryStd.hpp"
#include "Debug.hpp"

#include "System/Utils/List.hpp"
#include "System/Utils/Queue.hpp"
#include "System/Utils/Math.hpp"


#define COLOR_TO_INTEGER_RWRGBA(rwRGBAcolor) 	\
	uint32((rwRGBAcolor.alpha << 24) | (rwRGBAcolor.red << 16) | (rwRGBAcolor.green << 8) | rwRGBAcolor.blue)

#define COLOR_TO_INTEGER(r, g, b, a) 			\
	uint32((a << 24) | (r << 16) | (g << 8) | b)

#define CHECK_SIZE(type, targetSize)			\
    static_assert(sizeof(type) == targetSize, "incorrect size")

#define REF(p)          			(p)

#define STR(s)                      #s

#define COUNT_OF(ptr)	            int32(sizeof(ptr) / sizeof(ptr[0]))

#define BIT(no)			            (uint32(1 << (no)))

#define BITSOF(var)		            (sizeof(var) << 3)

#define BIT_SET(bitfield, no)       (bitfield |= (1 << no))

#define BIT_CLEAR(bitfield, no)     (bitfield &= ~(1 << no))

#define IS_BIT_SET(bitfield, no)    ((bitfield >> no) & 1)

#define FLAG_SET(maskfield, flag)\
	maskfield |= flag

#define FLAG_CLEAR(maskfield, flag)\
	maskfield &= ~flag

#define IS_FLAG_SET(flagfield, flag)\
	bool((flagfield & flag) == flag)

#define IS_FLAG_SET_ANY(flagfield, flag)\
	bool((flagfield & flag) != 0)

#define FLAG_CHANGE(flagfield, flag, set)	\
do											\
{											\
if (set)									\
	flagfield |= (flag);					\
else										\
	flagfield &= (~flag);					\
}											\
while (0)

#define ENUM_TO_FLAG(T)																\
	inline T operator~ (T a) { return (T)~(int32)a; }								\
	inline T operator| (T a, T b) { return (T)((int32)a | (int32)b); }				\
	inline T operator& (T a, T b) { return (T)((int32)a & (int32)b); }				\
	inline T operator^ (T a, T b) { return (T)((int32)a ^ (int32)b); }				\
	inline T& operator|= (T& a, T b) { return (T&)((int32&)a |= (int32)b); }		\
	inline T& operator&= (T& a, T b) { return (T&)((int32&)a &= (int32)b); }		\
	inline T& operator^= (T& a, T b) { return (T&)((int32&)a ^= (int32)b); }

#define ENUM_TO_FLAG_CLASS(T)														\
	inline friend T operator~ (T a) { return (T)~(int32)a; }						\
	inline friend T operator| (T a, T b) { return (T)((int32)a | (int32)b); }		\
	inline friend T operator& (T a, T b) { return (T)((int32)a & (int32)b); }		\
	inline friend T operator^ (T a, T b) { return (T)((int32)a ^ (int32)b); }		\
	inline friend T& operator|= (T& a, T b) { return (T&)((int32&)a |= (int32)b); }	\
	inline friend T& operator&= (T& a, T b) { return (T&)((int32&)a &= (int32)b); }	\
	inline friend T& operator^= (T& a, T b) { return (T&)((int32&)a ^= (int32)b); }

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

namespace TYPEDEF
{
	static const float DEFAULT_SCREEN_WIDTH = 640.0f;
	static const float DEFAULT_SCREEN_HEIGHT= 480.0f;
	static const float DEFAULT_ASPECTRATIO 	= (4.0f / 3.0f);
	static const float DEFAULT_VIEWWINDOW 	= (0.5f);
	static const float DEFAULT_CLIP_FAR 	= 100.0f;
	static const float DEFAULT_CLIP_NEAR 	= 0.1f;

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