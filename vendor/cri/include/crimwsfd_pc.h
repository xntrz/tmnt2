/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2001-2005 CRI-MW
 *
 * Library  : MWSFD Library (CRI Sofdec)
 * Module   : Library User's Header (PC-Specific)
 * File     : crimwsfd_pc.h
 * Date     : 2005-10-18
 * Version  : (see MWD_SFD_VER)
 *
 ****************************************************************************/
#ifndef	CRIMWSFD_PC_H_INCLUDED		/* Re-definition prevention */
#define	CRIMWSFD_PC_H_INCLUDED

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <crimwsfd.h>

/***************************************************************************
 *      MACRO CONSTANT
 ***************************************************************************/

/***************************************************************************
 *      Process MACRO
 ***************************************************************************/

/***************************************************************************
 *      Enum definition
 ***************************************************************************/

/***************************************************************************
 *      Data type definition
 ***************************************************************************/

/* �g�p�\�v���Z�b�T��� */
typedef struct {
	CriUint32		affinity_mask;  /* �A�t�B�j�e�B�}�X�N (Win32 API��SetThreadAffinityMask�ɑΉ�) */
} MwsfdProcessor_PC;


/***************************************************************************
 *      Function Declaration
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/* �g�p�\�v���Z�b�T�̐ݒ� */
void CRIAPI mwPlySetUsableProcessor_PC(MWPLY mwply, MwsfdProcessor_PC *processor);


#ifdef __cplusplus
}
#endif

#endif	/* CRIMWSFD_PC_H_INCLUDED */
