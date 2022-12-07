/***************************************************************************
 *
 *  CRI Middleware
 *
 *  Copyright (c) 2001-2007 CRI-MW
 *
 *  Library : ADX Library (CRI ADX for PC)
 *  Module  : Functions for PC
 *  File    : adx_pc.h
 *
 ***************************************************************************/
/* Prevention of redefinition */
#ifndef _ADXPC_H_INCLUDED
#define _ADXPC_H_INCLUDED

/***************************************************************************
 *       バージョン情報
 *       Version
 ***************************************************************************/
#define ADXPC_VER					"1.74"

/***************************************************************************
 *      インクルードファイル
 *      Include file
 ***************************************************************************/

/* Disable VC++ warning 
 *   In some cases, including windows.h, dsound.h causes the following warning:
 *       rpcasync.h(45) : warning C4115: '_RPC_ASYNC_STATE' : ...
 */
#if defined(_MSC_VER) && ((_MSC_VER >= 1200) && (_MSC_VER < 1300))
#pragma warning(push)
#pragma warning(disable:4115)
#endif

#include <windows.h>
#include <dsound.h>

#if defined(_MSC_VER) && ((_MSC_VER >= 1200) && (_MSC_VER < 1300))
#pragma warning(pop)
#endif

#include "cri_xpt.h"
#include "cri_adxt.h"

/***************************************************************************
 *      定数マクロ
 *      MACRO CONSTANT
 ***************************************************************************/
/* デフォルトプライオリティ */
/* Default thread priority */
#define ADXM_PRIO_VSYNC				(THREAD_PRIORITY_HIGHEST)		/* Vsyncスレッド */
#define ADXM_PRIO_FS				(THREAD_PRIORITY_ABOVE_NORMAL)	/* ファイル処理スレッド */
#define ADXM_PRIO_MWIDLE			(THREAD_PRIORITY_IDLE)			/* アイドルスレッド */

/* スタックサイズ */
/* Thread stack size */
#define	ADXM_STACK_SIZE_VSYNC		(1024 * 1024)
#define	ADXM_STACK_SIZE_FS			(1024 * 1024)
#define	ADXM_STACK_SIZE_MWIDLE		(1024 * 1024)

/* ディレクトリ探索モード		*/
/* Mode of directory look-up	*/
#define ADXFIC_MODE_ALLDIR			(-1)
#define ADXFIC_MODE_CURRENT			(0)

/* ADXFICハンドルステータス	 */
/* Status of ADXFIC handle */
#define ADXFIC_STAT_STOP			(0)
#define ADXFIC_STAT_CACHING			(1)
#define ADXFIC_STAT_CACHEEND		(2)
#define ADXFIC_STAT_ERROR			(3)

/* デフォルト同期周波数x100					*/
/* Default of virtual vsync frequency x100	*/
#define ADXPC_DEF_VHZ100			(6000)

/***************************************************************************
 *      処理マクロ
 *      MACRO FUNCTIONS
 ***************************************************************************/
/* ワーク領域最大サイズ計算マクロ */
/* Macro that calculates maximum work area size */
#define ADXFIC_CALC_WORK_SIZE(nfiles, max_flen) \
	((nfiles + 1) * (16 + maxflen) + 32)

/***************************************************************************
 *      データ型宣言
 *      Data type declaration
 ***************************************************************************/
/*--------------------------------------------------------------------------
 * フレームワーク種別（PC）
 * Type of framework
 *-------------------------------------------------------------------------*/
typedef enum {
	ADXM_FRAMEWORK_DEFAULT			= 0,
	ADXM_FRAMEWORK_PC_SINGLE_THREAD	= 1,
	ADXM_FRAMEWORK_PC_MULTI_THREAD	= 2,
} AdxmFramework;

/*--------------------------------------------------------------------------
 * スレッドセットアップパラメータ（PC）
 * Thread setup parameter for PC
 *-------------------------------------------------------------------------*/
typedef struct {
	/* プライオリティ */
	CriSint32			priority_vsync;		/* Vsyncスレッド */
	CriSint32 			priority_fs;		/* ファイル処理スレッド */
	CriSint32 			priority_mwidle;	/* アイドルスレッド */
} AdxmThreadSprm;

