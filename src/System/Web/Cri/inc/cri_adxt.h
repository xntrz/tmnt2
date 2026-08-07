#ifndef	_ADXT_H_INCLUDED
#define	_ADXT_H_INCLUDED
/****************************************************************************/
/*																			*/
/*			$title$ ＡＤＸデータ再生 ライブラリ								*/
/*					ADXT (ADX TALK) Library									*/
/*																			*/
/*				1997.2.25		written M.Oshimi							*/
/*																			*/
/****************************************************************************/

/* This is a meaningless comment line for avoiding Warning C4819. */

/*	Version number of ADXT library 		*/
#define	ADXT_VER	"10.67"

/*	Include Files	*/
#include "cri_xpt.h"
#include "sj.h"

/*	ストリームコントローラ				*/
/*	Stream Controller					*/
#ifndef ADXSTM_DEFINED
#define ADXSTM_DEFINED
typedef	void	*ADXSTM;
#endif

/*	ストリームジョイントデコーダ		*/
/*	Stream Joint decoder				*/
#ifndef ADXSJD_DEFINED
#define ADXSJD_DEFINED
typedef void	*ADXSJD;
#endif

/*	オーディオレンダラ					*/
/*	Audio Renderer						*/
#ifndef ADXRNA_DEFINED
#define ADXRNA_DEFINED
typedef void	*ADXRNA;
#endif

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

/****************************************************************************/
/*		定数マクロ															*/
/*		MACRO CONSTANT														*/
/****************************************************************************/

/*	最大同時発音数						*/
/*	Maximum number of ADXT handle		*/
/*
#if defined(XPT_TGT_PC)
#define	ADXT_MAX_OBJ		(32)
#else
#define	ADXT_MAX_OBJ		(16)
#endif
*/

/*	最大データサイズ					*/
/*	Maximum data size					*/
#define	ADXT_MAX_DATASIZE	(0x40000000)

/*	入力バッファのエキストラ領域の大きさ	*/
/*	Size of Extra area in input buffer		*/
#define ADXT_IBUF_XLEN (36)

/*	ADX Talk の出力バッファの大きさ　（サンプル単位）	*/
/*	Output buffer size of ADX Talk (unit:sample)		*/
#if defined (XPT_TGT_NITRO)
#define	ADXT_OBUF_SIZE	(0x0800)
#else
#define	ADXT_OBUF_SIZE	(0x2000)
#endif

#if !defined(XPT_TGT_PC_LE)
#define DECODE_AHX
#endif

#ifdef DECODE_AHX
#if defined (XPT_TGT_NITRO)
#define	ADXT_OBUF_DIST	(0x0860)
#else
#define	ADXT_OBUF_DIST	(0x2060)
#endif
#else
#define	ADXT_OBUF_DIST	(0x2020)
#endif

/*	最大サンプリング周波数				*/
/*	Maximum sampling frequency			*/
#define	ADXT_MAX_SFREQ	(48000)

/*	ADX Talkの再生方式	(ADXT_CALC_WORKマクロで使用)		*/
/*	Play method of ADX Talk (used 'ADXT_CALC_WORK' macro)	*/
#define ADXT_PLY_MEM	(0)			/*	メモリからの再生					*/
									/*	Play memory data					*/
#define ADXT_PLY_STM	(1)			/*	CDからのストリーム再生				*/
									/*	Stream play from CD-ROM				*/

#if	defined(XPT_TGT_NITRO)
/* ROMメディアからの読み込み */
#define ADXT_CALC_IBUFSIZE0(nch, sfreq)	( 2500*(nch)*((sfreq)/1000)/16)
#else
#define ADXT_CALC_IBUFSIZE0(nch, sfreq)	(25000*(nch)*((sfreq)/1000)/44)
#endif

#define ADXT_CALC_IBUFSIZE(nch, nstm, sfreq)	\
	((ADXT_CALC_IBUFSIZE0(nch, sfreq)*((nstm)+1)+2048)/2048*2048+ADXT_IBUF_XLEN)

#if defined(XPT_TGT_GC) || defined(XPT_TGT_WII)
#define	ADXT_RNABUF_SIZE	(0x1000)
#define ADXT_CALC_OBUFSIZE(nch)	\
	( (ADXT_OBUF_DIST+ADXT_RNABUF_SIZE)*(nch)*sizeof(CriSint16))
#elif defined(XPT_TGT_NITRO)
#define	ADXT_RNABUF_SIZE	(6*1024/2)
#define ADXT_CALC_OBUFSIZE(nch)	\
	( (ADXT_OBUF_DIST+ADXT_RNABUF_SIZE)*(nch)*sizeof(CriSint16))
#elif defined(XPT_TGT_KTZEVIO)
#define	ADXT_RNABUF_SIZE	(8 * 1024)
#define ADXT_CALC_OBUFSIZE(nch)	\
	((ADXT_OBUF_DIST+ADXT_RNABUF_SIZE) * (nch) * sizeof(CriSint16))
#else
#define ADXT_CALC_OBUFSIZE(nch)	\
	( ADXT_OBUF_DIST*(nch)*sizeof(CriSint16))
#endif

#if defined(XPT_TGT_EE) || defined(XPT_TGT_GC) || defined(XPT_TGT_WII)
#define ADXT_PL2BUF_SIZE	(1024)
#else
#define ADXT_PL2BUF_SIZE	(0)
#endif

#define ADXT_3D_MAX_NCH_IBUF	(1)

#if defined(XPT_TGT_EE) || defined(XPT_TGT_GC) || defined(XPT_TGT_PC) || defined(XPT_TGT_WII)
#define ADXT_3D_MAX_NCH_OBUF	(2)
#else
#define ADXT_3D_MAX_NCH_OBUF	(1)
#endif

#define ADXT_CALC_WORK(nch, stmflg, nstm, sfreq)	\
	(ADXT_CALC_IBUFSIZE(nch, nstm, sfreq) * (stmflg) + ADXT_CALC_OBUFSIZE(nch) + 64)

#define ADXT_CALC_IBUFSIZE_3D(nstm, sfreq)	\
	(ADXT_CALC_IBUFSIZE(ADXT_3D_MAX_NCH_IBUF, nstm, sfreq))

#define ADXT_OBUFSIZE_3D	(ADXT_CALC_OBUFSIZE(ADXT_3D_MAX_NCH_OBUF))

#define ADXT_CALC_WORK_3D(stmflg, nstm, sfreq)	\
	(ADXT_CALC_IBUFSIZE_3D(nstm, sfreq) * (stmflg) + ADXT_OBUFSIZE_3D + ADXT_PL2BUF_SIZE + 64)

/*	データ容量の計算		*/
/*	Calc ADX Data size		*/
#define	ADXT_CalcDataLen(sec, nch, sfreq)	((sec)*(nch)*(sfreq)*18/32 + 256)

/* AHXワーク領域サイズ		*/
/* Work size of AHX 		*/
#if defined(XPT_TGT_EE) || defined(XPT_TGT_PSP) || defined(XPT_TGT_NITRO)
#define ADXT_WORKSIZE_AHX	(6144)		/* 6KB */
#else
#define ADXT_WORKSIZE_AHX	(8192)		/* 8KB */
#endif

/*	入力バッファへの読み込み量						*/
/*	Number of sectors to read into the input buffer	*/
#define ADXT_MAX_CDBSCT		(75)
#define ADXT_MIN_CDBSCT		(65)
#define ADXT_PREP_RDSCT		(25)

/*	$define$ ADX Talkの動作状態(ADXT_STAT_～)	*/
/*	Status of ADX Talk							*/
#define	ADXT_STAT_STOP		(0)		/*	停止中 								*/
									/*	During standstill					*/
#define ADXT_STAT_DECINFO	(1)		/*	ＡＤＸ のヘッダ情報取得中			*/
									/*	Getting header information			*/
#define ADXT_STAT_PREP		(2)		/*	再生準備中 							*/
									/*	During play preparation				*/
#define ADXT_STAT_PLAYING	(3)		/*	デコード＆再生中 					*/
									/*	During decode and play				*/
#define ADXT_STAT_DECEND	(4)		/*	デコード終了 						*/
									/*	Decode end							*/
#define ADXT_STAT_PLAYEND	(5)		/*	再生終了 							*/
									/*	Play end							*/
#define ADXT_STAT_ERROR		(6)		/*	読み込みエラー発生					*/
									/*	Read-in error outbreak state		*/

/*	$define$ ADX Talkのエラーコード　(ADXT_ERR_～)	*/
/*	Error code of ADX Talk							*/
#define	ADXT_ERR_OK			(0)		/*	正常 								*/
									/*	Normality							*/
#define ADXT_ERR_SHRTBUF	(-1)	/*	入力バッファエンプティ				*/
									/*	The input buffer is empty			*/
#define ADXT_ERR_SNDBLK		(-2)	/*	サウンドブロックエラー				*/
									/*	Error of sound block				*/

/*	$define$ フィルタモード	(ADXT_FLTMODE_～)		*/
/*	Filter mode										*/
#define	ADXT_FLTMODE_CPU	(0)		/*	ＣＰＵによる展開モード				*/
									/*	by CPU								*/
#define	ADXT_FLTMODE_SCSP	(1)		/*	AICA-DSPによる展開モード			*/
									/*	by AICA-DSP							*/

/*	$define$ 再生モード	(ADXT_PMODE_～)				*/
/*	Play mode										*/
#define	ADXT_PMODE_FNAME	(0)		/*	ファイル名または FID による指定		*/
									/*	Play specified file name or file ID	*/
#define	ADXT_PMODE_AFS		(1)		/*	AFS による指定						*/
									/*	Play specified AFS file				*/
#define	ADXT_PMODE_MEM		(2)		/*	メモリ再生							*/
									/*	Play data from memory				*/
#define	ADXT_PMODE_SJ		(3)		/*	ストリームジョイント再生			*/
									/*	Play data from StreamJoint			*/
#define	ADXT_PMODE_SLFILE	(4)		/*	シームレス連続再生（ファイル）		*/
									/*	Seamless continuous play from file	*/

/*	$define$ エラーリカバーモード	(ADXT_RMODE_～)	*/
/*	Mode of error recovery							*/
#define	ADXT_RMODE_NOACT	(0)		/*	リカバー処理しない					*/
									/*	Do not recover						*/
#define	ADXT_RMODE_STOP		(1)		/*	自動停止							*/
									/*	Automatic stop						*/
#define	ADXT_RMODE_REPLAY	(2)		/*	自動再プレイ						*/
									/*	Automatic replay					*/

/*	パンポットの設定値		*/
/*	Panpot parameter		*/
#define	ADXT_PAN_LEFT		(-15)
#define	ADXT_PAN_CENTER		(0)
#define	ADXT_PAN_RIGHT		(15)
#define	ADXT_PAN_AUTO		(-128)	/*	MONO/STE によって自動的に切り替える	*/
									/*	Changes automatically by data		*/

/*	バランスの設定値		*/
/*	Panpot parameter		*/
#define	ADXT_BALANCE_LEFT		(-15)
#define	ADXT_BALANCE_CENTER		(0)
#define	ADXT_BALANCE_RIGHT		(15)

