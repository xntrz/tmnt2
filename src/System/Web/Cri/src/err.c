#include "err.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>


void ERR_CallErrFunc(const CriChar8* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char errbuf[2048];
    errbuf[0] = '\0';

    vsprintf(errbuf, fmt, args);
    printf("[CRI ERR] %s\n", errbuf);

#ifdef _DEBUG
    assert(CRI_FALSE && "CRI Error occured");
#endif /* _DEBUG */
    
    va_end(args);
};