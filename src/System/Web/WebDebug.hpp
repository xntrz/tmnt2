#pragma once


class CWebDebug
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Output(const char* fname, int32 fline, const char* format, ...);
    static void Fatal(const char* reason, ...);
    static void SetDispTimeFileLine(bool state);
    static void SetFatalReturn(bool state);
};