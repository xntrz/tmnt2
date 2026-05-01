#include "SoundDevice.hpp"
#include "Configure.hpp"


CSoundDevice::CSoundDevice(void)
{
    ;
};


CSoundDevice::~CSoundDevice(void)
{
    ;
};


uint32 CSoundDevice::GetFrameworkOptFlags(void) const
{
    uint32 OptFlag = 0;

    if (CConfigure::CheckArg("novag"))
        OptFlag |= 0x1;

    if (CConfigure::CheckArg("novox"))
        OptFlag |= 0x2;

    if (CConfigure::CheckArg("noseq"))
        OptFlag |= 0x4;

    if (CConfigure::CheckArg("sdlog"))
        OptFlag |= 0x8;

    if (CConfigure::CheckArg("sdlogldr"))
        OptFlag |= 0x10;

    return OptFlag;
};