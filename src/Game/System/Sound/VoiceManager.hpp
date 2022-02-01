#pragma once

#include "SeGroupID.hpp"


class CVoiceManager
{
public:
    static void Initialize(void) {};
    static void Terminate(void) {};
    static void SetVoice(SEGROUPID::VALUE idSeGroup, bool bIsHelpID) {};
};