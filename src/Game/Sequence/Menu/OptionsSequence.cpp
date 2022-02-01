#include "OptionsSequence.hpp"


/*static*/ CProcess* COptionsSequence::Instance(void)
{
    return new COptionsSequence;
};


COptionsSequence::COptionsSequence(void)
: CAnim2DSequence("BG")
{
    ;
};


COptionsSequence::~COptionsSequence(void)
{
    ;
};


bool COptionsSequence::OnAttach(const void* param)
{
    return true;
};


void COptionsSequence::OnDetach(void)
{
    ;
};


void COptionsSequence::OnMove(bool bResume, const void* param)
{
    ;
};


void COptionsSequence::OnDraw(void) const
{
    ;
};