/* スレッドのセットアップを行う際の注意点									*/
/* (a) セットアップパラメータは、フレームワークとして						*/
/* 　　ADXM_FRAMEWORK_Pc_MULTI_THREADを選んだ場合のみ有効です。				*/
/* (b) セットアップパラメータは、ADXM_SetupFramework関数に引数prmと			*/
/* 　　して渡します。NULLを指定するとデフォルト設定が使用されます。			*/
/* Notes when setup of thread												*/
/* (a) If you need to apply custom framework setting, set the setup			*/
/*     parameter to ADXM_SetupFramework function as argument "prm".			*/
/*     When NULL is passed, the default setting is used.	 				*/
/* (b) The setup parameter is effective only when you chose 				*/
/*     ADXM_FRAMEWORK_PC_MULTI_THREAD as framework.							*/

/*--------------------------------------------------------------------------
 * ファイルシステムのセットアップパラメータ構造体
 * Parameter structure for file system setup function
 *-------------------------------------------------------------------------*/
typedef struct {
	CriChar8	*rtdir;				/* ルートディレクトリ	*/
								/* Root directory		*/
} AdxpcSprmFs;

/*--------------------------------------------------------------------------
 * ファイル情報キャッシュハンドル（ADXFICハンドル）
 * File information cache handle (ADXFIC handle)
 *-------------------------------------------------------------------------*/
typedef void *ADXFIC;

/*--------------------------------------------------------------------------
 * V同期システム用オブジェクトの種類
 * Kind of the object for Vsync system
 *-------------------------------------------------------------------------*/
typedef enum {
	ADXPC_VIDEO_NONE = 0x00,
	ADXPC_VIDEO_DDRAW7 = 0x01,		// DirectDraw7
	ADXPC_VIDEO_D3D8 = 0x02,		// Direct3D8
} ADXPC_VIDEO;

/***************************************************************************
 *      変数宣言
 *      Prototype Function 
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Function 
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/***
*			フレームワーク設定関数
*			Framework setting function
***/

/* CRIミドルウェアのフレームワークセットアップ */
/* Setup the framework */
CriBool CRIAPI ADXM_SetupFramework(AdxmFramework framework, void *prm);

/* CRIミドルウェアのフレームワークの終了 */
/* Shutdown the framework */
CriBool CRIAPI ADXM_ShutdownFramework(void);

/***
 *          スレッド関係
 *          Thread
 ***/
 
/* ADXサーバ関数(非スレッド用)							*/
/* ADX server function(for using ADX without thread)	*/
void CRIAPI ADXPC_ExecServer(void);

/* ADXサーバ関数(スレッド用)						*/
/* ADX server function(for using ADX with thread)	*/
CriBool CRIAPI ADXPC_ExecServerEx(void *obj);

/***
 *          ファイル処理関係
 *          File proseccing
 ***/

/* ファイルシステムのセットアップ	*/
/* Set up the file system			*/
void CRIAPI ADXPC_SetupFileSystem(AdxpcSprmFs *sprmd);

/* ファイルシステムのシャットダウン	*/
/* Shut down the file system		*/
void CRIAPI ADXPC_ShutdownFileSystem(void);

/***
 *          V同期関係
 *          V-Sync system
 ***/

/* V同期システムをセットアップ	*/
/* Set up the V-Sync system		*/
CriSint32 CRIAPI ADXPC_SetupVsync(void *video_obj, CriSint32 mode);

/* V同期システムをシャットダウン	*/
/* Shut down the V-Sync system		*/
CriBool CRIAPI ADXPC_ShutdownVsync(void);

/* V同期システムを再設定	*/
/* Reset the V-Sync system	*/
CriSint32 CRIAPI ADXPC_ResetVsync(void);

/* ADXサーバー処理をVBlankからどれだけずらすか設定	*/
/* Set ADX server offset from VBlank				*/
void CRIAPI ADXPC_SetSvrOffset(CriSint32 offset_msec);

/* 仮想V周波数の設定（1/100Hz単位、60Hzなら6000）				*/
/* Set virtual Vsync frequency	(in unit of 1/100Hz, 60Hz=6000)	*/
void CRIAPI ADXPC_SetVirtualVhz(CriUint32 vhz100);

/* 仮想V周波数の取得			*/
/* Get virtual Vsync frequency	*/
CriUint32 CRIAPI ADXPC_GetVirtualVhz(void);

/* Presentタイミングの通知 */
void CRIAPI ADXPC_NotifyPresentTiming(void);

/* Presentタイミングマージンの設定 [%] */
void CRIAPI ADXPC_SetPresentTimingMargin(CriSint32 margin);

/***
 *          ファイル情報キャッシング用関数
 *          Functions for file information cache
 ***/

/* FICの初期化処理	*/
/* Initialize FIC	*/
void CRIAPI ADXFIC_Init(void);

