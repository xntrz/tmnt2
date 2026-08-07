#include "adxcrs.h"
#include "crs.h"
#include "err.h"
#include "ut.h"


static CriChar8 adxcrs_wk_area[CRI_CRS_WK_SIZE];
static CRICRS adxcrs_obj = CRI_NULL;
static CriSint32 adxcrs_lvl = 0;
static CriSint32 adxcrs_init_cnt = 0;


void
ADXCRS_Init(void)
{
    if (!adxcrs_init_cnt++)
    {
        adxcrs_obj = CRICRS_Create(&adxcrs_wk_area, sizeof(adxcrs_wk_area));
        adxcrs_lvl = 0;

        if (adxcrs_obj == CRI_NULL)
            ERR_CallErrFunc(CRI_STR("failed to create critical section"));
    };
};


void
ADXCRS_Finish(void)
{
    if (adxcrs_init_cnt > 0)
    {
        if (!--adxcrs_init_cnt)
        {
            if (adxcrs_obj != CRI_NULL)
            {
                CRICRS_Destroy(adxcrs_obj);
                adxcrs_obj = CRI_NULL;
            };

            adxcrs_lvl = 0;
        };
    };
};


void
ADXCRS_Enter(void)
{
    CRICRS_Enter(adxcrs_obj);
};


void
ADXCRS_Leave(void)
{
    CRICRS_Leave(adxcrs_obj);
};


void
ADXCRS_Lock(void)
{
    ;
};


void
ADXCRS_Unlock(void)
{
    ;
};