/*	ステレオ再生時のチャンネル番号		*/
/*	Channel number playing stereo data	*/
/*	ADXT_CH_L:left, ADXT_CH_R:right		*/
#define	ADXT_CH_L			(0)
#define	ADXT_CH_R			(1)

/*	マルチチャンネル再生用スピーカーID				*/
/*	Speaker IDs for multi-channel audio playback	*/
#define ADXT_SPEAKER_FRONT_LEFT			(0)
#define ADXT_SPEAKER_FRONT_RIGHT		(1)
#define ADXT_SPEAKER_BACK_LEFT 			(2)
#define ADXT_SPEAKER_BACK_RIGHT			(3)
#define ADXT_SPEAKER_FRONT_CENTER		(4)
#define ADXT_SPEAKER_LOW_FREQUENCY		(5)

/*	ダウンミックスの方法	*/
/*	Method of downmix */
#define ADXT_DOWNMIX_NONE				(0)
#define ADXT_DOWNMIX_MONO				(1)
#define ADXT_DOWNMIX_STEREO				(2)

/*	最大再生チャンネル数				*/
/*	Maximum number of play channel		*/
#define	ADXT_MAX_NCH		(2)

/*	サーバ関数の呼び出される頻度の規定値　60（回/秒）				*/
/*	Default value of frequency called server function(times/sec)	*/
#define ADXT_DEF_SVRFREQ	(60)

/*	Default value of output volume(0.1dB)	*/
#define	ADXT_DEF_OUTVOL		(0)			/*	  0 dB	*/
#define	ADXT_MAX_OUTVOL		(0)			/*	  0 dB	*/
#define	ADXT_MIN_OUTVOL		(-960)		/*	-96 dB	*/

/*	入力バッファの下限値 (エラー処理用、単位バイト）				*/
/*	Minimum data in the input buffer(byte) (for coping with error)	*/
#define	ADXT_MIN_BUFDATA	(64)

/*	エラー判別時間　（単位:秒)			*/
/*	Error distinction time(sec)			*/
#define	ADXT_EWAIT_PLY		(5)
#define	ADXT_EWAIT_NOTPLY	(20)

/*	インデックスの最大数				*/
/*	Maximum number of index				*/
#define	ADXT_MAX_IDX		(0xFFFF)

/*	ADXフォーマットタイプ				*/
/*	Format type of ADX					*/
#define ADXT_FMT_ADX		(1)
#define ADXT_FMT_AHX		(2)

/*	ファイル名の最大長	*/
/*	Maximum length of the file path		*/
#define	ADXT_MAX_FNAME		(256)

/*	デコードエラーモード	*/
/*	Decode error mode		*/
#define	ADXT_DECERR_MODE_ERRCB	(0)		/*	エラーコールバック	*/
										/*	Error callback		*/
#define	ADXT_DECERR_MODE_STAT	(1)		/*	ハンドル状態		*/
										/*	Handle state		*/

/****************************************************************************/
/*		データ型															*/
/*      Data type declaration												*/
/****************************************************************************/

/*	ADX Talk オブジェクト構造体		*/
/*	Structure of ADX Talk object	*/
typedef struct _adx_talk {
	CriSint8	used;						/*	使用中か否か					*/
	CriSint8	stat;						/*	動作状態						*/
	CriSint8	pmode;						/*	再生モード						*/
	CriSint8	maxnch;						/*	最大再生チャンネル数			*/
	ADXSJD	sjd;						/*	ADXストリームジョイントデコーダ	*/
	ADXSTM	stm;						/*	入力ストリームコントローラ		*/
	ADXRNA	rna;						/*	オーディオレンダラ				*/
	SJ		sjf;						/*	ファイル入力ストリームジョイント*/
	SJ		sji;						/*	入力ストリームジョイント		*/
	SJ		sjo[ADXT_MAX_NCH];			/*	出力ストリームジョイント		*/
	CriSint8	*ibuf;						/*	入力バッファ					*/
	CriSint32	ibuflen;					/*	入力バッファサイズ（バイト単位)	*/
	CriSint32	ibufxlen;					/*	入力バッファエクストラサイズ	*/
	CriSint16	*obuf;						/*	出力バッファ					*/
	CriSint32	obufsize;					/*	出力バッファサイズ（サンプル）	*/
	CriSint32	obufdist;					/*	出力バッファ間隔（サンプル）	*/
	CriSint32	svrfreq;					/*	サーバ関数呼び出し頻度			*/
	CriSint16	maxsct;						/*	入力バッファ内の最高セクタ数	*/
	CriSint16	minsct;						/*	入力バッファ内の最低セクタ数	*/
	CriSint16	outvol;						/*	出力ボリューム					*/
	CriSint16	outpan[ADXT_MAX_NCH];		/*	出力パンポットの設定値			*/
	CriSint16	outbalance;					/*	出力バランスの設定値			*/
	CriSint32	maxdecsmpl;					/*	最大デコードサンプル数			*/
	CriSint32	lpcnt;						/*	ループカウンタ					*/
	CriSint32	lp_skiplen;					/*	ループスキップバイト数			*/
	CriSint32	trp;						/*	トランスポーズ量 (セント)		*/
	CriSint32	wpos;						/*	メディア上の書き込み位置		*/
	CriSint32	mofst;						/*	メディア上のオフセット			*/
	CriSint16	ercode;						/*	エラーコード					*/
	CriSint32	edecpos;					/*	エラー検出用デコード位置		*/
	CriSint16	edeccnt;					/*	デコード位置カウンタ			*/
	CriSint16	eshrtcnt;					/*	入力ﾊﾞｯﾌｧｴﾝﾌﾟﾃｨｰｶｳﾝﾀ			*/
	CriSint8	lpflg;						/*	ループするか否か				*/
	CriSint8	autorcvr;					/*	自動的にエラー復帰するか否か	*/
	CriSint8	fltmode;					/*	フィルタモード					*/
	CriSint8	execflag;					/*	サーバを実行するか否か			*/
	CriSint8	pstwait_flag;				/*	音声出力開始待ちフラグ			*/
	CriSint8	pstready_flag;				/*	音声出力準備完了フラグ			*/
	CriSint8	pause_flag;					/*	一時停止フラグ					*/
	CriSint8	auto_pause_flag;			/*	自動一時停止フラグ（Wii用）		*/
	void	*amp;						/*	振幅抽出器						*/
	SJ		ampsji[ADXT_MAX_NCH];		/*	振幅抽出用入力SJ				*/
	SJ		ampsjo[ADXT_MAX_NCH];		/*	振幅抽出用出力SJ				*/
	CriSint32	time_ofst;					/*	時刻のオフセット				*/
	CriSint32	lesct;						/*	ループ再生時の最終セクタ番号	*/
	CriSint32	trpnsmpl;					/*			トラップサンプル番号	*/
	void	*lsc;						/*	連結ストリームコントローラ		*/
	CriSint8	lnkflg;						/*	連結再生フラグ					*/
	CriSint8	rsv;						/*	予約							*/
	CriSint16	rsv2;						/*	予約２							*/
	CriUint32 tvofst;						/*	スタート時間オフセット			*/
	CriUint32 svcnt;						/*	スタートV-Syncカウント			*/
	CriUint32 decofst;						/*	トータルデコードオフセット		*/
	#ifdef __EE__
	CriSint32	flush_nsmpl;				/*	フラッシュ時に挿入サンプル数	*/
	#endif
	CriSint8 stm_start_flg;				/*	ストリーム開始フラグ			*/
	CriSint8 ainf_sw;						/*	付加情報スイッチ(1=有効 0=無効)	*/
	CriSint16 rsv4;						/*	予約							*/
	CriChar8 *fname_wk;					/*	ファイル名領域　(work内)		*/
	CriChar8 *fname;						/*	ファイル名						*/
	void *dir;							/*	ディレクトリハンドル			*/
	CriUint32 ofst;						/*	オフセット（セクタ単位)			*/
	CriUint32 nsct;						/*	範囲 （セクタ単位)				*/
	//	2005.2.14 added by M.Oshimi
	CriUint32 lpoff_len;					/*	ループオフが有効になるデコード量*/
} ADX_TALK;
typedef	ADX_TALK	*ADXT;

/*	インデックスデータ	*/
/*	Index data			*/
typedef struct _adxt_idx {
	CriUint16 nidx;
	CriUint16 top;
} ADXT_IDX;

/*	スレッドパラメータ構造体		*/
/*	Parameter structure of Thread Param	*/
typedef struct {
	int	prio_lock;		/* Lock Thread priority				*/
	int	prio_safe;		/* Safe Thread priority				*/
	int	prio_vsync;		/* Vsync Thread priority			*/
	int	prio_fs;		/* Filesystem Thread priority		*/
	int	prio_main;		/* Main Thread Normary priority		*/
	int	prio_mwidle;	/* Middleware Idle Thread priority	*/
} ADXM_TPRM;

/*	スレッドパラメータ構造体(拡張版)				*/
/*	Parameter structure of Extended Thread Param	*/
typedef struct {
	int	prio_lock;		/* Lock Thread priority				*/
	int	prio_safe;		/* Safe Thread priority				*/
	int	prio_usrvsync;	/* User Vsync Thread priority		*/
	int	prio_vsync;		/* Vsync Thread priority			*/
	int	prio_fs;		/* Filesystem Thread priority		*/
	int	prio_main;		/* Main Thread Normary priority		*/
	int	prio_mwidle;	/* Middleware Idle Thread priority	*/
	int	prio_usridle;	/* User Idle Thread priority		*/
	
	void  *stkptr_usrvsync;	/* User Vsync Stack buffer address(In NULL, stack size is 4KB)	*/
	CriSint32 stksize_usrvsync;/* User Vsync Stack size										*/
	void  *stkptr_usridle;	/* User Idle Stack buffer address(In NULL, stack size is 8KB)	*/
	CriSint32 stksize_usridle;	/* User Idle Stack size											*/
} ADXM_TPRM_EX;

/*	ADXヘッダ情報構造体					*/
/*	Parameter structure of ADX header	*/
typedef struct {
	CriSint32 fmt;			/* Format type (ADX/AHX)			*/
	CriSint32 sfreq;		/* Sampling rate					*/
	CriSint32 nch;			/* Number of channel				*/
	CriSint32 bps;			/* Number of bit per sample			*/
	CriSint32 nsmpl;		/* Total number of sample			*/
	CriSint32 lptype;		/* Loop type						*/
	CriSint32 lpstart;		/* Loop start position				*/
	CriSint32 lpend;		/* Loop end position				*/
} ADXHINFO;


/* Callback funtion */
typedef void (*ADXM_CBFN)(void *obj);

/****************************************************************************/
/*		関数の宣言															*/
/*      Function Declaration												*/
/****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* $func$ ライブラリバージョンの取得
 * [書　式] CriChar8 *ADXT_GetVersion(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] バージョン文字列
 * [機　能] ADXライブラリのバージョン文字列を取得する。
 *  Gets the version of library
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] Version string
 * [Function] Gets the version string of ADX library.
 */
CriChar8 * CRIAPI ADXT_GetVersion(void);

