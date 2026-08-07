#ifndef ERR_H
#define ERR_H

#include "cri_xpt.h"
#include "ut.h"


//
//  ERR_NOIMPL - Macro that marks funcs or branches inside func
//               that is have to exist but not used at all in game.
//

#define ERR_NOIMPL() \
    ERR_CallErrFunc(CRI_STR("NO IMPL"))


void ERR_CallErrFunc(const CriChar8* fmt, ...);

#endif /* ERR_H */
