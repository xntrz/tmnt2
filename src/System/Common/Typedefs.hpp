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


typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#ifdef _TARGET_PC
typedef wchar_t wchar;
#define UTEXT(text)	L##text
#elif defined _TARGET_PS2
typedef unsigned int wchar;
#define UTEXT(text)	text
#endif

#include "MemoryStd.hpp"
#include "Debug.hpp"

#include "System/Utils/List.hpp"
#include "System/Utils/Queue.hpp"
#include "System/Utils/Math.hpp"

#define CHECK_SIZE(type, targetSize)	static_assert(sizeof(type) == targetSize, "incorrect size")
#define RWRGBALONGEX(rwrgba) 			(RWRGBALONG(rwrgba.red, rwrgba.green, rwrgba.blue, rwrgba.alpha))
#define REF(p)          				(p)
#define STR(s)                      	#s
#define COUNT_OF(ptr)	            	int32(sizeof(ptr) / sizeof(ptr[0]))

#define ALIGN_CHECK(v, a) 				((uint32(v) & ((uint32(a) - 1u))) == 0u)
#define ALIGN_ADJUST(v, a) 				(uint32(a) - (uint32(v) & (uint32(a) - 1u)))
#define ALIGN_ROUND_DOWN(v, a) 			(uint32(v) & ~(uint32(a) - 1u))
#define ALIGN_ROUND_UP(v, a) 			((uint32(v) + (uint32(a) - 1u)) & ~(uint32(a) - 1u))
#define ALIGN(v, a) 					(ALIGN_ROUND_UP(v, a))

#define BIT(no)			            	(1 << (no))
#define BITSOF(var)		            	(sizeof(var) << 3)
#define BIT_SET(bitfield, no)       	(bitfield |= (1 << no))
#define BIT_CLEAR(bitfield, no)     	(bitfield &= ~(1 << no))
#define BIT_TEST(bitfield, no)    		((bitfield >> no) & 1)

#define FLAG_SET(flagfield, flag)		((flagfield) |= (flag))
#define FLAG_CLEAR(flagfield, flag)		((flagfield) &= (~flag))
#define FLAG_TEST(flagfield, flag)		((flagfield & flag) == flag)
#define FLAG_TEST_ANY(flagfield, flag)	((flagfield & flag) != 0)
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

#define SWAP2(val)					\
	(((val) >> 8) & 0x00FF) | 		\
	(((val) << 8) & 0xFF00)

#define SWAP4(val)					\
	(((val) >> 24) & 0x000000FF) |	\
	(((val) >> 8)  & 0x0000FF00) | 	\
	(((val) << 8)  & 0x00FF0000) | 	\
	(((val) << 24) & 0xFF000000)

#define SWAP8(val)							\
	(((val) >> 56) & 0x00000000000000FF) |	\
	(((val) >> 40) & 0x000000000000FF00) | 	\
	(((val) >> 24) & 0x0000000000FF0000) |	\
	(((val) >> 8)  & 0x00000000FF000000) | 	\
	(((val) << 8)  & 0x000000FF00000000) |	\
	(((val) << 24) & 0x0000FF0000000000) |	\
	(((val) << 40) & 0x00FF000000000000) |	\
	(((val) << 56) & 0xFF00000000000000)

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
	static const float DEFAULT_ASPECTRATIO 	= (DEFAULT_SCREEN_WIDTH / DEFAULT_SCREEN_HEIGHT);
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