/* $func$ ADX Talk の初期化
 * [書　式] void ADXT_Init(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ADX Talk を初期化する。
 *			変数領域の初期化を行う。
 *  Initialization of ADX Talk
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Initializes ADX Talk. Initializes variable.
 */
void CRIAPI ADXT_Init(void);

/* $func$ ADX Talkの終了
 * [書　式] void ADXT_Finish(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ADX Talk の終了処理をする。
 *  Termination of ADX Talk
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Finalizes ADX Talk.
 */
void CRIAPI ADXT_Finish(void);

/*	$func$ すべてのADXTハンドルの消去
 * [書　式] void ADXT_DestroyAll(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] すべてのADXTハンドルを消去する。
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] You can destroy all ADXT handles.
 */
void CRIAPI ADXT_DestroyAll(void);

/*	$func$ すべてのADXTとADXFハンドルの消去
 * [書　式] void ADXT_CloseAllHandles(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] すべてのADXTハンドルとADXFハンドルを消去する。
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] You can destroy all ADXT handles and close all ADXF handles.
 */
void CRIAPI ADXT_CloseAllHandles(void);

/*	$func$ ADXTハンドル の生成
 * [書　式] ADXT ADXT_Create(CriSint32 maxnch, void *work, CriSint32 worksize);
 * [入　力] maxnch	: 最大再生チャンネル数（モノラルのみ１：ステレオ２）
 *			work	: ワーク領域
 *			worksize: ワーク領域のサイズ
 * [出　力] なし
 * [関数値] ADXTハンドル
 * [機　能] ADXTハンドルを生成する。
 *			work領域のサイズは、ADXT_CALC_WORKマクロによって求める。
 *  Creation of ADXT handle
 * [Inputs  ] maxnch  : Number of maximum channels(monoral:1, stereo:2)
 *			  work    : Working area
 *			  worksize: Size of working area(byte)
 * [Outputs ] None
 * [Return  ] ADXT handle
 * [Function] Creates ADXT handle.
 * [Remarks ] You calculate size of working area used 'ADXT_CALC_WORK' macro. 
 */
ADXT CRIAPI ADXT_Create(CriSint32 maxnch, void *work, CriSint32 worksize);

/*	$func$ ADXT 3Dハンドルの生成
 * [書　式] ADXT_Create3D(void *work, CriSint32 worksize);
 * [入　力] work	: ワーク領域
 *			worksize: ワーク領域のサイズ
 * [出　力] なし
 * [関数値] ADXTハンドル
 * [機　能] ADXTハンドルを生成する。
 *			work領域のサイズは、ADXT_CALC_WORK_3Dマクロによって求める。
 *  Creation of ADXT 3D handle
 * [Inputs  ] work    : Working area
 *			  worksize: Size of working area(byte)
 * [Outputs ] None
 * [Return  ] ADXT handle
 * [Function] Creates ADXT handle.
 * [Remarks ] You calculate size of working area used 'ADXT_CALC_WORK_3D' macro. 
 */
ADXT CRIAPI ADXT_Create3D(void *work, CriSint32 worksize);

/* $func$ ADXTハンドル の消去
 * [書　式] void ADXT_Destroy(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] 指定された ADXTハンドルを消去する。
 *  Destroy of ADXT handle
 * [Inputs  ] adxt   : ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Destroys specified ADXT handle.
 */
void CRIAPI ADXT_Destroy(ADXT adxt);

/* $func$ FID 指定による再生の開始
 * [書　式] void ADXT_StartAfs(ADXT adxt, CriSint32 patid, CriSint32 fid);
 * [入　力] adxt	: ADXTハンドル
 *			patid	: パーティション識別子
 *			fid		: ファイル識別子
 * [出　力] なし
 * [関数値] なし
 * [機　能] パーティション識別子とファイル識別子で指定されたＡＤＸファイルの
 *			再生を開始する。
 *  Play start of ADX data by specified file ID
 * [Inputs  ] adxt	: ADXT handl
 *			  patid	: Partition ID
 *			  fid	: File ID
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play ADX file when you specify partition ID and file ID.
 */
void CRIAPI ADXT_StartAfs(ADXT adxt, CriSint32 patid, CriSint32 fid);

/* $func$ ファイル名指定による再生の開始
 * [書　式] void ADXT_StartFname(ADXT adxt, const CriChar8 *fname);
 * [入　力] adxt	: ADXTハンドル
 *			fname	: ファイル名
 * [出　力] なし
 * [関数値] なし
 * [機　能] fname で指定されたＡＤＸファイルの再生を開始する。
 *  Play start of ADX data specified file name
 * [Inputs  ] adxt	: ADXT handle
 *			  fname	: File name
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play ADX file when you specify file name.
 */
void CRIAPI ADXT_StartFname(ADXT adxt, const CriChar8 *fname);

/* $func$ ファイル名指定による再生の開始
 * [書　式] void ADXT_StartFnameRange(ADXT adxt, CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);
 * [入　力] adxt	: ADXTハンドル
 *			fname	: ファイル名
 *			offset_sct	: ファイル内のオフセット（セクタ単位）
 *			range_sct	: 範囲（セクタ単位）
 * [出　力] なし
 * [関数値] なし
 * [機　能] fname で指定されたＡＤＸファイルの再生を開始する。
 *  Play start of ADX data specified file name
 * [Inputs  ] adxt	: ADXT handle
 *			  fname	: File name
 *			  offset_sct	: File offset from top of the file as a sector
 *			  range_sct		: File range as a sector
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play ADX file when you specify file name and the range as a sector.
 */
void CRIAPI ADXT_StartFnameRange(ADXT adxt, const CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);

/* $func$ ストリームジョイントによる再生の開始
 * [書　式] void ADXT_StartSj(ADXT adxt, SJ sj);
 * [入　力] adxt	: ADXTハンドル
 *			sj		: ストリームジョイント
 * [出　力] なし
 * [関数値] なし
 * [機　能] ストリームジョイントから得られるデータを再生する。
 *  Play start of ADX data from Stream Joint
 * [Inputs  ] adxt	: ADXT handle
 *			  sj	: Stream Joint
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play ADX file when you specify Stream Joint.
 */
void CRIAPI ADXT_StartSj(ADXT adxt, SJ sj);

/* $func$ メモリ指定による再生の開始
 * [書　式] void ADXT_StartMem(ADXT adxt, void *adxdat);
 * [入　力] adxt	: ADXTハンドル
 *			adxdat	: ＡＤＸデータのアドレス
 * [出　力] なし
 * [関数値] なし
 * [機　能] adxdatで指定されたＡＤＸデータを再生する。
 *  Play start of ADX data on memory
 * [Inputs  ] adxt	: ADXT handle
 *			  adxdat: Address of ADX data
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play ADX data on memory when you specify address.
 */
void CRIAPI ADXT_StartMem(ADXT adxt, void *adxdat);

/* $func$ メモリ指定による再生の開始
 * [書　式] void ADXT_StartMem2(ADXT adxt, void *adxdat, CriSint32 datlen);
 * [入　力] adxt	: ADXTハンドル
 *			adxdat	: ＡＤＸデータのアドレス
 *			datlen	: ＡＤＸデータの大きさ
 * [出　力] なし
 * [関数値] なし
 * [機　能] adxdata で指定されたＡＤＸデータを再生する。
 *  Play start of ADX data on memory (TYPE 2)
 * [Inputs  ] adxt	: ADXT handle
 *			  adxdat: Address of ADX data
 *			  datlen: Length of playing ADX data
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play ADX data on memory. Plays to specified length.
 */
void CRIAPI ADXT_StartMem2(ADXT adxt, void *adxdat, CriSint32 datlen);

/* $func$ メモリ指定による再生の開始
 * [書　式] void ADXT_StartMemIdx(ADXT adxt, void *acx, CriSint32 no);
 * [入　力] adxt	: ADXTハンドル
 *			acx		: ＡＣＸデータのアドレス
 *			no		: インデックス番号
 * [出　力] なし
 * [関数値] なし
 * [機　能] acx で指定されたＡＣＸデータのno番目のデータを再生する。
 *  Play start of ACX data
 * [Inputs  ] adxt	: ADXT handle
 *			  acx	: Address of ACX data
 *			  no	: Index number
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play a part of ACX data when you specify index number.
 */
void CRIAPI ADXT_StartMemIdx(ADXT adxt, void *acx, CriSint32 no);

/* $func$ 再生の停止
 * [書　式] void ADXT_Stop(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] ＡＤＸの再生を停止する。
 *  Play stop
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops to play ADX data.
 */
void CRIAPI ADXT_Stop(ADXT adxt);

/* $func$ 状態の取得
 * [書　式] CriSint32 ADXT_GetStat( ADXT adxt );
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 現在の ADXTハンドルの状態を表す定数
 * 				ADXT_STAT_STOP	 :	停止中
 *				ADXT_STAT_DECINFO:	ＡＤＸ のヘッダ情報取得中
 *				ADXT_STAT_PREP	 :	再生準備中
 *				ADXT_STAT_PLAYING:	デコード＆再生中
 *				ADXT_STAT_DECEND :	デコード終了
 *				ADXT_STAT_PLAYEND:	再生終了
 * [機　能] 現在のADXTハンドルの状態を取得する。
 *  Get status
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Status of ADXT handle
 * [Function] Obtains status of ADXT handle.
 */
CriSint32 CRIAPI ADXT_GetStat(ADXT adxt);

/* $func$ サンプル単位での再生時刻の取得
 * [書　式] void ADXT_GetTime( ADXT adxt, CriSint32 *ncount, CriSint32 *tscale );
 * [入　力] adxt	: ADXTハンドル
 * [出　力] ncount	: 再生サンプル数
 *			tscale	: サンプリング周波数 [ Hz ]
 * [関数値] なし
 * [機　能] サンプル単位で再生時刻を取得する。
 *  Get play time by sample unit
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] ncount: Number of play sample
 *			  tscale: Sampling frequency(Hz)
 * [Return  ] None
 * [Function] Obtains play time by sample unit.
 */
void CRIAPI ADXT_GetTime(ADXT adxt, CriSint32 *ncount, CriSint32 *tscale);

/* $func$ 実時間での再生時刻の取得
 * [書　式] CriSint32 ADXT_GetTimeReal(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 再生時刻 [ 1/100sec ]
 * [機　能] 実時間で再生時刻を取得する。
 *  Get play time in real time
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Play time(1/100sec)
 * [Function] Obtains play time in real time.
 */
CriSint32 CRIAPI ADXT_GetTimeReal(ADXT adxt);

/* $func$ 音声データの総サンプル数の取得
 * [書　式] CriSint32 ADXT_GetNumSmpl( ADXT adxt );
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 音声データの総サンプル数
 * [機　能] 再生中の ＡＤＸ データの総サンプル数を取得する。
 * [備　考] 状態が、再生準備中(ADXT_STAT_PREP)から再生終了(ADXT_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get a general sample number of sound data
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] General sample number of sound data
 * [Function] Obtains a general sample number of sound data.
 * [Remarks ] You can obtain a time between status of preparation
 *			  (ADXT_STAT_PREP) and status of play end(ADXT_STAT_PLAYEND).
 */
