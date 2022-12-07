#ifndef	_AIXP_H_INCLUDED
#define	_AIXP_H_INCLUDED
/****************************************************************************/
/*																			*/
/*			$title$  AIX 再生 ライブラリ									*/
/*																			*/
/*				2001.7.29		written M.Oshimi							*/
/*																			*/
/****************************************************************************/

/*	バージョン番号						*/
/*	Version number of AIXP library 		*/
#define	AIXP_VER	"3.13"

#include "cri_xpt.h"
#include "sj.h"
#include "cri_adxt.h"

/*	ストリームコントローラ				*/
/*	Stream Controller					*/
#ifndef ADXSTM_DEFINED
#define ADXSTM_DEFINED
typedef	void	*ADXSTM;
#endif

/*	ストリームジョイントデコーダ		*/
/*	Stream Joint decoder				*/
#ifndef AIXSJD_DEFINED
#define AIXSJD_DEFINED
typedef void	*AIXSJD;
#endif

/*	オーディオレンダラ					*/
/*	Audio Renderer						*/
#ifndef ADXRNA_DEFINED
#define ADXRNA_DEFINED
typedef void	*ADXRNA;
#endif

/****************************************************************************/
/*		定数マクロ															*/
/*		MACRO CONSTANT														*/
/****************************************************************************/

/*	最大同時発音数						*/
/*	Maximum number of AIXP handle		*/
#define	AIXP_MAX_OBJ		(4)

/*	最大トラック数						*/
/*	Maximum number of track				*/
#define	AIXP_MAX_TR			(32)

/*	最大サンプリング周波数				*/
/*	Maximum sampling frequency			*/
#define	AIXP_MAX_SFREQ		(48000)

/*	$define$ AIX Talkの動作状態(AIXP_STAT_～)	*/
/*	Status of AIX Talk							*/
#define	AIXP_STAT_STOP		(0)		/*	停止中 								*/
									/*	During standstill					*/
#define AIXP_STAT_PREP		(1)		/*	再生準備中 							*/
									/*	During play preparation				*/
#define AIXP_STAT_PLAYING	(2)		/*	デコード＆再生中 					*/
									/*	During decode and play				*/
#define AIXP_STAT_PLAYEND	(3)		/*	再生終了 							*/
									/*	Play end							*/
#define AIXP_STAT_ERROR		(4)		/*	再生終了 							*/
									/*	Play end							*/

/*	最大再生チャンネル数				*/
/*	Maximum number of play channel		*/
#define	AIXP_MAX_NCH		(2)

/*	マルチチャネル出力用パラメータ		*/
/*	Parameters for multi channal output	*/
#define AIXP_MC_NTR			(3)
#define AIXP_MC_NCH			(6)

/*	サーバ関数の呼び出される頻度の規定値　60(回/秒)				*/
/*	Default value of frequency called server function(times/sec)	*/
#define AIXP_DEF_SVRFREQ	(60)

/*	Default value of output volume(dB)	*/
#define	AIXP_DEF_OUTVOL		(0)

/*	ファイル名の最大長	*/
/*	Maximum length of the file path		*/
#define AIXP_MAX_FNAME		(256)

#define AIXP_CALC_WORK_COMMON(nch, nstm, sfreq, ntr)	\
	((ADXT_CALC_IBUFSIZE(nch, nstm, sfreq) + (ADXT_CALC_WORK(nch, ADXT_PLY_MEM, nstm, sfreq) - 64)) * (ntr) + 64)
#define AIXP_CALC_WORK_COMMON_3D(nstm, sfreq, ntr)	\
	((ADXT_CALC_IBUFSIZE_3D(nstm, sfreq) + (ADXT_CALC_WORK_3D(ADXT_PLY_MEM, nstm, sfreq) - 64)) * (ntr) + 64)

/* ワークサイズ	*/
/* Work size	*/
#ifdef XPT_TGT_PC
#define AIXP_CALC_WORK(nch, nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON(nch, nstm, sfreq, ntr) + ADXT_CALC_OBUFSIZE(nch))
#else	// #ifdef XPT_TGT_PC
#define AIXP_CALC_WORK(nch, nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON(nch, nstm, sfreq, ntr))
#endif	// #ifdef XPT_TGT_PC

/* リアルタイム3Dパンニングを行う際のワークサイズ				*/
/* Work size to create handle that can use realtime 3D panning	*/
#ifdef XPT_TGT_PC
#define AIXP_CALC_WORK_3D(nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON_3D(nstm, sfreq, ntr) + ADXT_OBUFSIZE_3D)
#else
#define AIXP_CALC_WORK_3D(nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON_3D(nstm, sfreq, ntr))
#endif

/* マルチフレーズ再生用ワーク領域										*/
/* Work size for multi phrase playback (required on 5 or more phases)	*/
#define AIXP_CALC_PHRASE_WORK(nph)			(((nph) * 16) + 64)

/*	入力バッファのエキストラ領域の大きさ	*/
/*	Size of Extra area in input buffer		*/
#define AIXP_IBUF_XLEN (8*1024)

/*	AIX Talk の出力バッファの大きさ　(サンプル単位)	*/
/*	Output buffer size of AIX Talk (unit:sample)		*/
#define	AIXP_OBUF_SIZE	(0x2000)

#ifndef XPT_TGT_PC_LE
#define DECODE_AHX
#endif

#ifdef DECODE_AHX
#define	AIXP_OBUF_DIST	(0x2060)
#else
#define	AIXP_OBUF_DIST	(0x2020)
#endif

/*	入力バッファへの読み込み量						*/
/*	Number of sectors to read into the input buffer	*/
#define AIXP_MAX_CDBSCT		(75)
#define AIXP_MIN_CDBSCT		(65)
#define AIXP_PREP_RDSCT		(25)

/*	パンポットの設定値		*/
/*	Panpot parameter		*/
#define	AIXP_PAN_LEFT		(-15)
#define	AIXP_PAN_CENTER		(0)
#define	AIXP_PAN_RIGHT		(15)
#define	AIXP_PAN_AUTO		(-128)	/*	MONO/STE によって自動的に切り替える	*/
									/*	Changes automatically by data		*/

/*	ステレオ再生時のチャンネル番号		*/
/*	Channel number playing stereo data	*/
/*	AIXP_CH_L:left, AIXP_CH_R:right		*/
#define	AIXP_CH_L			(0)
#define	AIXP_CH_R			(1)

/*	5.1ch再生時のスピーカ番号			*/
/*	Speaker number playing 5.1ch sound	*/
#define	AIXP_SPKID_FL		(ADXT_SPEAKER_FRONT_LEFT)		/* Front Left   */
#define	AIXP_SPKID_FR		(ADXT_SPEAKER_FRONT_RIGHT)		/* Front Right  */
#define	AIXP_SPKID_BL		(ADXT_SPEAKER_BACK_LEFT)		/* Back Left    */
#define	AIXP_SPKID_BR		(ADXT_SPEAKER_BACK_RIGHT)		/* Back Right   */
#define	AIXP_SPKID_FC		(ADXT_SPEAKER_FRONT_CENTER)		/* Front Center */
#define	AIXP_SPKID_LF		(ADXT_SPEAKER_LOW_FREQUENCY)	/* Low Freqency */
#define	AIXP_SPK_NUM		(6)
#define AIXP_TRK_NUM		(3)
#define	AIXP_SPK_VOL_MAX	(0)
#define	AIXP_SPK_VOL_MIN	(-10000)

/*	再生モード	*/
/*	Play mode	*/
#define	AIXP_PMODE_FNAME	(0)		/*	Play specified file name	*/
#define	AIXP_PMODE_AFS		(1)		/*	Play specified file ID		*/
#define	AIXP_PMODE_MEM		(2)		/*	Play data from memory		*/
#define	AIXP_PMODE_SJ		(3)		/*	Play data from StreamJoint	*/

/*	ミキサー動作モード	*/
/*	Mixer mode			*/
#define AIXP_MIX_HARDWARE	(0)
#define AIXP_MIX_SOFTWARE	(1)

/* 旧バージョンとの互換用 */
/* For compatibility with old version */
#define AIXP_StartFnameDolbyDigital		AIXP_StartFnameMultiChannel
#define AIXP_StartAfsDolbyDigital		AIXP_StartAfsMultiChannel
#define AIXP_StartMemDolbyDigital		AIXP_StartMemMultiChannel
#define AIXP_SetOutVolDolbyDigital		AIXP_SetOutVolMultiChannel

