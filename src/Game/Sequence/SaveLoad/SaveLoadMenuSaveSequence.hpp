#pragma once

#include "SaveLoadSequence.hpp"


class CSaveLoadMenuSaveSequence final : public CSaveLoadSequenceBase
{
public:
    static CProcess* Instance(void);

    CSaveLoadMenuSaveSequence(void);
};