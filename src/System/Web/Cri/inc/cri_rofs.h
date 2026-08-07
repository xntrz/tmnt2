/***********************************************************************
 *
 *  Software Library
 *  Copyright (c) 2001-2008 CRI-MW
 *
 *      Read Only File System
 *
 *  Module  : Library Header File
 *  File    : cri_rofs.h
 *  Date    : 2008-11-06
 *  Version : see (ROFS_VERSION_STR)
 *  Notes   : 
 *
 ***********************************************************************/

/*---------------------------------------------------------------------*/
#ifndef _CRI_ROFS_H_
#define _CRI_ROFS_H_

/*****************************************************************************
 *      インクルードファイル
 *		Include file
 *****************************************************************************/

/*---------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cri_xpt.h>

#ifndef CRI_XPT_H
	Please include "cri_xpt.h"		/* 予防 */
									/* Prevention */
#endif

/*****************************************************************************
 *      定数マクロ
 *		Macro Constant
 *****************************************************************************/

/* バージョン番号 */
/* Version Number */
#define ROFS_VERSION_STR		"2.04"		/* Version number */

/* セクタ長 */
/* Sector Length */
#define ROFS_SCT_LEN			(2048)

/* ファイル属性 */
/* File Attribute */
#define ROFS_ATR_DIRECTORY		(0x02)		/* ディレクトリ */
											/* Directory */

/* ボリューム名 */
/* Volume name */
#define ROFS_VOLNAME_LEN		(8)
#define ROFS_VOLNAME_SIZ		(ROFS_VOLNAME_LEN + 1)

/* ファイル名 */
/* File name */
#define ROFS_FNAME_LEN			(31)
#define ROFS_FNAME_SIZ			(ROFS_FNAME_LEN + 1)

/* ディレクトリ名 */
/* Directory name */
#define ROFS_DIRNAME_LEN		(31)
#define RPFS_DIRNAME_SIZ		(ROFS_DIRNAME_LEN+1)

/* ボリュームID */
/* Volume ID */
#define ROFS_VOLID_SIZ			(8)

/* CVMタイムスタンプ */
/* Time stamp of CVM */
#define ROFS_TMSTMP_SIZ			(8)

/* 作業バッファ */
/* Work buffer */
#define ROFS_WKBUF_NUM			(2)
#define ROFS_WKBUF_SIZ			(ROFS_SCT_LEN * ROFS_WKBUF_NUM + 64)


/*============================================================================
 *      ROFS_SetCond関数用のパラメータ
 *		Parameters for the ROFS_SetCond function
 *===========================================================================*/
/* ディレクトリレコードソース	*/
/* Directory record source		*/
#define ROFS_PARM_DIRSRC_DFL	(0)		/* default						*/
#define ROFS_PARM_DIRSRC_CVM	(0)		/* from CVM						*/
#define ROFS_PARM_DIRSRC_MEM	(1)		/* from direcotry record file	*/

/*****************************************************************************
 *      列挙定数
 *		Enumarate Constants
 *****************************************************************************/

/* ファイルハンドル状態 */
/* Status of handle */
typedef enum {
	ROFS_STAT_IDLE		= (0),			/* Idle status */
	ROFS_STAT_COMPLETE	= (1),			/* Tranfer complete status */
	ROFS_STAT_TRANS		= (2),			/* During transfer status */
	ROFS_STAT_ERR		= (3),			/* Error outbreak status */
	ROFS_STAT_NUM
} RofsStat;

/* シークモード */
/* Mode of seek */
typedef enum {
	ROFS_SEEK_SET		= (0),			/* Top of file */
	ROFS_SEEK_CUR		= (1),			/* Current position */
	ROFS_SEEK_END		= (2),			/* End of file */
	ROFS_SEEK_NUM
} RofsSeek;

/* エラーコード */
/* Error code */
typedef enum {
	ROFS_ERR_OK			= (0),			/* 正常終了 */
										/* Successful */
	ROFS_ERR_NG			= (-1),			/* 失敗 */
										/* Failure */

	ROFS_ERR_PRM		= (-100),		/* パラメータの設定エラー */
										/* Error of parameter */
	ROFS_ERR_ILLHDL		= (-101),		/* ファイルハンドルが不正 */
										/* Illigal file handle */
	ROFS_ERR_NOHNDL		= (-102),		/* ファイルハンドルに空きがない */
										/* Too many file handles */
	ROFS_ERR_VOLNAME	= (-103),		/* 不正なボリューム名 */
										/* Illigal volume name */
	ROFS_ERR_REGVOLNAME	= (-104),		/* 登録済みのボリューム名 */
										/* Already exist volume name */
	ROFS_ERR_VOLUME		= (-105),		/* ボリュームの無効エラー */
										/* Invalid Volume */
	ROFS_ERR_NOTDIR		= (-106),		/* ディレクトリでないものを指定した */
										/* Not directory*/
	ROFS_ERR_NOTFOUND	= (-107),		/* 存在しないファイル */
										/* No exist file */
	ROFS_ERR_ILLDIRBUF	= (-108),		/* 不正なディレクトリレコード */
										/* Illigal directory record */
	ROFS_ERR_DIROVER	= (-109),		/* ディレクトリエントリの最大数を超えた */
										/* Too many entry of directory */
	ROFS_ERR_NOTFOUND_TOD	= (-110),	/* ディレクトリレコードの終端が見つからない */
										/* Termination of a directory record is not found */
	ROFS_ERR_INVALID_DIRLIST= (-111),	/* 無効なディレクトリリスト */
										/* Invalid directory list */

	ROFS_ERR_BUSY		= (-200),		/* 他のコマンドを実行中 */
										/* Under processing of other commands */
	ROFS_ERR_TOUT		= (-201),		/* 内部処理でタイムアウトが発生した */
										/* Timeout */
	ROFS_ERR_PRIMITIVE	= (-202),		/* プリミティブ関数エラー */
										/* Primitive function error */
	ROFS_ERR_INIT		= (-203),		/* 初期化エラー */
										/* Initialization error */
	ROFS_ERR_REQRD		= (-204),		/* 読み込み要求エラー */
										/* Read request error */
	ROFS_ERR_SEEK		= (-205),		/* シークエラー */
										/* Seek error */
	ROFS_ERR_OPEN		= (-206),		/* ファイルオープンエラー */
										/* File open error */
	ROFS_ERR_READ		= (-207),		/* 読み込みエラー */
										/* Read error */

	ROFS_ERR_INTERNAL	= (-1000)		/* 内部エラー */
										/* Internal error */
} RofsErr;