/****************************************************************************/
/*		データ型															*/
/*      Data type declaration												*/
/****************************************************************************/

/* フレーズコントロール用コールバック関数 */
/*	Structure of phrase control           */
typedef CriSint32 (*AIXP_CBFN_PHCTRL)(void *obj, CriSint32 ph_now, CriSint32 ph_max);

#if defined(XPT_TGT_PC)
/* 振幅抽出機能用構造体 */
typedef struct {
	void *amp;
	SJ sji[AIXP_MAX_NCH];
	SJ sjo[AIXP_MAX_NCH];
	void *context;
} AIXAMP;
#endif

/*	AIX Talk オブジェクト構造体		*/
/*	Structure of AIX Talk object	*/
typedef	struct _aix_ply {
	CriSint8	used;						/*	使用中か否か					*/
	CriSint8	stat;						/*	動作状態						*/
	CriSint8	maxnch;						/*	最大再生チャンネル数			*/
	CriSint8	maxntr;						/*	最大再生トラック数				*/
	CriSint8	nch;						/*	現在の再生チャンネル数			*/
	CriSint8	ntr;						/*	現在の再生トラック数			*/
	CriSint8	seek_flag;					/*	シーク処理を行うかどうか		*/
	CriSint8	rsv1;						/*	予約							*/
	AIXSJD	sjd;						/*	AIXストリームジョイントデコーダ	*/
	ADXSTM	stm;						/*	入力ストリームコントローラ		*/
	ADXT	adxt[AIXP_MAX_TR];			/*	ADXT再生器						*/
	SJ		sjf;						/*	入力ストリームジョイント		*/
	SJ		sji;						/*	入力ストリームジョイント		*/
	SJ		sjo[AIXP_MAX_TR];			/*	出力ストリームジョイント		*/
	CriSint8	*ibuf;						/*	入力バッファ					*/
	CriSint32	ibufbsize;					/*	入力バッファサイズ(バイト単位)	*/
	CriSint32	ibufxsize;					/*	入力バッファエクストラサイズ	*/
	CriSint8	*obuf[AIXP_MAX_TR];			/*	出力バッファ					*/
	CriSint32	obufbsize;					/*	出力バッファサイズ(バイト単位)	*/
	CriSint32	obufxsize;					/*	出力バッファサイズ(バイト単位)	*/
	CriSint32	pause_flag;					/*	ポーズフラグ					*/
	CriSint32	demux_flag;					/*	デマルチプレクスフラグ			*/
	CriSint8	lpsw;						/*	ループスイッチ					*/
	CriSint8	lnksw;						/*	連結スイッチ					*/
	CriSint8	cmdstart;					/*	スタートコマンド				*/
	CriSint8	rsv2;						/*	予約							*/
	CriSint32	lpcnt;						/*	ループカウンタ					*/
	CriSint32	ldph;						/*	先行してロードするフレーズ数	*/
	CriSint32	curph;						/*	現在再生中のフレーズ番号		*/
	CriSint32	stph;						/*	再生開始フレーズ番号			*/
	CriSint32	lpsp;						/*	ループスタートフレーズ番号		*/
	CriSint32	lpep;						/*	ループエンドフレーズ番号		*/
	CriChar8	*fname_wk;					/*	ファイル名保持領域（work内）	*/
	CriChar8	*fname;						/*	ファイル名						*/
	void	*dir;						/*	ディレクトリ情報				*/
	CriUint32	ofst;						/*	ファイルオフセット				*/
	CriUint32	nsct;						/*	ファイルサイズ(セクタ)			*/
	CriSint32	pmode;						/*	再生モード						*/
	CriSint32	svrfreq;					/*	サーバ関数呼び出し頻度			*/
	CriFloat32 reload_time;				/*	再読み込み時間 (0:default)		*/
	CriSint32 master_volume;				/*	全体ボリューム					*/
	CriSint32 track_volume[AIXP_MAX_TR];	/*	トラックごとのボリューム		*/
	CriBool	use_3d_panning;				/*  3Dパンニングを行えるかどうか	*/
	CriSint32	time_offset;				/*	再生復帰時刻オフセット			*/
	CriSint32	time_scale;					/*	再生復帰時刻スケール			*/
	CriSint32	start_smpl_phrase;			/*	再生開始サンプル（フレーズ内）	*/
	CriSint32	start_smpl;					/*	再生開始サンプル				*/
	CriFloat32	start_time;					/*	再生開始時間					*/
	CriSint32	time_offset_seek;			/*	シークによる時刻オフセット		*/
	
	/* フレーズコントロール再生用パラメータ */
	AIXP_CBFN_PHCTRL phctrl_cbfn;
	void *phctrl_obj;
	
	/* マルチチャンネルオーディオ出力用パラメータ */
#if defined(XPT_SUPPORT_MULTICHANNEL)
	
#if defined(XPT_TGT_PC)
	/* PC環境固有のパラメータ */
	CriSint8 *wkpcm[AIXP_MAX_TR * AIXP_MAX_NCH];
	SJ sjpcm[AIXP_MAX_TR * AIXP_MAX_NCH];
	void *dcd[AIXP_MAX_TR];
	void *mcrna;
	CriBool attached;
	AIXAMP aixamp[AIXP_MAX_TR];
#elif defined(XPT_TGT_XB)
	/* Xbox環境固有のパラメータ */
	CriBool dolby_flag[AIXP_TRK_NUM];
	CriSint32 spk_idx[AIXP_SPK_NUM];
#elif defined(XPT_TGT_PS3PPU)
	/* PS3環境固有のパラメータ */
	CriSint32 group_id;
#endif
	
	/* スピーカー指定のボリュームコントロール用 */
	CriSint32 spk_vol[AIXP_SPK_NUM];
	
	/* Sofdec時刻更新用拡張パラメータ */
	CriSint32 last_sample;
	CriSint64 last_count;
#endif
} AIX_PLY;
typedef	AIX_PLY	*AIXP;

/****************************************************************************/
/*		関数の宣言															*/
/*      Function Declaration												*/
/****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* $func$ AIXPの初期化
 * [書　式] void AIXP_Init(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] AIXPを初期化する。
 *			変数領域の初期化を行う。
 *  Initialization of AIXP
 * [Format  ] void AIXP_Init(void);
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Initializes AIXP. Initializes variable.
 */
void CRIAPI AIXP_Init(void);

/* $func$ AIXPの終了
 * [書　式] void AIXP_Finish(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] AIXP の終了処理をする。
 *  Termination of AIXP
 * [Format  ] void AIXP_Finish(void);
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Finalizes AIXP.
 */
void CRIAPI AIXP_Finish(void);

/*	$func$ AIXPハンドル の生成
 * [書　式] AIXP AIXP_Create(CriSint32 maxtr, CriSint32 maxnch, void *work, CriSint32 worksize);
 * [入　力] maxntr	: 最大トラック数
 *			maxnch	: 最大再生チャンネル数(モノラルのみ１：ステレオ２)
 *			work	: ワーク領域
 *			worksize: ワーク領域のサイズ
 * [出　力] なし
 * [関数値] AIXPハンドル
 * [機　能] AIXPハンドルを生成する。
 *			work領域のサイズは、AIXP_CALC_WORKマクロによって求める。
 *  Creation of AIXP handle
 * [Format  ] AIXP AIXP_Create(CriSint32 maxtr, CriSint32 maxnch, void *work, CriSint32 worksize);
 * [Inputs  ] maxntr  : Number of maximum tracks
 *			  maxnch  : Number of maximum channels(monoral:1, stereo:2)
 *			  work    : Working area
 *			  worksize: Size of working area(byte)
 * [Outputs ] None
 * [Return  ] AIXP handle
 * [Function] Creates AIXP handle.
 * [Remarks ] Use 'AIXP_CALC_WORK' macro to calculate size of working area.
 */
AIXP CRIAPI AIXP_Create(CriSint32 maxntr, CriSint32 maxnch, void *work, CriSint32 worksize);

