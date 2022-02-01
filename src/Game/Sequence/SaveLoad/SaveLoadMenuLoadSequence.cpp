#include "SaveLoadMenuLoadSequence.hpp"


/*static*/ CProcess* CSaveLoadMenuLoadSequence::Instance(void)
{
    return new CSaveLoadMenuLoadSequence;
};


CSaveLoadMenuLoadSequence::CSaveLoadMenuLoadSequence(void)
: CSaveLoadSequenceBase(CSaveLoad::TYPE_LOAD)
{
    ;
};