/* 動作条件ID */
/* operating condition id */
typedef enum {
	ROFS_COND_DIRSRC		= (0),	/* ディレクトリレコードソース	*/
									/* directory record source		*/
	ROFS_COND_ECB_READ		= (1),	/* リードエラーコールバック		*/
									/* read error call back			*/
	ROFS_COND_ECB_LOADDIR	= (2),	/* ロードディレクトリコールバック	*/
									/* load directory error call back	*/
	ROFS_COND_END
} RofsCondId;

/*****************************************************************************
 *      処理マクロ
 *		Process Macro
 *****************************************************************************/
/* ライブラリ作業領域サイズ */
/* Library work area size */
#define ROFS_GET_LIBWORK_SIZ(max_open, max_volume, max_dirent) \
	(sizeof(RofsWork)+((max_open)+1)*sizeof(RofsHandle)+\
	(ROFS_GET_DIRREC_SIZ(max_dirent)+sizeof(RofsVolume))*(max_volume)+8)

/* ディレクトリレコード領域サイズ */
/* Directory record area size */
#define ROFS_GET_DIRREC_SIZ(n_dirent) \
	(sizeof(RofsDirRec)-sizeof(RofsDirRecEnt)+(n_dirent)*sizeof(RofsDirRecEnt))

/* ディレクトリの判定 */
/* judgment of a directory */
#define ROFS_IS_ATR_DIRECTORY(finf) \
	(((finf)->fatr & ROFS_ATR_DIRECTORY)?TRUE:FALSE)

/* セクタ数からバイト数への変換 */
/* Conversion to the byte number from the sector number */
#define ROFS_CONV_SCT2BYTE(n_sct)	((n_sct) * ROFS_SCT_LEN)

/* セクタ読み込みバッファサイズ */
/* Sector reading buffer size */
#define ROFS_GET_SCTBUF_SIZ(n_sct)	(ROFS_CONV_SCT2BYTE(n_sct) + 63)

/* ポインタの64byte境界合わせ */
/* 64-byte boundary of a pointer */
#define ROFS_ALIGN_64BYTE(ptr)		(((CriSintPtr)(ptr) + 63) & 0xffffffc0)

/*****************************************************************************
 *      データ型の定義
 *		Data type declaration
 *****************************************************************************/
/* ワーク領域 */
/* Work area */
typedef struct RofsStWrok		RofsWork;

/* プリミティブ関数テーブル */
/* Primitive functions table */
typedef struct RofsPfsIfTbl		RofsPfsTbl;

/* デバイスコントロール関数 */
/* Device control functions */
typedef struct RfpfsDevIfTbl	RofsDevTbl;

/* ライブラリインターフェース関数テーブル */
/* Library interface functions table */
typedef struct RfmaiLibIfTbl	RofsLif;

/* ディレクトリレコードエントリ */
/* Directory record entry */
typedef struct {
	CriUint32	fsize;						/* file size lower */
	CriUint32	fsize_ex;					/* file size upper */
	CriSint32	fad;						/* fad */
	CriUint8	fatr;						/* file attribute */
	CriUint8	sid;						/* filename search id */
	CriChar8	fname[ROFS_FNAME_SIZ];		/* filename */
	CriUint8	pad[2];						/* align padding */
} RofsDirRecEnt;	/* 48 bytes */

/* ディレクトリレコード */
/* Directory record */
typedef struct {
	CriSint32	dir_num;					/* number of record */
	CriSint32	max_ent;					/* max entry of directory record */
	CriSint32	dir_fad;					/* fad of directory record */
	CriChar8	volname[ROFS_VOLNAME_SIZ];	/* volume name */
	CriUint8	pad[3];						/* align padding */
	RofsDirRecEnt	dirrec_tbl[1];		/* record table */
} RofsDirRec;		/* 24 bytes */
typedef RofsDirRec	*RofsDirRecBuf;

