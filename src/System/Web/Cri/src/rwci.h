#ifndef RWCI_H
#define RWCI_H

#include "cri_xpt.h"

typedef struct CVFS_VTBL CVFS_VTBL;


void rwCiInit(void);
void rwCiFinish(void);
void rwCiSetRootDir(const CriChar8* rd);
CVFS_VTBL* rwCiGetInterface(void);

#endif /* RWCI_H */
