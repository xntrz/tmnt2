#include "Sound.hpp"

#ifdef TARGET_PC
#include "System/PC/Sound/SdDrv.hpp"
#include "System/PC/Sound/SdLoader.hpp"
#endif /* TARGET_PC */


void SoundFirstLoad(void)
{
    SoundLoad(0, 0);
};


void SoundLoad(int32 Command, int32 Param)
{
#ifdef TARGET_PC
    SdDrvLoad(Command, 0, Param);
#endif /* TARGET_PC */
};


void SoundSet(int32 Code)
{
    SoundSetEx(Code, 0, 0, 0);
};


void SoundSetEx(int32 Code, uint32 Param1, uint32 Param2, uint32 Param3)
{
#ifdef TARGET_PC
    SdDrvCodeSet(Code, Param1, Param2, Param3);
#endif /* TARGET_PC */
};


bool SoundPlaybackCheck(int32 Code)
{
#ifdef TARGET_PC
    return SdDrvPlaybackCheck(Code);
#endif /* TARGET_PC */
};


bool SoundVoiceBusyCheck(void)
{
#ifdef TARGET_PC
    return SdDrvVoiceBusyCheck();
#endif /* TARGET_PC */
};


bool SoundGetLoadInfo(void)
{
#ifdef TARGET_PC
    return SdDrvGetLoadInfo();
#endif /* TARGET_PC */
};


int32 SoundVoxCodeCheck(int32 SrvNo)
{
#ifdef TARGET_PC
    return SdDrvVoxCodeCheck(SrvNo);
#endif /* TARGET_PC */
};