/* ボリューム管理 */
/* Volume management */
typedef struct {
	void	*img_hn;					/* file handle of the CVM file */
	CriSint32	zisb;						/* image start position (0 = DVD-ROM) */
	CriSint32	ptbl_cba;					/* cba of path table */
	CriSint32	ptbl_size;					/* size of path table */
	RofsDirRecBuf	curdir;				/* current directory handle */
	CriSint16	req_cnt;					/* request counter */
	CriChar8	volname[ROFS_VOLNAME_SIZ];	/* volume name */
	CriUint8	pad[1];						/* align padding */
	/* 2002-09-26 Ver.1.30 miko { : CVMフォーマット変更 */
	CriUint32	flags;						/* flags */
	CriUint8	vol_id[ROFS_VOLID_SIZ];		/* volume id */
	/* } */
	/* 2003-05-27 Ver.1.60 miko { : dirlst対応 */
	CriUint8	vol_cvmtim[ROFS_TMSTMP_SIZ];	/* volume time stamp */
	/* } */
} RofsVolume;		/* 52 bytes */

/* ROFSファイルハンドル */
/* ROFS File Handle */
typedef struct {
	CriUint32		fsize;					/* file size lower */
	CriUint32		fsize_ex;				/* file size upper */
	RofsWork	*wk;					/* pointer of lib work */
	CriSint32		fid;					/* file id */
	CriSint32		fad;					/* fad */
	CriSint32		ofs;					/* offset */
	CriSint32		fsctsize;				/* sctor size of the file */
	RofsVolume	*vol;					/* image file volume */
	CriSint32		rsize;					/* reading size */
	CriSint32		trns_seg;				/* transferred block unit */
	CriSint32		trns_ofs;				/* transferred byte unit */
	CriSint32		trns_unit;				/* transfer unit (block) */
	CriUint8		*rdadr;					/* read address */
	CriSint16		used;					/* used flag */
	CriSint16		act;					/* handle act */
	CriSint16		stat;					/* handle status(RofsStat) */
	CriSint16		err;					/* error status */
} RofsHandle;		/* 72 bytes */
typedef RofsHandle	*Rofs;

/* ROFS作業領域 */
/* ROFS Work Area */
struct RofsStWrok {
	CriBool			f_init;				/* initialize flag */
	CriSint32			max_open;			/* max open files */
	CriSint32			max_volume;
	CriSint32			max_dirent;
	CriSint32			worksiz;
	CriUint32			exec_server_cnt;	/* counter */
	CriUint32			actflags;
	CriUint32			tout_read_max;
	Rofs			syshdl;				/* handle for system command */
	Rofs			hndtbl;				/* handle */
	RofsVolume		*vollist;			/* Volume information list */
	RofsVolume		*curvol;			/* information of the defalut Volume */
	RofsDirRecBuf	dirbuf;				/* start posision of directory record */
	RofsPfsTbl		*pfs;				/* primitive function table */
	RofsDevTbl		*dev;				/* device control function table */
	RofsLif			*liftbl;			/* library interface */
	void	(*g_errcb)(void *, CriChar8 *, CriSint32);	/* error callback */
	void			*g_errcb_1st;		/* error callback 1st arg. */
	CriUint32			*sctbuf;			/* sector buffer addres */
	CriUint32	sct_load_buf[ROFS_WKBUF_SIZ / 4];	/* sector buffer */
	RofsHandle		hndlist[1];			/* handle table */
}; /* 76 + 4160 + (72 * (handle num + 1)) */

/* ライブラリ初期化情報  */
/* Library initialize information */
typedef struct {
	CriSint32	max_open;			/* 同時にオープンできる最大ファイル数 */
								/* The number of the maximum files 
								 * which can be opened simultaneously */
	CriSint32	max_volume;			/* 同時に登録できる最大ボリューム数 */
								/* The number of the maximum volumes 
								 * which can be registered simultaneously */
	CriSint32	max_dirent;			/* ディレクトリに格納する最大ファイル数 */
								/* The number of the maximum files 
								 * stored in a directory */
	void	*rofs_work;			/* ライブラリ作業領域の先頭アドレス */
								/* The head address of a library work domain */
} RofsInitPara;

/* ファイル情報 */
/* File information */
typedef struct {
	CriUint32	fsize;					/* file size lower */
	CriUint32	fsize_ex;				/* file size upper */
	CriChar8	*fname;					/* file name */
	CriUint8	fatr;					/* file attribute */
	CriUint8	pad[3];					/* align padding */
} RofsFileInf;		/* 16 bytes */

/* ボリューム情報 */
/* Volume information */
typedef struct {
	CriChar8	*volname;				/* volume name */
	void	*fhn;					/* file handle of the volume */
} RofsVolumeInf;

/* 2002-10-24 Ver.1.330 miko : add -> 2005-11-15 update */
/* スクランブル情報 */
/* Scramble information */
typedef struct {
	RofsVolume	*vol;				/* image file volume */
	CriSint32		sctno;				/* sector number */
} RofsScrambleInf;

/* 2002-11-06 Ver.1.400 miko : add */
/* ボリュームパラメータ */
/* Volume parameter */
typedef struct {
	CriChar8	*scrkey;				/* スクランブル解除鍵文字列のポインタ */
									/* The pointer of descramble string */
} RofsVolumePrm;

/* エラーコールバック関数 */
/* Error callback function */
typedef void (*RofsErrFunc)(void *obj, CriChar8 *msg, CriSint32 errcode);