/*	$func$ AIXP 3Dハンドル の生成
 * [書　式] AIXP AIXP_Create3D(CriSint32 maxtr, void *work, CriSint32 worksize);
 * [入　力] maxntr	: 最大トラック数
 *			work	: ワーク領域
 *			worksize: ワーク領域のサイズ
 * [出　力] なし
 * [関数値] AIXPハンドル
 * [機　能] リアルタイム3Dパンニング可能なAIXPハンドルを生成する。
 *			work領域のサイズは、AIXP_CALC_WORK_3Dマクロによって求める。
 *  Creation of AIXP 3D handle
 * [Format  ] AIXP AIXP_Create3D(CriSint32 maxtr, void *work, CriSint32 worksize);
 * [Inputs  ] maxtr  : Number of maximum tracks
 *			  work    : Working area
 *			  worksize: Size of working area(byte)
 * [Outputs ] None
 * [Return  ] AIXP handle
 * [Function] Creates AIXP handle that can use realtime 3D panning.
 * [Remarks ] Use 'AIXP_CALC_WORK_3D' macro to calculate size of working area.
 */
AIXP CRIAPI AIXP_Create3D(CriSint32 maxntr, void *work, CriSint32 worksize);

/* $func$ AIXPハンドル の消去
 * [書　式] void AIXP_Destroy(AIXP aixp);
 * [入　力] AIXP	: AIXPハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] 指定された AIXPハンドルを消去する。
 *  Destroy of AIXP handle
 * [Format  ] void AIXP_Destroy(AIXP aixp);
 * [Inputs  ] AIXP   : AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Destroys specified AIXP handle.
 */
void CRIAPI AIXP_Destroy(AIXP aixp);

/* $func$ フレーズ再生用ワーク領域の設定
 * [書　式] void AIXP_SetPhraseWork(AIXP aixp, void *work, CriSint32 wksize);
 * [入　力] AIXP	: AIXPハンドル
 *			work	: ワーク領域
 *			worksize: ワーク領域のサイズ
 * [出　力] なし
 * [関数値] なし
 * [機　能] マルチフレーズのAIXファイル（5個以上のフレーズを含むAIXファイル）
 *          を再生するのに必要なワーク領域を設定します。
 *          フレーズ数が4個以下のAIXファイルを再生する場合には、
 *          本関数を使用する必要はありません。
 * [備　考] ワーク領域のサイズは、AIXP_CALC_PHRASE_WORKマクロによって計算します。
 *  Set the work area used for multi phrase playback
 * [Format  ] void AIXP_SetPhraseWork(AIXP aixp, void *work, CriSint32 wksize);
 * [Inputs  ] AIXP    : AIXP handle
 *			  work    : Working area
 *			  worksize: Size of working area(byte)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Set the work area used for multi phrase (5 or more phases) playback.
 *            If you play AIX files which contain less than 4 phases, you don't 
 *            need to call this function.
 * [Remarks ] Use 'AIXP_CALC_PHRASE_WORK' macro to calculate size of working area.
*/
void CRIAPI AIXP_SetPhraseWork(AIXP aixp, void *work, CriSint32 wksize);

/* $func$ ファイル名指定による再生の開始
 * [書　式] void AIXP_StartFname(AIXP aixp, CriChar8 *fname, void *atr);
 * [入　力] AIXP	: AIXPハンドル
 *			fname	: ファイル名
 *			atr		: ディレクトリ情報
 * [出　力] なし
 * [関数値] なし
 * [機　能] fname で指定されたAIXファイルの再生を開始する。
 *  Play start of AIX data specified file name
 * [Format  ] void AIXP_StartFname(AIXP aixp, CriChar8 *fname, void *atr);
 * [Inputs  ] AIXP	: AIXP handle
 *			  fname	: File name
 *			  atr	: Directry Information
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play AIX file when you specify file name.
 */
void CRIAPI AIXP_StartFname(AIXP aixp, const CriChar8 *fname, void *atr);

/* $func$ ファイルの範囲指定による再生の開始
 * [書　式] void AIXP_StartFnameRange(AIXP aixp, CriChar8 *fname, void *atr, CriSint32 ofst, CriSint32 nsct);
 * [入　力] AIXP	: AIXPハンドル
 *			fname	: ファイル名
 *			atr		: ディレクトリ情報
 *			ofst	: オフセット
 *			nsct	: サイズ(セクタ単位)
 * [出　力] なし
 * [関数値] なし
 * [機　能] fname ファイル内のofstとnsctで指定されたAIXデータの再生を開始する。
 *  Play start of AIX data specified file name and offset, size
 * [Format  ] void AIXP_StartFnameRange(AIXP aixp, CriChar8 *fname, void *atr, CriSint32 ofst, CriSint32 nsct);
 * [Inputs  ] AIXP	: AIXP handle
 *			  fname	: File name
 *			  atr	: Directry Information
 *			  ofst	: Offset(unit:sector)
 *			  nsct	: Size(unit:sector)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play AIX data when you specify offset and size.
 */
void CRIAPI AIXP_StartFnameRange(AIXP aixp, const CriChar8 *fname, void *atr, CriSint32 ofst, CriSint32 nsct);

/* $func$ FID 指定による再生の開始
 * [書　式] void AIXP_StartAfs(AIXP aixp, CriSint32 patid, CriSint32 fid);
 * [入　力] AIXP	: AIXPハンドル
 *			patid	: パーティション識別子
 *			fid		: ファイル識別子
 * [出　力] なし
 * [関数値] なし
 * [機　能] パーティション識別子とファイル識別子で指定されたAIXファイルの
 *			再生を開始する。
 *  Play start of AIX data by specified file ID
 * [Format  ] void AIXP_StartAfs(AIXP aixp, CriSint32 patid, CriSint32 fid);
 * [Inputs  ] AIXP	: AIXP handl
 *			  patid	: Partition ID
 *			  fid	: File ID
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play AIX file when you specify partition ID and file ID.
 */
void CRIAPI AIXP_StartAfs(AIXP aixp, CriSint32 patid, CriSint32 fid);

/* $func$ ストリームジョイントによる再生の開始
 * [書　式] void AIXP_StartSj(AIXP aixp, SJ sj);
 * [入　力] AIXP	: AIXPハンドル
 *			sj		: ストリームジョイント
 * [出　力] なし
 * [関数値] なし
 * [機　能] ストリームジョイントから得られるデータを再生する。
 *  Play start of AIX data from Stream Joint
 * [Format  ] void AIXP_StartSj(AIXP aixp, SJ sj);
 * [Inputs  ] AIXP	: AIXP handle
 *			  sj	: Stream Joint
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play AIX file when you specify Stream Joint.
 */
void CRIAPI AIXP_StartSj(AIXP aixp, SJ sj);

/* $func$ メモリ指定による再生の開始
 * [書　式] void AIXP_StartMem(AIXP aixp, void *aixdat, CriSint32 datlen);
 * [入　力] AIXP	: AIXPハンドル
 *			aixdat	: AIXデータのアドレス
 *			datlen	: AIXデータの大きさ
 * [出　力] なし
 * [関数値] なし
 * [機　能] aixdata で指定されたAIXデータを再生する。
 *  Play start of AIX data on memory
 * [Format  ] void AIXP_StartMem(AIXP aixp, void *aixdat, CriSint32 datlen);
 * [Inputs  ] AIXP	: AIXP handle
 *			  aixdat: Address of AIX data
 *			  datlen: Length of playing AIX data
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play AIX data on memory. Plays to specified length.
 */
void CRIAPI AIXP_StartMem(AIXP aixp, void *aixdat, CriSint32 datlen);

/* $func$ 再生の停止
 * [書　式] void AIXP_Stop(AIXP aixp);
 * [入　力] AIXP	: AIXPハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] AIXの再生を停止する。
 *  Play stop
 * [Format  ] void AIXP_Stop(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops to play AIX data.
 */
void CRIAPI AIXP_Stop(AIXP aixp);

/* $func$ 状態の取得
 * [書　式] CriSint32 AIXP_GetStat( AIXP aixp );
 * [入　力] AIXP	: AIXPハンドル
 * [出　力] なし
 * [関数値] 現在の AIXPハンドルの状態を表す定数
 * 				AIXP_STAT_STOP	 :	停止中
 *				AIXP_STAT_PREP	 :	再生準備中
 *				AIXP_STAT_PLAYING:	デコード＆再生中
 *				AIXP_STAT_PLAYEND:	再生終了
 *				AIXP_STAT_ERROR	 :	エラー
 * [機　能] 現在のAIXPハンドルの状態を取得する。
 *  Get status
 * [Format  ] CriSint32 AIXP_GetStat( AIXP aixp );
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] Status of AIXP handle
 * [Function] Obtains status of AIXP handle.
 */
CriSint32 CRIAPI AIXP_GetStat(AIXP aixp);

