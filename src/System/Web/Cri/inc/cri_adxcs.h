/***************************************************************************
 *
 *	CRI Middleware SDK
 *
 *	Copyright (c) 2003-2007 CRI-MW
 *
 *	Library	: ADX Library
 *	Module	: ADX Cinematic Sound System
 *	File	: cri_adxcs.h
 *	Create	: 2003-05-21
 *
 ***************************************************************************/

/* 多重定義防止					*/
/* Prevention of redefinition	*/
#ifndef _ADXCS_H_INCLUDED
#define _ADXCS_H_INCLUDED

/***************************************************************************
 *       バージョン情報
 *       Version
 ***************************************************************************/
#define ADXCS_VER	"1.25"

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include <cri_xpt.h>
#include <cri_adxt.h>
#include <cri_aixp.h>

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/* 内部ハンドル（ADXTまたはAIXP）の最大数				*/
/* The maximum number of internal handles(ADXT or AIXP)	*/
#define ADXCS_MAX_INTERNAL_HNDL			(2)


/* フェードアウト時間のデフォルト値		*/
/* The default value of fade-out time	*/
#define ADXCS_DEF_FADE_OUT_TIME			(30)				/* 0.5sec	*/

/* フェードイン時間のデフォルト値		*/
/* The default value of fade-in time	*/
#define ADXCS_DEF_FADE_IN_TIME			(0)

/* ボリューム変更時間のデフォルト値			*/
/* The default value of volume change time	*/
#define ADXCS_DEF_VOL_CHANGE_TIME		(0)

/* フェードイン開始オフセットのデフォルト値				*/
/* The default value of offset of fade-in start time	*/
/* from fade-out start time								*/
#define ADXCS_DEF_FADE_IN_START_OFFSET	(0)

/* フェード後のディレイ時間のデフォルト値	*/
/* The default value of fade-end delay		*/
#define ADXCS_DEF_FADE_END_DELAY		(60)				/* 1sec		*/

/* フェード時間の最小値				*/
/* The minimum value of fading time	*/
#define ADXCS_FADE_TIME_MIN				(0)

/* フェード時間の最大値				*/
/* The maximum value of fading time	*/
#define ADXCS_FADE_TIME_MAX				(1800)				/* 30sec	*/


/* ボイスハイライトのボリューム減少時間のデフォルト値			*/
/* The default value of volume decrease time of Voice-Highlight	*/
#define ADXCS_DEF_VHL_DEC_TIME			(30)				/* 0.5sec	*/

/* ボイスハイライトのボリューム増加時間のデフォルト値			*/
/* The default value of volume increase time of Voice-Highlight	*/
#define ADXCS_DEF_VHL_INC_TIME			(60)				/* 1sec		*/

/* ボイスハイライトのボリューム減少量のデフォルト値						*/
/* The default value of quantity of volume decrease of Voice-Highlight	*/
#define ADXCS_DEF_VHL_DEC_VOL			(120)				/* 12dB		*/


/* ADXCSハンドルの再生状態			*/
/* Playing state of a ADXCS handle	*/
#define ADXCS_STAT_STOP			(0)	/* 停止中			(Stop)				*/
#define ADXCS_STAT_PREP			(1) /* 再生準備中		(Prepare playing)	*/
#define ADXCS_STAT_PLAYING		(2) /* デコード＆再生中	(Decoding & Playing)*/
#define ADXCS_STAT_PLAYEND		(3) /* 再生終了			(End of playback)	*/
#define ADXCS_STAT_ERROR		(4) /* エラー発生		(Error)				*/

/* ADXCSハンドルのフェード状態		*/
/* Fading state of a ADXCS handle	*/
#define ADXCS_FADE_STAT_IDLE	(0)	/* 休止状態			(Idle state)		*/
#define ADXCS_FADE_STAT_FADING	(1)	/* フェード中		(Processing fading)	*/