/* プリミティブ関数テーブル */
/* Primitive functions table */
struct RofsPfsIfTbl {
	void		(*pfs_execserver)(void);
	void		(*pfs_reserve01)(RofsErrFunc, void *);
	CriSint32		(*pfs_reserve02)(CriSint8 *);
	CriSint32		(*pfs_reserve03)(void);
	void		*(*pfs_reserve04)(CriSint8 *, void *, CriSint32);
	void		(*pfs_reserve05)(void *);
	CriSint32		(*pfs_seek)(void *, CriSint32, RofsSeek);
	CriSint32		(*pfs_tell)(void *);
	CriSint32		(*pfs_reqrd)(void *, CriSint32, void *);
	CriSint32		(*pfs_reserve07)(void *, CriSint32, void *);
	void		(*pfs_stoptr)(void *);
	CriSint32		(*pfs_getstat)(void *);
	CriSint32		(*pfs_getsctlen)(void *);
	void		(*pfs_reserve08)(void *, CriSint32);
	CriSint32		(*pfs_getnumtr)(void *);
	CriSint32		(*pfs_reserve09)(CriSint8 *);
	CriSint32		(*pfs_reserve10)(CriSint8 *);
	CriSint32		(*pfs_reserve11)(void);
	CriSint32		(*pfs_reserve12)(CriSint8 *, void *, CriSint32);
	CriSint32		(*pfs_reserve13)(void *);
	CriSint32		(*pfs_reserve14)(CriSint8 *);
	CriSint32		(*pfs_reserve15)(CriSint8 *);
	CriSint32		(*pfs_reserve16)(CriSint8 *);
	CriSint32		(*pfs_reserve17)(CriSint8 *, void *);
	CriSint32		(*pfs_reserve18)(void *, CriSint32, CriSint32, CriSint32);
	CriSint32		(*pfs_reserve19)(void *, CriSint32, CriSint32, CriSint32);
	/* 2002-09-11 Ver.1.22a miko { : 64bit対応 */
	CriSint64		(*pfs_getnumtr64)(void *);
	/* } miko */
	/* 2008-02-04 Ver.2.00 Ryo Usuki { : cvFsIsAvailableRetry対応 */
	CriBool		(*pfs_isavailableretry)(void *);
	/* } Ryo Usuki */
};

/*****************************************************************************
 *      変数宣言
 *		Variable Declaration
 *****************************************************************************/

/*****************************************************************************
 *      関数宣言
 *		Function Declaration
 *****************************************************************************/

/*============================================================================
 *      ライブラリ初期化／終了
 *		Initialize and Finalize of Library
 *===========================================================================*/
/* ファイルシステムの初期化
 * [入  力] init_para	: ライブラリ初期化パラメータ
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 * (a) ライブラリ作業領域は ROFS_GET_LIBWORK_SIZマクロ で求めること。
 * (b) init_para.max_dirent は、"."と".."の２つ分を加算すること。
 */
/* Initialize of file system
 * [Inputs ] init_para	: Library initialization parameter
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) Decides the library work area size by ROFS_GET_LIBWORK_SIZ.
 * (b) init_para.max_dirent should add a part for two of "." and "..".
 */
RofsErr CRIAPI		ROFS_Init(RofsInitPara *init_para);

/* ファイルシステムの終了
 * [入  力] なし
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 */
/* Finalyze of file syste
 * [Inputs ] none
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
void CRIAPI		ROFS_Finish(void);

/* プリミティブ関数の登録
 * [入  力] pfs_tbl		: プリミティブ関数テーブル
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 * (a) デフォルトではCRI-MW標準の関数が登録されている。
 */
/* Registration of a primitive function
 * [Inputs ] 
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) The standard function for CRI-MW is registered by the default.
 */
RofsErr CRIAPI		ROFS_SetPfsFuncTbl(RofsPfsTbl *pfs_tbl);

/*============================================================================
 *      ボリューム操作
 *		Handle Volume
 *===========================================================================*/
/* ボリュームの追加
 * [入  力] volname		: ボリューム名
 *			img_hn		: オープン済みボリュームのハンドル（ファイルハンドル）
 *			volprm		: ボリュームパラメータ（不要ならNULL）
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 * (a) ボリューム名"ROFS"はライブラリ予約なので使用不可。
 * (b) volprm の不要なパラメータは 0 にしておくこと。
 */
/* Addition of volume (Expansion)
 * [Inputs ] volname	: volume name
 *			 img_hn		: Opend Volume handle (file handle)
 *			 volprm		: Volume parameter
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) Volume name "ROFS" is library reservation, it cannot be used.
 * (b) Please set the unnecessary parameter of volprm to 0.
 */
RofsErr CRIAPI		ROFS_AddVolume(CriChar8 *volname, void *img_hn, RofsVolumePrm *volprm);

/* ボリュームの削除
 * [入  力] volname		: ボリューム名
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 */
/* Deletion of volume
 * [Inputs ] volname	: volume name
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a)
 */
RofsErr CRIAPI		ROFS_DelVolume(CriChar8 *volname);

/* デフォルトボリュームの設定
 * [入  力] volname		: ボリューム名
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 */
/* Setup of default volume
 * [Inputs ] volname	; volume name
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_SetDefVolume(CriChar8 *volname);

/* デフォルトボリュームの取得
 * [入  力] なし
 * [出  力] なし
 * [関数値] ボリューム名
 * [備  考] 
 */
/* Gets default volume
 * [Inputs ] none
 * [outputs] none
 * [Return ] volume name
 * [Notes  ] 
 */
