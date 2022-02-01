#include "SaveLoadMenuSaveSequence.hpp"


/*static*/ CProcess* CSaveLoadMenuSaveSequence::Instance(void)
{
    return new CSaveLoadMenuSaveSequence;
};


CSaveLoadMenuSaveSequence::CSaveLoadMenuSaveSequence(void)
: CSaveLoadSequenceBase(CSaveLoad::TYPE_SAVE)
{

};