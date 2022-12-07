#pragma once

#include "PCTypedefs.hpp"


class CPCDebug
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void ReplaceConsole(void);
    static void MaximizeConsole(bool state);
    static void Output(const char* fname, int32 fline, const char* format, ...);
    static void Fatal(const char* reason, ...);
};