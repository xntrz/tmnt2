#include "SdLog.hpp"


/*extern*/ bool SdLogEnabled = false;


void SdLog(const char* fname, int fline, const char* format, ...)
{
#ifdef _DEBUG
    if (SdLogEnabled)
    {
        char buffer[4096];
        buffer[0] = '\0';

        char prefix[] = "[SOUND] ";
        std::strcat(buffer, prefix);

        va_list vl;
        va_start(vl, format);
        int written = std::vsnprintf(
            buffer + (sizeof(prefix) - 1),
            sizeof(buffer) - (sizeof(prefix) - 1),
            format,
            vl
        );
        va_end(vl);

        if (written >= 0)
        {
            int bufferLen = ((sizeof(prefix) - 1) + written);
            int bufferLenRemain = sizeof(buffer) - bufferLen;

            if ((buffer[bufferLen - 1] != '\n') &&
                (bufferLenRemain >= 2))
            {
                buffer[bufferLen + 0] = '\n';
                buffer[bufferLen + 1] = '\0';
            };
        };

        CDebug::Output(fname, fline, buffer);
    };
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