/* サウンドの種類	*/
/* Type of sound	*/
#define ADXCS_SND_TYPE_NONE		(0)	/* 無所属			(Independent)		*/
#define ADXCS_SND_TYPE_BGM		(1)	/* BGM				(Background music)	*/
#define ADXCS_SND_TYPE_VOICE	(2)	/* セリフ			(Voice)				*/
#define ADXCS_SND_TYPE_SE		(3)	/* 効果音			(Sound effect)		*/

/* コールバック関数の種類		*/
/* Type of callback function	*/
#define ADXCS_CB_START_FADE_OUT		(0) /* フェードアウト開始				*/
										/* Start of fade-out				*/
#define ADXCS_CB_START_FADE_IN		(1) /* フェードイン開始					*/
										/* Start of fade-in					*/
#define ADXCS_CB_START_VOL_CHANGE	(2) /* フェードボリュームチェンジ開始	*/
										/* Start of volume chage			*/
#define ADXCS_CB_END_FADE_OUT		(3) /* フェードアウト終了				*/
										/* End of fade-out					*/
#define ADXCS_CB_END_FADE_IN		(4) /* フェードイン終了					*/
										/* End of fade-in					*/
#define ADXCS_CB_END_VOL_CHANGE		(5) /* ボリューム変更終了				*/
										/* End of volume change				*/
#define ADXCS_CB_END_FADE_ALL		(6) /* 全フェード終了					*/
										/* End of all process of fading		*/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/* ADXCSハンドル	*/
/* ADXCS Handle		*/
#ifndef ADXCS_DEFINED
#define ADXCS_DEFINED
typedef void *ADXCS;
#endif

/* ハンドル作成パラメータ		*/
/* Handle creation parameter	*/
typedef struct {
	CriSint32	max_stm;		/* 最大同時再生ディスクストリーム数				*/
							/* Maximum number of stream which play at the	*/
							/* same time									*/
	CriSint32	max_ch;			/* 最大チャネル数								*/
							/* Maximum number of channel					*/
	CriSint32	max_sfreq;		/* 最大サンプリング周波数						*/
							/* Maximum sampling frequency					*/
	CriSint32	max_aix_track;	/* AIXの最大トラック数							*/
							/* Maximum number of track of AIX				*/ 
	CriSint32	play_method;	/* 再生方式(ADXT_PLY_MEM / ADXT_PLY_STM)		*/
							/* Playback method(ADXT_PLY_MEM / ADXT_PLY_STM)	*/
	CriSint8	sound_type;		/* サウンドの種類								*/
							/* Type of sound								*/
	CriBool	use_crossfade;	/* クロスフェードをするか否か					*/
							/* Whether handle can use crossfade function	*/
	CriBool	use_aix;		/* AIXを使用するか否か							*/
							/* Whether handle can use AIX playback function	*/
	CriBool	use_ahx;		/* AHXを使用するか否か							*/
							/* Whether handle can use AHX playback function	*/
	CriBool	use_3d_panning;	/* リアルタイム3Dパンニングを使用するか否か		*/
							/* Whether handle can use realtime 3D panning	*/
} AdxcsCrePrm;

/* 
 * 注意：
 * リアルタイム3Dパンニングを使用する場合（use_3d_panningがTRUE）、最大チャネ
 * ル数は無視され、プラットフォーム固有の値になります。
 * また、このとき入力可能な音声はモノラルのみです。
 * Notice:
 * If you want to use 3D panning(use_3d_panning flag is TRUE), maximum number
 * of channel(max_ch) is ignored. It is set to the platform's unique value
 * internally.
 * And then, only a monaural sound can be inputted.
 */

/***************************************************************************
 *      変数宣言
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 *      初期化・終了処理
 *		Initialization / Finish
 *-------------------------------------------------------------------------*/
/* 初期化処理						*/
/* Initializes the ADXCS library	*/
void CRIAPI ADXCS_Init(void);

/* 終了処理						*/
/* Finishes	the ADXCS library	*/
void CRIAPI ADXCS_Finish(void);