CriChar8 * CRIAPI		ROFS_GetDefVolume(void);

/* ボリュームハンドルの切り替え
 * [入  力] volname		: ボリューム名
 *			img_hn		: オープン済みボリュームのハンドル（ファイルハンドル）
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 */
/* The change of a volume handle
 * [Inputs ] volname	: volume name
 *			 img_hn		: Opend Volume handle (file handle)
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_SwitchImgHn(CriChar8 *volname, void *img_hn);

/*============================================================================
 *      ファイル操作
 *		Handle File
 *===========================================================================*/
/* ファイルのオープン
 * [入  力] fname		: ファイル名
 *			dir_buf		: ディレクトリレコード
 * [出  力] なし
 * [関数値] ROFSハンドル
 * [備  考] 
 * (a) dir_buf が NULL の場合は、カレントディレクトリレコードを参照する。
 */
/* Opening of a file
 * [Inputs ] fname		: File name
 *			 dir_buf	: Directory record
 * [outputs] none
 * [Return ] ROFS handle
 * [Notes  ] 
 * (a) When dir_buf is NULL, a current directory record is referenced.
 */
Rofs CRIAPI		ROFS_Open(CriChar8 *fname, void *dir_buf);

/* セクタ指定によるファイルのオープン
 * [入  力] volname		: ボリューム名
 *			stsct		: 先頭セクタ番号
 *			nsct		: セクタ数
 * [出  力] なし
 * [関数値] ROFSハンドル
 * [備  考] 
 */
/* Opening of the file by sector specification
 * [Inputs ] volname	: volume name
 *			 stsct		: Top sector number
 *			 nsct		: The number of sector
 * [outputs] none
 * [Return ] ROFS handle
 * [Notes  ] 
 */
Rofs CRIAPI		ROFS_OpenRange(CriChar8 *volname, CriSint32 stsct, CriSint32 nsct);

/* ファイルのクローズ
 * [入  力] rofs		: ROFSハンドル
 * [出  力] なし
 * [関数値] なし
 * [備  考] 
 */
/* Closing of a file
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 */
void CRIAPI		ROFS_Close(Rofs rofs);

/* ファイル現在位置の移動
 * [入  力] rofs		: ROFSハンドル
 *			nsct		: シーク開始位置からの移動セクタ数
 *			sk_mode		: シークモード
 * [出  力] なし
 * [関数値] ファイル先頭からのセクタ数
 * [備  考] 
 */
/* Movement of a file present position
 * [Inputs ] rofs		: ROFS handle
 *			 nsct		: The number of move sectors from a seeking start position
 *			 sk_mode	: Mode of seek
 * [outputs] none
 * [Return ] The number of sectors from a file top
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_Seek(Rofs rofs, CriSint32 nsct, RofsSeek sk_mode);

/* ファイル現在位置の取得
 * [入  力] rofs		: ROFSハンドル
 * [出  力] なし
 * [関数値] ファイル先頭からのセクタ数
 * [備  考] 
 */
/* Gets file present position
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] The number of sectors from a file top
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_Tell(Rofs rofs);

/* 読み込みの要求
 * [入  力] rofs		: ROFSハンドル
 *			nsct		: セクタ数
 * [出  力] buf			: 読み込み先バッファ
 * [関数値] 要求を受け付けたセクタ数
 * [備  考] 
 * (a) buf の先頭をターゲットが指定する境界に揃えておくこと。
 */
/* Request of reading
 * [Inputs ] rofs		: ROFS handle
 *			 nsct		: The number of sectors
 * [outputs] buf		: Reading buffer
 * [Return ] The number of sectors which received the request
 * [Notes  ] 
 * (a) Must be keep the boundary where a target specifies the address of buf
 */
CriSint32 CRIAPI		ROFS_ReqRd(Rofs rofs, CriSint32 nsct, void *buf);

/* 転送の停止
 * [入  力] rofs		: ROFSハンドル
 * [出  力] なし
 * [関数値] なし
 * [備  考]
 */
/* Stop of transfer
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 */
void CRIAPI		ROFS_StopTr(Rofs rofs);

/* サーバ処理の実行
 * [入  力] なし
 * [出  力] なし
 * [関数値] なし
 * [備  考] 
 * (a) ADXのADXF_SetupRofs関数を使っている場合は、本関数を使わないこと。
 *	   ※現在はADXとの併用必須。単体では動作しないので、本関数は使わない。
 */
/* Execution of server
 * [Inputs ] none
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 * (a) Don't use this function when use the ADXF_SetupRofs function 
 *     of ADX is being used.
 *     * present : indispensable alone. Combined use with ADX.
 *                 Don't use this function.
 */
void CRIAPI		ROFS_ExecServer(void);

/*============================================================================
 *      情報の取得
 *		Get Information
 *===========================================================================*/
/* ファイルハンドル状態の取得
 * [入  力] rofs		: ROFSハンドル
 * [出  力] なし
 * [関数値] ファイルハンドルステータス
 * [備  考] 
 */
/* Gets file handle status
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] file handle status
 * [Notes  ] 
 */
RofsStat CRIAPI	ROFS_GetStat(Rofs rofs);

/* ファイルサイズの取得
 * [入  力] fname		: ファイル名
 * [出  力] なし
 * [関数値] ファイルサイズ（バイト数）
 * [備  考] 
 * (a) 負値の場合はサイズでは無く、エラーを示す。
 * (b) ファイルサイズが2GB以上の場合 0x7fffffff を返す。
 */