CriSint32 CRIAPI ADXT_GetNumSmpl(ADXT adxt);

/* $func$ 音声データのサンプリング周波数の取得
 * [書　式] CriSint32 ADXT_GetSfreq(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 音声データのサンプリング周波数
 * [機　能] 再生中の サンプリング周波数を求める。
 * [備　考] 状態が、再生準備中(ADXT_STAT_PREP)から再生終了(ADXT_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get sampling frequecy
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Sampling frequency
 * [Function] Obtains sampling frequency.
 * [Remarks ] You can obtain a sampling frequency between status of preparation
 *			 (ADXT_STAT_PREP) and status of play end(ADXT_STAT_PLAYEND).
 */
CriSint32 CRIAPI ADXT_GetSfreq(ADXT adxt);

/* $func$ 音声データのチャンネル数の取得
 * [書　式] CriSint32 ADXT_GetNumChan(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 音声データのチャンネル数の取得
 * [機　能] 再生中の音声のチャンネル数を求める。
 * [備　考] 状態が、再生準備中(ADXT_STAT_PREP)から再生終了(ADXT_STAT_PLAYEND)
 *			までの時に取得可能。
 */
CriSint32 CRIAPI ADXT_GetNumChan(ADXT adxt);

/* $func$ ヘッダ長の取得
 * [書　式] CriSint32 CriSint32 ADXT_GetHdrLen(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 音声データのヘッダ長
 * [機　能] 再生中の音声のヘッダ長を求める。
 * [備　考] 状態が、再生準備中(ADXT_STAT_PREP)から再生終了(ADXT_STAT_PLAYEND)
 *			までの時に取得可能。
 */
CriSint32 CRIAPI ADXT_GetHdrLen(ADXT adxt);

/* $func$ 音声データの量子化の取得
 * [書　式] CriSint32 ADXT_GetFmtBps(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 音声データの量子化ビット数
 * [機　能] 再生中の音声の量子化ビット数を求める。
 * [備　考] 状態が、再生準備中(ADXT_STAT_PREP)から再生終了(ADXT_STAT_PLAYEND)
 *			までの時に取得可能。
 */
CriSint32 CRIAPI ADXT_GetFmtBps(ADXT adxt);

/*	$func$ パンポットの設定
 * [書　式] void ADXT_SetOutPan(ADXT adxt, CriSint32 ch, CriSint32 pan);
 * [入　力] adxt	: ADXTハンドル
 *			ch		: チャネル番号 (0, 1)
 *						ADXT_CH_L=0, ADXT_CH_R=1
 *			pan		: パン設定値 (-15～+15, -128)
 *						ADXT_PAN_LEFT=-15, ADXT_PAN_CENTER=0
 *						ADXT_PAN_RIGHT=15, ADXT_PAN_AUTO=-128
 * [出　力] なし
 * [関数値] なし
 * [機　能] 出力パンポットを設定する。
 *			AUTO の場合は、ADX データがモノラルかステレオかによって自動的に
 *			パンが設定される。
 *  Set panpot
 * [Inputs  ] adxt	: ADXT handle
 *			  ch	: Channel number(ADXT_CH_L:0, ADXT_CH_R:1)
 *			  pan	: Panpot(from -15 to +15, -128)
 *						ADXT_PAN_LEFT=-15, ADXT_PAN_CENTER=0
 *						ADXT_PAN_RIGHT=15, ADXT_PAN_AUTO=-128
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets panpot of specified channel number.
 *			 Sets automatically panpot by data, monoral or stereo, when you 
 *			 specified 'ADXT_PAN_AUTO'.
 */
void CRIAPI ADXT_SetOutPan(ADXT adxt, CriSint32 ch, CriSint32 pan);

/*	$func$ パンポットの取得
 * [書　式] CriSint32 ADXT_GetOutPan(ADXT adxt, CriSint32 ch);
 * [入　力] adxt	: ADXTハンドル
 *			ch		: チャネル番号 (0, 1)
 *						ADXT_CH_L=0, ADXT_CH_R=1
 * [出　力] なし
 * [関数値] パンポットの設定値
 * [機　能] 出力パンポットを取得する。
 *  Get panpot
 * [Inputs  ] adxt	: ADXT handle
 *			  ch	: Channel number(ADXT_CH_L:0, ADXT_CH_R:1)
 * [Outputs ] None
 * [Return  ] Panpot
 * [Function] Obtains panpot of specified channel number.
 */
CriSint32 CRIAPI ADXT_GetOutPan(ADXT adxt, CriSint32 ch);

/*	$func$ 音源位置の設定
 * [書　式] void ADXT_SetOutPos(ADXT adxt, CriSint32 x, CriSint32 y);
 * [入　力] adxt	: ADXTハンドル
 *			x, y	: 音源位置 (-127～+127)
 * [出　力] なし
 * [関数値] なし
 * [機　能] 音源の位置を設定する。
 *  Set position
 * [Inputs  ] adxt	: ADXT handle
 *			  x, y	: Position (from -127 to +127)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets position.
 */
void CRIAPI ADXT_SetOutPos(ADXT adxt, CriSint32 x, CriSint32 y);

/*	$func$ バランスの設定
 * [書　式] void ADXT_SetOutBalance(ADXT adxt, CriSint32 balance);
 * [入　力] adxt	: ADXTハンドル
 *			balance	: バランス設定値 (-15～+15)
 *						ADXT_BALANCE_LEFT=-15, ADXT_BALANCE_CENTER=0
 *						ADXT_BALANCE_RIGHT=15
 * [出　力] なし
 * [関数値] なし
 * [機　能] 出力バランスを設定する。
 *  Set output balance
 * [Inputs  ] adxt		: ADXT handle
 *			  balance	: balance(from -15 to +15)
 *						ADXT_BALANCE_LEFT=-15, ADXT_BALANCE_CENTER=0
 *						ADXT_BALANCE_RIGHT=15
 * [Outputs ] None
 * [Return  ] None
 * [Function] Set output balance.
 */
void CRIAPI ADXT_SetOutBalance(ADXT adxt, CriSint32 balance);

/*	$func$ バランスの取得
 * [書　式] CriSint32 ADXT_GetOutBalance(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] バランスの設定値
 * [機　能] 出力バランスを取得する。
 *  Get output balance
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Output balance
 * [Function] Obtains output balance.
 */
CriSint32 CRIAPI ADXT_GetOutBalance(ADXT adxt);

/*	$func$ 出力ボリュームの設定
 * [書　式] void ADXT_SetOutVol(ADXT adxt, CriSint32 vol);
 * [入　力] adxt	: ADXTハンドル
 *			vol		: 減衰レベル　(0:-0dB, -960:-96.0dB)
 * [出　力] なし
 * [関数値] なし
 * [機　能] 出力ボリュームを設定する。
 *			volの設定値	0    :    -0dB  減衰なし
 *						-30  :    -3dB  約70%
 *						-60  :    -6dB  約50%
 *						-960 : -96.0dB  最大の減衰量
 *  Set volume
 * [Inputs  ] adxt	: ADXT handle
 *			  vol	: Volume (0:-dB, -960:-96.0dB)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the volume of a ADXT handle.
 *			 'vol'value	0    :    -0dB  No attenuation
 *						-30  :    -3dB  about 70%
 *						-60  :    -6dB  about 50%
 *						-960 : -96.0dB  Maximum attenuation
 */
void CRIAPI ADXT_SetOutVol(ADXT adxt, CriSint32 vol);

/*	$func$ 出力ボリュームの取得
 * [書　式] CriSint32 ADXT_GetOutVol(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 出力ボリュームの設定値　(0:-0dB ～ -960:-96.0dB)
 * [機　能] 出力ボリュームを取得する。
 *  Get volume
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Volume (0:-0dB, -960:-96.0dB)
 * [Function] Obtains the volume for a ADXT handle.
 */
CriSint32 CRIAPI ADXT_GetOutVol(ADXT adxt);

/*	$func$ デフォルト出力ボリュームの取得
 * [書　式] CriSint32 ADXT_GetDefOutVol(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] デフォルト出力ボリューム(0:-0dB ～ -960:-96.0dB)
 * [機　能] ADXデータ内に含まれるデフォルト出力ボリュームを取得する。
 */
CriSint32 CRIAPI ADXT_GetDefOutVol(ADXT adxt);

/*	$func$ デフォルトパンポットの取得
 * [書　式] CriSint32 ADXT_GetDefOutPan(ADXT adxt CriSint32 chno);
 * [入　力] adxt	: ADXTハンドル
 *          chno    : チャンネル番号
 * [出　力] なし
 * [関数値] デフォルトパンポットの設定値
 * [機　能] ADXデータ内に含まれるデフォルトパンポットを取得する。
 */
CriSint32 CRIAPI ADXT_GetDefOutPan(ADXT adxt, CriSint32 chno);

/*	$func$ データ識別文字列の取得
 * [書　式] CriChar8 *ADXT_GetDataId(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] データ識別文字列
 * [機　能] ADXデータ内に含まれるデータ識別文字列を取得する。
 */
CriChar8 * CRIAPI ADXT_GetDataId(ADXT adxt);


/*	$func$ データ識別文字列の取得(ADXデータ)
 * [書　式] CriChar8 *ADXT_GetDataIdFromMem(CriChar8 *addr);
 * [入　力] addr	: ADXデータのアドレス
 * [出　力] なし
 * [関数値] データ識別文字列
 * [機　能] ADXデータ内に含まれるデータ識別文字列を取得する。
 */
CriChar8 * CRIAPI ADXT_GetDataIdFromMem(CriChar8 *addr);

/*	$func$ 付加情報を有効にするか否かの設定
 * [書　式] void ADXT_SetAinfSw(ADXT adxt, CriSint32 sw)
 * [入　力] adxt	: ADXTハンドル
 *          sw		: スイッチ（1=有効、0=無効）
 * [出　力] なし
 * [関数値] なし
 * [機　能] 付加情報に指定されているボリューム／パンポットで、再生するか否かを設定する。
 */
void CRIAPI ADXT_SetAinfSw(ADXT adxt, CriSint32 sw);

/*	$func$ 付加情報が有効か否かの取得
 * [書　式] CriSint32 ADXT_GetAinfSw(ADXT adxt)
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] スイッチ（1=有効、0=無効）
 * [機　能] 付加情報のボリューム／パンポットで、再生されているか否かを取得する。
 */
CriSint32 CRIAPI ADXT_GetAinfSw(ADXT adxt);

/*	$func$ サーバ関数の呼び出し頻度のデフォルト値の設定	(１秒当たりの呼び出し回数)
 * [書　式] void ADXT_SetDefSvrFreq(CriSint32 freq);
 * [入　力] freq	: サーバ関数の呼び出し頻度のデフォルト値 (１秒当たりの呼び出し回数)
 * [出　力] なし
 * [関数値] なし
 * [機　能] サーバ関数(ADXT_ExecServer関数)の呼び出し頻度のデフォルト値の設定する。
 *			通常、設定する必要はない。
 */
void CRIAPI ADXT_SetDefSvrFreq(CriSint32 freq);