/*--------------------------------------------------------------------------
 *      ハンドルの作成・破棄
 *		Handle Creation / Destruction
 *-------------------------------------------------------------------------*/
/* ADXCSハンドルの作成		*/
/* Creates a ADXCS handle	*/
ADXCS CRIAPI ADXCS_Create(const AdxcsCrePrm *cprm,
						  CriSint8 *work, CriSint32 work_size);

/* ADXCSハンドルの破棄		*/
/* Destroys a ADXCS handle	*/
void CRIAPI ADXCS_Destroy(ADXCS adxcs);


/* ワーク領域サイズを計算する			*/
/* Calculates the work size of a handle	*/
CriSint32 CRIAPI ADXCS_CalcWorkCprm(const AdxcsCrePrm *cprm);


/* AHX再生機能の組み込み					*/
/* Attaches the function of AHX playback	*/
void CRIAPI ADXCS_AttachAhx(ADXCS adxcs);

/* AHX再生機能の分離						*/
/* Detaches the function of AHX playback	*/
void CRIAPI ADXCS_DetachAhx(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      基本再生処理
 *		Basic Playback
 *-------------------------------------------------------------------------*/
/* ファイル名指定による再生の開始			*/
/* Starts a playback by specified filename	*/
void CRIAPI ADXCS_StartFname(ADXCS adxcs, const CriChar8 *fname);

/* ファイル名指定による再生の開始（範囲指定付き）					*/
/* Starts a playback by specified filename and the range as sector 	*/
void CRIAPI ADXCS_StartFnameRange(ADXCS adxcs, const CriChar8 *fname, CriSint32 ofst, CriSint32 nsct);

/* AFSファイル内のFID指定による再生の開始			*/
/* Starts a playback by specified FID in a AFS file	*/
void CRIAPI ADXCS_StartAfs(ADXCS adxcs, CriSint32 patid, CriSint32 fid);

/* メモリ内ファイルの再生の開始									*/
/* Starts a playback by specified address of a file in memory	*/
void CRIAPI ADXCS_StartMem(ADXCS adxcs, void *dat, CriSint32 datlen);

/* メモリインデックス指定による再生の開始									*/
/* Starts a playback by specified index and address of a ACX data in memory	*/
void CRIAPI ADXCS_StartMemIdx(ADXCS adxcs, void *acx, CriSint32 no);

/* 再生の停止		*/
/* Stops a playback	*/
void CRIAPI ADXCS_Stop(ADXCS adxcs);

/* 一時停止の設定				*/
/* Pauses or cancels pausing	*/
void CRIAPI ADXCS_Pause(ADXCS adxcs, CriSint32 sw);

/* 一時停止状態の取得	*/
/* Gets pausing state	*/
CriSint32 CRIAPI ADXCS_GetStatPause(ADXCS adxcs);

/* 再生状態の取得		*/
/* Gets playing state	*/
CriSint32 CRIAPI ADXCS_GetStat(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      パラメータ設定
 *		Parameter Settings
 *-------------------------------------------------------------------------*/
/* ハンドル作成パラメータの取得		*/
/* Gets a handle creation parameter	*/
const AdxcsCrePrm *CRIAPI ADXCS_GetCprm(ADXCS adxcs);

/* ループフラグの設定	*/
/* Sets the loop flag	*/
void CRIAPI ADXCS_SetLpFlg(ADXCS adxcs, CriSint32 flg);

/* サウンドの種類の設定	*/
/* Sets the type of sound	*/
void CRIAPI ADXCS_SetType(ADXCS adxcs, CriSint32 type);

/* サウンドの種類の取得	*/
/* Gets the type of sound	*/
CriSint32 CRIAPI ADXCS_GetType(ADXCS adxcs);


/* 出力ボリュームの設定	*/
/* Sets a output volume	*/
void CRIAPI ADXCS_SetOutVol(ADXCS adxcs, CriSint32 vol);

/* 出力ボリュームの取得	*/
/* Gets a output volume	*/
CriSint32 CRIAPI ADXCS_GetOutVol(ADXCS adxcs);

/* 次回再生時の出力ボリュームの設定			*/
/* Sets a output volume of next playback	*/
void CRIAPI ADXCS_SetOutVolNextPlayback(ADXCS adxcs, CriSint32 vol);

/* トラックごとの出力ボリュームの設定	*/
/* Sets a output volume of each track	*/
void CRIAPI ADXCS_SetTrackVol(ADXCS adxcs, CriSint32 trno, CriSint32 vol);

/* トラックごとの出力ボリュームの取得	*/
/* Gets a output volume of each track	*/
CriSint32 CRIAPI ADXCS_GetTrackVol(ADXCS adxcs, CriSint32 trno);


/* フェードアウト時間の設定（V-Sync単位）	*/
/* Sets a time of fade-out(V-Sync)			*/
void CRIAPI ADXCS_SetFadeOutTime(ADXCS adxcs, CriSint32 time);

/* フェードアウト時間の取得（V-Sync単位）	*/
/* Gets a time of fade-out(V-Sync)			*/
CriSint32 CRIAPI ADXCS_GetFadeOutTime(ADXCS adxcs);

/* フェードイン時間の設定（V-Sync単位）	*/
/* Sets a time of fade-in(V-Sync)		*/
void CRIAPI ADXCS_SetFadeInTime(ADXCS adxcs, CriSint32 time);

/* フェードイン時間の取得（V-Sync単位）	*/
/* Gets a time of fade-in(V-Sync)		*/
CriSint32 CRIAPI ADXCS_GetFadeInTime(ADXCS adxcs);

/* ボリューム変更時間の設定（V-Sync単位）	*/
/* Sets a time of volume change(V-Sync)		*/
void CRIAPI ADXCS_SetVolChangeTime(ADXCS adxcs, CriSint32 time);

/* ボリューム変更時間の取得（V-Sync単位）	*/
/* Gets a time of volume change(V-Sync)		*/
CriSint32 CRIAPI ADXCS_GetVolChangeTime(ADXCS adxcs);


/* フェードアウトからの相対フェードイン開始時刻の設定（V-Sync単位）			*/
/* Sets a offset of a fade-in start time from a fade-out start time(V-Sync)	*/
void CRIAPI ADXCS_SetFadeInStartOffset(ADXCS adxcs, CriSint32 offset);

/* フェードアウトからの相対フェードイン開始時刻の取得（V-Sync単位）			*/
/* Gets a offset of a fade-in start time from a fade-out start time(V-Sync)	*/
CriSint32 CRIAPI ADXCS_GetFadeInStartOffset(ADXCS adxcs);


/* フェード後のディレイ時間の設定（V-Sync単位） */
/* Sets a fade-end delay time(V-Sync)			*/
void CRIAPI ADXCS_SetFadeEndDelay(ADXCS adxcs, CriSint32 delay);

/* フェード後のディレイ時間の取得（V-Sync単位） */
/* Gets a fade-end delay time(V-Sync)			*/
CriSint32 CRIAPI ADXCS_GetFadeEndDelay(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      内部ハンドルの取得
 *		Getting Internal Handle
 *-------------------------------------------------------------------------*/
/* 現在アクティブな内部ADXTハンドルを取得			*/
/* Gets a internal ADXT handle which is active now	*/
ADXT CRIAPI ADXCS_GetActiveAdxt(ADXCS adxcs);

/* 次に切り替わる予定の内部ADXTハンドルを取得							*/
/* Gets a internal ADXT handle which will be changed at a next start	*/
ADXT CRIAPI ADXCS_GetNextAdxt(ADXCS adxcs);

/* インデックス指定で内部ADXTハンドルを取得	*/
/* Gets a internal ADXT handle from index	*/
ADXT CRIAPI ADXCS_GetAdxt(ADXCS adxcs, CriSint32 index);

/* 内部ADXTハンドルの数を取得				*/
/* Gets the number of internal ADXT handles	*/
CriSint32 CRIAPI ADXCS_GetNumAdxt(ADXCS adxcs);


/* 現在アクティブな内部AIXPハンドルを取得			*/
/* Gets a internal AIXP handle which is active now	*/
AIXP CRIAPI ADXCS_GetActiveAixp(ADXCS adxcs);

/* 次に切り替わる予定の内部AIXPハンドルを取得							*/
/* Gets a internal AIXP handle which will be changed at a next start	*/
AIXP CRIAPI ADXCS_GetNextAixp(ADXCS adxcs);

/* インデックス指定で内部AIXPハンドルを取得	*/
/* Gets a internal AIXP handle from index	*/
AIXP CRIAPI ADXCS_GetAixp(ADXCS adxcs, CriSint32 index);

/* 内部AIXPハンドルの数を取得				*/
/* Gets the number of internal AIXP handles	*/
CriSint32 CRIAPI ADXCS_GetNumAixp(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      フェード処理
 *		Fading Process
 *-------------------------------------------------------------------------*/
/* フェード処理をキャンセルして元の音量に戻す			*/
/* Cancels the fading process and turns the volume back	*/
void CRIAPI ADXCS_CancelFading(ADXCS adxcs);

/* フェード状態の取得		*/
/* Gets the fading state	*/
CriSint32 CRIAPI ADXCS_GetFadeStat(ADXCS adxcs);

/* フェード終了コールバック関数の登録			*/
/* Registers the function of fade-end callback	*/
void CRIAPI ADXCS_SetCbFunc(ADXCS adxcs,
							void (*fn)(ADXCS adxcs, CriSint32 cb_type));

/*--------------------------------------------------------------------------
 *      ボイスハイライト
 *		Voice-Highlight
 *-------------------------------------------------------------------------*/
/* ボイスハイライト機能を有効にする		*/
/* Enables the Voice-Highlight function	*/
void CRIAPI ADXCS_EnableVhl(void);

/* ボイスハイライト機能を無効にする			*/
/* Disables the Voice-Highlight function	*/
void CRIAPI ADXCS_DisableVhl(void);


/* ボリューム減少に要する時間を設定する（V-Sync単位）	*/
/* Sets the time(V-Sync) of the volume decrease			*/
void CRIAPI ADXCS_SetVhlDecTime(CriSint32 dec_time);

/* ボリューム減少に要する時間を取得する（V-Sync単位）	*/
/* Gets the time(V-Sync) of the volume decrease			*/
CriSint32 CRIAPI ADXCS_GetVhlDecTime(void);

/* ボリューム増加に要する時間を設定する（V-Sync単位）	*/
/* Sets the time(V-Sync) of the volume increase			*/
void CRIAPI ADXCS_SetVhlIncTime(CriSint32 inc_time);

/* ボリューム増加に要する時間を取得する（V-Sync単位）	*/
/* Gets the time(V-Sync) of the volume increase			*/
CriSint32 CRIAPI ADXCS_GetVhlIncTime(void);

/* ボリューム減少量を設定する		*/
/* Sets the quantity of the volume decrease	*/
void CRIAPI ADXCS_SetVhlDecVol(CriSint32 dec_vol);

/* ボリューム減少量を取得する		*/
/* Gets the quantity of the volume decrease	*/
CriSint32 CRIAPI ADXCS_GetVhlDecVol(void);

/*--------------------------------------------------------------------------
 *      サーバ処理
 *		Server Process
 *-------------------------------------------------------------------------*/
/* サーバ処理				*/
/* Executes server process	*/
void CRIAPI ADXCS_ExecServer(void);

/* ハンドル毎のサーバ処理					*/
/* Executes server process of each handle	*/
void CRIAPI ADXCS_ExecHndl(ADXCS adxcs);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef _ADXCS_H_INCLUDED */

/* end of file */