/* Gets file size
 * [Inputs ] fname		: file name
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint32 CRIAPI		ROFS_GetFileSiz(CriChar8 *fname);

/* ファイルサイズの取得（ディレクトリレコード指定）
 * [入  力] fname		: ファイル名
 *			dir_buf		: ディレクトリレコード
 * [出  力] なし
 * [関数値] ファイルサイズ（バイト数）
 * [備  考] 
 * (a) 負値の場合はサイズでは無く、エラーを示す。
 * (b) ファイルサイズが2GB以上の場合 0x7fffffff を返す。
 */
/* Gets file size
 * [Inputs ] fname		: file name
 *			 dir_buf	: directory record
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint32 CRIAPI		ROFS_GetFileSizEx(CriChar8 *fname, void *dir_buf);

/* セクタ長の取得
 * [入  力] なし
 * [出  力] なし
 * [関数値] セクタ長（バイト数）
 * [備  考] 
 */
/* Gets sector length
 * [Inputs ] none
 * [outputs] none
 * [Return ] sector length (byte)
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetSctLen(void);

/* 転送済みサイズの取得
 * [入  力] rofs		: ROFSハンドル
 * [出  力] なし
 * [関数値] 転送済みデータサイズ（バイト数）
 * [備  考] 
 */
/* Gets transferred data size
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] transferred date size (byte)
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetNumTr(Rofs rofs);

/* ボリューム情報の取得
 * [入  力] volname		: ボリューム名
 * [出  力] volinf		: ボリューム情報
 * [関数値] エラー
 * [備  考] 
 */
/* Gets valume information
 * [Inputs ] volname	: volume name
 * [outputs] volinf		: Volume information
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_GetVolumeInf(CriChar8 *volname, RofsVolumeInf *volinf);

/*============================================================================
 *      ディレクトリ操作
 *		Handle Directory
 *===========================================================================*/
/* ディレクトリレコードの読み込み
 * [入  力] dirname		: ディレクトリ名
 *			n_dirent	: レコード数の上限
 * [出  力] dir_buf		: ディレクトリレコード
 * [関数値] エラー
 * [備  考] 
 * (a) 完了復帰。
 * (b) n_dirent が実際のレコード数未満の場合は、ROFS_ERR_DIROVER が返る。
 */
/* Reading of a directory record
 * [Inputs ] dirname	: directory name
 *			 n_dirent	: The maximum of the number of records
 * [outputs] dir_buf	: directory record
 * [Return ] error code
 * [Notes  ] 
 * (a) This function is not return, while the function is processing.
 * (b) Return value is ROFS_ERR_DIROVER when n_dirent is under the 
 *     actual number of directory records. */
RofsErr CRIAPI		ROFS_LoadDir(CriChar8 *dirname, void *dir_buf, CriSint32 n_dirent);

/* カレントディレクトリの設定
 * [入  力] volname		: ボリューム名
 *			dir_buf		: ディレクトリレコード
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 */
/* Setup of a current directory
 * [Inputs ] volname	: volume name
 *			 dir_buf	: directory record
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_SetDir(CriChar8 *volname, void *dir_buf);

/* カレントディレクトリの設定(ISO9660ディレクトリレコード) 
 * [入  力] volname		: ボリューム名
 *			dir_buf		: ディレクトリレコード
 *			nsct		: ディレクトリレコードのセクタ数
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 * (a) サンプルプログラム参照
 */
/* Setup of a current directory (Directory record of the ISO9660 image)
 * [Inputs ] volname	: volume name
 *			 dir_buf	: directory record
 *			 nsct		: the number of sectores of directory record
 * [outputs] 
 * [Return ] error code
 * [Notes  ] 
 * (a) refer to sample program.
 */
RofsErr CRIAPI		ROFS_SetIsoDir(CriChar8 *volname, void *dir_buf, CriSint32 nsct);

/* カレントディレクトリの変更
 * [入  力] dirname		: ディレクトリ名
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 * (a) 完了復帰。
 * (b) n_dirent が実際のレコード数未満の場合は、ROFS_ERR_DIROVER が返る。
 */
/* Changeing of a current directory
 * [Inputs ] dirname	: directory name
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) This function is not return, while the function is processing.
 * (b) Return value is ROFS_ERR_DIROVER when n_dirent is under the 
 *     actual number of directory records.
 */
RofsErr CRIAPI		ROFS_ChangeDir(CriChar8 *dirname);

/* ファイル存在の判定
 * [入  力] fname		: ファイル名
 * [出  力] なし
 * [関数値] 有無（TRUE = 有 , FALSE = 無）
 * [備  考] 
 */
/* Judgment of file existence
 * [Inputs ] fname		: file name
 * [outputs] none
 * [Return ] TRUE = exist, FALSE = not exist
 * [Notes  ] 
 */
CriBool CRIAPI		ROFS_IsExistFile(CriChar8 *fname);

/* ファイル数の取得
 * [入  力] volname		: ボリューム名
 * [出  力] なし
 * [関数値] ファイル数（"."と".."を除くディレクトリの数を含む）
 * [備  考] 
 */
