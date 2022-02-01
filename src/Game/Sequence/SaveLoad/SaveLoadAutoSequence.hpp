#pragma once

#include "SaveLoadSequence.hpp"


class CSaveLoadAutoSequence final : public CSaveLoadSequenceBase
{
public:
    static CProcess* Instance(void);

    CSaveLoadAutoSequence(void);
};