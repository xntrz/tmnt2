#include "adxt.h"
#include "adxf.h"
#include "adxcrs.h"
#include "cvfs.h"


static CriSint32 adxt_init_cnt = 0;


void CRIAPI
ADXT_Init(void)
{
    if (!adxt_init_cnt++)
    {
        ADXCRS_Init();
        ADXF_Init();
    };
};


void CRIAPI
ADXT_Finish(void)
{
    if (adxt_init_cnt == 0)
        return;
    
    if (!--adxt_init_cnt)
    {
        ADXF_Finish();
        ADXCRS_Finish();
    };
};


void CRIAPI
ADXT_ExecFsSvr(void)
{
    ADXCRS_Enter();
    cvFsExecServer();
    cvFsExecIdle();
    ADXF_ExecServer();
    ADXCRS_Leave();
};


void CRIAPI
ADXT_ExecFsServer(void)
{
    ADXCRS_Enter();
    ADXT_ExecFsSvr();
    ADXCRS_Leave();
};