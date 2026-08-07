#include "WebError.hpp"
#include "WebDebug.hpp"


/*static*/ void CWebError::Show(const char* Text)
{
    CWebDebug::SetFatalReturn(true);
    CWebDebug::Fatal(Text);
    CWebDebug::SetFatalReturn(false);
};


/*static*/ void CWebError::ShowNoRet(const char* Text)
{
    CWebDebug::SetFatalReturn(false);
    CWebDebug::Fatal(Text);
};