/*	$func$ サーバ関数の呼び出し頻度の設定	(１秒当たりの呼び出し回数)
 * [書　式] void ADXT_SetSvrFreq(ADXT adxt, CriSint32 freq);
 * [入　力] adxt	: ADXTハンドル
 *			freq	: サーバ関数の呼び出し頻度 (１秒当たりの呼び出し回数)
 * [出　力] なし
 * [関数値] なし
 * [機　能] サーバ関数(ADXT_ExecServer関数)の呼び出し頻度の設定する。
 *			デフォルトでは、60が設定されている。
 *			通常、設定する必要はない。
 *  Set the frequency called server function(times/sec)
 * [Inputs  ] adxt	: ADXT handle
 *			  freq	: Frequency called server function(times/sec)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the frequency called server function(ADXT_ExecServer).
 *			 You do not need to set it usually. Default value is 60.
 */
void CRIAPI ADXT_SetSvrFreq(ADXT adxt, CriSint32 freq);

/*	$func$ 再読み込み開始時間の設定
 * [書　式] void ADXT_SetReloadTime(ADXT adxt,float time,CriSint32 nch,CriSint32 sfreq);
 * [入　力] adxt	: ADXTハンドル
 *			time	: 再読み込み開始時間
 *			nch		: チャンネル数
 *			sfreq	: サンプリング周波数
 * [出　力] なし
 * [関数値] なし
 * [機　能] 入力バッファへの再読み込み開始時間を設定する。
 *			入力バッファ内のデータ量が time 秒より少なくなるとディスクから
 *			データを読み込む。
 *  Set the time of start sector to reload
 * [Inputs  ] adxt	: ADXT handle
 *			  time  : start remain time
 *			  nch   : number of channel
 *			  sfreq : sampling frequency
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets of the time of start sector to reload into input buffer.
 *			 Reads data from disc when a quantity of data in the input 
 *			 buffer becomes less than 'time' [second].
 */
void CRIAPI ADXT_SetReloadTime(ADXT adxt, float time, CriSint32 nch, CriSint32 sfreq);

void CRIAPI ADXT_ResetReloadTime(ADXT adxt);

/*	$func$ 再読み込み開始セクタ数の設定
 * [書　式] void ADXT_SetReloadSct(ADXT adxt, CriSint32 minsct);
 * [入　力] adxt	: ADXTハンドル
 *			minsct	: 再読み込み開始セクタ数
 * [出　力] なし
 * [関数値] なし
 * [機　能] 入力バッファへの再読み込み開始セクタ数を設定する。
 *			入力バッファ内のデータ量が minsct セクタより少なくなるとディスク
 *			からデータを読み込む。
 *  Set the number of start sector to reload
 * [Inputs  ] adxt	: ADXT handle
 *			  minsct: start sector number
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets of the number of start sector to reload into input buffer.
 *			 Reads data from disc when a quantity of data in the input 
 *			 buffer becomes less than 'minsct' [sector].
 */
void CRIAPI ADXT_SetReloadSct(ADXT adxt, CriSint32 minsct);

/*	$func$ 入力バッファ内のセクタ数の取得
 * [書　式] CriSint32 ADXT_GetNumSctIbuf(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] セクタ数
 * [機　能] 入力バッファに読み込まれているセクタ数を取得する。
 *  Get the amount of data stored in input buffer(sector)
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Number of sector
 * [Function] Obtains the amount of data that is stored in input buffer.
 */
CriSint32 CRIAPI ADXT_GetNumSctIbuf(ADXT adxt);

/*	$func$ 出力バッファ内のサンプル数の取得
 * [書　式] CriSint32 ADXT_GetNumSmplObuf(ADXT adxt, CriSint32 chno);
 * [入　力] adxt	: ADXTハンドル
 *			chno	: チャンネル番号
 * [出　力] なし
 * [関数値] サンプル数
 * [機　能] 出力バッファ内のサンプル数を取得する。
 */
CriSint32 CRIAPI ADXT_GetNumSmplObuf(ADXT adxt, CriSint32 chno);

/*	$func$ 入力バッファ内再生時間
 * [書　式] float ADXT_GetIbufRemainTime(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 再生可能時間　(単位：秒)
 * [機　能] 入力バッファにあるデータで再生可能な時間を取得する。
 *			ADXデータのみ対応。
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Playable time (sec)
 * [Function] Obtains playable time using only data in input buffer.
 *			  You can use this function only playing ADX data.
 */
float CRIAPI ADXT_GetIbufRemainTime(ADXT adxt);

/*	$func$ 入力バッファに十分なデータがあるか否かの検査
 * [書　式] CriSint32 ADXT_IsIbufSafety(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] TRUE(1):十分なデータがある、FALSE(0):データ不足
 * [機　能] 入力バッファに十分なデータがあるか否かを検査する。
 *			ADXT_SetReloadSct関数によって指定された再読み込み開始セクタ数以上の
 *			データが入力バッファ内に存在する場合、TRUEが返される。
 *  Check whether the input buffer is having enough data or not
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] TRUE(1):Has enough data, FALSE(0):Data are insufficient
 * [Function] Checks whether the input buffer is having enough data or not.
 *			  Returns 'TRUE' when data more than the number of sector appointed
 *			  exist in the input buffer.
 */
CriSint32 CRIAPI ADXT_IsIbufSafety(ADXT adxt);

/*	$func$ エラーリカバリーモードの指定
 * [書　式] void ADXT_SetAutoRcvr(ADXT adxt, CriSint32 rmode);
 * [入　力] adxt	: ADXTハンドル
 *			rmode	: エラーリカバリーの方法
 *					  ADXT_RMODE_NOACT, ADXT_RMODE_STOP, ADXT_RMODE_REPLAY
 * [出　力] なし
 * [関数値] なし
 * [機　能] エラーリカバリーの方法を設定する。
 *			ADXT_RMODE_NOACT : エラーリカバリーしない
 *			ADXT_RMODE_STOP  : 自動的に停止し、動作状態がADXT_STAT_STOPになる。
 *			ADXT_RMODE_REPLAY: CDからのデータの供給が途切れたときに、自動的に
 *							   ファイルの先頭から再生する。
 *							   その他の場合は、自動的に停止しする。
 *  Set a mode of error recovery
 * [Inputs  ] adxt	: ADXT handle
 *			  rmode	: Method of error recovery
 *					  ADXT_RMODE_NOACT, ADXT_RMODE_STOP, ADXT_RMODE_REPLAY
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the method that recover from error.
 *			 ADXT_RMODE_NOACT : Does not recover
 *			 ADXT_RMODE_STOP  : Stops automatically
 *			 ADXT_RMODE_REPLAY: Replay from the top of a file automatically 
 *							    when data supply from CD-ROM broke off.
 *							    Other cases stop automatically.
 */
void CRIAPI ADXT_SetAutoRcvr(ADXT adxt, CriSint32 rmode);

/*	$func$ 再生が終了したか否かの検査
 * [書　式] CriSint32 ADXT_IsCompleted(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] TRUE(1):再生終了、FALSE(0):再生中
 * [機　能] 再生が終了したか否かを調べる。
 *  Check whether the play finished or not
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] TRUE(1):Finished, FALSE(0):Playing
 * [Function] Checks whether the play finished or not.
 */
CriSint32 CRIAPI ADXT_IsCompleted(ADXT adxt);

/*	$func$ 各ハンドルのサーバ関数　（内部状態の更新)
 * [書　式] void ADXT_ExecHndl(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] 各ハンドルの内部状態を更新する。
 *			ADXT_ExecServer内から呼び出される。
 *  Server function of each handle
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status.
 *			  This function is called from 'ADXT_ExecServer' function.
 */
void CRIAPI ADXT_ExecHndl(ADXT adxt);

/*	$func$ サーバ関数　（内部状態の更新)
 * [書　式] void ADXT_ExecServer(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ライブラリの内部状態を更新する。
 *			V-Sync 毎に呼び出さなければならない。
 *  Server function
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status of library.
 */
void CRIAPI ADXT_ExecServer(void);
void CRIAPI ADXT_ExecDecServer(void);

/*	$func$ ファイルシステムサーバ関数　（内部状態の更新)
 * [書　式] void ADXT_ExecFsSvr(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ライブラリの内部状態を更新する。
 *			V-Sync 毎に呼び出さなければならない。
 *			優先度をADXT_ExecServerよりも低くする。
 *  File System Server function
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status of library.
 */
void CRIAPI ADXT_ExecFsSvr(void);
void CRIAPI ADXT_ExecFsServer(void);

/*	$func$ 連結ストリームコントローラサーバ関数　（内部状態の更新)
 * [書　式] void ADXT_ExecLscSvr(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ライブラリの内部状態を更新する。
 *			V-Sync 毎に呼び出さなければならない。
 *			優先度をADXT_ExecServerよりも低くする。
 *  Linking Stream Controller Server function
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status of library.
 */
void CRIAPI ADXT_ExecLscSvr(void);

/*	$func$ ファイルシステムサーバが処理中か否か
 * [書　式] CriSint32 ADXT_IsActiveFsSvr(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] TRUE:処理中／FALSE:処理していない
 * [機　能] ファイルシステムサーバが処理中か否かをチェックする。
 *  Check active File System Server function
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] TRUE:active/FALSE:no active
 * [Function] Checks that File System server function is active.
 */
CriSint32 CRIAPI ADXT_IsActiveFsSvr(void);

/*	$func$ エラーコードの取得
 * [書　式] CriSint32 ADXT_GetErrCode(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] エラーコードを取得する。
 *  Get error code
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Obtains the error code.
 */
CriSint32 CRIAPI ADXT_GetErrCode(ADXT adxt);

/*	$func$ エラーコードのクリア
 * [書　式] void ADXT_ClearErrCode(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] エラーコードをクリアする。
 *  Clear error code
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Clears the error code.
 */
void CRIAPI ADXT_ClearErrCode(ADXT adxt);

/*	$func$ ループポイント情報の取得
 * [書　式] void ADXT_GetLpInfo(ADXT adxt, CriSint32 *lpst, CriSint32 *lped);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] lpst	: ループ開始サンプル
 *			lped	: ループ終了サンプル
 * [関数値] なし
 * [機　能] ループ開始位置とループ終了位置の情報を取得する。
 * [備　考] 状態が、再生準備中(ADXT_STAT_PREP)から再生終了(ADXT_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get position of loop points
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] lpst	: Loop start point
 *            lped	: Loop end point
 * [Return  ] None
 * [Function] Get information of the loop start position and the loop end position. 
 * [Remarks ] You can obtain a time between status of preparation
 *			  (ADXT_STAT_PREP) and status of play end(ADXT_STAT_PLAYEND).
 */
void ADXT_GetLpInfo(ADXT adxt, CriSint32 *lpst, CriSint32 *lped);

/*	$func$ ループした回数の取得
 * [書　式] CriSint32 ADXT_GetLpCnt(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] ループした回数
 * [機　能] ループした回数を取得する。
 *  Get the number of times played a loop
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Number of times played a loop
 * [Function] Obtains the number of times that played a loop.
 */
CriSint32 CRIAPI ADXT_GetLpCnt(ADXT adxt);

