#include "SaveLoadCheckSequence.hpp"


/*static*/ CProcess* CSaveLoadCheckSequence::Instance(void)
{
    return new CSaveLoadCheckSequence;
};


CSaveLoadCheckSequence::CSaveLoadCheckSequence(void)
: CSaveLoadSequenceBase(CSaveLoad::TYPE_CHECK)
{
    ;
};