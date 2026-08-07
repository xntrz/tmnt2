#include "crs.h"
#include "err.h"
#include "ut.h"


typedef struct
{
    void* dummy;
} CRICRS_obj;


CRICRS
CRICRS_Create(void* wkarea, CriSint32 wksize)
{
    if (wkarea == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("work area is NULL"));
        return CRI_NULL;
    };

    if (wksize < CRI_CRS_WK_SIZE)
    {
        ERR_CallErrFunc(CRI_STR("work area size is too short"));
        return CRI_NULL;
    };
    
    CRICRS_obj* obj = (CRICRS_obj*)CRI_ALIGN(((CriUintPtr)wkarea), sizeof(CriUintPtr));

    obj->dummy = CRI_NULL;

    return obj;
};


void
CRICRS_Destroy(CRICRS crs)
{
    if (crs == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("NULL pointer is passed"));
        return;
    };
};


CriSint32
CRICRS_Enter(CRICRS crs)
{
    if (crs == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("NULL pointer is passed"));
        return CRI_NG;
    };

    return CRI_OK;
};


CriSint32
CRICRS_Leave(CRICRS crs)
{
    if (crs == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("NULL pointer is passed"));
        return CRI_NG;
    };

    return CRI_OK;
};