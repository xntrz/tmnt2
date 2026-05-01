#include "Sound.hpp"

#include "System/Common/Sound/SdDrv.hpp"
#include "System/Common/Sound/SdLoader.hpp"


void SoundFirstLoad(void)
{
    SoundLoad(0, 0);
};


void SoundLoad(int32 Command, int32 Param)
{
    SdDrvLoad(Command, 0, Param);
};


void SoundSet(int32 Code)
{
    SoundSetEx(Code, 0, 0, 0);
};


void SoundSetEx(int32 Code, uint32 Param1, uint32 Param2, uint32 Param3)
{
    SdDrvCodeSet(Code, Param1, Param2, Param3);
};


bool SoundPlaybackCheck(int32 Code)
{
    return SdDrvPlaybackCheck(Code);
};


bool SoundVoiceBusyCheck(void)
{
    return SdDrvVoiceBusyCheck();
};


int32 SoundGetLoadInfo(void)
{
    return SdDrvGetLoadInfo();
};


int32 SoundVoxCodeCheck(int32 SrvNo)
{
    return SdDrvVoxCodeCheck(SrvNo);
};