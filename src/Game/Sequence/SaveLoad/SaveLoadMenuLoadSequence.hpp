#pragma once

#include "SaveLoadSequence.hpp"


class CSaveLoadMenuLoadSequence final : public CSaveLoadSequenceBase
{
public:
    static CProcess* Instance(void);

    CSaveLoadMenuLoadSequence(void);
};