/* $func$ サンプル単位での再生時刻の取得
 * [書　式] void AIXP_GetTime( AIXP aixp, CriSint32 *ncount, CriSint32 *tscale );
 * [入　力] aixp    : AIXPハンドル
 * [出　力] ncount	: 再生サンプル数
 *			tscale	: サンプリング周波数 [ Hz ]
 * [関数値] なし
 * [機　能] サンプル単位で再生時刻を取得する。
 *  Get play time by sample unit
 * [Format  ] void AIXP_GetTime( AIXP aixp, CriSint32 *ncount, CriSint32 *tscale );
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] ncount: Number of play sample
 *			  tscale: Sampling frequency(Hz)
 * [Return  ] None
 * [Function] Obtains play time by sample unit.
 */
void CRIAPI AIXP_GetTime(AIXP aixp, CriSint32 *ncount, CriSint32 *tscale);

/* $func$ ADXTハンドルの取得
 * [書　式] ADXT AIXP_GetAdxt(AIXP aixp, CriSint32 trno);
 * [入　力] AIXP	: AIXPハンドル
 *			trno	: トラック番号
 * [出　力] なし
 * [関数値] adxt	: ADXTハンドル
 * [機　能] ADXTハンドルを取得する。
 *  Getting a ADXT handle of specified track
 * [Format  ] ADXT AIXP_GetAdxt(AIXP aixp, CriSint32 trno);
 * [Inputs  ] AIXP	: AIXP handle
 *			  trno	: Track No.
 * [Outputs ] None
 * [Return  ] ADXT handle
 * [Function] Obtains a ADXT handle of specified track.
 */
ADXT CRIAPI AIXP_GetAdxt(AIXP aixp, CriSint32 trno);

/*	$func$ 再読み込み開始時間の設定
 * [書　式] void AIXP_SetReloadTime(AIXP aixp, CriFloat32 time);
 * [入　力] AIXP	: AIXPハンドル
 *			time	: 再読み込み開始時間（単位：秒）
 * [出　力] なし
 * [関数値] なし
 * [機　能] 入力バッファへの再読み込み開始時間を設定する。
 *			バッファ内のデータ量が time 秒分を下回った場合に
 *			ディスクからデータを読み込むよう設定します。
 *  Set the time of start sector to reload
 * [Format  ] void AIXP_SetReloadTime(AIXP aixp, CriFloat32 time);
 * [Inputs  ] AIXP	: AIXP handle
 *			  time  : start remain time (unit:second)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets of the time of start sector to reload into input buffer.
 *			  Reads data from disc when a quantity of data in the input 
 *			  buffer becomes less than 'time' [second].
 */
void CRIAPI AIXP_SetReloadTime(AIXP aixp, CriFloat32 time);

/*	$func$ 入力バッファ内再生時間
 * [書　式] CriFloat32 AIXP_GetIbufRemainTime(AIXP aixp);
 * [入　力] AIXP	: AIXPハンドル
 * [出　力] なし
 * [関数値] 再生可能時間　(単位：秒)
 * [機　能] 入力バッファにあるデータで再生可能な時間を取得する。
 *			AIXデータのみ対応。
 * [Format  ] CriFloat32 AIXP_GetIbufRemainTime(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] Playable time (sec)
 * [Function] Obtains playable time using only data in input buffer.
 *			  You can use this function only playing AIX data.
 */
CriFloat32 CRIAPI AIXP_GetIbufRemainTime(AIXP aixp);

/*	$func$ 各ハンドルのサーバ関数　(内部状態の更新)
 * [書　式] CriSint32 AIXP_ExecHndl(AIXP aixp);
 * [入　力] AIXP	: AIXPハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] 各ハンドルの内部状態を更新する。
 *			AIXP_ExecServer内から呼び出される。
 *  Server function of each handle
 * [Format  ] CriSint32 AIXP_ExecHndl(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status.
 *			  This function is called from 'AIXP_ExecServer' function.
 */
void CRIAPI AIXP_ExecHndl(AIXP aixp);

/*	$func$ サーバ関数　(内部状態の更新)
 * [書　式] CriSint32 AIXP_ExecServer(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ライブラリの内部状態を更新する。
 *			V-Sync 毎に呼び出さなければならない。
 *  Server function
 * [Format  ] CriSint32 AIXP_ExecServer(void);
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status of library.
 */
void CRIAPI AIXP_ExecServer(void);

/*	$func$ ループした回数の取得
 * [書　式] CriSint32 AIXP_GetLpCnt(AIXP aixp);
 * [入　力] AIXP	: AIXPハンドル
 * [出　力] なし
 * [関数値] ループした回数
 * [機　能] ループした回数を取得する。
 *  Get the number of times played a loop
 * [Format  ] CriSint32 AIXP_GetLpCnt(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] Number of times played a loop
 * [Function] Obtains the number of times that played a loop.
 */
CriSint32 CRIAPI AIXP_GetLpCnt(AIXP aixp);

/*	$func$ ループスイッチの設定
 * [書　式] void AIXP_SetLpSw(AIXP aixp, CriSint32 sw);
 * [入　力] aixp	: AIXPハンドル
 *			sw		: 1=ループする、0=ループしない
 * [出　力] なし
 * [関数値] なし
 * [機　能] ループするか否かを設定する。
 */
void CRIAPI AIXP_SetLpSw(AIXP aixp, CriSint32 sw);

/*	$func$ フレーズリンクスイッチの設定
 * [書　式] void AIXP_SetLnkSw(AIXP aixp, CriSint32 sw);
 * [入　力] aixp	: AIXPハンドル
 *			sw		: 1=連結する、0=連結しない
 * [出　力] なし
 * [関数値] なし
 * [機　能] フレーズを連続再生するか否かを設定する。
 */
void CRIAPI AIXP_SetLnkSw(AIXP aixp, CriSint32 sw);

/*	$func$ 再生開始フレーズ番号の設定
 * [書　式] void AIXP_SetStartPh(AIXP aixp, CriSint32 phno);
 * [入　力] aixp	: AIXPハンドル
 *			phno	: フレーズ番号
 * [出　力] なし
 * [関数値] なし
 * [機　能] 再生開始フレーズ番号を設定する。
 */
void CRIAPI AIXP_SetStartPh(AIXP aixp, CriSint32 phno);

/*	$func$ 再生開始フレーズ番号の取得
 * [書　式] CriSint32 AIXP_GetStartPh(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] なし
 * [関数値] フレーズ番号
 * [機　能] 再生開始フレーズ番号を取得する。
 */
CriSint32 CRIAPI AIXP_GetStartPh(AIXP aixp);

/*	$func$ ループ情報の設定
 * [書　式] void AIXP_SetLpInfo(AIXP aixp, CriSint32 start_phno, CriSint32 length);
 * [入　力] aixp		: AIXPハンドル
 *			start_phno	: 開始フレーズ番号
 *			length		: ループするフレーズ数
 * [出　力] なし
 * [関数値] なし
 * [機　能] ループ情報(開始フレーズ番号と長さ)を設定する。
 *  Set the loop infomation
 * [Format  ] void AIXP_SetLpInfo(AIXP aixp, CriSint32 start_phno, CriSint32 length);
 * [Inputs  ] AIXP			: AIXP handle
 *			  start_phno	: start phrase number
 *			  length		: loop length in phrase count
 * [Outputs ] None
 * [Return  ] None
 * [Function] Set the loop infomation.
 */
void CRIAPI AIXP_SetLpInfo(AIXP aixp, CriSint32 start_phno, CriSint32 length);

/*	$func$ ループ情報の取得
 * [書　式] void AIXP_GetLpInfo(AIXP aixp, CriSint32 *start_phno, CriSint32 *length);
 * [入　力] aixp		: AIXPハンドル
 *			start_phno	: 開始フレーズ番号を格納する変数へのポインタ
 *			length		: ループするフレーズ数を格納する変数へのポインタ
 * [出　力] なし
 * [関数値] フレーズ番号
 * [機　能] ループスタートフレーズ番号を取得する。
 *  Get the loop infomation
 * [Format  ] void AIXP_GetLpInfo(AIXP aixp, CriSint32 *start_phno, CriSint32 *length);
 * [Inputs  ] AIXP			: AIXP handle
 *			  start_phno	: pointer to start phrase number
 *			  length		: pointer to loop length in phrase count
 * [Outputs ] None
 * [Return  ] None
 * [Function] Get the loop infomation.
 */
void CRIAPI AIXP_GetLpInfo(AIXP aixp, CriSint32 *start_phno, CriSint32 *length);

