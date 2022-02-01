#pragma once

#include "PCTypedefs.hpp"


class CPCDebug
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void ReplaceConsole(void);
    static void Output(const char* format, ...);
    static void Fatal(const char* reason);
};