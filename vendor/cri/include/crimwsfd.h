/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2001-2008 CRI Middleware Co., Ltd.
 *
 * Library  : MWSFD Library (CRI Sofdec)
 * Module   : Library User's Header
 * File     : crimwsfd.h
 * Date     : 2008-11-12
 * Version  : (see MWD_SFD_VER)
 *
 ****************************************************************************/
#ifndef	INCLUDED_CRIMWSFD_H		/* Re-definition prevention */
#define	INCLUDED_CRIMWSFD_H

/*	Version No.	*/
#define	MWD_SFD_VER		"4.75"
#define	MWD_SFD_NAME	"MWSFD"

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <cri_xpt.h>
#include <cri_adxt.h>
//#include <cri_log.h>
#include <sj.h>

/***************************************************************************
 *      MACRO CONSTANT
 ***************************************************************************/
/* 垂直同期周波数  */
/* Vsync frequency */
#define MWSFD_VHZ_59_94			(59.94f)
#define MWSFD_VHZ_50_00			(50.00f)
#define MWSFD_VHZ_60_00			(60.00f)

/* 合成モード		*/
/* Composition Mode	*/
#define MWSFD_COMPO_AUTO		(0x00000000)	/* 自動						*/
												/* Auto						*/
#define MWSFD_COMPO_OPAQ		(0x00000011)	/* 不透明					*/
												/* opacity					*/
#define MWSFD_COMPO_ALPHFULL	(0x00000021)	/* フルAlpha合成			*/
												/* Full Alpha				*/
#define	MWSFD_COMPO_ALPHLUMI	(0x00000031)	/* ルミナンスキー合成 		*/
												/* Luminance key alpha		*/
#define	MWSFD_COMPO_ALPH3		(0x00000041)	/* 3値アルファ 				*/
												/* 3-value alpha			*/
#define MWSFD_COMPO_Z			(0x00000101)	/* Ｚ値						*/
												/* Z value					*/

/* ファイルタイプ		*/
/* File type			*/
#define	MWSFD_FTYPE_NON			(0)
#define	MWSFD_FTYPE_SFD			(1)
#define	MWSFD_FTYPE_MPV			(2)
#define MWSFD_FTYPE_VONLYSFD	(3)
#define MWSFD_FTYPE_M2TS		(5)		/* implemented only in a special platform */
#define	MWSFD_FTYPE_M4V			(6)		/* implemented only in a special platform */
#define	MWSFD_FTYPE_M4V_SFD		(7)		/* implemented only in a special platform */
#define	MWSFD_FTYPE_H264		(8)		/* implemented only in a special platform */
#define	MWSFD_FTYPE_H264_SFD	(9)		/* implemented only in a special platform */
#define	MWSFD_FTYPE_M4V_USF 	(10)	/* implemented only in a special platform */
#define MWSFD_FTYPE_H264_TS		(11)	/* implemented only in a special platform */

/* Audio Volume MAX, MIN */
#define	MWSFD_OUTVOL_MAX		(0)		/* MAX Volume */
#define	MWSFD_OUTVOL_MIN		(-960)	/* MIN Volume */

/* Audio Channel	*/
#define MWSFD_CH_L				(0)		/* Left		*/
#define MWSFD_CH_R				(1)		/* Right	*/

/*	Panpot Value	*/
#define	MWSFD_PAN_LEFT			(-15)
#define	MWSFD_PAN_CENTER		(0)
#define	MWSFD_PAN_RIGHT			(15)
#define	MWSFD_PAN_AUTO			(-128)

/* 出力バッファ幅のデフォルト値 	*/
/* Default value of out-buffer size	*/
#define MWSFD_DFL_OUTBUFSIZE	(0)

/* 同期モード	*/
/* Sync Mode	*/
#define MWSFD_SYNCMODE_NONE		(0)		/* 無同期		*/ /* No sync		*/
#define MWSFD_SYNCMODE_VSYNC	(1)		/* V-Sync同期	*/ /* Sync Vint		*/
#define MWSFD_SYNCMODE_AUDIO	(2)		/* Audio 同期	*/ /* Sync Audio	*/
#define MWSFD_SYNCMODE_USRTIME	(4)		/* ユーザ時刻同期 */
#define MWSFD_SYNCMODE_EXTCLOCK	(5)		/* 外部クロック同期 */
#define MWSFD_SYNCMODE_EASYINFINI (6)	/* 簡易無限再生 */ /* Easy infinite video only playback */

/* メモリ再生時のファイルサイズ省略(サイズ自動取得) */
#define MWSFD_FILESIZE_UNKNOWN			(0xFFFFFFFF)

/* PIDの無効値 (無指定と等価、無指定の場合はライブラリ内部のマップ解析が有効) */
#define MWSFD_PID_VOID			(-1)

/* Z Value Range	*/
#define MWSFD_ZVAL_MIN			(0x00000000)	/* Minimum Z Value (32bit) */
#define MWSFD_ZVAL_MAX			(0x7fffffff)	/* Maximum Z Value (32bit) */
#define MWSFD_ZVAL_RANGE		(MWSFD_ZVAL_MAX - MWSFD_ZVAL_MIN)

/* [for Compatibility] */
#define	MWD_PLY_FTYPE_NON		MWSFD_FTYPE_NON
#define	MWD_PLY_FTYPE_SFD		MWSFD_FTYPE_SFD
#define	MWD_PLY_FTYPE_MPV		MWSFD_FTYPE_MPV
#define MWD_PLY_COMPO_OPEQ		MWSFD_COMPO_OPAQ
#define MWD_PLY_SYNC_NONE		MWSFD_SYNCMODE_NONE
#define MWD_PLY_SYNC_VSYNC		MWSFD_SYNCMODE_VSYNC
#define MWD_PLY_SYNC_AUDIO		MWSFD_SYNCMODE_AUDIO
#define MWD_CH_L				MWSFD_CH_L
#define MWD_CH_R				MWSFD_CH_R
#define	MWD_PAN_LEFT			MWSFD_PAN_LEFT
#define	MWD_PAN_CENTER			MWSFD_PAN_CENTER
#define	MWD_PAN_RIGHT			MWSFD_PAN_RIGHT
#define	MWD_PAN_AUTO			MWSFD_PAN_AUTO

/***************************************************************************
 *      Process MACRO
 ***************************************************************************/
/* Convert mwGetTime()'s return-value to second	*/
#define	MWSFD_TIME2SEC(ncount, tscale)		((ncount) / (tscale))

/* Convert mwGetTime()'s return-value to milli-second	*/
#define	MWSFD_TIME2MSEC(ncount, tscale)	((1000 * (ncount)) / (tscale))

/* ピクチャレイヤユーザデータ用のバッファサイズ計算 */
/* Calculate buffer size of user data in picture layer */
#define MWSFD_SIZE_PICUSRBUF(npool, size1pic)	(((npool) + 4) * (size1pic))

/* mwPlyStartIpicture()用の最大BPS指定値マクロ */
/* 引数は最大ピクチャデータサイズ[byte]  */
#define MWSFD_CALC_MAX_BPS_SEEKIPIC(max_picdata_size)	((max_picdata_size + 4096) * 16)

/* ビデオPTS処理用ワークサイズ計算 */
#define MWSFD_CALC_WORK_VPTS(picnum)					((picnum) * 16 + 4)

/* [for Compatibility] */
#define	MWD_PLY_TIME2SEC(ncount, tscale)	MWSFD_TIME2SEC((ncount), (tscale))
#define	MWD_PLY_TIME2MSEC(ncount, tscale)	MWSFD_TIME2MSEC((ncount), (tscale))

/***************************************************************************
 *      Enum declaration
 ***************************************************************************/