/*	$func$ ループフラグの設定
 * [書　式] void ADXT_SetLpFlg(ADXT adxt, CriSint32 flg);
 * [入　力] adxt	: ADXTハンドル
 *			flg		: 1=ループする、0=ループしない
 * [出　力] なし
 * [関数値] なし
 * [機　能] ループするか否かを設定する。
 *			再生中は、ループを解除することのみ可能。
 *			再度、ループを設定する時は、再生開始直前に設定すること。
 *  Set the loop flag
 * [Inputs  ] adxt	: ADXT handle
 *			  flg	: 1=loop, 0=not loop
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets whether play a loop or not.
 *			  You can only release looping play while playing.
 *			  If you set this flag again,
 *			  you have to set just before start playing.
 */
void CRIAPI ADXT_SetLpFlg(ADXT adxt, CriSint32 flg);

/*	$func$ 入力ストリームジョイントの取得
 * [書　式] SJ ADXT_GetInputSj(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] ストリームジョイントハンドル
 * [機　能] 入力用ストリームジョイントを取得する。
 *  Get input Stream Joint
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Stream Joint
 * [Function] Obtains Stream Joint for the input.
 */
SJ CRIAPI ADXT_GetInputSj(ADXT adxt);

/*	$func$ 音声出力開始待ちの設定
 * [書　式] void ADXT_SetWaitPlayStart(ADXT adxt, CriSint32 flg);
 * [入　力] adxt	: ADXTハンドル
 *			flg		: 1=ウェイトする、0=音声出力を即座に開始する
 * [出　力] なし
 * [関数値] なし
 * [機　能] 音声の出力の開始待ちを設定する。
 *  Set waiting for starting to output the sound
 * [Inputs  ] adxt	: ADXT handle
 *			  flg	: 1=waits, 0=starts to output sound
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets waiting for starting to output the sound.
 */
void CRIAPI ADXT_SetWaitPlayStart(ADXT adxt, CriSint32 flg);

/*	$func$ 音声出力開始準備完了か否かの判定
 * [書　式] CriSint32 ADXT_IsReadyPlayStart(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 1:準備完了／0:準備中
 * [機　能] 音声出力開始の準備が完了か否かを判定する。
 *  Check whether preparations completion
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Checks whether ADXT handle was preparations completion.
 */
CriSint32 CRIAPI ADXT_IsReadyPlayStart(ADXT adxt);

/*	$func$ 一時停止の設定
 * [書　式] void ADXT_Pause(ADXT adxt, CriSint32 sw);
 * [入　力] adxt	: ADXTハンドル
 *			sw		: 1=一時停止、0=再開
 * [出　力] なし
 * [関数値] なし
 * [機　能] 一時停止するか否かを設定する。
 *  Pause/Continue
 * [Inputs  ] adxt	: ADXT handle
 *			  sw	: 1=pause, 0=continue
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops temporarily by a specified switch and release temporary 
 *			  standstill.
 */
void CRIAPI ADXT_Pause(ADXT adxt, CriSint32 sw);

/*	$func$ 一時停止状態の取得
 * [書　式] CriSint32 ADXT_GetStatPause(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] 一時停止状態 1=一時停止、0=非一時停止
 * [関数値] なし
 * [機　能] 一時停止状態を取得する。
 *  Pause/Continue
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] 1=pause, 0=not pause
 * [Return  ] None
 * [Function] Obtains the status of pause.
 */
CriSint32 CRIAPI ADXT_GetStatPause(ADXT adxt);

/*	$func$ 全体一時停止の設定
 * [書　式] void ADXT_PauseAll(CriSint32 sw);
 * [入　力] sw		: 1=一時停止、0=再開
 * [出　力] なし
 * [関数値] なし
 * [機　能] すべての再生を一時停止するか否かを設定する。
 *          ハンドルごとの一時停止より優先して適用される。
 * Pause/Continue all playbacks
 * [Inputs  ] sw	: 1=pause, 0=continue
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops temporarily all playbacks by a specified switch and
 *            release temporary standstill.
 *            Applies in preference to pause on each handle.
 */
void CRIAPI ADXT_PauseAll(CriSint32 sw);

/*	$func$ 全体一時停止状態の取得
 * [書　式] CriSint32 ADXT_GetStatPauseAll(void);
 * [入　力] なし
 * [出　力] 一時停止状態 1=一時停止、0=非一時停止
 * [関数値] なし
 * [機　能] 全体の一時停止状態を取得する。
 * Get state of pausing of all playbacks
 * [Inputs  ] None
 * [Outputs ] 1=pause, 0=not pause
 * [Return  ] None
 * [Function] Obtains the status of pausing of all playbacks.
 */
CriSint32 CRIAPI ADXT_GetStatPauseAll(void);

/*	$func$ ダイレクトセンドレベルの設定
 * [書　式] void ADXT_SetDrctLvl(ADXT adxt, CriSint32 drctlvl)
 * [入　力] adxt	: ADXTハンドル
 *			drctlvl	: ダイレクトセンドレベル　(0dB ～ -45dB)
 * [出　力] なし
 * [関数値] なし
 * [機　能] ダイレクトセンドレベルを設定する。
 */
void CRIAPI ADXT_SetDrctLvl(ADXT adxt, CriSint32 drctlvl);

/*	$func$ ダイレクトセンドレベルの取得
 * [書　式] CriSint32 ADXT_GetDrctLvl(ADXT adxt)
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] ダイレクトセンドレベル　(0dB ～ -45dB)
 * [機　能] ダイレクトセンドレベルを取得する。
 */
CriSint32 CRIAPI ADXT_GetDrctLvl(ADXT adxt);

/*	$func$ エフェクタの設定
 * [書　式] void ADXT_SetFx(ADXT adxt, CriSint32 fxch, CriSint32 fxlvl);
 * [入　力] adxt	: ADXTハンドル
 *			fxch	: エフェクタの入力チャンネル番号 (0～15)
 *			fxlvl	: エフェクタの入力レベル　(0dB ～ -45dB)
 * [出　力] なし
 * [関数値] なし
 * [機　能] エフェクタへの出力を設定する。
 *  Set the effect
 * [Inputs  ] adxt	: ADXT handle
 *			  fxch	: Input channel number to Effector (0-15)
 *			  fxlvl	: Input level to Effector(from 0 to -45[dB])
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets output to the effect.
 */
void CRIAPI ADXT_SetFx(ADXT adxt, CriSint32 fxch, CriSint32 fxlvl);

/*	$func$ エフェクタ設定値の取得
 * [書　式] void ADXT_GetFx(ADXT adxt, CriSint32 *fxch, CriSint32 *fxlvl);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] fxch	: エフェクタへの入力チャンネル番号 (0～15)
 *			fxlvl	: エフェクタの入力レベル　(0dB ～ -45dB)
 * [関数値] なし
 * [機　能] エフェクタの設定値を取得する。
 *  Get the effect value
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] fxch	: Input channel number to Effector(0-15)
 *			  fxlvl	: Input level to Effector(from 0 to -45[dB])
 * [Return  ] None
 * [Function] Obtains the effect value.
 */
void CRIAPI ADXT_GetFx(ADXT adxt, CriSint32 *fxch, CriSint32 *fxlvl);

/*	$func$ フィルタの設定
 * [書　式] void ADXT_SetFilter(ADXT adxt, CriSint32 coff, CriSint32 q);
 * [入　力] adxt	: ADXTハンドル
 *			coff	: カットオフ周波数
 *			q		: 尖鋭度
 * [出　力] なし
 * [関数値] なし
 * [機　能] フィルタへの出力を設定する。
 *  Set the filter
 * [Inputs  ] adxt	: ADXT handle
 *			  coff	: Cut off frequency
 *			  q		: 
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the filter.
 */
void CRIAPI ADXT_SetFilter(ADXT adxt, CriSint32 coff, CriSint32 q);

/*	$func$ フィルタ設定値の取得
 * [書　式] void ADXT_GetFilter(ADXT adxt, CriSint32 *coff, CriSint32 *q);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] coff	: カットオフ周波数
 *			q		: 尖鋭度
 * [関数値] なし
 * [機　能] フィルタへの出力値を取得する。
 *  Get the filter value
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] coff	: Cut off frequency
 *			  q		: 
 * [Return  ] None
 * [Function] Obtains the filter value.
 */
void CRIAPI ADXT_GetFilter(ADXT adxt, CriSint32 *coff, CriSint32 *q);

/*	$func$ トランスポーズの設定
 * [書　式] void ADXT_SetTranspose(ADXT adxt, CriSint32 transps, CriSint32 detune);
 * [入　力] adxt	: ADXTハンドル
 *			transps	: トランスポーズ量
 *			detune	: デチューン量
 * [出　力] なし
 * [関数値] なし
 * [機　能] トランスポーズを設定する。
 *  Set transpose
 * [Inputs  ] adxt		: ADXT handle
 *			  transps	: Amount of transpose
 *			  detune	: Amount of detune
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the transpose.
 */
void CRIAPI ADXT_SetTranspose(ADXT adxt, CriSint32 transps, CriSint32 detune);

/*	$func$ トランスポーズの取得
 * [書　式] void ADXT_GetTranspose(ADXT adxt, CriSint32 *transps, CriSint32 *detune);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] transps	: トランスポーズ量
 *			detune	: デチューン量
 * [関数値] なし
 * [機　能] トランスポーズ量を取得する。
 *  Get tanspose
 * [Inputs  ] adxt		: ADXT handle
 * [Outputs ] transps	: Amount of transpose
 *			  detune	: Amount of detune
 * [Return  ] None
 * [Function] Obtains the amount of transpose.
 */
void CRIAPI ADXT_GetTranspose(ADXT adxt, CriSint32 *transps, CriSint32 *detune);

/*	$func$ 再生速度比の設定
 * [書　式] void ADXT_SetSpeedRational(ADXT adxt, CriSint32 speed_n, CriSint32 speed_d);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] speed_n	: 再生速度比：分子(numerator)
 *			speed_d	: 再生速度比：分母(denominator)
 * [関数値] なし
 * [機　能] 再生周波数を変更する。変更量はもともとの再生周波数との比で表す。
 *  Set Speed Rational
 * [Inputs  ] adxt		: ADXT handle
 * [Outputs ] speed_n	: numerator
 *			  speed_d	: denominator
 * [Return  ] None
 * [Function] 
 */
void ADXT_SetSpeedRational(ADXT adxt, CriSint32 speed_n, CriSint32 speed_d);
    
/*	$func$ エラー関数の登録
 * [書　式] void ADXT_EntryErrFunc(void (*func)(void *obj, CriSint8 *msg),
 *																void *obj);
 * [入　力] func	: エラー処理関数
 *			obj		: エラー処理関数の第1引数
 * [出　力] なし
 * [関数値] なし
 * [機　能] エラー処理関数を登録する。
 *  Entry error callback function
 * [Inputs  ] func	: Error handling function
 *			  obj	: First argument of error handling function
 * [Outputs ] None
 * [Return  ] None
 * [Function] Entry error callback function.
 */