/*	$func$ 一時停止の設定
 * [書　式] void AIXP_Pause(AIXP aixp, CriSint32 sw);
 * [入　力] AIXP	: AIXPハンドル
 *			sw		: 1=一時停止、0=再開
 * [出　力] なし
 * [関数値] なし
 * [機　能] 一時停止するか否かを設定する。
 *  Pause/Continue
 * [Format  ] void AIXP_Pause(AIXP aixp, CriSint32 sw);
 * [Inputs  ] AIXP	: AIXP handle
 *			  sw	: 1=pause, 0=continue
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops temporarily by a specified switch and release temporary 
 *			  standstill.
 */
void CRIAPI AIXP_Pause(AIXP aixp, CriSint32 sw);

/*	$func$ 一時停止状態の取得
 * [書　式] CriSint32 AIXP_GetStatPause(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] 一時停止状態 1=一時停止、0=非一時停止
 * [関数値] なし
 * [機　能] 一時停止状態を取得する。
 *  Pause/Continue
 * [Format  ] CriSint32 AIXP_GetStatPause(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] 1=pause, 0=not pause
 * [Return  ] None
 * [Function] Obtains the status of pause.
 */
CriSint32 CRIAPI AIXP_GetStatPause(AIXP aixp);

/*			強制モノラル出力スイッチの設定	
[書  式]	void AIXP_SetOutoputMono(CriSint32 flag);
[引  数]	CriSint32 flag		強制モノラル出力フラグ(OFF:0, ON:1)
[戻り値]	なし					
[機  能]	ステレオデータを強制的にモノラルデータとして出力する。
[備　考]	
*/
/* 2003-02-26 現在未実装 */
/* Not implemented... */
/* void CRIAPI AIXP_SetOutputMono(CriSint32 flag); */

/* $func$ 音声データの総サンプル数の取得
 * [書　式] CriSint32 AIXP_GetTotalNumSmpl(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] なし
 * [関数値] 音声データの総サンプル数
 * [機　能] 再生中の AIX データの総サンプル数を取得する。
 * [備　考] 状態が、再生中(AIXP_STAT_PLAYING)から再生終了(AIXP_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get total number of sample of the sound data
 * [Format  ] CriSint32 AIXP_GetTotalNumSmpl(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] None
 * [Return  ] Total number of sample of sound data
 * [Function] Obtains a total number of sample of sound data.
 * [Remarks ] You can obtain when status is between
 *			  playing(AIXP_STAT_PLAYING) and play end(AIXP_STAT_PLAYEND).
 */
CriSint32 CRIAPI AIXP_GetTotalNumSmpl(AIXP aixp);

/* $func$ トラック数の取得
 * [書　式] CriSint32 AIXP_GetNumTrack(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] なし
 * [関数値] トラック数
 * [機　能] 再生中の AIX データのトラック数を取得する。
 * [備　考] 状態が、再生中(AIXP_STAT_PLAYING)から再生終了(AIXP_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get number of the track
 * [Format  ] CriSint32 AIXP_GetNumTrack(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] None
 * [Return  ] Number of the track
 * [Function] Obtains number of the track.
 * [Remarks ] You can obtain when status is between
 *			  playing(AIXP_STAT_PLAYING) and play end(AIXP_STAT_PLAYEND).
 */
CriSint32 CRIAPI AIXP_GetNumTrack(AIXP aixp);

/* $func$ フレーズ数の取得
 * [書　式] CriSint32 AIXP_GetNumPhrase(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] なし
 * [関数値] トラック数
 * [機　能] 再生中の AIX データのフレーズ数を取得する。
 * [備　考] 状態が、再生中(AIXP_STAT_PLAYING)から再生終了(AIXP_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get number of the phrase
 * [Format  ] CriSint32 AIXP_GetNumPhrase(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] None
 * [Return  ] Number of the phrase
 * [Function] Obtains number of the phrase.
 * [Remarks ] You can obtain when status is between
 *			  playing(AIXP_STAT_PLAYING) and play end(AIXP_STAT_PLAYEND).
 */
CriSint32 CRIAPI AIXP_GetNumPhrase(AIXP aixp);

/* $func$ ソング数の取得
 * [書　式] CriSint32 AIXP_GetNumSong(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] なし
 * [関数値] トラック数
 * [機　能] 再生中の AIX データのソング数を取得する。
 * [備　考] 状態が、再生中(AIXP_STAT_PLAYING)から再生終了(AIXP_STAT_PLAYEND)
 *			までの時に取得可能。
 *  Get number of the song
 * [Format  ] CriSint32 AIXP_GetNumSong(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] None
 * [Return  ] Number of the song
 * [Function] Obtains number of the song
 * [Remarks ] You can obtain when status is between
 *			  playing(AIXP_STAT_PLAYING) and play end(AIXP_STAT_PLAYEND).
 */
CriSint32 CRIAPI AIXP_GetNumSong(AIXP aixp);

/*	$func$ 出力ボリュームの取得
 * [書　式] CriSint32 AIXP_GetOutVol(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] なし
 * [関数値] 減衰レベル　(0:-0dB, -999:-99.9dB)
 * [機　能] 出力ボリュームを取得する。
 *  Get volume
 * [Format  ] CriSint32 AIXP_GetOutVol(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] None
 * [Return  ] Volume (0:-dB, -999:-99.9dB)
 * [Function] Gets the volume.
 */
CriSint32 CRIAPI AIXP_GetOutVol(AIXP aixp);

/*	$func$ 出力ボリュームの設定
 * [書　式] void AIXP_SetOutVol(AIXP aixp, CriSint32 vol);
 * [入　力] aixp	: AIXPハンドル
 *			vol		: 減衰レベル　(0:-0dB, -999:-99.9dB)
 * [出　力] なし
 * [関数値] なし
 * [機　能] 出力ボリュームを設定する。
 *			volの設定値	0    :    -0dB  減衰なし
 *						-30  :    -3dB  約70%
 *						-60  :    -6dB  約50%
 *						-999 : -99.9dB  最大の減衰量
 *  Set volume
 * [Format  ] void AIXP_SetOutVol(AIXP aixp, CriSint32 vol);
 * [Inputs  ] aixp	: AIXP handle
 *			  vol	: Volume (0:-dB, -999:-99.9dB)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the volume.
 *			 'vol'value	0    :    -0dB  No attenuation
 *						-30  :    -3dB  about 70%
 *						-60  :    -6dB  about 50%
 *						-999 : -99.9dB  Maximum attenuation
 */
void CRIAPI AIXP_SetOutVol(AIXP aixp, CriSint32 vol);

/*	$func$ トラックごとの出力ボリュームの取得
 * [書　式] CriSint32 AIXP_GetTrackVol(AIXP aixp, CriSint32 trno);
 * [入　力] aixp	: AIXPハンドル
 *          trno    : トラック番号
 * [出　力] なし
 * [関数値] 減衰レベル　(0:-0dB, -999:-99.9dB)
 * [機　能] トラック単位の出力ボリュームを取得する。
 *  Get track volume
 * [Format  ] CriSint32 AIXP_GetTrackVol(AIXP aixp, CriSint32 trno);
 * [Inputs  ] aixp	: AIXP handle
 *            trno  : Track No.
 * [Outputs ] None
 * [Return  ] Volume (0:-dB, -999:-99.9dB)
 * [Function] Gets the volume of specified track.
 */
CriSint32 CRIAPI AIXP_GetTrackVol(AIXP aixp, CriSint32 trno);

/*	$func$ トラックごとの出力ボリュームの設定
 * [書　式] void AIXP_SetTrackVol(AIXP aixp, CriSint32 trno, CriSint32 vol);
 * [入　力] aixp	: AIXPハンドル
 *          trno    : トラック番号
 *			vol		: 減衰レベル　(0:-0dB, -999:-99.9dB)
 * [出　力] なし
 * [関数値] なし
 * [機　能] トラック単位の出力ボリュームを設定する。
 *			volの設定値	0    :    -0dB  減衰なし
 *						-30  :    -3dB  約70%
 *						-60  :    -6dB  約50%
 *						-999 : -99.9dB  最大の減衰量
 *  Set track volume
 * [Format  ] void AIXP_SetTrackVol(AIXP aixp, CriSint32 trno, CriSint32 vol);
 * [Inputs  ] aixp	: AIXP handle
 *            trno  : Track No.
 *			  vol	: Volume (0:-dB, -999:-99.9dB)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the volume of specified track.
 *			 'vol'value	0    :    -0dB  No attenuation
 *						-30  :    -3dB  about 70%
 *						-60  :    -6dB  about 50%
 *						-999 : -99.9dB  Maximum attenuation
 */
