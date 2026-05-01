#include "SdAL.hpp"
#include "SdTable.hpp"
#include "SdLog.hpp"
#include "SdTypes.hpp"

#include <AL/alc.h>
#include <AL/alext.h>

#if defined(TARGET_WEB)
#include <emscripten.h>
#endif /* defined(TARGET_WEB) */


static ALCdevice* SdAlDevice = nullptr;
static ALCcontext* SdAlContext = nullptr;


#if defined(TARGET_WEB)
EM_JS(void, SdALWebResumeContext, (), {
    if (typeof AL !== 'undefined' && AL.currentContext && AL.currentContext.ctx) {
        AL.currentContext.ctx.resume();
    }
});
#endif /* defined(TARGET_WEB) */


static inline const char*
SdALGetDeviceError(void)
{
    ALCenum DeviceErr = alcGetError(SdAlDevice);
    if (DeviceErr != ALC_NO_ERROR)
        return alcGetString(SdAlDevice, DeviceErr);

    return "ALC NO ERROR";
};


bool SdALInitialize(void)
{
    SdAlDevice = alcOpenDevice(nullptr);
    if (!SdAlDevice)
    {
        SDLOG("AL device create failed! (0x%x)\n", alGetError());
        return false;
    };

    SdAlContext = alcCreateContext(SdAlDevice, nullptr);
    if (!SdAlContext)
    {
        SDLOG("AL context create failed: %s\n", SdALGetDeviceError());
        return false;
    };

    if (!alcMakeContextCurrent(SdAlContext))
    {
        SDLOG("AL context make current failed: %s\n", SdALGetDeviceError());
        return false;
    };

#if defined(TARGET_WEB)
    SdALWebResumeContext();
#endif /* defined(TARGET_WEB) */

    return true;
};


void SdALTerminate(void)
{
    alcMakeContextCurrent(nullptr);
    
    if (SdAlContext)
    {
        alcDestroyContext(SdAlContext);
        SdAlContext = nullptr;
    };

    if (SdAlDevice)
    {
        alcCloseDevice(SdAlDevice);
        SdAlDevice = nullptr;
    };
};


int32 SdALConvVolToDsVol(int32 _vol)
{
    /* converts framework volume to directsound volume type */

    if (_vol > 0x3FFF)
        _vol = 0x3FFF;

    int32 idx = (_vol >> 6);
    return -SdDsVolumeTable[idx];
};


float SdALConvDsVolToGain(int32 _volDS)
{
    float gain = std::powf(10.0f, (float)_volDS / 2000.0f);

    if (gain < 0.0f)
        gain = 0.0f;

    return gain;
};


int32 SdALBuffSetPan(ALuint _source, int32 _chLeft, int32 _chRight)
{
    ASSERT(_source != 0);

    int32 _chHigh = (_chLeft > _chRight) ? _chLeft :
                                           _chRight;

    float pan = 0.0f;
    int32 total = _chLeft + _chRight;

    if (total > 0)
        pan = (float)(_chRight - _chLeft) / (float)total;

    alCall(alSource3f(_source, AL_POSITION, pan, 0.0f, 0.0f));

    return _chHigh;
};


int32 SdALGetWaveFormat(const SdWaveDataHdr_t* _header)
{
    return (_header->Channels == 1) ? AL_FORMAT_MONO16 :
                                      AL_FORMAT_STEREO16;
};


bool SdALIsBufferSubDataSupport(void)
{
    return false;
};


bool SdALActivateCtx(void)
{
    if (!alcMakeContextCurrent(SdAlContext))
    {
        SDLOG("AL activate ctx failed: %s\n", SdALGetDeviceError());
        return false;
    };

    return true;
};


void SdALDeactivateCtx(void)
{
    if (!alcMakeContextCurrent(nullptr))
    {
        SDLOG("AL deactivate ctx failed: %s\n", SdALGetDeviceError());
    };
};