/* Gets the number of files
 * [Inputs ] volname	: volume name
 * [outputs] none
 * [Return ] the number of files 
 *           (the number of the directories except "." and ".." is included)
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetNumFiles(CriChar8 *volname);

/* ディレクトリ情報の取得
 * [入  力] volname		: ボリューム名
 *			num			: レコード数の上限
 * [出  力] flist		: ファイル情報（レコード数分の配列）
 * [関数値] レコード数
 * [備  考] 
 */
/* Gets directory information
 * [Inputs ] voname		: volume name
 *			 num		: The maximum of the number of records
 * [outputs] flist		: file information (array of the number of records)
 * [Return ] the number of records
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetDirInf(CriChar8 *volname, RofsFileInf *flist, CriSint32 num);

/*============================================================================
 *      スクランブル
 *		Scramble
 *===========================================================================*/
/* スクランブル情報の取得
 * [入  力] fname		: ファイル名（又はディレクトリ名）
 * [出  力] scrinf		: スクランブル情報
 * [関数値] エラー
 * [備  考] 
 * (a) サンプルプログラム参照
 */
/* Gets scramble information
 * [Inputs ] fname		: file name (or directory name)
 * [outputs] scrinf		: scramble information
 * [Return ] error code
 * [Notes  ] 
 * (a) refer to sample program.
 */
RofsErr CRIAPI		ROFS_GetScrambleInf(CriChar8 *fname, RofsScrambleInf *scrinf);

/* スクランブルの解除
 * [入  力] buf			: スクランブルを解除するデータが入ったバッファ
 *			sctnum		: 解除するセクタ数
 *			scrinf		: スクランブル情報
 * [出  力] scrinf		: スクランブル情報（更新後）
 * [関数値] エラー
 * [備  考] 
 * (a) scrinf の内容は更新される。
 * (b) サンプルプログラム参照
 */
/* Descramble
 * [Inputs ] buf		: scrambled data
 *			 sctnum		: buffer size (the number of sector)
 *			 scrinf		: scramble information
 * [outputs] scrinf		: scramble information (after update)
 * [Return ] error code
 * [Notes  ] 
 * (a) The contents of scrinf are updated.
 * (b) refer to sample program.
 */
RofsErr CRIAPI		ROFS_Descramble(void *buf, CriSint32 sctnum, RofsScrambleInf *scrinf);

/*============================================================================
 *      エラー処理
 *		Handle Error
 *===========================================================================*/
/* エラーコールバック関数の登録
 * [入  力] errfunc		: エラーコールバック関数
 *			obj			: エラーオブジェクト
 * [出  力] なし
 * [関数値] なし
 * [備  考] 
 */
/* Registration of an error callback function
 * [Inputs ] errfunc	: error callack function
 *			 obj		: error object
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 */
void CRIAPI		ROFS_EntryErrFunc(RofsErrFunc errfunc, void *obj);

/* 最後に発生したエラーを取得する
 * [入  力] なし
 * [出  力] なし
 * [関数値] エラー
 * [備  考] 
 */
/* Gets last error code
 * [Inputs ] none
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_GetLastError(void);

/*============================================================================
 *      64bit対応
 *		64bit Operation
 *===========================================================================*/
/* 転送済みサイズの取得 / 64bit版 / PS2版
 * [入  力] rofs		: ROFSハンドル
 * [出  力] なし
 * [関数値] 転送済みデータサイズ（バイト数）
 * [備  考] 
 */
/* Gets transferred data size / 64bit / for PS2
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] transferred date size (byte)
 * [Notes  ] 
 */
CriSint64 CRIAPI		ROFS_GetNumTr64(Rofs rofs);

/* ファイルサイズの取得 / 64bit版 / PS2版
 * [入  力] fname		: ファイル名
 * [出  力] なし
 * [関数値] ファイルサイズ（バイト数）
 * (a) 負値の場合はサイズでは無く、エラーを示す。
 * (b) ファイルサイズが2GB以上の場合 0x7fffffffffffffff を返す。
 */
/* Gets file size / 64bit / for PS2
 * [Inputs ] fname		: file name
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint64 CRIAPI		ROFS_GetFileSiz64(CriChar8 *fname);

/* ファイルサイズの取得（ディレクトリレコード指定） / 64bit版 / PS2版
 * [入  力] fname		: ファイル名
 *			dir_buf		: ディレクトリレコード
 * [出  力] なし
 * [関数値] ファイルサイズ（バイト数）
 * [備  考] 
 * (a) 負値の場合はサイズでは無く、エラーを示す。
 * (b) ファイルサイズが2GB以上の場合 0x7fffffff を返す。
 */
/* Gets file size
 * [Inputs ] fname		: file name
 *			 dir_buf	: directory record
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint64 CRIAPI		ROFS_GetFileSizEx64(CriChar8 *fname, void *dir_buf);

/*============================================================================
 *      ディレクトリレコードファイル関連
 *		For directory recored file
 *===========================================================================*/
/* ディレクトレコードファイルとCVMの整合性チェック
 * [入  力] vol		: ボリューム名
 *			cvmtim	: CVMファイルのタイムスタンプデータ（ビルダが出力する）
 * [出  力] なし
 * [関数値] エラー
 * [備  考]
 * ディレクトリレコードファイルは、実際に使うCVMから作成しなくてはならない。
 * 本関数で不整合が起きていないかをチェックできる。
 * 以下の場合にエラーが返る。
 * (a) ボリューム名が間違っている。（例. 未登録ボリューム）
 * (b) ディレクトリリストが現在のCVMファイルから作られたものではない。
 */
