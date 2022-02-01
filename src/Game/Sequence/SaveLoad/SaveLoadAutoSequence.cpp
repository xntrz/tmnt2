#include "SaveLoadAutoSequence.hpp"


/*static*/ CProcess* CSaveLoadAutoSequence::Instance(void)
{
    return new CSaveLoadAutoSequence;
};


CSaveLoadAutoSequence::CSaveLoadAutoSequence(void)
: CSaveLoadSequenceBase(CSaveLoad::TYPE_AUTOSAVE)
{
    ;
};