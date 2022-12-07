/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2001-2008 CRI-MW
 *
 * Library : UT Library
 * Module  : Library header
 * File    : cri_ut.h
 * Date    : 2008-11-06
 *
 ****************************************************************************/

#ifndef INCLUDED_CRI_UT_H		/* Re-definition prevention */
#define INCLUDED_CRI_UT_H

/*******************************************************************
 *      Include file
 *******************************************************************/
/*--- CRI-MW header ---*/
#include	<cri_xpt.h>

#ifdef  __cplusplus
extern "C" {					/* __cplusplus */
#endif
/*******************************************************************
 *      Macro constant
 *******************************************************************/
#define CRIUT_VER	"1.76"	/* Version string */

/*--- UTPAD ---*/
#define	UTPAD_PORT1			(0)
#define	UTPAD_PORT2			(1)
#define	UTPAD_PORT3			(2)
#define	UTPAD_PORT4			(3)
#define	UTPAD_PORT_MAX		(4)
#define UTPAD_UP			(0x0001)
#define UTPAD_DOWN			(0x0002)
#define UTPAD_LEFT			(0x0004)
#define UTPAD_RIGHT			(0x0008)
#define UTPAD_BUTTON1		(0x0010)
#define UTPAD_BUTTON2		(0x0020)
#define UTPAD_BUTTON3		(0x0040)
#define UTPAD_BUTTON4		(0x0080)
#define UTPAD_L				(0x0100)
#define UTPAD_R				(0x0200)
#define UTPAD_START			(0x0400)	/* Enter */
#define UTPAD_BUTTON5		(0x0800)
#define UTPAD_BUTTON6		(0x1000)
#define UTPAD_PAUSE			(0x2000)	/* Space */

/*--- UTSPR ---*/
/* �o�b�t�@�T�C�Y�擾�}�N�� */
#define UTSPR_CALCBUFSIZE(max_id)		(sizeof(UTSPR)*(max_id))

/*--- UTMEM ---*/
/* �������Ǘ��\���̂Ŏg�p���郁�����T�C�Y�v�Z */
#define UTMEM_CalcBufSize(_size)	(sizeof(UtMem)+(_size))
#define UTMEM_CALCBUFSIZE(_size)	(UTMEM_CalcBufSize(_size))

/*******************************************************************
 *      Enumarate constant
 *******************************************************************/

/*******************************************************************
 *      Process macro
 *******************************************************************/

/*******************************************************************
 *      Data type declaration
 *******************************************************************/
/*--- UTPAD ---*/
/* Peripheral Data type declaration */
typedef struct { 
	Uint32	on;
	Uint32	press;
	Float32	x;
	Float32	y;
	Float32	x2;
	Float32	y2;
	Uint32	last_on;
} UTPAD_PERIPHERAL;

/*--- UTSPR ---*/
typedef struct {
	Sint32		width, height;
	Sint32		tex_width, tex_height;
	void		*tex;
} UTSPR;

typedef struct {
	Sint32		left;
	Sint32		top;
	Sint32		right;
	Sint32		bottom;
} UTSPRRECT;

/*--- UTMEM ---*/
typedef void (*UtMemCb)(void);

typedef struct tagUtMemNode {
	struct tagUtMemNode	*prev;
	struct tagUtMemNode	*next;
	Sint32				id;
	Sint32				size;
	Char8				*name;
	Sint32				rsv;
	Sint32				rsv2;
	Sint32				rsv3;
}UtMemNode;

/* �������Ǘ��\���� */
typedef struct {
	void		*buf;				/* �o�b�t�@ */
	Sint32		buf_size;			/* �o�b�t�@�T�C�Y */
	Sint32		cur_id;				/* �A���P�[�gID */
	Sint32		num_free_warning;	/* ���f������\���̂���Free�Ăяo���������� */
	UtMemCb		alloc_cb;			/* �A���P�[�g���̃R�[���o�b�N�֐� */
	Sint32		alloc_cb_id;		/* �R�[���o�b�N������A���P�[�gID */
	UtMemCb		err_cb;				/* �G���[�R�[���o�b�N�֐� */

	Sint32		alloc_size;			/* ���݂̃������m�ۃT�C�Y */
	Sint32		max_alloc_size;		/* �s�[�N���̃������m�ۃT�C�Y */
	Sint32		num_alloc;			/* ���݂̃������m�ی� */

	UtMemNode	*low_start_node;	/* LOW�̈�̃X�^�[�g�m�[�h */
	UtMemNode	*low_end_node;		/* LOW�̈�̃G���h�m�[�h */

	UtMemNode	*high_end_node;		/* HIGH�̈�̃X�^�[�g�m�[�h */
	UtMemNode	*high_start_node;	/* HIGH�̈�̃G���h�m�[�h */
}UtMem;

/*******************************************************************
 *      Function macro
 *******************************************************************/

/*******************************************************************
 *      Variable declaration
 *******************************************************************/

/*******************************************************************
 *      External Function declaration
 *******************************************************************/

/*******************************************************************
 *      Function declaration
 *******************************************************************/
/*--- UTLIB ---*/
const char * CRIAPI UTLIB_GetVer(void);

/*--- UTPAD ---*/
void CRIAPI UTPAD_Init(void);
void CRIAPI UTPAD_Finish(void);
const UTPAD_PERIPHERAL * CRIAPI UTPAD_GetPeripheral(Uint32 port);

/*--- UTPRT ---*/
Bool CRIAPI UTPRT_Init(void *p);
void CRIAPI UTPRT_Finish(void);
void CRIAPI UTPRT_Draw(void);
void CRIAPI UTPRT_Clear(void);
void CRIAPI UTPRT_SetColor(Uint8 r, Uint8 g, Uint8 b);
void CRIAPI UTPRT_Print(Uint32 x, Uint32 y, const char *fmt, ...);
void CRIAPI UTPRT_PutChar(Uint32 x, Uint32 y, Sint32 c);

/*--- UTSPR ---*/
Bool CRIAPI UTSPR_Init(void *obj);
void CRIAPI UTSPR_Finish(void);
Bool CRIAPI UTSPR_Create(UTSPR *sprite, const char *fname);
Bool CRIAPI UTSPR_CreateMem(UTSPR *sprite, const void *buf, Sint32 size);
void CRIAPI UTSPR_Destroy(UTSPR *sprite);
void CRIAPI UTSPR_Draw(UTSPR *sprite, Float32 x, Float32 y, Float32 z, const UTSPRRECT *srcrect);
void CRIAPI UTSPR_SetBuf(void *buf, Uint32 max_id);
Bool CRIAPI UTSPR_LoadID(Uint32 id, const Char8 *fname);
void CRIAPI UTSPR_DrawID(Uint32 id, Float32 x, Float32 y, Float32 z, const UTSPRRECT *srcrect);
void CRIAPI UTSPR_GetSizeID(Uint32 id, Uint32 *width, Uint32 *height);

/*--- UTTMR ---*/
void CRIAPI UTTMR_Init(void);
void CRIAPI UTTMR_Finish(void);
Uint32 CRIAPI UTTMR_GetCount(void);
Uint32 CRIAPI UTTMR_DiffCount(Uint32 count1, Uint32 count2);
Uint32 CRIAPI UTTMR_CountToMicro(Uint32 count);
Uint32 CRIAPI UTTMR_GetUnit(void);

/*--- UTMEM ---*/
/* �������Ǘ������� */
UtMem* UTMEM_Init(void *buf, Sint32 buf_size, UtMemCb err_cb);

/* �A���C�����g�w��t���̃������m�� */
void* UTMEM_AllocAlign(UtMem *mem, Sint32 size, Sint32 align);

/* �������m��(4byte�A���C�����g) */
void* UTMEM_Alloc(UtMem *mem, Sint32 size);

/* �A���C�����g�w��t����HIGH�̈惁�����m�� */
void* UTMEM_HighAllocAlign(UtMem *mem, Sint32 size, Sint32 align);

/* HIGH�̈惁�����m��(4byte�A���C�����g) */
void* UTMEM_HighAlloc(UtMem *mem, Sint32 size);

/* ��������� */
void UTMEM_Free(UtMem *mem, void *buf);

/* �m�ۍς݃������̃T�C�Y�V�������N(HIGH�̈�͎g�p�s��) */
void UTMEM_Shrink(UtMem *mem, void *buf, Sint32 size);

/* �m�ۂ��Ă��郁��������S�ăN���A */
void UTMEM_Clear(UtMem *mem);

/* �S�̂̃������m�ۃT�C�Y���擾(�o�C�g�P��) */
Sint32 UTMEM_GetAllocSize(UtMem *mem);

/* ���݂܂łɊm�ۂ����������̃s�[�N���̍��v�T�C�Y���擾(�o�C�g�P��) */
Sint32 UTMEM_GetMaxAllocSize(UtMem *mem);

/* ���������Ɋ��蓖�Ă��o�b�t�@�T�C�Y���擾(�o�C�g�P��) */
Sint32 UTMEM_GetBufSize(UtMem *mem);

/* �󂫗̈�̃T�C�Y���擾(�o�C�g�P��) */
Sint32 UTMEM_GetEmptySize(UtMem *mem);

/* �m�ۂ��Ă��郁���������擾(���P��) */
Sint32 UTMEM_GetNumAlloc(UtMem *mem);

/* �Ō�Ɋm�ۂ�����������ID���擾 */
Sint32 UTMEM_GetLastAllocID(UtMem *mem);

/* ����������̌x�������擾 */
Sint32 UTMEM_GetNumFreeWarn(UtMem *mem);

/* �G���[�R�[���o�b�N�֐���o�^ */
void UTMEM_SetErrCb(UtMem *mem, UtMemCb err_cb);

/* �������m�ێ��̃R�[���o�b�N�֐���o�^ */
void UTMEM_SetAllocCb(UtMem *mem, UtMemCb alloc_cb, Sint32 id);

/* ���Ԃ̍ő�T�C�Y		 */
Uint32 UTMEM_GetMaxPoreSize(UtMem *mem);

/* �������̈�ւ̖��O�̐ݒ� */
void UTMEM_SetName(void *ptr, Char8 *name);

void UTMEM_DumpName(UtMem *mem);

void UTMEM_Dump(UtMem *mem, Char8 *fname);

#ifdef	__cplusplus
}								/* __cplusplus */
#endif

#endif							/* Re-definition prevention */

/* end of file */
