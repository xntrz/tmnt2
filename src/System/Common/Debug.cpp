#include "Debug.hpp"


// 
//  Message tags:
//      [RW]    - RenderWare debug messages
//      [SYS]   - System module debug messages
//      [GAME]  - Game module debug messages 
//      [AFS]   - Afs archive subsystem messages
//      [SND]   - Sound subsystem messages
//      [MOV]   - Movie subsystem messages
// 


#ifdef RWDEBUG
static void RwDebugMsgEndpoint(RwDebugType type, const RwChar* string)
{
    switch (type)
	{
	case RwDebugType::rwDEBUGASSERT:
		ASSERT(false, string);
		break;

    case RwDebugType::rwDEBUGTRACE:
        //OUTPUT("[RW::TRACE] %s\n", string);
        break;
        
    case RwDebugType::rwDEBUGMESSAGE:
        //OUTPUT("[RW::MSG] %s\n", string);
        break;
        
    case RwDebugType::rwDEBUGERROR:
		OUTPUT("[RW::ERROR] %s\n", string);
		break;

	default:
		ASSERT(false);
		break;
	};
};
#endif


/*static*/ void(*CDebug::Output)(const char* format, ...) = nullptr;
/*static*/ void(*CDebug::Fatal)(const char* reason) = nullptr;


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


/*static*/ void CDebug::StartRWDebug(void)
{
#ifdef RWDEBUG    
    RwDebugSetHandler(RwDebugMsgEndpoint);
    RwDebugSetTraceState(TRUE);
#endif
};


/*static*/ void CDebug::StopRWDebug(void)
{
#ifdef RWDEBUG
    RwDebugSetTraceState(FALSE);
    RwDebugSetHandler(NULL);
#endif
};


/*static*/ void CDebug::Assert(const char* expression, const char* fname, int32 fline)
{
    Assert(expression, fname, fline, "Unknown error.\nProgram is ended compulsorily.\n");
};


/*stat*/ void CDebug::Assert(const char* expression, const char* fname, int32 fline, const char* format, ...)
{
    static char buff[4096] = { 0 };

    int32 written = sprintf_s(buff, sizeof(buff),
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
        written += vsprintf_s(buff + written, sizeof(buff) - written, format, vl);
        va_end(vl);
    };

	Output(buff);
    Fatal(buff);
};