void CRIAPI ADXT_EntryErrFunc(void (*func)(void *obj, CriSint8 *msg), void *obj);

/*	$func$ 音声データの放棄
 * [書　式] CriSint32 ADXT_DiscardSmpl(ADXT adxt, CriSint32 nsmpl)
 * [入　力] adxt	: ADXTハンドル
 *			nsmpl	: サンプル数
 * [出　力] なし
 * [関数値] 捨てたサンプル数
 * [機　能] デコードした音声データを捨てる。
 *  Discard sound data
 * [Inputs  ] adxt	: ADXT handle
 *			  nsmpl	: Number of sample
 * [Outputs ] None
 * [Return  ] sample number discarded
 * [Function] Discards sound data decoded.
 */
CriSint32 CRIAPI ADXT_DiscardSmpl(ADXT adxt, CriSint32 nsmpl);

/*	リトライ回数の設定	*/
void CRIAPI ADXT_SetNumRetry(CriSint32 num);

/*	リトライ設定値の取得	*/
CriSint32 CRIAPI ADXT_GetNumRetry(void);

/*	エラー回数の取得	*/
CriSint32 CRIAPI ADXT_GetNumErr(ADXT adxt);

/*	$func$ ヘッダ情報の取得
 * [書　式] void ADXT_GetHdrInfo(CriSint8 *buf, CriSint32 bsize, ADXHINFO *hinfo);
 * [入　力] buf		: ADXデータが格納されたバッファ
 *			bsize	: バッファのサイズ
 * [出　力] hinfo	: ヘッダ情報
 * [関数値] なし
 * [機　能] オンメモリのADXデータを解析し、ヘッダ情報を取得する。
 *			取得できるパラメータについてはADXHINFOを参照してください。
 *	Gets the ADX header information
 * [Inputs  ] buf	: Buffer including the ADX data
 *			  bsize	: Buffer size
 * [Outputs ] hinfo	: ADX header information
 * [Return  ] None
 * [Function] Analyzes the ADX data in buffer to get the header information.
 *			  The results of analysis is stored into the ADXHINFO structure.
*/
void CRIAPI ADXT_GetHdrInfo(CriSint8 *buf, CriSint32 bsize, ADXHINFO *hinfo);

/*	$func$ ファイル読み込みステータスの取得
 * [書　式] CriSint32 ADXT_GetStatRead(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 1=読み込み中/読み込み準備中、0=非読み込み中/非読み込み準備中
 * [機　能] 読み込み状態を取得する
 */
CriSint32 CRIAPI ADXT_GetStatRead(ADXT adxt);

/*	$func$ STMの取得
 * [書　式] ADXSTM ADXT_GetStm(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] STM
 * [機　能] STMを取得する。
 */
ADXSTM CRIAPI ADXT_GetStm(ADXT adxt);

/* データ供給停止通知 */
void CRIAPI ADXT_TermSupply(ADXT adxt);

/***
*		スピーカーへのルーティング設定
*		Functions for audio routing to speakers
**/
/* スピーカーへのセンドレベルの取得			*/
/* Gets the sending level to the speaker 	*/
CriSint32 CRIAPI ADXT_GetSendSpeakerLevel(
	ADXT adxt, CriSint32 ch_id, CriSint32 spk_id);

/* スピーカーへのセンドレベルの割り当て		*/
/* Sets the sending level to the speaker 	*/
void CRIAPI ADXT_SetSendSpeakerLevel(
	ADXT adxt, CriSint32 ch_id, CriSint32 spk_id, CriSint32 volume);

/* センドレベル割り当てのリセット				*/
/* Resets all sending levels to the speakers 	*/
void CRIAPI ADXT_ResetAllSendSpeakerLevel(ADXT adxt);

/***
*		ダウンミックスの設定
*		Functions for audio downmix
**/
/* ダウンミックス方法の設定 */
/* Sets the method of downmix */
void CRIAPI ADXT_SetDownmixMode(CriSint32 mode);

/***
*			フィルタ作成用コールバック
*
*			デコードする毎に登録された関数が呼び出される。
*			(*user_flt_cbf)(void *obj, CriSint32 ch, void *data, CriSint32 dtlen);
*			obj:	ADXT_EntryFltFunc関数の第３引数
*			ch:		チャンネル番号　(0:左, 1:右)
*			data:	展開されたデータ (ADXの場合は、16bitPCM)
*			dtlen:	展開されたデータのバイト数
*			※　振幅抽出機能とは共用できない。
**/

/*	フィルタコールバック関数の登録	*/
void CRIAPI ADXT_EntryFltFunc(ADXT adxt,
		void (*f)(void *obj, CriSint32 ch, void *data, CriSint32 dtlen), void *obj);

/* $func$ デコードサンプル数の取得
 * [書　式] CriSint32 ADXT_GetDecNumSmpl(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] デコードした音声データの総サンプル数
 * [機　能] デコードした音声データの総サンプル数を取得する。
 * [備　考] 状態が、再生準備中(ADXT_STAT_PREP)から再生終了(ADXT_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get a decoded sample number of sound data
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] decoded sample number of sound data
 * [Function] Obtains a decoded sample number of sound data.
 * [Remarks ] You can obtain a time between status of preparation
 *			  (ADXT_STAT_PREP) and status of play end(ADXT_STAT_PLAYEND).
 */
CriSint32 CRIAPI ADXT_GetDecNumSmpl(ADXT adxt);

/*	デコードしたデータ量の取得 (バイト単位)	*/
CriSint32 CRIAPI ADXT_GetDecDtLen(ADXT adxt);

/*	デコードコールバックの登録 */
void CRIAPI ADXT_SetCbDec(ADXT adxt, 
	void (*func)(void *obj, CriSint32 ibyte, CriSint32 obyte), void *obj);

/* $func$ オーディオレンダラの取得
 * [書　式] ADXRNA ADXT_GetRna(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] オーディオレンダラ
 * [機　能] ADXTハンドルの持つオーディオレンダラを取得する。
 * [備　考] 
 *  Getting Audio Renderer 
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Audio Renderer
 * [Function] Obtains Audio Renderer of ADXT handle.
 * [Remarks ] 
 */
ADXRNA CRIAPI ADXT_GetRna(ADXT adxt);


/***
*		シームレス連続再生機能
*
*		・ループ設定の無い単純なADX データのみ連続再生可能
*		・最大16ファイルまで登録可能
*
***/

/*	最大登録ファイル数									*/
/*	Number of maximum files to entry 					*/
#define	ADXT_MAX_ENTRY_FILES		(16)

/*	$func$ シームレス連続再生ファイルの登録
 * [書　式] void ADXT_EntryFname(ADXT adxt, const CriChar8 *fname)
 * [入　力] adxt	: ADXTハンドル
 *			fname	: ファイル名
 * [出　力] なし
 * [関数値] なし
 * [機　能] シームレス連続再生用のファイルを登録する。
 *			ループ設定のない単純なADXデータのみ登録できる。
 *  Entry file for seamless continuous play
 * [Inputs  ] adxt	: ADXT handle
 *			  fname	: ADX File name
 * [Outputs ] None
 * [Return  ] None
 * [Function] Entry file for seamless continuous play.
 *			  You can only simple ADX data file without looping.
 */
void CRIAPI ADXT_EntryFname(ADXT adxt, const CriChar8 *fname);

/*	シームレス連続再生ファイルの登録（範囲指定）	*/
void CRIAPI ADXT_EntryFnameRange(ADXT adxt, const CriChar8 *fname, CriSint32 ofst_sct, CriSint32 range_sct);

/*	$func$ シームレス連続再生サブファイルの登録
 * [書　式] void ADXT_EntryAfs(ADXT adxt, CriSint32 patid, CriSint32 fid)
 * [入　力] adxt	: ADXTハンドル
 *			patid	: パーティションID
 *			fid		: ファイルID
 * [出　力] なし
 * [関数値] なし
 * [機　能] シームレス連続再生用のサブファイルを登録する。
 *			ループ設定のない単純なADXデータのみ登録できる。
 *  Entry file for seamless continuous play
 * [Inputs  ] adxt	: ADXT handle
 *			  patid	: Partition ID
 *			  fid	: File ID
 * [Outputs ] None
 * [Return  ] None
 * [Function] Entry subfile for seamless continuous play.
 *			  You can only simple ADX data subfile without looping.
 */
void CRIAPI ADXT_EntryAfs(ADXT adxt, CriSint32 patid, CriSint32 fid);

/*	$func$ シームレス連続再生の開始
 * [書　式] void ADXT_StartSeamless(ADXT adxt)
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] シームレス連続再生を開始する。
 *  Start seamless continuous play
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Start seamless continuous play.
 */
void CRIAPI ADXT_StartSeamless(ADXT adxt);

/*	$func$ シームレスループ再生の設定
 * [書　式] void ADXT_SetSeamlessLp(ADXT adxt, CriSint32 flg)
 * [入　力] adxt	: ADXTハンドル
 *			flg		: 0-ループ再生しない,1-ループ再生する
 * [出　力] なし
 * [関数値] なし
 * [機　能] 現在、登録されているファイルを繰り返し再生する。
 *			ファイルのすべてのデータを読み終わると自動的に再度登録される。
 *  Set seamless loop play
 * [Inputs  ] adxt	: ADXT handle
 *			  flg	: 0-Off, 1-On
 * [Outputs ] None
 * [Return  ] None
 * [Function] You can play all entried files with looping.
 */
void CRIAPI ADXT_SetSeamlessLp(ADXT adxt, CriSint32 flg);

/*	$func$ シームレスループ再生の開始
 * [書　式] void ADXT_StartFnameLp(ADXT adxt, const CriChar8 *fname)
 * [入　力] adxt	: ADXTハンドル
 *			fname	: ファイル名
 * [出　力] なし
 * [関数値] なし
 * [機　能] 指定されたファイルを繰り返し再生する。
 *  Start seamless loop play
 * [Inputs  ] adxt	: ADXT handle
 *			  fname	: ADX File Name
 * [Outputs ] None
 * [Return  ] None
 * [Function] Start seamless loop plaing your specified file (fname).
 */
void CRIAPI ADXT_StartFnameLp(ADXT adxt, const CriChar8 *fname);

/*	シームレスループ再生の開始（範囲指定）	*/
void CRIAPI ADXT_StartFnameRangeLp(ADXT adxt, const CriChar8 *fname, CriSint32 ofst_sct, CriSint32 range_sct);

/*	AFSシームレスループ再生の開始
 * [書　式] ADXT_StartAfsLp(ADXT adxt, CriSint32 patid, CriSint32 fid)
 * [入　力] adxt	: ADXTハンドル
 *			patid	: パーティションID
 *			fid		: ファイルID
 * [出　力] なし
 * [関数値] なし
 * [機　能] 指定されたAFSファイルを繰り返し再生する。
 *  Start seamless loop play
 * [Inputs  ] adxt	: ADXT handle
 *			  patid	: Partition ID
 *			  fid	: File ID
 * [Outputs ] None
 * [Return  ] None
 * [Function] Start seamless loop plaing your specified AFS file.
 */
void CRIAPI ADXT_StartAfsLp(ADXT adxt, CriSint32 patid, CriSint32 fid);