/*	Status of MWPLY handles		*/
typedef	enum {
	MWSFD_STAT_STOP			= 0,		/*	stoped						*/
	MWSFD_STAT_PREP			= 1,		/*	preparing					*/
	MWSFD_STAT_PLAYING		= 2,		/*	playing						*/
	MWSFD_STAT_PLAYEND		= 3,		/*	end of playing				*/
	MWSFD_STAT_ERROR		= 4,		/*	error was occured			*/

	/* [for Compatibility] */
	MWE_PLY_STAT_STOP		= 0,		/*	stoped						*/
	MWE_PLY_STAT_PREP		= 1,		/*	preparing					*/
	MWE_PLY_STAT_PLAYING	= 2,		/*	playing						*/
	MWE_PLY_STAT_PLAYEND	= 3,		/*	end of playing				*/
	MWE_PLY_STAT_ERROR		= 4,		/*	error was occured			*/

	MWSFD_STAT_END,
	MWSFD_STAT_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdStat, MWE_PLY_STAT;

/* デコードサーバ */
/* Decode Server  */
typedef enum {
	MWSFD_DEC_SVR_IDLE		= 0,	/* メインの余り時間でデコードする。 */
									/* Decode in idel thread            */
	MWSFD_DEC_SVR_MAIN		= 1,	/* メイン処理内でデコードする。		*/
									/* Decode in main thread            */

	MWSFD_DEC_SVR_END,
	MWSFD_DEC_SVR_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdDecSvr;

/* デコード結果バッファデータ形式	*/
/* Decoder's output buffer data format	*/
typedef enum {
    MWSFD_BUFFMT_DEFAULT		= 0,	/* Default format		*/
    MWSFD_BUFFMT_MB_YCC420		= 1,	/* YCC 4:2:0 Macroblock */
    MWSFD_BUFFMT_MB_ARGB8888	= 2,	/* RGB 32bit Macroblock */
    MWSFD_BUFFMT_PLN_YCC420		= 3,	/* YCC 4:2:0 Plane      */
	/* [for Compatibility] */
    MWE_PLY_BUFFMT_MB_YCC420    = 1,	/* YCC 4:2:0 Macroblock */
    MWE_PLY_BUFFMT_MB_ARGB8888  = 2,	/* RGB 32bit Macroblock */
    MWE_PLY_BUFFMT_PLN_YCC420   = 3,	/* YCC 4:2:0 Plane      */

    MWSFD_BUFFMT_END,
	MWSFD_BUFFMT_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdBufFmt, MWE_PLY_BUFFMT;

/* ルミナンス合成の透過キー				*/
/* Luminance alpha's transparence key	*/
typedef enum {
	MWSFD_LUMIKEY_BLACK		= 0,	/* Black key transparence */
	MWSFD_LUMIKEY_WHITE		= 1,	/* White key transparence */

	MWSFD_LUMIKEY_END,
	MWSFD_LUMIKEY_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdLumiKey;

/* フレームタイプ	*/
/* Frame type		*/
typedef enum {
	MWSFD_FRMTYPE_UNKNOWN		= 0,	/* Unknown */
	MWSFD_FRMTYPE_PROGRESSIVE	= 1,	
	MWSFD_FRMTYPE_INTERLACE		= 2,

	MWSFD_FRMTYPE_END,
	MWSFD_FRMTYPE_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdFrmType;

/* ピクチャタイプ	*/
/* Picture type		*/
typedef enum {
    MWSFD_PTYPE_I			= 1,	/* I Picture */
    MWSFD_PTYPE_P			= 2,	/* P Picture */
    MWSFD_PTYPE_B			= 3,	/* B Picture */
    MWSFD_PTYPE_D			= 4,	/* D Picture */
	/* [for Compatibility] */
    MWE_PLY_PTYPE_I			= 1,	/* I Picture */
    MWE_PLY_PTYPE_P			= 2,	/* P Picture */
    MWE_PLY_PTYPE_B			= 3,	/* B Picture */
    MWE_PLY_PTYPE_D			= 4,	/* D Picture */

    MWSFD_PTYPE_END,
	MWSFD_PTYPE_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdPtype, MWE_PLY_PTYPE;

/* フレームのYUV変換モード			*/
/* YUV conversion mode of the frame	*/
typedef enum {
	MWSFD_YUVMODE_UNKNOWN			= 0,	
	MWSFD_YUVMODE_ITU_R_BT_601		= 1,	/* [standard] It's CCIR601	*/
	MWSFD_YUVMODE_CSC				= 2,	/* Color space compensation	*/
	MWSFD_YUVMODE_BASIC_YCBCR		= 3,	/* [n/a] Basic YCbCr		*/
	MWSFD_YUVMODE_HIGH_CONTRAST		= 4,	/* [n/a] High contrast		*/
	MWSFD_YUVMODE_RAW_YUV			= 5,	/* [n/a] Raw YUV data		*/
	MWSFD_YUVMODE_HSYUV				= 6,	/* Simple coefficient color space conversion */

	MWSFD_YUVMODE_END,
	MWSFD_YUVMODE_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdYuvMode;

/* フレーム取得同期モード			*/
/* Sync mode of mwPlyGetCurFrm()	*/
typedef enum {
	MWSFD_FRMSYNC_TIME		= 0,	/* Sync with time	*/
	MWSFD_FRMSYNC_NONE		= 1,	/* No sync			*/

	MWSFD_FRMSYNC_END,
	MWSFD_FRMSYNC_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdFrmSync;

/* ビデオコーデック			*/
/* Codec detail infomation	*/
typedef enum MwsfdVideoCodec {
    MWSFD_VIDEOCODEC_UNKNOWN     = 0,    /* 未知 */
    MWSFD_VIDEOCODEC_M1V         = 1,    /* MPEG-1 Video, Sofdec Video */
    MWSFD_VIDEOCODEC_M2V         = 2,    /* MPEG-2 Video */
    MWSFD_VIDEOCODEC_M4V         = 3,    /* MPEG-4 Video */
    MWSFD_VIDEOCODEC_H264        = 4,    /* H.264 */

    MWSFD_VIDEOCODEC_END,
	MWSFD_VIDEOCODEC_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdVideoCodec;

/* ディザモード	*/
/* Dither mode	*/
typedef enum MwsfdDitherMode {
	MWSFD_DITHERMODE_AUTO	= 0,    /* 自動 */
	MWSFD_DITHERMODE_ON		= 1,    /* ディザ有り */
	MWSFD_DITHERMODE_OFF	= 2,    /* ディザ無し */

	MWSFD_DITHERMODE_END,
	MWSFD_DITHERMODE_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdDitherMode;

/***************************************************************************
 *      Data type declaration
 ***************************************************************************/
/*	MWPLY handle	*/
typedef void* MWPLY;

/* フレームプールハンドル */
typedef CriSint32 MwsfdFrmpoolHn;

/* Sofdecの初期化パラメータ構造体							*/
/* Parameter structure of Sofdec initialization function	*/
typedef struct {
	CriFloat32		vhz;			/* 垂直同期周波数[Hz]						*/
								/* Vsync frequency[Hz]						*/
	CriSint32		disp_cycle;		/* (未使用) 表示更新周期[v]				 	*/
								/* (Not in use) Display update cycle[v]		*/
	CriSint32		disp_latency;	/* (未使用) 表示レイテンシ[v]				*/
								/* (Not in use) Display latency[v]			*/
	MwsfdDecSvr	dec_svr;		/* デコードサーバ				 			*/
								/* Decode Server							*/
	CriSint32		rsv[4];			/* 予約 (全て0を設定して下さい) 			*/
								/* Reserved(Please set 0 in all of area)	*/
} MwsfdInitPrm, MWS_PLY_INIT_SFD;

/* ハンドル生成パラメータ構造体				*/
/* Parameter structure of handle creation	*/
typedef struct {
	CriSint32	ftype;				/* 再生するストリームの種別 				*/
								/* File type								*/
	CriSint32	max_bps;			/* 最大のビットストリーム量　(bit/sec)		*/
								/* Maximum number of bit per second			*/
	CriSint32	max_width;			/* 再生画像サイズの最大幅					*/
								/* Maximum width of video stream			*/
	CriSint32	max_height;			/* 再生画像サイズの最大高さ					*/
								/* Maximum height of video stream			*/
	CriSint32	nfrm_pool_wk;		/* システム領域の有効フレームプール数。		*/
								/* 通常は(2)の値を指定する。これは参照		*/
								/* フレームプール枚数を含まない数。			*/
								/* フレーム落ちが発生した場合は、この値を	*/
								/* 増やしてください。						*/
								/* Number of effective frame pools in 		*/
								/* system memory. Normaly this number is 2.	*/
								/* this number dosen't contain number of 	*/
								/* reference frame pools. If frame is 		*/
								/* droped,you have to increase this number.	*/
	CriSint32	max_stm;			/* 同時読み込みストリーム数(ADX含む)		*/
								/* 値が0の場合はデフォルト値(1)とみなす。	*/
								/* The number of maximum streams playing 	*/
								/* at the same time. 						*/
								/* This number include ADXT/ADXF streams.	*/
								/* If the value is zero then assume 1.		*/
	CriSint8	*work;				/* ワーク領域								*/
								/* Address of working area					*/
	CriSint32	wksize;				/* ワーク領域サイズ							*/
								/* Size of working area						*/
	CriSint32  compo_mode;			/* 合成モード								*/
								/* Composition mode							*/
	MwsfdBufFmt		buffmt;		/* デコード出力フレームバッファデータ形式	*/
								/* 値が0の場合はデフォルト形式とみなす。	*/
								/* Buffer format of decoder	output			*/
								/* If the value is 0 then assume defualt.	*/

	/* 以下のメンバは特定のターゲットでしか使用しない。       */
	/* The following members are used with a specific target. */
	CriSint32	outer_frmpool_num;	/* 外部フレームプール数。これは参照フレーム	*/
								/* プール枚数を含めた値。					*/
								/* Number of outer frame pools. 			*/
								/* this number contain number of reference	*/
								/* frame pools.								*/
	CriSint32	outer_frmpool_size;	/* 外部フレームプール用バッファサイズ		*/
	                            /* （１フレーム分）							*/
								/* Size of outer frame pool.                */
								/* (Size for one frame)                     */
	CriUint8	**outer_frmpool_ptr;/* 外部フレームプール用バッファポインタ配列 */
	                            /* へのポインタ								*/
								/* Pointer to pointer array for outer frame */
								/* pools.									*/
	MwsfdFrmpoolHn	frmpool_hn;	/* フレームプールハンドル                   */
	
	CriSint32	rsv[1];			/* 予約 (全て0を設定して下さい) 			*/
								/* Reserved(Please set 0 in all of area)	*/
} MwsfdCrePrm, MWS_PLY_CPRM_SFD;

/* 字幕データパラメータ	*/
/* Subtitle parameter		*/
typedef struct {
	CriBool	update;			/* update data flag								*/
	CriSint32	datasize;		/* size of data [byte]							*/
	CriSint32	tunit;			/* time unit									*/
	CriSint32	intime;			/* start display time.  sec = intime / tunit.	*/
	CriSint32	durtime;		/* duration of display time. sec = intime / tunit.	*/
} MwsfdSbtPrm;

/* MPEG-2フレーム情報		*/
/* MPEG-2 detail infomation	*/
typedef struct {
	CriSint64				pts;						/* PES   :PTS 33bit (-1:PTS情報なし) */
	CriSint32				picture_structure;			/* PIC_C :ピクチャ構成 0:予約 1:top-field, 2:bottom-f. 3:frame */
	CriSint32				chroma_format;				/* SEQ_X :色差フォーマット 0:予約 1:420 2:422 3:444	*/
	CriSint32				bit_rate;					/* SEQ,X :ビットレート 18+12=30bit */
	CriSint32				vbv_buffer_size;			/* SEQ,X :VBVバッファサイズ 10+8=18bit */
	CriSint16				display_horizontal_size;	/* SEQ_D :表示幅 14bit */
	CriSint16				display_vertical_size;		/* SEQ_D :表示高さ 14bit */
	CriSint8				progressive_frame;			/* PIC_C :0:インタレース 1:プログレッシブ */
	CriSint8				top_field_first;			/* PIC_C :トップフィールド優先フラグ */
	CriSint8				repeat_first_field;			/* PIC_C :フィールド反復表示フラグ */
	CriSint8				aspect_ratio_information;	/* SEQ   :アスペクト比情報 4bit */
	CriSint8				constrained_parameters_flag;/* SEQ   :制約パラメータフラグ */
	CriSint8				frame_rate_extension_n;		/* SEQ_X :フレームレート拡張n 2bit */
	CriSint8				frame_rate_extension_d;		/* SEQ_X :フレームレート拡張d 5bit */
	CriUint8				profile_and_level_indication;/*SEQ_X :プロファイル、レベル 8bit */
	CriSint8				progressive_sequence;		/* SEQ_X :プログレッシブシーケンスフラグ */
	CriSint8				low_delay;					/* SEQ_X :低遅延フラグ */
	CriSint8				video_format;				/* SEQ_D :ビデオフォーマット 3bit */
	CriSint8				colour_description;			/* SEQ_D :色情報記録フラグ 1bit */
	CriUint8				colour_primaries;			/* SEQ_D :情報源原色の色度座標 8bit */
	CriUint8				transfer_characteristics;	/* SEQ_D :変換特性 8bit */
	CriUint8				matrix_coefficients;		/* SEQ_D :マトリクス係数 8bit */
	CriSint16				num_slices;					/* slice :スライス数 */
} MwsfdFrmM2v;

/* MPEG-4フレーム情報		*/
/* MPEG-4 detail infomation	*/
typedef struct {
	CriSint64				pts;						/* PES   :PTS 33bit (-1:PTS情報なし) */
	void				*header;					/* MPEG-4ビデオの各種ヘッダ情報 */
} MwsfdFrmM4v;

/* コーデック固有のフレーム情報		*/
/* Codec-specific frame information	*/
typedef struct {
	MwsfdVideoCodec		videocodec;		/* ビデオコーデック */
	union {
		MwsfdFrmM2v		frmm2v;			/* MPEG-2フレーム情報（MPEG-1, SofdecVideoでも有効） */
		MwsfdFrmM4v		frmm4v;			/* MPEG-4フレーム情報 */
	} u;
} MwsfdFrmCodec;

/* フレーム情報構造体 */
/* Frame Information  */
typedef struct {
	CriUint8			*bufadr;			/* デコード結果バッファアドレス		*/
										/* Decoder's output buffer address	*/
	CriSint32			frmid;				/* Frame ID */
	MwsfdBufFmt		buffmt;				/* デコード結果バッファデータ形式	*/
										/* Decoder's output buffer format	*/
	CriSint32			width;				/* 横ピクセル数						*/
										/* Width by the pixel				*/
	CriSint32			height;				/* 縦ピクセル数						*/
										/* Height by the pixel				*/
	CriSint32			disp_width;			/* 表示横ピクセル数					*/
										/* Effective Width by the pixel		*/
	CriSint32			disp_height;		/* 表示縦ピクセル数					*/
										/* Effective Height by the pixel	*/
	CriSint32			x_mb;				/* 横マクロブロック数				*/
										/* Width by the macroblock			*/
	CriSint32			y_mb;				/* 縦マクロブロック数				*/
										/* Height by the macroblock			*/
	MwsfdPtype		ptype;				/* ピクチャタイプ					*/
										/* Picture type						*/
	CriSint32			fps;				/* フレームレート[fps * 1000]		*/
										/* Frame rate [fps * 1000]			*/
	CriSint32			fno;				/* フレーム番号						*/
										/* Count of frames					*/
	CriSint32			time;				/* 表示時刻							*/
										/* Time of disp						*/
	CriSint32			tunit;				/* 表示時刻単位						*/
										/* Unit of display time				*/
	CriSint32			concat_cnt;			/* 連結処理回数						*/
										/* Count of concatenation			*/
	CriSint32			fno_per_file;		/* ファイル毎のフレーム番号			*/
										/* Count of frames per file			*/
	CriSint32			time_per_file;		/* ファイル毎の再生時刻				*/
										/* Playtime per file				*/
	CriSint32			errcnt;				/* データエラー発生回数				*/
										/* Count of data error				*/
	CriSint32			rcvcnt;				/* データエラー回復回数				*/
										/* Count of error recovery			*/
	void			*usrdatptr;			/* Picture layer user data address	*/
	CriSint32			usrdatsize;			/* Picture layer user data size		*/
	MwsfdFrmType	frmtype;			/* フレームタイプ 					*/
										/* Frame type		 				*/

	MwsfdYuvMode	yuvmode;			/* YUV変換モード					*/
										/* YUV conversion mode				*/
	CriUint32			zmin;				/* Minimum dynamic range for Z movie */
	CriUint32			zmax;				/* Maximum dynamic range for Z movie */

	MwsfdFrmCodec	frmcodec;			/* コーデック固有のフレーム情報		*/

	void			*sud_ptr;
	CriSint32			sud_size;
} MwsfdFrmObj, MWS_PLY_FRM;

/* 再生情報構造体 		*/
/* Playback information	*/
typedef struct {
	CriSint32		drop_frm_cnt;		/* Count of drop frame 						*/
	CriSint32		skip_dec_cnt;		/* Count of skip decode						*/
	CriSint32		skip_disp_cnt;		/* Count of skip frame was not obtatined	*/
	CriSint32		skip_emptyb_cnt;	/* Count of skip empty-Bpicture 			*/
	CriSint32		no_supply_cnt;		/* Count of data supply shortage			*/
	CriFloat32		average_fps;
} MwsfdPlyInf;

/* ファイルヘッダ情報		*/
/* File header information	*/
typedef struct {
	CriBool			playable;		/* TRUE: playable, FALSE: not playable	*/
	CriSint32		ftype;			/* File type (MWSFD_FTYPE_ )			*/
	CriSint32		width;			/* Width by the pixel					*/
	CriSint32		height;			/* Height by the pixel					*/
	CriSint32		disp_width;		/* Effective Width by the pixel			*/
	CriSint32		disp_height;	/* Effective Height by the pixel		*/
	CriSint32		fps;			/* Frame rate [fps * 1000]				*/
	CriSint32		frmnum;			/* Total video frames					*/
	CriSint32		fxtype;			/* F/X type	(MWSFD_COMPO_ )				*/
	CriSint32		ave_bps;		/* (Not Support Yet!!!)					*/

	/* <Elementary stream in SFD file> */
	CriSint32		num_video_ch;	/* the number of video channel stream	*/
	CriSint32		num_audio_ch;	/* the number of audio channel stream 	*/

	/* <Default audio channel stream information>	*/
	CriSint32		a_sfreq;		/* Audio: sampling freqency 			*/
	CriSint32		a_nch;			/* Audio: 0:NONE 1:MONO: 2:STEREO		*/

	/* <Video additional information>	*/
	CriSint32		max_picdata_size;

	CriUint64		file_size;		/* [Byte] */
} MwsfdHdrInf;

/*	Malloc Function	*/
typedef void *(*MwsfdMallocFn)(void *obj, CriUint32 size);

/*	Free Function	*/
typedef void (*MwsfdFreeFn)(void *obj, void *ptr);

/* YCbCrプレーン情報構造体 */
/* YCbCr Plane Information */
typedef struct {
	CriUint8 		*y_ptr;		/* Y  Buffer Pointer */
	CriUint8 		*cb_ptr;	/* Cb Buffer Pointer */
	CriUint8 		*cr_ptr;	/* Cr Buffer Pointer */
	CriSint32		y_width;	/* Y  Buffer width   */
	CriSint32		cb_width;	/* Cb Buffer width   */
	CriSint32		cr_width;	/* Cr Buffer width   */
} MwsfdYccPlane, MWS_PLY_YCC_PLANE;

/* I picture シーク再生用情報構造体     */
/* I pciture seek playback information  */
typedef struct {
	CriUint32		offset;		/* Byte offset from top of the file */
	CriSint32		fno;		/* Frame number in an I picture movie */
} MwsfdSeekIpic;

/* Ｚ変換用コールバック関数 */
/* ※Ｚバッファ用変換テーブルをアプリケーションが自作するための関数。
 * 	orgtbl : デコード結果から、Ｚ値(0x00000000～0x7FFFFFFF)への変換テーブルポインタ。
 *           テーブル実体はMWPLYハンドルの中にある。
 *  znear, zfar : ニア／ファー
 *  ztbl   : Ｚ値(0x00000000～0x7FFFFFFF)からＺバッファ内容への変換テーブルポインタ。
 *           関数内でCriSint16かCriSint32にキャストして使用する。
 *           テーブル実体はMWPLYハンドルの中にある。
 */
typedef void (*MwsfdMakeZTblCbFn)(CriUint32 *orgtbl, CriFloat32 znear, CriFloat32 zfar, void *ztbl);

/* 流量カウンタ */
/* Flow counter	*/
typedef struct {
	CriSint64			supplied;		/* 供給サイズ[byte]                                     */
									/* supplied data size [byte]                            */
	CriSint64			consumed;		/* 消費サイズ[byte] (破棄サイズを含む)                  */
									/* consumed data size [byte] (including discarded size) */
	CriSint64			discarded;		/* 破棄サイズ[byte]                                     */
									/* discarded data size [byte]                           */
} MwsfdFlowCnt;

/* 流量情報構造体 	*/
/* Flow information	*/
typedef struct {
	MwsfdFlowCnt	system;		/* system decoder flow counter                                 */
	MwsfdFlowCnt	video;		/* video decoder flow counter ("discarded" is not implemented) */
	MwsfdFlowCnt	audio;		/* audio decoder flow counter ("discarded" is not implemented) */
	MwsfdFlowCnt	adxtin;		/* adxt input sj flow counter ("discarded" is not implemented) */
} MwsfdFlowInf;

/* 外部クロック関数 */
typedef void (*MwsfdExtClockFn)(void *usrobj, CriSint32 *time, CriSint32 *tunit);

/* フレーム変換完了コールバック関数 */
typedef void (*MwsfdCnvFrmCbFn)(void *usrobj, CriSint32 frmid);





/* Maximum number of Audio channel in a movie file */
#define MWSFD_MAX_AUDIO_CH		(32)

/* Audio Codec */
typedef enum {
	MWSFD_AUDIO_CODEC_NONE		= 0,	/* NONE							*/
	MWSFD_AUDIO_CODEC_SFA		= 1, 	/* Sofdec Audio					*/
	MWSFD_AUDIO_CODEC_MPA		= 2,	/* MPEG Audio					*/
	MWSFD_AUDIO_CODEC_AC3		= 3,	/* AC3							*/
	MWSFD_AUDIO_CODEC_AHX		= 4,	/* AHX							*/
	MWSFD_AUDIO_CODEC_AIX		= 5,	/* AIX (Multi Channel Audio)	*/

	MWSFD_AUDIO_CODEC_END
} MwsfdAudioCodec;

/* Audio Codec Info */
typedef struct {
	CriBool				exist_flag;
	MwsfdAudioCodec		audio_codec;
	CriSint32				num_channel;
	CriSint32				sampling_rate;
} MwsfdAudioInfo;

/* Audio Detail Info in Sofdec Header */
typedef struct {
	CriBool			used;
	MwsfdAudioInfo	audio_info[MWSFD_MAX_AUDIO_CH];
} MwsfdHdrAudioDetail;


/***************************************************************************
 *      Function Declaration
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================================*
 *		基本再生処理
 *      Basic functions
 *=========================================================================*/
/* MWSFD(Sofdec)ライブラリの初期化	*/
/* Initialization of Sofdec library	*/
void CRIAPI mwPlyInitSfdFx(MwsfdInitPrm *iprm);

/* MWSFD(Sofdec)ライブラリの終了	*/
/* Termination of Sofdec library 	*/
void CRIAPI mwPlyFinishSfdFx(void);

/* 作業領域サイズの計算										*/
/* Calculation of working area size from create parameter	*/
CriSint32 CRIAPI mwPlyCalcWorkCprmSfd(MwsfdCrePrm *cprm);

/* MWSFDハンドルの生成				*/
/* Create MWPLY handle for Sofdec	*/
MWPLY CRIAPI mwPlyCreateSofdec(MwsfdCrePrm *cprm);

/* 再生ハンドルの消去						*/
/* Destroy MWPLY handle						*/
void CRIAPI mwPlyDestroy(MWPLY mwply);

/* 再生開始（ファイルからの再生）								*/
/* Start playing by file name 									*/
void CRIAPI mwPlyStartFname(MWPLY mwply, const CriChar8 *fname);

/* 再生停止									*/
/* Stop playing  							*/
void CRIAPI mwPlyStop(MWPLY mwply);

/* ハンドル状態の取得								*/
/* Get status of MWPLY handle						*/
MwsfdStat CRIAPI mwPlyGetStat(MWPLY mwply);

/* フレームの取得           */
/* Get frame buffer pointer */
void CRIAPI mwPlyGetCurFrm(MWPLY mwply, MwsfdFrmObj *frm);

/* フレームの解放 			*/
/* Release frame buffer		*/
void CRIAPI mwPlyRelCurFrm(MWPLY mwply);

/* 再生サンプル数の取得														*/
/* Get playing time															*/
/* If playing movie is 30 fps, *tscale is 30 and *ncount is total number of */
/*  displayed frames														*/
void CRIAPI mwPlyGetTime(MWPLY mwply, CriSint32 *ncount, CriSint32 *tscale);

/* ポーズ／ポーズ解除									*/
/* Set pause switch.  sw=0(Continue), 1(Pause)			*/
void CRIAPI mwPlyPause(MWPLY mwply, CriSint32 sw);

/* ポーズ状態の取得 */
/* Get pause ON/OFF	*/
CriBool CRIAPI mwPlyIsPause(MWPLY mwply);

/* コマ送り   */
/* Step frame */
void CRIAPI mwPlyStepFrame(MWPLY mwply);

/* <NEW> Get frame */
/* <新> フレームの取得 */
/* [入力]
 *    mwply    :MWPLYハンドル
 * [出力]
 *    frmid    :取得できたフレームのフレームID（変数実体はユーザが定義）
 *    frm      :取得できたフレームのフレーム情報構造体（変数実体はユーザが定義）
 * [関数値]
 *    TRUE     : フレームが取得できた
 *    FALSE    : フレームが取得できなかった
 * [備考]
 * ・mwPlyGetCurFrm関数/mwPlyRelCurFrm関数との併用は禁止。
 * ・表示時刻になっているフレームを取得する。
 * ・同一フレームは一度だけ取得できる。
 * ・古いフレームを解放しなくても、新しいフレームを取得できる。
 * ・フレーム内容はmwPlyRelFrm関数で解放するか、mwPlyStop関数実行まで保持。
 * ・mwPlyGetFrm関数とmwPlyRelFrm関数は同一スレッド内で実行すること。
 */
CriBool CRIAPI mwPlyGetFrm(MWPLY mwply, CriSint32 *frmid, MwsfdFrmObj *frm);

/* <NEW> Release frame */
/* <新> フレームの解放 */
/* [入力]
 *    mwply    :MWPLYハンドル
 *    frmid    :解放するフレームのフレームID
 * [備考]
 * ・mwPlyGetCurFrm関数/mwPlyRelCurFrm関数との併用は禁止。
 * ・引数frmidで指定したフレームを解放する。
 * ・同じフレームIDに対しての解放は１度のみ。２度目以降はエラーコールバック。
 * ・フレームを取得した順番と解放する順番は一致しなくてよい。
 * ・全ての取得済みフレームを解放しない限り、MWPLYハンドルはPLAYEND状態にならない。
 * ・mwPlyGetFrm関数とmwPlyRelFrm関数は同一スレッド内で実行すること。
 */
void CRIAPI mwPlyRelFrm(MWPLY mwply, CriSint32 frmid);

/*=========================================================================*
 *		拡張ファイル再生
 *		Extended File Playback
 *=========================================================================*/
/* FID 指定による再生の開始 				*/
/* Start AFS palyback by specified file ID	*/
void CRIAPI mwPlyStartAfs(MWPLY mwply, CriSint32 patid, CriSint32 fid);

/* パックファイル内SFDデータの再生開始 */
/* Start playing SFD data in packed file */
/* [Inputs  ] mwply			: MWPLY handle
 *			  fname			: File name
 *			  offset_sct	: File offset from top of the file as a sector
 *			  range_sct		: File range as a sector					 */
void CRIAPI mwPlyStartFnameRange(MWPLY mwply, const CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);

/* SJによる再生の開始								*/
/* Start playing by stream joint					*/
void CRIAPI mwPlyStartSj(MWPLY mwply, SJ sji);

/* データ供給終了の通知(SJ再生用) */
/* Notify the end of data supply, for mwPlyStartSj() */
void CRIAPI mwPlyTermSupply(MWPLY mwply);

/* アドレス指定メモリによる再生開始										*/
/* Start playing by memory												*/
void CRIAPI mwPlyStartMem(MWPLY mwply, const void *addr, CriSint32 len);
/* アドレス指定メモリによるループ再生開始								*/
/* Start looping playback by memory										*/
void CRIAPI mwPlyStartMemLp(MWPLY mwply, const void *data, CriSint32 dtlen);

/* メモリ連結再生の開始 */
/* - 全ての連結ファイルを登録し終わったら、mwPlyReleaseMemSeamless関数を呼ぶ */
/* - mwPlyReleaseMemSeamless関数を呼ばない場合は、再生終了状態にならない */
void CRIAPI mwPlyStartMemSeamless(MWPLY mwply);
/* メモリ連結再生の登録 */
/* - mwPlyStartMemSeamless関数の呼出し後に有効 */
/* - 同時に登録できるのは１６個まで */
void CRIAPI mwPlyEntryMemSeamless(MWPLY mwply, const void *data, CriSint32 dtlen);
/* メモリ連結再生の登録終了宣言 */
void CRIAPI mwPlyReleaseMemSeamless(MWPLY mwply);
/* メモリ連結再生の登録数取得 */
CriSint32 CRIAPI mwPlyGetNumMemSeamless(MWPLY mwply);

/*=========================================================================*
 *		シームレスループ再生
 *		Seamless Loop Playback
 *=========================================================================*/
/* シームレスループ再生の開始	*/
/* Start semaless loop playback	*/
void CRIAPI mwPlyStartFnameLp(MWPLY mwply, const CriChar8 *fname);

/* シームレスループ再生の解除		*/
/* Release semaless loop playback	*/
void CRIAPI mwPlyReleaseLp(MWPLY mwply);

/* FID 指定によるシームレスループ再生の開始	  			*/
/* Start semaless loop playback by specified file ID	*/
void CRIAPI mwPlyStartAfsLp(MWPLY mwply, CriSint32 patid, CriSint32 fid);

/* パックファイル内SFDデータのシームレスループ再生の開始	*/
/* Start looping playing SFD data in packed file */
void CRIAPI mwPlyStartFnameRangeLp(MWPLY mwply, const CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);

/*=========================================================================*
 *		シームレス連続再生
 *		Seamless Continuous Playback
 *=========================================================================*/
/* シームレス連続再生ファイルの登録				*/
/* Entry file of Seamless continuous playback	*/
void CRIAPI mwPlyEntryFname(MWPLY mwply,const CriChar8 *fname);

/* シームレス連続再生の開始				*/
/* Start seamless continuous playback	*/
void CRIAPI mwPlyStartSeamless(MWPLY mwply);

/* シームレス連続再生の解除				*/
/* Release seamless continuous playback	*/
void CRIAPI mwPlyReleaseSeamless(MWPLY mwply);

/*	シームレスループ再生の設定	*/
/*	Set semaless loop play   	*/
void CRIAPI mwPlySetSeamlessLp(MWPLY mwply, CriSint32 flg);

/* 現在登録されているファイル数の取得			*/
/* Get number of file entried seamless loop 	*/
CriSint32 CRIAPI mwPlyGetNumSlFiles(MWPLY mwply);

/* シームレス連続再生に登録してあるファイル名の取得	*/
/* Get file name of entried seamless				*/
const CriChar8 * CRIAPI mwPlyGetSlFname(MWPLY mwply, CriSint32 stm_no);

/* シームレス連続再生ファイルの登録 (AFS)			*/
/* Entry file of Seamless continuous playback (AFS)	*/
void CRIAPI mwPlyEntryAfs(MWPLY mwply, CriSint32 patid, CriSint32 fid);

/* パックファイル内SFDデータのシームレス連続再生登録				*/
/* Entry SFD data in packed file to seamless continuous playback	*/
void CRIAPI mwPlyEntryFnameRange(MWPLY mwply, const CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);

/*=========================================================================*
 *		オーディオ設定
 *		Audio Configuration
 *=========================================================================*/
/* オーディオ出力ボリュームの設定							*/
/* Set Output Volume (vol= 0 to -960)						*/
void CRIAPI mwPlySetOutVol(MWPLY mwply, CriSint32 vol);

/* オーディオ出力ボリュームの取得				*/
/* Get Output Volume (return value= 0 to -960)	*/
CriSint32 CRIAPI mwPlyGetOutVol(MWPLY mwply);

/* オーディオ出力パンポットの設定										*/
/* Set output panpot c=0(Mono/Left),1(Right)							*/
/* p = +15(Right),0(Center),-15(Left),-128(Auto)						*/
void CRIAPI mwPlySetOutPan(MWPLY mwply, CriSint32 chno, CriSint32 pan);

/* オーディオ出力パンポットの取得								*/
/* Get output panpot c=0(Mono/Left),1(Right)					*/
CriSint32 CRIAPI mwPlyGetOutPan(MWPLY mwply, CriSint32 chno);

/* オーディオストリームチャネル数の取得		*/
/* Get the number of audio channel streams	*/
CriSint32 CRIAPI mwPlyGetNumAudioCh(MWPLY mwply);

/* オーディオストリームチャネルの設定	*/
/* Set audio stream channel 			*/
void CRIAPI mwPlySetAudioCh(MWPLY mwply, CriSint32 ch);

/* オーディオストリームチャネルの取得 (-1: 未定または再生なし)	*/
/* Get audio stream channel (-1: pending or out of work) */
CriSint32 CRIAPI mwPlyGetAudioCh(MWPLY mwply);

/*=========================================================================*
 *		各種オーディオ設定
 *		Various Audio Configuration
 *=========================================================================*/
/* ボイストラック再生用ワークバッファサイズの取得 */
/* Get size of work buffer for voice track in movie */
CriSint32 CRIAPI mwPlyGetVoiceWorkSize(void);

/* ボイストラック再生機能の追加 */
/* Attach voice track playback function to MWPLY */
void CRIAPI mwPlyAttachVoice(MWPLY mwply, void *wkptr, CriSint32 wksize);

/* ボイストラック再生機能の分離 */
/* Detach voice track playback function from MWPLY */
void CRIAPI mwPlyDetachVoice(MWPLY mwply);

/* ボイストラックチャネル番号の設定 */
/* Set channel number of voice track */
void CRIAPI mwPlySetVoiceCh(MWPLY mwply, CriSint32 ch);

/* AHX再生用ワークバッファサイズの取得 */
/* Get size of work buffer for AHX in movie */
CriSint32 CRIAPI mwPlyGetAhxWorkSize(void);

/* AHX再生機能の追加 */
/* Attach AHX to MWPLY */
void CRIAPI mwPlyAttachAhx(MWPLY mwply, void *wkptr, CriSint32 wksize, CriSint32 ch);

/* AHX再生機能の分離 */
/* Detach AHX from MWPLY */
void CRIAPI mwPlyDetachAhx(MWPLY mwply);

/* AHXチャネル番号の設定 */
/* Set channel number of AHX */
void CRIAPI mwPlySetAhxCh(MWPLY mwply, CriSint32 ch);

/*=========================================================================*
 *		サブタイトル（字幕）
 *		Subtitle
 *=========================================================================*/
/* 字幕データの取得 */
/* Get subtitle data */
CriSint32 CRIAPI mwPlyGetSubtitle(MWPLY mwply, CriUint8 *bufptr, CriSint32 bufsize, MwsfdSbtPrm *sbtprm);

/* 字幕データのチャネル設定		*/
/* Set channel of subtitle data	*/
void CRIAPI mwPlySetSubtitleCh(MWPLY mwply, CriSint32 chno);

/* サブタイトルチャネル数の取得 */
/* Get the number of subtitle channel */
CriSint32 CRIAPI mwPlyGetNumSubtitleCh(MWPLY mwply);

/* 字幕データのシフトタイム設定		*/
/* Set shift time of subtitle data	*/
void CRIAPI mwPlySetSubtitleScount(MWPLY mwply, CriSint32 scount);

/*=========================================================================*
 *		フレーム変換設定
 *		FrameConversion Configuration
 *=========================================================================*/

/* 出力バッファサイズの設定 */
/* Set size of out buffer 	*/
void CRIAPI mwPlyFxSetOutBufPitchHeight(MWPLY mwply, CriSint32 pitch, CriSint32 height);
/* 出力バッファサイズの取得 */
/* Get size of out buffer 	*/
void CRIAPI mwPlyFxGetOutBufPitchHeight(MWPLY mwply, CriSint32 *pitch, CriSint32 *height);

/* [for Compatibility] */
void CRIAPI mwPlyFxSetOutBufSize(MWPLY mwply, CriSint32 width, CriSint32 height);
void CRIAPI mwPlyFxGetOutBufSize(MWPLY mwply, CriSint32 *width, CriSint32 *height);

/* 合成モードの設定 	*/
/* Set composition mode	*/
void CRIAPI mwPlyFxSetCompoMode(MWPLY mwply, CriSint32 mode);
/* 合成モードの取得 */
/* Get composition mode	*/
CriSint32 CRIAPI mwPlyFxGetCompoMode(MWPLY mwply);

/* ルミナンスキーパラメータの設定		*/
/* Set luminance key alpha parameter	*/
void CRIAPI mwPlyFxSetLumiPrm(MWPLY mwply, CriSint32 in_th, CriSint32 out_th, MwsfdLumiKey key);
/* ルミナンスキーパラメータの取得 		*/
/* Get luminance key alpha parameter	*/
void CRIAPI mwPlyFxGetLumiPrm(MWPLY mwply, CriSint32 *in_th, CriSint32 *out_th, MwsfdLumiKey *key);

/* ３値アルファ用パラメータ設定 */
/* Set alpha 3-value parameter */
void CRIAPI mwPlyFxSetAlp3Prm(MWPLY mwply, CriUint8 zero, CriUint8 half, CriUint8 full);
/* ３値アルファ用パラメータ取得 */
/* Get alpha 3-value parameter */
void CRIAPI mwPlyFxGetAlp3Prm(MWPLY mwply, CriUint8 *zero, CriUint8 *half, CriUint8 *full);

/*=========================================================================*
 *		Ｚムービ
 *		Z movie
 *=========================================================================*/
/* 16bit-Ｚバッファへの変換 		*/
/* Convert to 16bit depth Z-value	*/
void CRIAPI mwPlyFxCnvFrmZ16(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *zout);
/* 32bit-Ｚバッファへの変換 					*/
/* Convert to 32bit depth Z-value	*/
void CRIAPI mwPlyFxCnvFrmZ32(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *zout);

/* Ｚクリップの設定 */
/* Set Z-clip		*/
void CRIAPI mwPlyFxSetZclip(MWPLY mwply, CriFloat32 znear, CriFloat32 zfar);
/* Ｚクリップの取得 */
/* Get Z-clip		*/
void CRIAPI mwPlyFxGetZclip(MWPLY mwply, CriFloat32 *znear, CriFloat32 *zfar);

/* Ｚ変換テーブル作成コールバック関数の登録 */
void CRIAPI mwPlyFxSetCbFnMakeZTbl(MWPLY mwply, MwsfdMakeZTblCbFn func);

/*=========================================================================*
 *		情報取得
 *		Information
 *=========================================================================*/

/* F/X種別の取得 */
/* Get F/X type from SFD data */
CriSint32 CRIAPI mwPlyGetFxType(MWPLY mwply);

/* ファイル情報の取得			*/
/* Get file header information	*/
void CRIAPI mwPlyGetHdrInf(CriSint8 *buf, CriSint32 bsize, MwsfdHdrInf *hdrinf);

/* ヘッダ情報（オーディオ詳細）の取得	*/
/* Get audio detail header information	*/
void CRIAPI mwPlyGetHdrAudioDetail(CriSint8 *buf, CriSint32 bsize, MwsfdHdrAudioDetail *audio_detail);

/* 2008-10-02 */
/* ファイル名指定による ファイル情報 ヘッダ情報（オーディオ詳細）の取得	*/
/* Get header informations by filename	*/
CriBool CRIAPI mwPlyReadAndGetHdrInfs(const CriChar8 *fname,MwsfdHdrInf *hdrinf,MwsfdHdrAudioDetail *audio_detail);

/* 総フレーム数の取得 				*/
/* Get the number of movie frame	*/
CriSint32 CRIAPI mwPlyGetTotalFrmNum(MWPLY mwply);

/*=========================================================================*
 *		特殊設定
 *		Special Configuration
 *=========================================================================*/
/* Malloc／Free 関数の登録 */
/* Set memory allocation / free callback function */
void CRIAPI mwPlySetMallocFn(MwsfdMallocFn mallocfn, MwsfdFreeFn freefn, void *obj);

/* 再生速度の設定(有理数で指定) */
/* Set playback speed by rational number */
void CRIAPI mwPlySetSpeedRational(MWPLY mwply, CriSint32 speed_numerator, CriSint32 speed_denominator);

/* 再生速度の設定(標準速度 = 1000) */
/* Set playback speed (normal speed = 1000) */
void CRIAPI mwPlySetSpeed(MWPLY mwply, CriSint32 speed);

/* 同期モードの設定			*/
/* Set synchronization mode	*/
void CRIAPI mwPlySetSyncMode(MWPLY mwply, CriSint32 mode);
/* 同期モードの取得			*/
/* Get synchronization mode	*/
CriSint32 CRIAPI mwPlyGetSyncMode(MWPLY mwply);

/* タイマチャネルの設定 */
/* Set timer channel	*/
void CRIAPI mwPlySetTimerCh(CriSint32 chno);
/* タイマチャネルの取得 */
/* Get timer channel	*/
CriSint32 CRIAPI mwPlyGetTimerCh(void);

/* ピクチャレイヤのユーザデータ用バッファの追加 */
/* Attach buffer for user data in picture layer */
void CRIAPI mwPlyAttachPicUsrBuf(MWPLY mwply, void *bufptr, CriSint32 bufsize, CriSint32 size1pic);

/* フレーム取得同期モードの設定				*/
/* Set frame obtain synchronization mode	*/
void CRIAPI mwPlySetFrmSync(MWPLY mwply, MwsfdFrmSync frmsync);
/* フレーム取得同期モードの取得				*/
/* Get frame obtain synchronization mode	*/
MwsfdFrmSync CRIAPI mwPlyGetFrmSync(MWPLY mwply);

/* 外部クロック関数の登録
 * [入力]
 *    mwply             :MWPLYハンドル
 *    extclock_fn       :外部クロック関数
 *    extclock_max      :クロック値の最大値。(8ビットの場合0x000000FF, 32ビットの場合0xFFFFFFFF)
 *    extclock_usrobj   :ユーザオブジェクト(外部クロック関数の第1引数としてユーザに渡す)
 * [外部クロックの定義]
 * ムービの再生中かどうかに関わらず、常に単調増加する時刻(カウンタ)。
 * [備考]
 * 登録したクロック関数は、安定して単調増加する値を出力する必要がある。
 * 何らかのハードウェアから得られる生のクロック値でもよく、再生中にオーバフロー
 * することがあってもよいが、オーバフローが発生する周期は時刻更新を行う周期より
 * 十分に大きくなければならない。
 * ライブラリは前回クロック値との差分が負値となることでオーバフロー発生を検出する。
 * オーバフロー検出時は、クロック値差分に extclock_max + 1 を加算してオーバフロー
 * を補償する。
 */
void CRIAPI mwPlySetCbExtClock(MWPLY mwply, MwsfdExtClockFn extclock_fn, CriSint32 extclock_max, void *extclock_usrobj);

/* 準備フレーム数の設定 */
void CRIAPI mwPlySetNumPrepFrm(MWPLY mwply, CriSint32 prep_frm);

/* 準備フレーム数の取得 */
CriSint32 CRIAPI mwPlyGetNumPrepFrm(MWPLY mwply);

/*=========================================================================*
 *		デバッグ用
 *		Debugging
 *=========================================================================*/
/* Sofdecライブラリバージョン文字列の取得 */
/* Get Sofdec library version string */
const CriChar8 * CRIAPI mwPlyGetVersionStr(void);

/* 再生時間上限の設定（デフォルトは 9時間57分 0秒）			*/
/* Set playback limit time (default = 09-57-00(HH-MM-SS))	*/
void CRIAPI mwPlySetLimitTime(MWPLY mwply, CriSint32 sec);

/* 再生情報の取得 			*/
/* Get playback information	*/
void CRIAPI mwPlyGetPlyInf(MWPLY mwply, MwsfdPlyInf *plyinf);

/* デコードした全ピクチャ数の取得	*/
/* Get Number of Decoded Frames 	*/
CriSint32 CRIAPI mwPlyGetNumTotalDec(MWPLY mwply);

/* デコード済み保持フレーム数の取得	*/
/* Get Number of Pooled Frames 		*/
CriSint32 CRIAPI mwPlyGetNumDecPool(MWPLY mwply);

/* コマ落ちしたフレーム数の取得	*/
/* Get number of droped frame	*/
CriSint32 CRIAPI mwPlyGetNumDropFrm(MWPLY mwply);
/* デコード落ちしたフレーム数の取得				*/
/* Get number of frames was skipped to decode	*/
CriSint32 CRIAPI mwPlyGetNumSkipDec(MWPLY mwply);
/* 表示段のスキップ回数の取得					*/
/* Get number of frames was skipped to obtain	*/
CriSint32 CRIAPI mwPlyGetNumSkipDisp(MWPLY mwply);
/* スキップしたエンプティBピクチャ枚数の取得	*/
/* Get the number of skipped empty-B picture	*/
CriSint32 CRIAPI mwPlyGetNumSkipEmptyB(MWPLY mwply);

/* 流量情報の取得           */
/* Get flow information	    */
void CRIAPI mwPlyGetFlowInf(MWPLY mwply, MwsfdFlowInf *flowinf);

/* B-Pictureデコードのスキップ設定 	*/
/* Set B-Picture skip decode 		*/
void CRIAPI mwPlySetBpicSkip(MWPLY mwply, CriBool sw);
/* エンプティBピクチャのデコードスキップ設定 	*/
/* Set empty-B picture skip decode				*/
void CRIAPI mwPlySetEmptyBpicSkip(MWPLY mwply, CriBool sw);
/* P-Pictureデコードのスキップ設定 	*/
/* Set P-Picture skip decode 		*/
void CRIAPI mwPlySetPpicSkip(MWPLY mwply, CriBool sw);

/* ADXTハンドルの取得	*/
/* Get ADXT handle		*/
ADXT CRIAPI mwPlyGetAdxtHn(MWPLY mwply);
/* 入力ストリームジョイントの取得	*/
/* Get SJ of Input          		*/
SJ CRIAPI mwPlyGetInputSj(MWPLY mwply);
/* ビデオデコーダハンドルの取得	*/
/* Get Video decoder handle		*/
void * CRIAPI mwPlyGetSfdHn(MWPLY mwply);

/* 残フレーム数の取得 (取り出し可能なフレーム数の取得) */
CriSint32 CRIAPI mwPlyGetNumRemainFrm(MWPLY mwply);

/* ログ記録機能の追加 */
/* Attach Log recording function */
//void CRIAPI mwPlyAttachLog(CriLog log);

/* ログ記録機能の分離 */
/* Attach Log recording function */
//void CRIAPI mwPlyDetachLog(void);

/*=========================================================================*
 *		非標準関数
 *		Nonstandard Functions
 *=========================================================================*/
/* [for Compatibility] */
/* Please call ADXM_WaitVsync(). This function is same function. */
void CRIAPI mwPlySwitchToIdle(void);

/* サーバ区切り問い合わせ							*/
/* Determine whether process is in server border	*/
CriBool CRIAPI mwPlyIsSvrBdr(void);

/* YCbCrプレーンの計算        */
/* Calculation of YCbCr plane */
void CRIAPI mwPlyCalcYccPlane(CriUint8 *bufptr, CriSint32 width, CriSint32 height, 
								MwsfdYccPlane *ycc);

/* 音声出力スイッチの設定			*/
/* Set audio output switch			*/
void CRIAPI mwPlySetAudioSw(MWPLY mwply, CriSint32 sw);

/* ビデオストリームチャネル数の取得			*/
/* Get the number of video channel streams	*/
CriSint32 CRIAPI mwPlyGetNumVideoCh(MWPLY mwply);
/* ビデオストリームチャネルの設定	*/
/* Set video stream channel 		*/
void CRIAPI mwPlySetVideoCh(MWPLY mwply, CriSint32 ch);
/* ビデオストリームチャネルの取得 (-1: 未定または再生なし)	*/
/* Get video stream channel (-1: pending or out of work) */
CriSint32 CRIAPI mwPlyGetVideoCh(MWPLY mwply);

/* ビデオ出力スイッチの設定	*/
/* Set video output switch	*/
void CRIAPI mwPlySetVideoSw(MWPLY mwply, CriSint32 sw);

/* ハンドル毎のVsync処理	*/
/* V-sync function 			*/
void CRIAPI mwPlyVsyncHndl(MWPLY mwply);
/* ハンドル毎のサーバ処理 		*/
/* Execute decoding one frame	*/
CriBool CRIAPI mwPlyExecSvrHndl(MWPLY mwply);

/* In the case of that no use ADXM_SetupThrd() and ADXM_ExecMain(). */
/* スレッド外部管理時の時刻更新サーバ */
/* [返り値] TRUE : やれることが残っている。FALSE: もうやることは無い。 */
CriBool CRIAPI mwPlyExecVsyncServer(void);
/* In the case of that no use ADXM_SetupThrd() and ADXM_ExecMain(). */
/* スレッド外部管理時のデコードサーバ */
/* [返り値] TRUE : やれることが残っている。FALSE: もうやることは無い。 */
/* ATTN: マルチスレッドで動かす場合はサーバ区切り処理に注意。 */
CriBool CRIAPI mwPlyExecDecServer(void);

/* キーワードの設定(NULL指定でキーワード無効) */
void CRIAPI mwPlySetKeyString(MWPLY mwply, const CriChar8 *str);
/* ファイル情報の取得（キーワード指定）	*/
void CRIAPI mwPlyGetHdrInfWithKey(CriSint8 *buf, CriSint32 bsize, const CriChar8 *str, MwsfdHdrInf *hdrinf);

/* 時刻更新をVbInで自動的に行うかどうかの設定							*/
/* sw = ON	: VbInで自動的に更新（デフォルト）							*/
/*    = OFF	: 自動的に更新は行わない（mwPlyUpdateTime関数で更新する）	*/
void CRIAPI mwPlySetUpdateTimeAuto(MWPLY mwply, CriBool sw);

/* 時刻の更新
 * [備考]
 * mwPlySetUpdateTimeAuto関数でOFFを指定した場合、定期的に呼び出す必要がある。
 * メインループのV同期待ちの直後など、V期間中のできるだけ同じタイミング
 * が望ましい。
 * 呼び出し回数を管理する必要はない。つまり、メインループが処理落ちした後
 * に、回数を補うために何度も呼び出さなくてよい。
 */
void CRIAPI mwPlyUpdateTime(MWPLY mwply);

/*=========================================================================*/
/*=========================================================================*
 *		フレーム変換
 *		Frame Conversion
 *=========================================================================*/
/*=========================================================================*/
/* ARGB8888プレーンフォーマットへの変換	*/
/* Convert to plane ARGB8888 			*/
void CRIAPI mwPlyFxCnvFrmARGB8888(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *out);

/* YUV422プレーンフォーマット変換	*/
/* Convert to plane YUV422 			*/
void CRIAPI mwPlyFxCnvFrmYUV422(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *out);

/* Y84C44プレーンフォーマット変換	*/
/* Convert to plane Y84C44 			*/
void CRIAPI mwPlyFxCnvFrmY84C44(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *yout, CriUint8 *cout);

/* ARGB8888マクロブロックフォーマットへの変換 */
/* Conver to macloblock ARGB8888 */
void CRIAPI mwPlyFxCnvFrmMbARGB8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

/* RGBA8888プレーンフォーマットへの変換 */
/* Conver to plane RGBA8888 */
void CRIAPI mwPlyFxCnvFrmRgba8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

/* BGRA8888プレーンフォーマットへの変換 */
/* Conver to plane BGRA8888 */
void CRIAPI mwPlyFxCnvFrmBgra8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

#if defined(XPT_TGT_PC)
void CRIAPI mwPlyFxCnvFrmAbgr8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);
void CRIAPI mwPlyFxCnvFrmClipAbgr8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
#endif

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*
 * [矩形指定によるフレーム変換処理]
 *=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*
 *  (lux, luy) : 変換もとの左上座標（偶数指定）
 *  (w, h)     : 変換する領域の幅と高さ
 * [パラメータ指定の例]
 *  ----+------------------------------------------------------------------
 *      |  上半分     下半分     左半分     右半分     縦横半分の中央
 *  ----+------------------------------------------------------------------
 *  lux |    0          0          0       width/2        width/4
 *  luy |    0       height/2      0          0          height/4
 *  w   |  width      width     width/2    width/2        width/2
 *  h   | height/2   height/2    height     height       height/2
 *=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*
 * [Convert frame by clipping rectangle area]
 *=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*
 *  (lux, luy) : coordinate of left upper (luy is even onlly)
 *  (w, h)     : width and height of rectangle area
 * [example]
 *  ----+------------------------------------------------------------------
 *      | upper half  lower half  left half  right half    
 *  ----+------------------------------------------------------------------
 *  lux |     0           0           0       width/2      
 *  luy |     0        height/2       0          0         
 *   w  |   width       width      width/2    width/2      
 *   h  |  height/2    height/2     height     height      
 *=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* 矩形範囲指定でARGB8888に変換 */
/* Convert frame to ARGB8888 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipARGB8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
/* 矩形範囲指定でYUV422に変換 */
/* Convert frame to YUV422 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipYUV422(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* RGB888プレーンフォーマットへの変換	*/
/* Convert to plane RGB888 				*/
void CRIAPI mwPlyFxCnvFrmRGB888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

/* 矩形指定のRGB565変換関数					*/
/* Convert frame to RGB565 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB565(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
/* 矩形指定のRGB565変換関数（ディザ付き）	*/
/* Convert frame to RGB565 with dithering by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB565WithDither(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* 矩形指定のRGB555変換関数					*/
/* Convert frame to RGB555 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB555(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
/* 矩形指定のRGB555変換関数（ディザ付き）	*/
/* Convert frame to RGB555 with dithering by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB555WithDither(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* 矩形範囲指定でRGBA8888に変換 */
/* Convert frame to RGBA8888 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRgba8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* 矩形範囲指定でBGRA8888に変換 */
/* Convert frame to BGRA8888 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipBgra8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* アルファのみプレーンへの変換 */
/* ※変換前にアルファプレーン用のピッチ設定を行ってください */
void CRIAPI mwPlyFxCnvFrmA8(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *alphaout);
/* Yのみプレーンへの変換（３値アルファのときのみ値が書き変わる） */
/* ※変換前にYプレーン用のピッチ設定を行ってください */
void CRIAPI mwPlyFxCnvFrmY8(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *yout);

/*=========================================================================*
 *		非同期のフレーム変換
 *		Async Frame Conversion
 *=========================================================================*/
void CRIAPI mwPlyFxCnvFrmRGB565Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);
void CRIAPI mwPlyFxCnvFrmARGB1555Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);
void CRIAPI mwPlyFxCnvFrmARGB4444Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);
void CRIAPI mwPlyFxCnvFrmARGB8888Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);


/*=========================================================================*
 *		非同期のフレーム変換（クリップ）
 *		Async Frame Conversion（Clip）
 *=========================================================================*/
void CRIAPI mwPlyFxCnvFrmClipRGB565Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj, 
																			CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
void CRIAPI mwPlyFxCnvFrmClipARGB1555Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj, 
																			CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
void CRIAPI mwPlyFxCnvFrmClipARGB4444Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj, 
																			CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
void CRIAPI mwPlyFxCnvFrmClipARGB8888Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj, 
																			CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);


/*=========================================================================*
 *		個別プラットフォーム向け
 *		For Each Platform-specific Functions
 *=========================================================================*/

/*-------------------------------------------------------------------------*
 *		ドルビーデジタル設定
 *		Dolby Digital Configuration
 *-------------------------------------------------------------------------*/
/* DolbyDigital再生用ワークバッファサイズの取得 */
/* Get size of work buffer for DolbyDigital */
CriSint32 CRIAPI mwPlyGetDolbyDigitalWorkSize(void);
/* DolbyDigital再生機能の追加 */
/* Attach DolbyDigital */
void CRIAPI mwPlyAttachDolbyDigital(MWPLY mwply, void *wkptr, CriSint32 wksize);
/* DolbyDigital再生機能の分離 */
/* Detach DolbyDigital */
void CRIAPI mwPlyDetachDolbyDigital(MWPLY mwply);
/* DolbyDigitalストリームチャネル番号の設定(デフォルトはチャネル１) */
/* Set channel number of DolbyDigital (Default channel = 1) */
void CRIAPI mwPlySetDolbyDigitalCh(MWPLY mwply, CriSint32 ch);

/*-------------------------------------------------------------------------*
 *		マルチチャンネルオーディオ設定
 *		Multi Channel Audio Configuration
 *-------------------------------------------------------------------------*/
/* マルチチャンネル再生用ワークバッファサイズの取得 */
/* Get size of work buffer for multi channel audio */
CriSint32 CRIAPI mwPlyGetMultiChannelWorkSize(void);
/* マルチチャンネルオーディオ再生機能の追加 */
/* Attach MultiChannel Audio */
void CRIAPI mwPlyAttachMultiChannel(MWPLY mwply, void *wkptr, CriSint32 wksize);
/* マルチチャンネルオーディオ再生機能の分離 */
/* Detach MultiChannel Audio */
void CRIAPI mwPlyDetachMultiChannel(MWPLY mwply);
/* マルチチャネルオーディオのストリームチャネル番号の設定(デフォルトはチャネル１) */
/* Set channel number of MultiChannel Audio (Default channel = 1) */
void CRIAPI mwPlySetMultiChannelCh(MWPLY mwply, CriSint32 ch);

/*-------------------------------------------------------------------------*
 *		プラットフォーム固有の各種拡張ファイル再生
 *		Platform Specific Extended File Playback
 *-------------------------------------------------------------------------*/
/* Movie start by LSN */
void CRIAPI mwPlyStartLsn(MWPLY mwply, CriUint32 lsn, CriUint32 bytesize);
/* Movie start looping playback by LSN */
void CRIAPI mwPlyStartLsnLp(MWPLY mwply, CriUint32 lsn, CriUint32 bytesize);

/*-------------------------------------------------------------------------*
 *		プラットフォーム固有の特殊設定
 *		Platform Specific Special Configuration
 *-------------------------------------------------------------------------*/
/* パックサイズの設定（デフォルト 2048 byte） */
/* Set system stream pack size (default = 2048)  */
void CRIAPI mwPlySetPackSize(CriSint32 packsize);
/* サムネイルモードの設定（ON: 1/8縮小画像を出力する） */
/* Set thumbnail mode (ON: Output 1/8 reduced image)  */
void CRIAPI mwPlySetThumbnail(MWPLY mwply, CriSint32 sw);
/* 高速色変換処理の指定 ON:速度優先, OFF:画質優先(デフォルト) */
void CRIAPI mwPlySetHighSpeedConversion(int sw);
/* 上下逆さまに変換するかどうかを設定する */
void CRIAPI mwPlySetCnvBottomUp(MWPLY mwply, CriBool sw);
/* 上下逆さまに変換するかどうかを取得する */
CriBool CRIAPI mwPlyGetCnvBottomUp(MWPLY mwply);
/* ビデオPTS時刻管理の設定 (MPEG-2 TS/PS再生用) */
void CRIAPI mwPlySetVideoPts(MWPLY ply, void *ptswork, CriSint32 ptsworksiz);
/* ビデオPIDの設定 (MPEG-2 TS再生用、複数プログラムの場合にPID指定が必須) */
void CRIAPI mwPlySetVideoPid(MWPLY ply, CriSint32 pid);
/* オーディオPIDの設定 (MPEG-2 TS再生用、複数プログラム場合にPID指定が必須) */
void CRIAPI mwPlySetAudioPid(MWPLY ply, CriSint32 pid);
/* PESストリーム再生の設定（ON: PESストリームを再生する、OFF:PS/TSを再生する） */
/* Set PES stream playback mode (ON: playback PES,  OFF: playback PS/TS)  */
void CRIAPI mwPlySetPesSw(MWPLY mwply, CriSint32 sw);

/* 
 * 下記の関数は廃止し、ハンドル作成パラメータで外部フレームプール用
 * ワークバッファを指定できるようにした。
 * void CRIAPI mwPlySetDecodeOutBuf(CriSint32 bufnum, CriSint32 bufsize, CriUint8 **bufptr);
 */

/* 外部フレームプール用ワークバッファサイズの計算 */
CriSint32 CRIAPI mwPlyCalcOuterFrmPool(CriSint32 max_width, CriSint32 max_height);
/* フレーム変換用ワークバッファサイズの計算 */
CriSint32 CRIAPI mwPlyCalcCnvFrmWorkSize(CriSint32 max_width, CriSint32 max_height, CriSint32 compo_mode);
/* フレーム変換用ワークバッファの設定 */
void CRIAPI mwPlySetCnvFrmWorkBuffer(MWPLY mwply, CriUint8 *workbuf, CriSint32 worksize);
/* フレーム変換用ワークバッファの解放 */
void CRIAPI mwPlyReleaseCnvFrmWorkBuffer(MWPLY mwply);

/* 無限ループ再生の設定 */
void CRIAPI mwPlySetInfiniteSingleLoop(MWPLY mwply, CriBool sw);
/* 無限ループ再実行回数の取得 */
CriUint32 CRIAPI mwPlyGetInfiniteLoopCount(MWPLY mwply);
/* 再生時間上限の取得（単位[秒]） */
CriSint32 CRIAPI mwPlyGetLimitTime(MWPLY mwply);

/* 再生停止要求 */
/* MEMO: 実際の停止処理はADXM_ExecMain関数で自動的に動き、ハンドル状態がSTOPになる。 */
/*       mwPlyStop/mwPlyDestroyの負荷増を抑えるための仕組み。 */
/*       ハンドル状態がSTOPになってからmwPlyDestroyを呼ぶと負荷があがらない。*/
void CRIAPI mwPlyRequestStop(MWPLY mwply);

/* 再生中ファイル情報の取得						*/
/* Get header information of playing movie file	*/
void CRIAPI mwPlyGetPlayingHdrInf(MWPLY mwply, MwsfdHdrInf *hdrinf);

/* 再読み込み開始時間の設定 */
/* Set reload timing */
void CRIAPI mwPlySetReloadTime(MWPLY mwply, CriFloat32 time);

/* 最大読み込みセクタ数の設定 */
/* Set maximam read sectors */
void CRIAPI mwPlySetMaxReadSct(MWPLY mwply, CriUint32 nsct);

/* Ｉのみムービの指定ピクチャ再生 */
void CRIAPI mwPlyStartIpicture(MWPLY mwply, const CriChar8 *fname, CriUint32 fno, CriUint32 dec_num, 
						MwsfdSeekIpic *seekipic_tbl, CriUint32 seekipic_num);

/* ファイル名指定による ファイル情報 ヘッダ情報（オーディオ詳細）の取得	*/
/* Get header informations by filename	*/
CriBool mwPlyReadAndGetHdrInf(
	const CriChar8 *fname,
	MwsfdHdrInf *hdrinf,
	MwsfdHdrAudioDetail *audio_detail);

/*=========================================================================*
 *		非公開関数
 *      Closed functions
 *-------------------------------------------------------------------------*
 *	互換性のために関数宣言を残してあります。通常は使用する必要ありません。
 *	These function declarations are retained for compatibility.
 *	Usually, it is not necessary to use these functions.
 *=========================================================================*/
/* 次フレーム取得可能問い合わせ				*/
/* Determine whether next frame is ready	*/
CriBool CRIAPI mwPlyIsNextFrmReady(MWPLY mwply);
/* 次フレームのピクチャユーザデータの取得 */
/* Get next frame user data in picture layer */
void CRIAPI mwPlyGetNextPicUsr(MWPLY mwply, void **usrdatptr, CriSint32 *usrdatsize);



#ifdef __cplusplus
}
#endif

#endif	/* INCLUDED_CRIMWSFD_H */