void CRIAPI AIXP_SetTrackVol(AIXP aixp, CriSint32 trno, CriSint32 vol);

/* $func$ データ流量(バイトレイト)の取得
 * [書　式] CriSint32 AIXP_GetByteRate(AIXP aixp);
 * [入　力] aixp	: AIXPハンドル
 * [出　力] なし
 * [関数値] データ流量(バイトレイト)
 * [機　能] 再生中の AIX データのデータ流量を取得する。
 * [備　考] 状態が、再生中(AIXP_STAT_PLAYING)から再生終了(AIXP_STAT_PLAYEND)
 *			までの時に取得可能。
 */
CriSint32 CRIAPI AIXP_GetByteRate(AIXP aixp);

/* $func$ フレーズコントロール用コールバック関数の登録
 * [書　式] void AIXP_SetCbPhraseControl(AIXP aixp, AIXP_CBFN_PHCTRL cbfunc, void *obj);
 * [入　力] AIXP	: AIXPハンドル
 *			cbfunc	: フレーズコントロールコールバック関数
 *			obj		: 任意の引き渡しオブジェクト
 * [出　力] なし
 * [関数値] なし
 * [機　能] フレーズコントロール用のコールバック関数を登録します
 * [注　意] 本関数とAIXP_SetNumPreloadPhrases関数とを併用することはできません。
 *  Setting callback function for phrase control
 * [Format  ] void AIXP_SetCbPhraseControl(AIXP aixp, AIXP_CBFN_PHCTRL cbfunc, void *obj);
 * [Inputs  ] AIXP	 : AIXP handle
 *			  cbfunc : callback function for phrase controls.
 *			  obj	 : Object for argument
 * [Outputs ] None
 * [Return  ] None
 * [Function] Setting callback function for phrase controls.
 * [Note    ] It is impossible to use both this function and 
 *            the AIXP_SetNumPreloadPhrases function.
 */
void CRIAPI AIXP_SetCbPhraseControl(AIXP aixp, AIXP_CBFN_PHCTRL cbfunc, void *obj);

/* $func$ プリロードフレーズ数の設定
 * [書　式] void AIXP_SetNumPreloadPhrases(AIXP aixp, CriSint32 num);
 * [入　力] AIXP	: AIXPハンドル
 *			num		: 再生開始時に一括ロードするフレーズ数
 * [出　力] なし
 * [関数値] なし
 * [機　能] 再生開始時に一括ロードするフレーズの数を設定します
 * [備　考] プリロードフレーズ数のデフォルト値は1です。
 *　　　　　ロードするフレーズの数を変更する際には、1以上の値を設定してください。
 * [注　意] 本関数とAIXP_SetCbPhraseControl関数とを併用することはできません。
 *  Set the number of preload phrases
 * [Format  ] void AIXP_SetNumPreloadPhrases(AIXP aixp, CriSint32 num);
 * [Inputs  ] AIXP	 : AIXP handle
 *			  num    : Number of preload phrases
 * [Outputs ] None
 * [Return  ] None
 * [Function] Set the number of phrases that will be preloaded at 
 *            the beginning of the playback.
 * [Remarks ] The default value of 'num' is 1.
 *            You must specify 1 or greater to 'num'.
 * [Note    ] It is impossible to use both this function and 
 *            the AIXP_SetCbPhraseControl function.
 */
void CRIAPI AIXP_SetNumPreloadPhrases(AIXP aixp, CriSint32 num);

/* $func$ プリロードフレーズ数の取得
 * [書　式] CriSint32 AIXP_GetNumPreloadPhrases(AIXP aixp);
 * [入　力] AIXP	: AIXPハンドル
 * [出　力] なし
 * [関数値] 再生開始時に一括ロードするフレーズ数
 * [機　能] 再生開始時に一括ロードするフレーズの数を取得します
 *  Get the number of preload phrases
 * [Format  ] CriSint32 AIXP_GetNumPreloadPhrases(AIXP aixp);
 * [Inputs  ] AIXP	 : AIXP handle
 * [Outputs ] None
 * [Return  ] Number of preload phrases
 * [Function] Get the number of phrases that will be preloaded at 
 *            the beginning of the playback.
 */
CriSint32 CRIAPI AIXP_GetNumPreloadPhrases(AIXP aixp);

/* $func$ デフォルトのサーバ処理周波数の設定
 * [書　式] void AIXP_SetDefSvrFreq(CriSint32 freq);
 * [入　力] freq : サーバ関数の呼び出し頻度（1秒当たりの呼び出し回数）
 * [出　力] なし
 * [関数値] なし
 * [機　能] サーバ関数が一回にデコードする量を制御します。
 *　　　　　デフォルトでは、60（Vsync）が設定されています。
 * [備　考] 本関数は、Vysnc周波数が60Hzではない環境で、デコード量を
 *　　　　　調整するために使用します。
 */
void CRIAPI AIXP_SetDefSvrFreq(CriSint32 freq);

/* $func$ サーバ処理周波数の設定
 * [書　式] void AIXP_SetSvrFreq(AIXP aixp, CriSint32 freq);
 * [入　力] aixp : AIXPハンドル
 *　　　　　freq : サーバ関数の呼び出し頻度（1秒当たりの呼び出し回数）
 * [出　力] なし
 * [関数値] なし
 * [機　能] サーバ関数が一回にデコードする量をハンドル単位で制御します。
 *　　　　　デフォルトでは、60（Vsync）が設定されています。
 * [備　考] 本関数は、音声の再生ピッチを上げる場合等、ハンドル単位で
 *　　　　　データのデコード量を調整したい場合に使用します。
 *　　　　　引数freqの値には「1秒分の音声データを何Vsyncでデコードするか」
 *　　　　　を指定します。
 *　　　　　例えば、30を指定した場合には、60Vsyncでデコードする分の音声
 *　　　　　データを、半分の30Vsyncでデコードしてしまいます。
 */
void CRIAPI AIXP_SetSvrFreq(AIXP aixp, CriSint32 freq);

/* $func$ デコードデータの破棄
 * [書　式] CriSint32 AIXP_DiscardSmpl(AIXP aixp, CriSint32 nsmpl);
 * [入　力] aixp  : AIXPハンドル
 *			nsmpl : 破棄するサンプル数
 * [出　力] なし
 * [関数値] なし
 * [機　能] デコードした音声データを破棄します。
 *［備　考］本関数は、再生位置をスキップするために使用します。
 * [注　意] この関数はADXTハンドルのステータスがADXT_STAT_PLAYINGになっ
 *　　　　　てから呼ぶ必要があります
 *　　　　　また、大量のデータを破棄した場合、出力データの枯渇によりサウ
 *　　　　　ンドバッファ内の音声が部分的にループして再生される場合があり
 *　　　　　ます。
 */
CriSint32 CRIAPI AIXP_DiscardSmpl(AIXP aixp, CriSint32 nsmpl);


/* $func$ 再生開始位置の設定（秒単位）
 * [書　式] void CRIAPI AIXP_SetStartTime(AIXP aixp, CriFloat32 start_time);
 * [入　力] AIXP		: AIXPハンドル
 *			start_time	: 再生開始位置（秒単位）
 * [出　力] なし
 * [関数値] なし
 * [機　能] 再生を開始する位置を秒単位で設定します。
 * [備　考] 再生中（PREPやPLAYEND含む）に設定した場合、次回再生開始時に適用されます。
 * [注　意] (a) 本関数とAIXP_SetStartPh関数、AIXP_SetStartSmpl関数とを併用することはできません。（後に呼び出した方が優先）
 * 			(b) ループ再生には使用できません。
 * 			(c) フレーズを含むデータの再生には使用できません。
 * 			(d) メモリ再生（AIXP_StartMem関数による再生）には使用できません。
 * Set the start position of a playback by the second
 * [Format  ] void CRIAPI AIXP_SetStartTime(AIXP aixp, CriFloat32 start_time);
 * [Inputs  ] AIXP			: AIXP handle
 *			  start_time	: Start position of a playback by the second
 * [Outputs ] None
 * [Return  ] None
 * [Function] This function sets the start position of a playback by the second.
 * [Remarks ] If you call this function after a playback is started,
 *            the setting is applied at the next playback.
 * [Note    ] (a) It is impossible to use both this function and the
 * 				  AIXP_SetStartPh function or the AIXP_SetStartSmpl function.
 * 				  Only the function that has been last called is effective.
 * 			  (b) This function can not be used for a looping playback.
 * 			  (c) This function can not be used for the AIX data which is
 *				  including phrase information.
 * 			  (d) This function can not be used for the playback started by
 * 				  AIXP_StartMem function.
 */
