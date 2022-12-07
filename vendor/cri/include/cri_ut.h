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
/* バッファサイズ取得マクロ */
#define UTSPR_CALCBUFSIZE(max_id)		(sizeof(UTSPR)*(max_id))

/*--- UTMEM ---*/
/* メモリ管理構造体で使用するメモリサイズ計算 */
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

/* メモリ管理構造体 */
typedef struct {
	void		*buf;				/* バッファ */
	Sint32		buf_size;			/* バッファサイズ */
	Sint32		cur_id;				/* アロケートID */
	Sint32		num_free_warning;	/* 分断化する可能性のあるFree呼び出しをした回数 */
	UtMemCb		alloc_cb;			/* アロケート時のコールバック関数 */
	Sint32		alloc_cb_id;		/* コールバックさせるアロケートID */
	UtMemCb		err_cb;				/* エラーコールバック関数 */

	Sint32		alloc_size;			/* 現在のメモリ確保サイズ */
	Sint32		max_alloc_size;		/* ピーク時のメモリ確保サイズ */
	Sint32		num_alloc;			/* 現在のメモリ確保個数 */

	UtMemNode	*low_start_node;	/* LOW領域のスタートノード */
	UtMemNode	*low_end_node;		/* LOW領域のエンドノード */

	UtMemNode	*high_end_node;		/* HIGH領域のスタートノード */
	UtMemNode	*high_start_node;	/* HIGH領域のエンドノード */
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
/* メモリ管理初期化 */
UtMem* UTMEM_Init(void *buf, Sint32 buf_size, UtMemCb err_cb);

/* アライメント指定付きのメモリ確保 */
void* UTMEM_AllocAlign(UtMem *mem, Sint32 size, Sint32 align);

/* メモリ確保(4byteアライメント) */
void* UTMEM_Alloc(UtMem *mem, Sint32 size);

/* アライメント指定付きのHIGH領域メモリ確保 */
void* UTMEM_HighAllocAlign(UtMem *mem, Sint32 size, Sint32 align);

/* HIGH領域メモリ確保(4byteアライメント) */
void* UTMEM_HighAlloc(UtMem *mem, Sint32 size);

/* メモリ解放 */
void UTMEM_Free(UtMem *mem, void *buf);

/* 確保済みメモリのサイズシュリンク(HIGH領域は使用不可) */
void UTMEM_Shrink(UtMem *mem, void *buf, Sint32 size);

/* 確保しているメモリ情報を全てクリア */
void UTMEM_Clear(UtMem *mem);

/* 全体のメモリ確保サイズを取得(バイト単位) */
Sint32 UTMEM_GetAllocSize(UtMem *mem);

/* 現在までに確保したメモリのピーク時の合計サイズを取得(バイト単位) */
Sint32 UTMEM_GetMaxAllocSize(UtMem *mem);

/* 初期化時に割り当てたバッファサイズを取得(バイト単位) */
Sint32 UTMEM_GetBufSize(UtMem *mem);

/* 空き領域のサイズを取得(バイト単位) */
Sint32 UTMEM_GetEmptySize(UtMem *mem);

/* 確保しているメモリ数を取得(個数単位) */
Sint32 UTMEM_GetNumAlloc(UtMem *mem);

/* 最後に確保したメモリのIDを取得 */
Sint32 UTMEM_GetLastAllocID(UtMem *mem);

/* メモリ解放の警告数を取得 */
Sint32 UTMEM_GetNumFreeWarn(UtMem *mem);

/* エラーコールバック関数を登録 */
void UTMEM_SetErrCb(UtMem *mem, UtMemCb err_cb);

/* メモリ確保時のコールバック関数を登録 */
void UTMEM_SetAllocCb(UtMem *mem, UtMemCb alloc_cb, Sint32 id);

/* 隙間の最大サイズ		 */
Uint32 UTMEM_GetMaxPoreSize(UtMem *mem);

/* メモリ領域への名前の設定 */
void UTMEM_SetName(void *ptr, Char8 *name);

void UTMEM_DumpName(UtMem *mem);

void UTMEM_Dump(UtMem *mem, Char8 *fname);

#ifdef	__cplusplus
}								/* __cplusplus */
#endif

#endif							/* Re-definition prevention */

/* end of file */
