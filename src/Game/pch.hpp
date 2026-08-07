#pragma once

#include "System/pch.hpp"

#include "Game/System/Movie/MoviePreprocess.hpp"
#include "Game/System/Sound/SoundPreprocess.hpp"
#include "Game/System/Utils/MathFunction.hpp"
#include "Game/System/Utils/List.hpp"


#if defined(TARGET_PC)
    #define TMNT2_FEATURE_KEYBOARD
    #define TMNT2_FEATURE_MOVIE
    #define TMNT2_FEATURE_DISPLAYRESO
#elif defined(TARGET_WEB)
    #define TMNT2_FEATURE_KEYBOARD
    #define TMNT2_FEATURE_TOUCHCONTROLLER
    //#define TMNT2_FEATURE_MOVIE
#endif

#if defined(TMNT2_TRIAL)
#undef TMNT2_FEATURE_MOVIE
#endif /* defined(TMNT2_TRIAL) */


#define ANIM_FRAMERATE                  (60.0f)
#define ANIM_DURATION_FRAMES(frames)    (static_cast<float>(frames) / ANIM_FRAMERATE)


/* copy of DEFINE_ENUM_FLAG_OPERATORS macro from winnt header - that allows to use bit operations whith enums */
template<size_t sz>
struct _underlying_type_size;

template<>
struct _underlying_type_size<sizeof(uint64)> { typedef uint64 type; };

template<>
struct _underlying_type_size<sizeof(uint32)> { typedef uint32 type; };

template<>
struct _underlying_type_size<sizeof(uint16)> { typedef uint16 type; };

template<>
struct _underlying_type_size<sizeof(uint8)>  { typedef uint8 type; };

template<class T>
struct _underlying_type_uint { typedef typename _underlying_type_size<sizeof(T)>::type type; };


#if (__cplusplus >= 201402)
#define ENUM_FLAG_OP_CONSTEXPR 
#else
#define ENUM_FLAG_OP_CONSTEXPR
#endif 
    

#define DEFINE_ENUM_FLAG_OPS(T, ...)                                        \
    __VA_ARGS__ inline ENUM_FLAG_OP_CONSTEXPR T operator~ (T a)             \
    {                                                                       \
        using type = typename _underlying_type_uint<T>::type;               \
        return T(~((type)a));                                               \
    };                                                                      \
                                                                            \
    __VA_ARGS__ inline ENUM_FLAG_OP_CONSTEXPR T operator| (T a, T b)        \
    {                                                                       \
        using type = typename _underlying_type_uint<T>::type;               \
        return T(((type)a) | ((type)b));                                    \
    };                                                                      \
                                                                            \
    __VA_ARGS__ inline ENUM_FLAG_OP_CONSTEXPR T operator& (T a, T b)        \
    {                                                                       \
        using type = typename _underlying_type_uint<T>::type;               \
        return T(((type)a) & ((type)b));                                    \
    };                                                                      \
                                                                            \
    __VA_ARGS__ inline ENUM_FLAG_OP_CONSTEXPR T operator^ (T a, T b)        \
    {                                                                       \
        using type = typename _underlying_type_uint<T>::type;               \
        return T(((type)a) ^ ((type)b));                                    \
    };                                                                      \
                                                                            \
    __VA_ARGS__ inline ENUM_FLAG_OP_CONSTEXPR T& operator|= (T& a, T b)     \
    {                                                                       \
        using type = typename _underlying_type_uint<T>::type;               \
        return (T&)(((type&)a) |= ((type)b));                               \
    };                                                                      \
                                                                            \
    __VA_ARGS__ inline ENUM_FLAG_OP_CONSTEXPR T& operator&= (T& a, T b)     \
    {                                                                       \
        using type = typename _underlying_type_uint<T>::type;               \
        return (T&)(((type&)a) &= ((type)b));                               \
    };                                                                      \
                                                                            \
    __VA_ARGS__ inline ENUM_FLAG_OP_CONSTEXPR T& operator^= (T& a, T b)     \
    {                                                                       \
        using type = typename _underlying_type_uint<T>::type;               \
        return (T&)(((type&)a) ^= ((type)b));                               \
    }