void CRIAPI AIXP_SetStartTime(AIXP aixp, CriFloat32 start_time);

/* $func$ 再生開始位置の取得（秒単位）
 * [書　式] CriFloat32 CRIAPI AIXP_GetStartTime(AIXP aixp);
 * [入　力] AIXP		: AIXPハンドル
 * [出　力] なし
 * [関数値] 再生開始位置（秒単位）
 * [機　能] 再生を開始する位置を秒単位で取得します。
 *  Get the start position of a playback by the second
 * [Format  ] CriFloat32 CRIAPI AIXP_GetStartTime(AIXP aixp);
 * [Inputs  ] AIXP			: AIXP handle
 * [Outputs ] None
 * [Return  ] Start position of a playback by the second
 * [Function] This function gets the start position of a playback by the second.
 */
CriFloat32 CRIAPI AIXP_GetStartTime(AIXP aixp);

/* $func$ 再生開始位置の設定（サンプル単位）
 * [書　式] void CRIAPI AIXP_SetStartSmpl(AIXP aixp, CriSint32 start_smpl);
 * [入　力] AIXP		: AIXPハンドル
 *			start_smpl	: 再生開始位置（サンプル単位）
 * [出　力] なし
 * [関数値] なし
 * [機　能] 再生を開始する位置をサンプル単位で設定します。
 * [備　考] 再生中（PREPやPLAYEND含む）に設定した場合、次回再生開始時に適用されます。
 * [注　意] (a) 本関数とAIXP_SetStartPh関数、AIXP_SetStartTime関数とを併用することはできません。（後に呼び出した方が優先）
 * 			(b) ループ再生には使用できません。
 * 			(c) フレーズを含むデータの再生には使用できません。
 * 			(d) メモリ再生（AIXP_StartMem関数による再生）には使用できません。
 * Set the start position of a playback by the number of samples
 * [Format  ] void CRIAPI AIXP_SetStartSmpl(AIXP aixp, CriSint32 start_smpl);
 * [Inputs  ] AIXP			: AIXP handle
 *			  start_smpl	: Start position of a playback by the number of samples
 * [Outputs ] None
 * [Return  ] None
 * [Function] This function sets the start position of a playback by the second.
 * [Remarks ] If you call this function after a playback is started,
 *            the setting is applied at the next playback.
 * [Note    ] (a) It is impossible to use both this function and the
 * 				  AIXP_SetStartPh function or the AIXP_SetStartTime function.
 * 				  Only the function that has been last called is effective.
 * 			  (b) This function can not be used for a looping playback.
 * 			  (c) This function can not be used for the AIX data which is
 *				  including phrase information.
 * 			  (d) This function can not be used for the playback started by
 * 				  AIXP_StartMem function.
 */
void CRIAPI AIXP_SetStartSmpl(AIXP aixp, CriSint32 start_smpl);

/* $func$ 再生開始位置の取得（サンプル単位）
 * [書　式] CriSint32 CRIAPI AIXP_GetStartSmpl(AIXP aixp);
 * [入　力] AIXP		: AIXPハンドル
 * [出　力] なし
 * [関数値] 再生開始位置（サンプル単位）
 * [機　能] 再生を開始する位置をサンプル単位で取得します。
 *  Get the start position of a playback by the number of samples
 * [Format  ] CriSint32 CRIAPI AIXP_GetStartSmpl(AIXP aixp);
 * [Inputs  ] AIXP			: AIXP handle
 * [Outputs ] None
 * [Return  ] Start position of a playback by the number of samples
 * [Function] This function gets the start position of a playback by the number of samples.
 */
CriSint32 CRIAPI AIXP_GetStartSmpl(AIXP aixp);

/*--------------------------------------------------------------------------*/
/*		振幅取得用関数														*/
/*		Functions for amplitude analysis									*/
/*--------------------------------------------------------------------------*/

/* $func$ 振幅取得用ワーク領域サイズの計算
 * [書　式] CriSint32 AIXP_CalcAmpWork(CriSint32 maxtr, CriSint32 maxnch);
 * [入　力] maxtr	: 最大トラック数
 *          maxnch	: 最大チャンネル数
 * [出　力] なし
 * [関数値] ワーク領域サイズ
 * [機　能] 振幅取得機能を使用するのに必要なワーク領域のサイズを計算します。
 * Work area size calculation
 * [Format  ] CriSint32 AIXP_CalcAmpWork(CriSint32 maxtr, CriSint32 maxnch);
 * [Inputs  ] maxtr	: Number of maximum tracks
 *            maxch	: Number of maximum channels
 * [Outputs ] None
 * [Return  ] Work area size
 * [Function] Calculate work area size used for amplitude analysis.
 */
CriSint32 CRIAPI AIXP_CalcAmpWork(CriSint32 maxtr, CriSint32 maxnch);

/* $func$ 振幅取得用作業領域の設定
 * [書　式] void AIXP_SetAmpWork(AIXP aixp, void *work, CriSint32 wksize);
 * [入　力] AIXP	: AIXPハンドル
 *          work	: ワーク領域
 *          wksize	: ワーク領域サイズ
 * [出　力] なし
 * [関数値] なし
 * [機　能] AIXPハンドルに振幅取得機能を付加します
 * Set the work area used for amplitude analysis
 * [Format  ] void AIXP_SetAmpWork(AIXP aixp, void *work, CriSint32 wksize);
 * [Inputs  ] AIXP		: AIXP handle
 *            work		: Working area
 *            worksize	: Size of working area (Byte)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Set the work area to AIXP handle and enable the amplitude 
 *            analysis function.
 */
void CRIAPI AIXP_SetAmpWork(AIXP aixp, void *work, CriSint32 wksize);

/* $func$ 振幅の取得
 * [書　式] CriSint32 AIXP_GetAmplitude(AIXP aixp, CriSint32 tr, CriSint32 ch);
 * [入　力] AIXP	: AIXPハンドル
 *          tr		: トラック番号
 *          ch		: チャンネル番号
 * [出　力] なし
 * [関数値] なし
 * [機　能] 再生中の音声の振幅を取得します。
 * Get the amplitude of the audio signal
 * [Format  ] CriSint32 AIXP_GetAmplitude(AIXP aixp, CriSint32 tr, CriSint32 ch);
 * [Inputs  ] AIXP		: AIXP handle
 *            tr		: Track number
 *            ch		: Channel number
 * [Outputs ] None
 * [Return  ] None
 * [Function] Get the amplitude of the audio signal under playing.
 */
CriSint32 CRIAPI AIXP_GetAmplitude(AIXP aixp, CriSint32 tr, CriSint32 ch);

/* $func$ 振幅の取得（時刻オフセット指定付き）
 * [書　式] CriSint32 AIXP_GetAmplitude2(
 *              AIXP aixp, CriSint32 tr, CriSint32 ch, CriSint32 msec, CriSint32 *msec2);
 * [入　力] AIXP	: AIXPハンドル
 *          tr		: トラック番号
 *          ch		: チャンネル番号
 *          msec	: 要求時刻（msecミリ秒前の振幅を取得）
 *          msec2	: 取得できた振幅の時刻（実際に遡れた時刻）
 * [出　力] なし
 * [関数値] なし
 * [機　能] 再生前の音声の振幅を事前に取得します。
 * Gets the amplitude of the audio signal (with time offset)
 * [Format  ] CriSint32 AIXP_GetAmplitude2(
 *                AIXP aixp, CriSint32 tr, CriSint32 ch, CriSint32 msec, CriSint32 *msec2);
 * [Inputs  ] AIXP		: AIXP handle
 *            tr		: Track number
 *            ch		: Channel number
 *            msec		: Request time (Get the amplitude which will be 
 *                        played after 'msec' millisecond.)
 *            msec2		: Actual time (actual time of acquired amplitude.)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Get the amplitude of the audio signal before it played.
 */
CriSint32 CRIAPI AIXP_GetAmplitude2(
	AIXP aixp, CriSint32 tr, CriSint32 ch, CriSint32 msec, CriSint32 *msec2);

/*--------------------------------------------------------------------------*/
/*		暗号解除用関数														*/
/*		Functions for encrypted file										*/
/*--------------------------------------------------------------------------*/