/* 終了処理		*/
/* Finish FIC	*/
void CRIAPI ADXFIC_Finish(void);

/* 作業領域サイズの取得		*/
/* Get size of work area	*/
CriSint32 CRIAPI ADXFIC_GetWorkSize(const CriChar8 *dname, CriSint32 mode);

/* ADXFICハンドルの作成	*/
/* Create ADXFIC handle	*/
ADXFIC CRIAPI ADXFIC_Create(const CriChar8 *dname, CriSint32 mode, void *work, CriSint32 wksize);

/* ADXFICハンドルの消去		*/
/* Destroy ADXFIC handle	*/
void CRIAPI ADXFIC_Destroy(ADXFIC fic);

/* キャッシングされたファイル数の取得	*/
/* Get number of cached files			*/
CriSint32 CRIAPI ADXFIC_GetNumFiles(ADXFIC fic);

/* 利用されている作業領域サイズの取得	*/
/* Get size of used work area			*/
CriSint32 CRIAPI ADXFIC_GetUsedWkSize(ADXFIC fic);

/* ファイル探索を行ったディレクトリ名の取得 */
/* Get directory name for look-up			*/
CriChar8 * CRIAPI ADXFIC_GetDirName(ADXFIC fic);

/* キャッシングされたファイルのサイズを取得（セクタ単位）	*/
/* Get size of cached file (sector)							*/
CriSint32 CRIAPI ADXFIC_GetFsizeSct(ADXFIC fic, const CriChar8 *fname);

/* キャッシングされたファイルのサイズを取得（バイト単位）	*/
/* Get size of cached file (Byte)							*/
CriSint64 CRIAPI ADXFIC_GetFileSize(ADXFIC fic, const CriChar8 *fname);

/* ファイルがキャッシュされているかどうかのチェック */
/* Check whether the file is cached					*/
CriBool CRIAPI ADXFIC_IsFileCached(const CriChar8 *fname);

/* 指定ファイルのキャッシュ無効化		*/
/* Disable caching of specified file	*/
void CRIAPI ADXFIC_DisableFile(ADXFIC fic, const CriChar8 *fname);

/* 指定ファイルのキャッシュ有効化		*/
/* Enable chaching of specified file	*/
void CRIAPI ADXFIC_EnableFile(ADXFIC fic, const CriChar8 *fname);

/* ステータスの取得	*/
/* Get state		*/
CriSint32 CRIAPI ADXFIC_GetStat(ADXFIC fic);

/***
 *          その他
 *          Others
 ***/

/* バージョン文字列の取得	*/
/* Get version string		*/
CriChar8 * CRIAPI ADXPC_GetVersion(void);

/***
 *          デバッグ用
 *          For debugging
 ***/

#ifdef _DEBUG

/* ファイル名のデバッグ出力	*/
/* Print cached file names	*/
void CRIAPI ADXFIC_DebugPrintFlist(ADXFIC fic);

/* 全てのファイル名のデバッグ出力 */
/* Print all cached file names	*/
void CRIAPI ADXFIC_DebugPrintFlistAll(void);

#endif	// #ifdef _DEBUG

#ifdef __cplusplus
}
#endif	// __cplusplus

/***************************************************************************
 *      旧バージョンとの互換用
 *      For compatibility with old version
 ***************************************************************************/
#define ADXPC_SetupThrd(tprm)			ADXM_SetupThrd(tprm)
#define ADXPC_SetupThread(tprm)			ADXM_SetupThrd(tprm)
#define ADXPC_ShutdownThread()			ADXM_ShutdownThrd()
#define ADXPC_SPRM_DVD					AdxpcSprmFs
#define ADXPC_SPRM_WINFS				AdxpcSprmFs
#define ADXPC_SPRM_ANSI					AdxpcSprmFs
#define ADXPC_SetupWinFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SetupDvdFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SPRM_WINFS				AdxpcSprmFs
#define ADXPC_SPRM_DVD					AdxpcSprmFs
#define ADXPC_SPRM_ANSI					AdxpcSprmFs
#define ADXPC_SetupWinFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SetupDvdFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SetupAnsiFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_ShutdownWinFs()			ADXPC_ShutdownFileSystem()
#define ADXPC_ShutdownDvdFs()			ADXPC_ShutdownFileSystem()
#define ADXPC_ShutdownAnsiFs()			ADXPC_ShutdownFileSystem()

#endif	// #ifndef _ADXPC_H_INCLUDED

/* end of file */
