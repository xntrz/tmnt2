#include "Sound.hpp"

#ifdef TARGET_PC
#include "System/PC/Sound/SdDrv.hpp"
#include "System/PC/Sound/SdLoader.hpp"
#else
#error Not implemented for current target
#endif


void SoundFirstLoad(void)
{
    SoundLoad(0, 0);
};


void SoundLoad(int32 Command, int32 Param)
{
#ifdef TARGET_PC
    SdDrvLoad(Command, 0, Param);
#else
#error Not implemented for current target
#endif
};


void SoundSet(int32 Code)
{
    SoundSetEx(Code, 0, 0, 0);
};


void SoundSetEx(int32 Code, uint32 Param1, uint32 Param2, uint32 Param3)
{
#ifdef TARGET_PC
    SdDrvCodeSet(Code, Param1, Param2, Param3);
#else
#error Not implemented for current target
#endif
};


bool SoundPlaybackCheck(int32 Code)
{
#ifdef TARGET_PC
    return SdDrvPlaybackCheck(Code);
#else
#error Not implemented for current target
#endif
};


bool SoundVoiceBusyCheck(void)
{
#ifdef TARGET_PC
    return SdDrvVoiceBusyCheck();
#else
#error Not implemented for current target
#endif
};


bool SoundGetLoadInfo(void)
{
#ifdef TARGET_PC
    return SdDrvGetLoadInfo();
#else
#error Not implemented for current target
#endif
};


int32 SoundVoxCodeCheck(int32 SrvNo)
{
#ifdef TARGET_PC
    return SdDrvVoxCodeCheck(SrvNo);
#else
#error Not implemented for current target
#endif
};