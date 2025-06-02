#pragma once

/* Output format specified for HRESULT */
#define PRIhr PRIxPTR


/* Wrapper for DirectSound objects calls that return HRESULT, for log if failed */
#ifdef _DEBUG
#define HRRET(f) 											\
do 															\
{															\
    HRESULT hResult = f;									\
    if (FAILED(hResult))									\
        SDLOG(#f" failed (hresult = %" PRIhr ")", hResult);	\
} while (0)
#else /* _DEBUG */
#define HRRET(f) (f)
#endif /* _DEBUG */


/* Common log */
#ifdef _DEBUG
#define SDLOG(format, ...)     SdLog(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
#else /* _DEBUG */
#define SDLOG(format, ...)     ((void)0)
#endif /* _DEBUG */


/* fatal macro */
#define SDFATAL(reason, ...)	SdFatal(reason, ##__VA_ARGS__)


/*
 * All function that should exist but not implemented(or some may be implemented) due of unusing in PC version of game
 * is marked with this macro in their bodies, mostly of this functions is: seq, seq ctrl or drv ctrl module
 */
#define SDNOIMPL() 				(SDFATAL("An attempt to call unimplemented func: " __FUNCTION__))


extern bool SdLogEnabled;


void SdLog(const char* fname, int fline, const char* format, ...);
void SdFatal(const char* reason, ...);