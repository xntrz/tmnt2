#pragma once

#ifdef _DEBUG

#define SDLOG(format, ...) \
    SdLog(__FILENAME__, __LINE__, format, ##__VA_ARGS__)

#else /* _DEBUG */

#define SDLOG(format, ...) \
    ((void)0)

#endif /* _DEBUG */


#define SDFATAL(reason, ...) \
    SdFatal(reason, ##__VA_ARGS__)


/*
 * All function that should exist but not implemented (or some may be implemented)
 * due of unusing in PC version of game is marked with this macro in their bodies,
 * mostly of this functions is: seq, seq ctrl or drv ctrl module
 */
#define SDNOIMPL() \
    (SDFATAL("An attempt to call unimplemented func: ", __FUNCTION__))


void SdLogInitialize(bool bEnable, bool bLogLoad);
void SdLogTerminate(void);
void SdLogBankLoad(int32 _bank);
void SdLogCodeLoad(int32 _code);
void SdLog(const char* fname, int fline, const char* format, ...);
void SdFatal(const char* reason, ...);