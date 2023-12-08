#include "PCError.hpp"
#include "PCDebug.hpp"


/*static*/ void CPCError::Show(const char* Text)
{
    CPCDebug::SetFatalReturn(true);
    CPCDebug::Fatal(Text);
    CPCDebug::SetFatalReturn(false);
};


/*static*/ void CPCError::ShowNoRet(const char* Text)
{
    CPCDebug::SetFatalReturn(false);
    CPCDebug::Fatal(Text);
};