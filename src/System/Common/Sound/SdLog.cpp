#include "SdLog.hpp"
#include "SdLoader.hpp"
#include "SdThread.hpp"


static FILE* SdLogBankFptr = nullptr;
static FILE* SdLogCodeFptr = nullptr;
static SdMutex_t* SdLogMutex = nullptr;
static bool SdLogEnabled = false;
static bool SdLogLoad = false;


static inline const char*
MakeLogLoadFName(const char* _filename)
{
    static thread_local char filename[256];
    SdMakeFilename(filename, _filename);
    return filename;
};


void SdLogInitialize(bool bEnable, bool bLogLoad)
{
    SdLogEnabled = bEnable;
    SdLogLoad = bLogLoad;

    if (SdLogLoad)
        SdLogMutex = SdMutexCreate();
};


void SdLogTerminate(void)
{
    if (SdLogBankFptr)
    {
        std::fclose(SdLogBankFptr);
        SdLogBankFptr = nullptr;
    };

    if (SdLogCodeFptr)
    {
        std::fclose(SdLogCodeFptr);
        SdLogCodeFptr = nullptr;
    };

    if (SdLogMutex)
    {
        SdMutexDestroy(SdLogMutex);
        SdLogMutex = nullptr;
    };

    SdLogLoad = false;
    SdLogEnabled = false;
};


void SdLogBankLoad(int32 _bank)
{
    if (!SdLogLoad)
        return;

    SdMutexLock(SdLogMutex);

    if (!SdLogBankFptr)
        SdLogBankFptr = std::fopen(MakeLogLoadFName("sdlog_load_bank.txt"), "w");

    if (SdLogBankFptr)
    {
        std::fprintf(SdLogBankFptr, "%d\n", _bank);
        std::fflush(SdLogBankFptr);
    };

    SdMutexUnlock(SdLogMutex);
};


void SdLogCodeLoad(int32 _code)
{
    if (!SdLogLoad)
        return;

    SdMutexLock(SdLogMutex);

    if (!SdLogCodeFptr)
        SdLogCodeFptr = std::fopen(MakeLogLoadFName("sdlog_load_code.txt"), "w");

    if (SdLogCodeFptr)
    {
        std::fprintf(SdLogCodeFptr, "0x%X\n", _code);
        std::fflush(SdLogCodeFptr);
    };

    SdMutexUnlock(SdLogMutex);
};


void SdLog(const char* fname, int fline, const char* format, ...)
{
#ifdef _DEBUG
    if (!SdLogEnabled)
        return;

    char buffer[4096];    
    buffer[0] = '\0';

    va_list vl;
    va_start(vl, format);
    int32 written = std::vsnprintf(buffer, sizeof(buffer), format, vl);
    written = Clamp(written, 0, (int32)(sizeof(buffer) - 1));
    va_end(vl);

    char text[4096];
    text[0] = '\0';

    std::snprintf(text, sizeof(text), "[SOUND] %s", buffer);

    CDebug::Output(fname, fline, text);
#endif /* _DEBUG */	
};


void SdFatal(const char* reason, ...)
{
    char buffer[4096];
    buffer[0] = '\0';

    va_list vl;
    va_start(vl, reason);
    std::vsnprintf(buffer, sizeof(buffer), reason, vl);
    va_end(vl);

    CDebug::Fatal(buffer);
};