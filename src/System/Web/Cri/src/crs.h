#ifndef CRS_H
#define CRS_H

#include "cri_xpt.h"

#define CRI_CRS_WK_SIZE (28)


typedef void* CRICRS;


CRICRS
CRICRS_Create(void* wkarea, CriSint32 wksize);

void
CRICRS_Destroy(CRICRS crs);

CriSint32
CRICRS_Enter(CRICRS crs);

CriSint32
CRICRS_Leave(CRICRS crs);

#endif /* CRS_H */