/*	$func$ シームレス連続再生の解除
 * [書　式] void ADXT_ReleaseSeamless(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] シームレス連続再生の解除する。
 *  Release seamless continuous play
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Release seamless continuous play.
 */
void CRIAPI ADXT_ReleaseSeamless(ADXT adxt);

/*	$func$ 登録ファイル数の取得
 * [書　式] CriSint32 ADXT_GetNumFiles(ADXT adxt)
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 登録されているファイル数
 * [機　能] 登録されているファイル数を取得する。
 *  Get number of entried files
 * [Inputs  ] adxt	: ADXT handle
 * [Outputs ] None
 * [Return  ] Number of Entried Files
 * [Function] Get number of entried files
 */
CriSint32 CRIAPI ADXT_GetNumFiles(ADXT adxt);

/*	連結再生の設定　(1:する、0:しない)	*/
void CRIAPI ADXT_SetLnkSw(ADXT adxt, CriSint32 sw);

/*	連結再生の取得　(1:する、0:しない)	*/
CriSint32 CRIAPI ADXT_GetLnkSw(ADXT adxt);

/*	ファイルの登録をリセット(STOP状態でのみ有効)	*/
void CRIAPI ADXT_ResetEntry(ADXT adxt);

/***
*			振幅抽出機能
***/

/*	振幅取得用作業領域の大きさの計算	*/
CriSint32 CRIAPI ADXT_CalcAmpWork(CriSint32 maxnch);
/*	振幅取得用作業領域の設定	*/
void CRIAPI ADXT_SetAmpWork(ADXT adxt, void *work, CriSint32 wksize);
/*	振幅の取得	*/
CriSint32 CRIAPI ADXT_GetAmplitude(ADXT adxt, CriSint32 ch);
/*	振幅の取得 (時刻オフセット指定付き)	*/
CriSint32 CRIAPI ADXT_GetAmplitude2(ADXT adxt, CriSint32 ch, CriSint32 msec, CriSint32 *msec2);

/***
*			Sofdec用拡張関数
***/

/*	V-Sync割り込み用関数			*/
/*	Shinobi環境では、初期化時に割り込みに自動的に登録されるので、		*/
/*	使用しません。Naomi環境でSofdec Movieを再生する場合には、			*/
/*	この関数をV-Sync割り込みから実行してください。						*/
void CRIAPI ADXT_VsyncProc(void);

/*	時刻の単位の設定　(0:Sfreq(Audio), 1:V-Sync(NTSC), 2:V-sync(PAL)	*/
void CRIAPI ADXT_SetTimeMode(CriSint32 mode);

/* ADXヘッダの判定 */
CriSint32 CRIAPI ADXT_IsHeader(CriSint8 *adr, CriSint32 siz, CriSint32 *hdrsiz);

/* ADX終了コードの判定 */
CriSint32 CRIAPI ADXT_IsEndcode(CriSint8 *adr, CriSint32 siz, CriSint32 *endsiz);
/*
	adr   ：終了判定ポインタ(ADXデータの36バイト境界)
	siz   ：終了判定サイズ (判定に必要なサイズは高々数バイト
			かもしれませんが、ファイルリードして、バッファに
			蓄積されているサイズを入力します)
	ndsiz：終了コードサイズ（フッタサイズ）
*/

/* 無音データの強制挿入 */
CriSint32 CRIAPI ADXT_InsertSilence(ADXT adxt, CriSint32 nch, CriSint32 nsmpl);
/*
	nsmpl 　：挿入要求サンプル数 [sample/1ch]
	関数値  ：挿入実行サンプル数 (0 ～ nsmpl)
			  ADXT入力用SJに十分な空きがあれば nsmpl の端数を
			  切り捨てた値が返る。
*/

/*			強制モノラル出力スイッチの設定
[書  式]	void ADXT_SetOutputMono(CriSint32 flag);
[引  数]	CriSint32 flag		強制モノラル出力フラグ(OFF:0, ON:1)
[戻り値]	なし					
[機  能]	ステレオデータを強制的にモノラルデータとして出力する。
[備　考]	
*/
void CRIAPI ADXT_SetOutputMono(CriSint32 flag);

/*	ヘッダ情報の強制挿入	*/
void CRIAPI ADXT_InsertHdrSfa(ADXT adxt, CriSint32 nch, CriSint32 sfreq, CriSint32 nsmpl);
/*	タイムオフセットの取得	*/
CriSint32 CRIAPI ADXT_GetTimeOfst(ADXT adxt);
/*	タイムオフセットの設定	*/
void CRIAPI ADXT_SetTimeOfst(ADXT adxt, CriSint32 ofst);
/*	サンプル数の調節	*/
CriSint32 CRIAPI ADXT_AdjustSmpl(ADXT adxt, CriSint32 nsmpl);
/* デフォルトフォーマットの外部指定 */
void CRIAPI ADXT_SetDefFmt(ADXT adxt, CriSint32 fmt);

/***
*		AHX 用関数
*		Functions for AHX
***/

/*	$func$ AHXの組み込み
 * [書　式] void ADXT_AttachAhx(ADXT adxt, void *work, CriSint32 worksize);
 * [入　力] adxt	: ADXTハンドル
 *			work	: AHXワーク領域
 *			worksize: AHXワーク領域のサイズ
 * [出　力] なし
 * [関数値] なし
 * [機　能] AHXの組み込みを行い、AHXの利用を可能にする。
 *  Attach AHX
 * [Inputs  ] adxt    : ADXT handle
 *			  work    : Working area for AHX
 *			  worksize: Size of AHX working area(byte)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Attach AHX, Use of AHX is attained.
 */
void CRIAPI ADXT_AttachAhx(ADXT adxt, void *work, CriSint32 worksize);

/*	$func$ AHXの分離
 * [書　式] void ADXT_DetachAhx(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] AHXを分離し、AHXの利用をやめる。
 *  Detach AHX
 * [Inputs  ] adxt    : ADXT handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Detach AHX, Unuse of AHX is attained.
 */
void CRIAPI ADXT_DetachAhx(ADXT adxt);

/***
*		暗号解除用関数
*		Functions for encrypted file
***/

/* ハンドル毎のキーコードの設定 */
void CRIAPI ADXT_SetKeyString(ADXT adxt, CriChar8 *str);

/* 全体のキーコードの設定 */
void CRIAPI ADXT_SetDefKeyString(CriChar8 *str);


/***
*		デコードエラー用関数
*		Functions for Decode error
***/

/*	デコードエラーモードの設定	*/
/*	Sets decode error mode		*/
void CRIAPI ADXT_SetDecErrMode(CriSint32 mode);
/*	デコードエラーモードの取得	*/
/*	Gets decode error mode		*/
CriSint32 CRIAPI ADXT_GetDecErrMode(void);

/***
*		ADXマネージャ関数
*		ADX Manager Functions
***/

/*	ADXスレッドの初期化					*/
/*	Initialization of ADX Thread 		*/
void CRIAPI ADXM_SetupThrd(ADXM_TPRM *tprm);
/*	ADXスレッドの終了処理				*/
/*	Termination of ADX Thread Extended 	*/
void CRIAPI ADXM_ShutdownThrd(void);
/*	ADXスレッドパラメータの取得			*/
/*	Gets parameters of ADX Thread 		*/
void CRIAPI ADXM_GetThrdParam(ADXM_TPRM *tprm);

/*	ADXスレッドの初期化(拡張版)				*/
/*	Initialization of ADX Thread Extended 	*/
void CRIAPI ADXM_SetupThrdEx(ADXM_TPRM_EX *tprm);
/*	ADXスレッドの終了処理(拡張版)			*/
/*	Termination of ADX Thread Extended 		*/
void CRIAPI ADXM_ShutdownThrdEx(void);

/*	ユーザVSYNCスレッドへの関数登録				*/
/*	Set callback function on UsrVsync Thread	*/
void CRIAPI ADXM_SetCbUsrVsync(CriBool (*func)(void *obj), void *obj);
/*	ユーザアイドルスレッドへの関数登録			*/
/*	Set callback function on UsrIdle Thread		*/
void CRIAPI ADXM_SetCbUsrIdle(CriBool (*func)(void *obj), void *obj);
/*	ユーザアイドルサーバの区切り処理						*/
/*	Execution UsrIdle Thread for termination of function	*/
void CRIAPI ADXM_GotoUsrIdleBdr(void);
/*	UsrVsyncサーバ関数の実行	*/
/*	Execution UsrVsync Server	*/
CriSint32 CRIAPI ADXM_ExecSvrUsrVsync(void);
/*	UsrIdleサーバ関数の実行		*/
/*	Execution UsrIdle Server	*/
CriSint32 CRIAPI ADXM_ExecSvrUsrIdle(void);
/*	Vsyncサーバ関数	*/
CriSint32 CRIAPI ADXM_ExecSvrVsync(void);
/*	ファイルシステムサーバ関数	*/
CriSint32 CRIAPI ADXM_ExecSvrFs(void);
/*	メインループ内で呼び出す関数	*/
CriSint32 CRIAPI ADXM_ExecSvrMain(void);
/*	ミドルウェア用スレッドで呼び出す関数	*/
CriSint32 CRIAPI ADXM_ExecSvrMwIdle(void);
/*	すべてのサーバ処理を呼び出す関数	*/
CriSint32 CRIAPI ADXM_ExecSvrAll(void);

/*	エラー関数の登録								*/
void CRIAPI ADXM_SetCbErr(void (*fn)(void *obj, CriChar8 *emsg), void *obj);
/*	負荷測定用関数	*/
void CRIAPI ADXM_SetCbSleepMwIdle(void (*fn)(void *), void *obj);
/*	スレッド初期化が行われたか否かの判定	*/
CriBool CRIAPI ADXM_IsSetupThrd(void);

/*	スレッドのロック	*/
void CRIAPI ADXM_Lock(void);
/*	スレッドのアンロック	*/
void CRIAPI ADXM_Unlock(void);
/* ロックレベルの取得 */
CriSint32 CRIAPI ADXM_GetLockLevel(void);

/*	V-Sync待ち関数	*/
void CRIAPI ADXM_WaitVsync(void);

/*	V-Sync割り込みハンドラ	*/
CriSint32 CRIAPI ADXM_ExecVint(CriSint32 arg);

/*	メインスレッドで呼び出す関数	*/
CriSint32 CRIAPI ADXM_ExecMain(void);

/* アイドルスレッドのトップ／エンドコールバック関数の登録 */
void CRIAPI ADXM_SetCbIdleTopEnd(ADXM_CBFN topfn, void *topobj, ADXM_CBFN endfn, void *endobj);

/***
*		WAVファイルの特殊再生用関数
*		Functions for WAV file special playback
***/

/*	WAVファイルの無限再生スイッチの設定	*/
void CRIAPI ADXT_SetEndlessPlaySw(ADXT adxt, CriSint32 sw);
/*	WAVファイルの無限再生スイッチの取得	*/
CriSint32 CRIAPI ADXT_GetEndlessPlaySw(ADXT adxt);


#ifdef __cplusplus
}
#endif

#endif

/* end of file */
