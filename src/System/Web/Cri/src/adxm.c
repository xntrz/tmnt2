#include "adxm.h"
#include "adxt.h"
#include "adxf.h"
#include "ut.h"


void CRIAPI
ADXM_SetupThrd(ADXM_TPRM* tprm)
{
    ;
};


void CRIAPI
ADXM_ShutdownThrd(void)
{
    ;
};


CriSint32 CRIAPI
ADXM_ExecMain(void)
{
    ADXT_ExecFsServer();
    UT_OsIdle();
    return 0;
};