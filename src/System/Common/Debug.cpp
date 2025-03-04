#include "Debug.hpp"


#ifdef RWDEBUG
static bool s_bRwDebugAssertSupressFlag = false;

static void RwDebugMsgEndpoint(RwDebugType type, const RwChar* string)
{
    switch (type)
    {
    case RwDebugType::rwDEBUGASSERT:
        //if (!s_bRwDebugAssertSupressFlag)
        //    ASSERT(false, string);
        //else
        //    OUTPUT("[rwDEBUGASSERT] %s\n", string);
        break;

    case RwDebugType::rwDEBUGTRACE:
        break;
        
    case RwDebugType::rwDEBUGMESSAGE:
        //OUTPUT("[rwDEBUGMESSAGE] %s\n", string);
        break;
        
    case RwDebugType::rwDEBUGERROR:
        OUTPUT("[rwDEBUGERROR] %s\n", string);
        break;

    default:
        ASSERT(false);
        break;
    };
};
#endif /* RWDEBUG */


/*static*/ void(*CDebug::Output)(const char* fname, int32 fline, const char* format, ...) = nullptr;
/*static*/ void(*CDebug::Fatal)(const char* reason, ...) = nullptr;


/*static*/ void CDebug::Initialize(void)
{
    Output = nullptr;
    Fatal = nullptr;
};


/*static*/ void CDebug::Terminate(void)
{
    Output = nullptr;
    Fatal = nullptr;
};


/*static*/ void CDebug::StartRwDebug(void)
{
#ifdef RWDEBUG    
    RwDebugSetHandler(RwDebugMsgEndpoint);
    RwDebugSetTraceState(TRUE);
    s_bRwDebugAssertSupressFlag = false;
#endif /* RWDEBUG */
};


/*static*/ void CDebug::StopRwDebug(void)
{
#ifdef RWDEBUG
    RwDebugSetTraceState(FALSE);
    RwDebugSetHandler(NULL);
#endif /* RWDEBUG */
};


/*static*/ void CDebug::SupressRwDebugAssert(bool bSupress)
{
#ifdef RWDEBUG
    s_bRwDebugAssertSupressFlag = bSupress;
#endif /* RWDEBUG */
};


/*static*/ void CDebug::Assert(const char* expression, const char* fname, int32 fline)
{
    Assert(expression, fname, fline, "Unknown error.\nProgram is ended compulsorily.\n");
};


/*stat*/ void CDebug::Assert(const char* expression, const char* fname, int32 fline, const char* format, ...)
{
    static char buff[4096] = { 0 };

    int32 written = std::sprintf(
        buff,
        "Assertion!\n"
        "\n"
        "Expression: %s\n"
        "File: %s(%u)\n"
        "Description:\n",
        expression,
        fname,
        fline
    );

    if (written)
    {
        va_list vl;
        va_start(vl, format);
        written += std::vsprintf(buff + written, format, vl);
        va_end(vl);
    };

    Output(fname, fline, buff);
    Fatal(buff);
};