/* $func$ キーコードの設定
 * [書　式] void AIXP_SetKeyString(AIXP aixp, CriChar8 *str);
 * [入　力] aixp	: AIXPハンドル
 *          str     : キー文字列
 * [出　力] なし
 * [関数値] なし
 * [機　能] AIXハンドルに対して暗号化解除用のキー文字列を設定する。
 * [備　考] キー文字列が設定された状態でも、暗号化されていないAIXファイル
 *          については再生が可能。
 */
void CRIAPI AIXP_SetKeyString(AIXP aixp, CriChar8 *str);

/*--------------------------------------------------------------------------*/
/*		マルチチャンネル音声再生機能										*/
/*		Functions for multi-channel audio playback							*/
/*--------------------------------------------------------------------------*/
/*	$func$ マルチチャンネル再生機能の組み込み
 * [書　式] void AIXP_AttachMultiChannel(AIXP aixp, void *work, CriSint32 wksize);
 * [入　力] aipx	: AIXPハンドル
 *			work	: マルチチャンネル再生用ワーク領域（=NULL）
 *			wksize	: マルチチャンネル再生用ワーク領域（=0）
 * [出　力] なし
 * [関数値] なし
 * [機　能] AIXPハンドルに対してマルチチャンネル再生機能を追加する。
 *          本関数実行後に再生されるAIX音声は、全てマルチチャンネルオーディオ
 *          として再生される。
 * [備  考] 現状、追加のワーク領域は不要。
 *  Attachs multi-channel playback function
 * [Format  ] void AIXP_AttachMultiChannel(AIXP aixp, void *work, CriSint32 wksize);
 * [Inputs  ] aixp    : AIXP handle
 *			  work    : Working area for multi-channel playback (=NULL)
 *			  worksize: Size of multi-channel playback working area (=0)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Attachs multi-channel playback function.
 *            Once this function has been called,
 *            all AIX data is played as multi-channel audio.
 * [Remarks ] Currently, no additional work area is required.
 */
void CRIAPI AIXP_AttachMultiChannel(AIXP aixp, void *work, CriSint32 wksize);

/*	$func$ マルチチャンネル再生機能の取り外し
 * [書　式] void AIXP_DetachMultiChannel(AIXP aixp);
 * [入　力] aipx	: AIXPハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] AIXPハンドルからマルチチャンネル再生機能を取り外す。
 *  Detachs multi-channel playback function
 * [Format  ] void AIXP_DetachMultiChannel(AIXP aixp);
 * [Inputs  ] aixp    : AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Detachs multi-channel playback function.
 */
void CRIAPI AIXP_DetachMultiChannel(AIXP aixp);

/* $func$ ファイル名指定による再生の開始(PCでのマルチチャンネル出力用)
 * [書　式] void AIXP_StartFnameMultiChannel(AIXP aixp, CriSint8 *fname);
 * [入　力] AIXP	: AIXPハンドル
 *			fname	: ファイル名
 * [出　力] なし
 * [関数値] なし
 * [機　能] fname で指定されたAIXファイルを5.1chで再生する。
 * [備  考] 本マクロは旧バージョンとの互換性のために用意されています。
 *  Play start of AIX data specified file name (support PC multi channel audio output)
 * [Format  ] void AIXP_StartFnameMultiChannel(AIXP aixp, CriSint8 *fname);
 * [Inputs  ] AIXP	: AIXP handle
 *			  fname	: File name
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play 5.1ch AIX file when you specify file name.
 * [Remarks ] This macro is only for compatibility with old version.
 */
#define AIXP_StartFnameMultiChannel(aixp, fname, atr)		\
{															\
	AIXP_AttachMultiChannel(aixp, NULL, 0);					\
	AIXP_StartFname(aixp, fname, atr);						\
}															\

/* $func$ FID 指定による再生の開始(PCでのマルチチャンネル出力用)
 * [書　式] void AIXP_StartAfsMultiChannel(AIXP aixp, CriSint32 patid, CriSint32 fid);
 * [入　力] AIXP	: AIXPハンドル
 *			patid	: パーティション識別子
 *			fid		: ファイル識別子
 * [出　力] なし
 * [関数値] なし
 * [機　能] パーティション識別子とファイル識別子で指定されたAIXデータを5.1chで再生する。
 * [備  考] 本マクロは旧バージョンとの互換性のために用意されています。
 *  Play start of AIX data by specified file ID (support PC multi channel audio output)
 * [Format  ] void AIXP_StartAfsMultiChannel(AIXP aixp, CriSint32 patid, CriSint32 fid);
 * [Inputs  ] AIXP	: AIXP handl
 *			  patid	: Partition ID
 *			  fid	: File ID
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play 5.1ch AIX data when you specify partition ID and file ID.
 * [Remarks ] This macro is only for compatibility with old version.
 */
#define AIXP_StartAfsMultiChannel(aixp, patid, fid)			\
{															\
	AIXP_AttachMultiChannel(aixp, NULL, 0);					\
	AIXP_StartAfs(aixp, patid, fid);						\
}															\

/* $func$ メモリ指定による再生の開始(PCでのマルチチャンネル出力用)
 * [書　式] void AIXP_StartMemMultiChannel(AIXP aixp, void *aixdat, CriSint32 datlen);
 * [入　力] AIXP	: AIXPハンドル
 *			aixdat	: AIXデータのアドレス
 *			datlen	: AIXデータの大きさ
 * [出　力] なし
 * [関数値] なし
 * [機　能] aixdata で指定されたAIXデータを5.1chで再生する。
 * [備  考] 本マクロは旧バージョンとの互換性のために用意されています。
 *  Play start of AIX data on memory (support PC multi channel audio output)
 * [Format  ] void AIXP_StartMemMultiChannel(AIXP aixp, void *aixdat, CriSint32 datlen);
 * [Inputs  ] AIXP	: AIXP handle
 *			  aixdat: Address of AIX data
 *			  datlen: Length of playing AIX data
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play 5.1ch AIX data on memory. Plays to specified length.
 * [Remarks ] This macro is only for compatibility with old version.
 */
#define AIXP_StartMemMultiChannel(aixp, aixdat, datlen)		\
{															\
	AIXP_AttachMultiChannel(aixp, NULL, 0);					\
	AIXP_StartMem(aixp, aixdat, datlen);					\
}															\

/* $func$ スピーカー別ボリューム指定(PCでのマルチチャンネル出力用)
 * [書　式] void AIXP_SetOutVolMultiChannel(AIXP aixp, CriSint32 spkid, CriSint32 vol);
 * [入　力] AIXP	: AIXPハンドル
 *			spkid	: スピーカーID
 *			vol		: ボリューム
 * [出　力] なし
 * [関数値] なし
 * [機　能] spkid で指定されたスピーカーのボリュームを設定する。
 *  Setting volumes to speaker (support PC multi channel audio output)
 * [Format  ] void AIXP_SetOutVolMultiChannel(AIXP aixp, CriSint32 spkid, CriSint32 vol);
 * [Inputs  ] AIXP	: AIXP handle
 *			  spkid	: Speaker ID
 *			  vol	: Volume
 * [Outputs ] None
 * [Return  ] None
 * [Function] Setting volumes specified by spkid.
 */
void CRIAPI AIXP_SetOutVolMultiChannel(AIXP aixp, CriSint32 spkid, CriSint32 vol);

#if defined(XPT_TGT_PC)
/*--------------------------------------------------------------------------*/
/*		PC用拡張機能														*/
/*		Functions for PC													*/
/*--------------------------------------------------------------------------*/
/*	$func$ デフォルトミキサーモードの設定
 * [書　式] void AIXP_SetDefMixerMode(CriSint32 mode);
 * [入　力] mode	: ミキサーモード
 * [出　力] なし
 * [関数値] なし
 * [機　能] ミキサーの動作モードを設定する。
 *			modeの設定値	AIXP_MIX_HARDWARE : ハードウェアミキシング
 *							AIXP_MIX_SOFTWARE : ソフトウェアミキシング
 *  Set default mixer mode
 * [Format  ] void AIXP_SetDefMixerMode(CriSint32 mode);
 * [Inputs  ] mode	: mixer mode
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets the mixer mode.
 *			 'mode'value	AIXP_MIX_HARDWARE : Use hardware mixer
 *							AIXP_MIX_SOFTWARE : Use software mixer
 */
void CRIAPI AIXP_SetDefMixerMode(CriSint32 mode);
#endif

#ifdef __cplusplus
}
#endif

#endif

/* end of file */