/* Check validness of the directory list
 * [Inputs ] vol	: Volume name
 *			 cvmtim	: Time stamp of CVM file (Output by ROFSBLD)
 * [outputs] none
 * [Return ] error code
 * [Notes  ]
 * You have to create a directory record file from actually used CVM.
 * It can be confirmed whether mismatching has occurred with this function.
 * Error code is returned in the following cases.
 * (a) When the volume name is wrong. (EX. non registerd volume)
 * (b) When the directory list is not created from current CVM file.
 */
RofsErr CRIAPI	ROFS_CheckDirrecFile(CriChar8 *volname, void *cvmtim);

/*============================================================================
 *      その他
 *		Other
 *===========================================================================*/
/* バージョン文字列の取得
 * [入  力] なし
 * [出  力] なし
 * [関数値] バージョン文字列のポインタ
 * [備  考]
 */
/* Gets version string
 * [Inputs ] none
 * [outputs] none
 * [Return ] version string
 * [Notes  ] 
 */
const CriChar8	* CRIAPI	ROFS_GetVersionStr(void);

/* データ読み込み待ちタイムアウトの設定
 * [入  力] sec		: タイムアウトするまでの時間（秒）
 * [出  力] なし
 * [関数値] なし
 * [備  考]
 * (a) データ読み込み時、sec で設定した時間が経過してもデータが
 *     読み込まれない場合、ROFS_ERR_TOUT が発生する。
 * (b) 本関数で設定した待ち時間は、ボリュームの登録時やディレクトリ
 *     の読み込みなど、完了復帰の読み込みの場合に適用される。
 * (c) デフォルトでは約５秒に設定されている。
 */
/* Setup of the waiting timeout for data reading
 * [Inputs ] sec	: Time until it carries out a timeout (second)
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 * (a) At the time of data reading, even if the time set up by sec passes,
 *     when data is not read, ROFS_ERR_TOUT occurs. 
 * (b) In readings of a completion return, such as registration of volume,
 *     and a directory, the waiting time set up with this function is applied.
 * (c) Default is about 5 seconds.
 */
void CRIAPI		ROFS_SetTimeoutTime(CriUint32 sec);

/* 動作条件の設定 
 * [入  力] rofs		: ROFSハンドル（NULL = 全体）
 *			condid		: 動作条件ID
 *			val			: 設定値
 * [出  力] なし
 * [関数値] エラー
 * [備  考]
 */
/* Set operating condition
 * [Inputs ] rofs		: ROFS handle
 *			 condid		: operating condition id
 *			 val		: parameter
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI	ROFS_SetCond(Rofs rofs, RofsCondId condid, CriSint32 val);

/* 動作条件の取得
 * [入  力] rofs		: ROFSハンドル（NULL = 全体）
 *			condid		: 動作条件ID
 * [出  力] val			: 設定値
 * [関数値] エラー
 * [備  考]
 */
/* Get operating condition
 * [Inputs ] rofs		: ROFS handle
 *			 condid		: operating condition id
 * [outputs] val		: parameter
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI	ROFS_GetCond(Rofs rofs, RofsCondId condid, CriSint32 *val);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CRI_ROFS_H_ */

/*****************************************************************************
 *      技術情報
 *		Technical information
 *****************************************************************************/
#if 0
(1) ディレクトリレコードモード
  　通常本ライブラリはCVMファイルからディレクトリレコードを取得します。
  ディレクトリレコードモードを ROFS_PARM_DIRSRC_MEM に設定することで、
  外部に持つことが可能です。
  　このモードではライブラリ作業領域やAddVolumeやLoadDir時のオーバヘッド
  を減らすことができます。

  設定方法）
    err = ROFS_SetCond(NULL, ROFS_COND_DIRSRC, ROFS_PARM_DIRSRC_MEM);

  注意事項）
  (a) このモードを使うには、あらかじめROFSBLDでディレクトレコードファイルを
      出力しておく必要がある。
  (b) ROFS_Init関数の引数、RofsInitPara構造体のmax_direntメンバは最低３必要。
  (c) ROFS_Init関数の直後に設定すること。
      こうすることで、ROFS_AddVolume関数実行時にルートディレクトリの読み込み
      処理が無くなるのでマウントが速くなる。
  (d) このモードでは以下の関数を使ってはならない。
      ROFS_ChangeDir
  (e) 元のモードには戻せない。

/*---------------------------------------------------------------------------*/

(1) Directory record mode
  Usualy this liblary get directory records fom CVM file.
  Supported that change directory with directory record file which is 
  created by rofsbld.exe.
  When AddVolume and LoadDir it can save the liblary work memory and 
  reduce process overhead.

  How to)
    err = ROFS_SetCond(NULL, ROFS_COND_DIRSRC, ROFS_PARM_DIRSRC_MEM);

  Note)
  (a) In order to use this mode, it is necessary to output directory lists
      with ROFSBLD beforehand.
  (b) The argument of a ROFS_Init function and the max_dirent member of a 
      RofsInitPara structure object are at least 3 necessities.
  (c) Please call immediately after a ROFS_Init function.
      By carrying out like this, since reading processing of a 
      root directory is lost at the time of ROFS_AddVolume function 
      execution, mount becomes quick.
  (d) Do not use the following functions.
      ROFS_ChangeDir
  (e) It cannot restore the original mode.
#endif

/* end of file */
