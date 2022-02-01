#pragma once

#include "SaveLoadSequence.hpp"


class CSaveLoadCheckSequence final : public CSaveLoadSequenceBase
{
public:
    static CProcess* Instance(void);

    CSaveLoadCheckSequence(void);
};