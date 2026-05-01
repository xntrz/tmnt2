#pragma once

#include "SdLog.hpp"

#include <AL/al.h>


#ifdef _DEBUG
#define alCall(x)                                               \
do                                                              \
{                                                               \
    (x);                                                        \
                                                                \
    if (alGetError() != AL_NO_ERROR) {                          \
        SDLOG(#x" call error: %s\n", alGetString(alGetError()));\
    };                                                          \
} while(false)
#else /* _DEBUG */
#define alCall(x) (x)
#endif /* _DEBUG */


struct SdWaveDataHdr_t;


bool SdALInitialize(void);
void SdALTerminate(void);
int32 SdALConvVolToDsVol(int32 _vol);
float SdALConvDsVolToGain(int32 _volDS);
int32 SdALBuffSetPan(ALuint _source, int32 _chLeft, int32 _chRight);
int32 SdALGetWaveFormat(const SdWaveDataHdr_t * _header);
bool SdALIsBufferSubDataSupport(void);
bool SdALActivateCtx(void);
void SdALDeactivateCtx(void);