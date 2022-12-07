/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 1998-2007 CRI-MW
 *
 * Library  : CRI Middleware Library
 * Module   : CRI Common Header for Windows
 * File     : cri_xpt.h
 * Date     : 2007-07-26
 * Version  : 1.14
 *
 ****************************************************************************/
#ifndef CRI_XPT_H
#define CRI_XPT_H

#if defined(__BORLANDC__)
#define XPT_TGT_BCB
#else
#define XPT_TGT_PC
#define XPT_TGT_PC_PRO
#endif

#define XPT_CCS_LEND
#define XPT_SUPPORT_MULTICHANNEL

#include "cri_xpts_win.h"
#include "cri_xpt_post.h"

#endif  /* CRI_XPT_H */
/* End Of File */
