#include "StaffRollSequence.hpp"


/*static*/ CProcess* CStaffRollSequence::Instance(void)
{
    return new CStaffRollSequence;
};


CStaffRollSequence::CStaffRollSequence(void)
{
    ;
};


CStaffRollSequence::~CStaffRollSequence(void)
{
    ;
};


bool CStaffRollSequence::OnAttach(const void* param)
{
    return true;
};


void CStaffRollSequence::OnDetach(void)
{
    ;
};


void CStaffRollSequence::OnMove(bool bResume, const void* param)
{
    ;
};


void CStaffRollSequence::OnDraw(void) const
{
    ;
};