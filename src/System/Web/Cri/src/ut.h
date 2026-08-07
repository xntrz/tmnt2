#ifndef UT_H
#define UT_H

#include "cri_xpt.h"

#define CRI_MAX_FSIZE_32 \
    (0x7FFFFFFF)

#define CRI_ALIGN_ROUND_UP(v, a) \
    (((v) + ((a) - 1u)) & ~((a) - 1u))

#define CRI_ALIGN(v, a) \
    (CRI_ALIGN_ROUND_UP(v, a))

#define CRI_NSCT_TO_SIZE(nsct, sctsz) \
    ((CriSint32)((nsct) * (sctsz)))

#define CRI_SIZE_TO_NSCT(size, sctsz) \
    ((CriSint32)(CRI_ALIGN((size), (sctsz)) / (sctsz)))

#define CRI_COUNTOF(x) \
    ((CriSint32)(sizeof(x) / sizeof(x[0])))

#if defined(_MSC_VER)
    #define CRI_STR(x) \
        (x " (" __FUNCTION__ ")")
#else /* defined(_MSC_VER) */
    extern CriChar8 __ut_c_cristr[];
    #define CRI_STR(x)                                          \
        (__ut_c_cristr[0] = '\0',                               \
         UT_Sprintf(__ut_c_cristr, "%s (%s)", x, __FUNCTION__), \
        __ut_c_cristr)
#endif /* defined(_MSC_VER) */


void UT_OsSleep(CriUint32 ms);
void UT_OsIdle(void);
void UT_Sprintf(CriChar8* buf, const CriChar8* fmt, ...);

#endif /* UT_H */
