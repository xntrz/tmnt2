#include "MovieSequence.hpp"


/*static*/ CProcess* CMovieSequence::Instance(void)
{
    return new CMovieSequence;    
};


CMovieSequence::CMovieSequence(void)
{
    ;
};


CMovieSequence::~CMovieSequence(void)
{
    ;
};


bool CMovieSequence::OnAttach(const void* param)
{
    OUTPUT("[GAME] MovieSequence attach. MovieID: %d\n", param);
    return true;
};


void CMovieSequence::OnDetach(void)
{
    OUTPUT("[GAME] MovieSequence detach\n");
};


void CMovieSequence::OnMove(bool bResume, const void* param)
{
    Ret();
};


void CMovieSequence::OnDraw(void) const
{
    ;
};

