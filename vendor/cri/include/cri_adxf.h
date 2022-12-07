#ifndef	_CRI_ADXF_H_INCLUDED
#define	_CRI_ADXF_H_INCLUDED
/****************************************************************************/
/*																			*/
/*			$title$ ＡＤＸファイルシステム ライブラリ						*/
/*				ADXF (ADX File System) Library								*/
/*																			*/
/*				1998.7.3		written by S.Hosaka							*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*      インクルードファイル												*/
/*      Include file														*/
/****************************************************************************/

#include "cri_xpt.h"
#include "sj.h"

/****************************************************************************/
/*		定数マクロ															*/
/*		MACRO CONSTANT														*/
/****************************************************************************/

/*	Version number of ADX File System	*/
#define	ADXF_VER				"7.71"

/*	ファイル名の文字列長				*/
/*	Maximum number of file name strings	*/
#define ADXF_FNAME_MAX    		(256)

/*	最大パーティション数				*/
/*	Maximum number of partition			*/
#define	ADXF_PART_MAX			(256)

/*	最大ファイル数						*/
/*	Maximum number of file				*/
#define	ADXF_FILE_MAX			(65536)

/*	ADXFハンドルオブジェクトの個数		*/
/*	Maximum number of ADXF handle		*/
#define	ADXF_OBJ_MAX			(16)

/*	ハンドルの状態						*/
/*	Status of handle					*/
#define	ADXF_STAT_STOP			(1)			/*	停止中						*/
											/*	During standstill			*/
#define ADXF_STAT_READING		(2)			/*	データ読み込み中			*/
											/*	During data read-in			*/
#define ADXF_STAT_READEND		(3)			/*	データ読み込み終了			*/
											/*	Data read-in end			*/
#define ADXF_STAT_ERROR			(4)			/*	読み込みエラー発生			*/
											/*	Read-in error outbreak state*/

/*	シークタイプ						*/
/*	Type of seek						*/
#define	ADXF_SEEK_SET			(0)			/*	ファイルの先頭				*/
											/*	The beginning of file		*/
#define ADXF_SEEK_CUR			(1)			/*	現在の位置					*/
											/*	The current position		*/
#define ADXF_SEEK_END			(2)			/*	ファイルの終端				*/
											/*	The end of file				*/

/*	エラーコード						*/
/*	Error code							*/
#define	ADXF_ERR_OK				(0)			/*	正常終了					*/
											/*	Successful					*/
#define	ADXF_ERR_FATAL			(-1)		/*	外部のエラー				*/
											/*	Fatal error					*/
#define	ADXF_ERR_INTERNAL		(-2)		/*	内部のエラー				*/
											/*	Internal error				*/
#define	ADXF_ERR_PRM			(-3)		/*	不正なパラメータ			*/
											/*	Illegal parameter			*/
#define	ADXF_ERR_AFS_FILE		(-4)		/*	不正なAFSファイル			*/
											/*	Illegal AFS file			*/
#define ADXF_ERR_FSIZE			(-5)		/*	ファイルサイズ未取得		*/
											/*	Cannot get file size yet	*/

/*	既定値								*/
/*	Default value						*/
#define	ADXF_DEF_SCT_SIZE		(2048)		/*	CD-ROMのセクタサイズ		*/
											/*	Sector size in CD-ROM		*/
#ifdef __EE__
#define	ADXF_DEF_DMA_ALIGN		(64)		/*	DMAのアライメント			*/
											/*	Alignment of DMA transfer	*/
#else
#define	ADXF_DEF_DMA_ALIGN		(32)		/*	DMAのアライメント			*/
											/*	Alignment of DMA transfer	*/
#endif

#define	ADXF_DEF_REQ_RD_SCT		(512)		/*	リードリクエストサイズ		*/
											/*	read request size			*/

/*	デバッグ用	*/
/*	for Debug	*/
//#define	ADXF_CMD_HSTRY_MAX		(256)		/*	コマンド履歴の個数			*/
#define	ADXF_CMD_HSTRY_MAX		(16)		/*	コマンド履歴の個数			*/
											/*	Number of command history	*/

#define	ADXF_CMD_OPEN			(1)			/*	ADXF_Open					*/
#define	ADXF_CMD_OPEN_AFS		(2)			/*	ADXF_OpenAfs				*/
#define	ADXF_CMD_CLOSE			(3)			/*	ADXF_Close					*/
#define	ADXF_CMD_READ_NW32		(4)			/*	ADXF_ReadNw32				*/
#define	ADXF_CMD_STOP			(5)			/*	ADXF_Stop					*/
#define	ADXF_CMD_SEEK			(6)			/*	ADXF_Seek					*/
#define	ADXF_CMD_STOP_NW		(7)			/*	ADXF_StopNw					*/

#define	ADXF_CMD_NUM_MAX		(16)

#define	ADXF_FMG_TYPE_SHORT		(0)
#define	ADXF_FMG_TYPE_LONG		(1)

/* 旧バージョンとの互換用 */
/* For compatibility with old version */
#define ADXF_GetPtStatEx		ADXF_GetPtStat
/****************************************************************************/
/*		列挙定数															*/
/*		Enumarate Constants													*/
/****************************************************************************/

/****************************************************************************/
/*		処理マクロ															*/
/*		Process MACRO														*/
/****************************************************************************/

/*	パーティション情報に必要な領域の計算						*/
/*	Calculation of necessary area for partition information		*/
//	"6"は、adxf_ptinfoのメンバー数(adxf_ptinfo+size+nfile+nentry+type+rev+curdir+ofst)
#define	ADXF_PTIF_CMN_SZ				((6*sizeof(CriSint32))+(ADXF_FNAME_MAX*sizeof(CriSint8)))
//	実際のファイル管理領域サイズ
#define	ADXF_CALC_PTINFO_REAL_SIZE(n)	(ADXF_PTIF_CMN_SZ+(((n)+1)*sizeof(CriSint16)))
//	４バイトアライメントに調整
#define	ADXF_CALC_PTINFO_SIZE(n)		((ADXF_CALC_PTINFO_REAL_SIZE(n)+sizeof(CriSint16))/4*4)

//	ファイル管理領域サイズ変更(１ファイル当たり２バイト→４バイト)
//	コモン領域サイズは共通
//	実際のファイル管理領域サイズ
#define	ADXF_CALC_PTINFO_FMGL_REAL_SIZE(n)	(ADXF_PTIF_CMN_SZ+(((n)+1)*sizeof(CriSint32)))
//	４バイトアライメントに調整
#define	ADXF_CALC_PTINFO_FMGL_SIZE(n)		((ADXF_CALC_PTINFO_FMGL_REAL_SIZE(n)+sizeof(CriSint32))/4*4)

#define	ADXF_CALC_ADD_PTINFO_SIZE(n)	(ADXF_PTIF_CMN_SZ+((n)*(2*sizeof(CriSint16)+sizeof(CriSint32))))

/*	ロードパーティション用テンポラリバッファのサイズ計算	*/
/*	Calculation of temporary buffer size for load-partition	*/
#define	ADXF_CALC_TMPBUF_SIZE(n)	((((n+1)*8)+(ADXF_DEF_SCT_SIZE-1))/ADXF_DEF_SCT_SIZE*ADXF_DEF_SCT_SIZE)


/****************************************************************************/
/*		データ型															*/
/*      Data type declaration												*/
/****************************************************************************/

#ifndef ADXSTM_DEFINED
#define ADXSTM_DEFINED
typedef void *ADXSTM;
#endif

/*	ADXFハンドル		*/
/*	ADXF handle			*/
#ifndef ADXF_DEFINED
#define	ADXF_DEFINED
typedef	struct _adx_fs {
	CriSint8	used;					/*	使用中か否か						*/
	CriSint8	stat;					/*	ハンドルの状態						*/
	CriSint8	sjflag;					/*	外部から設定されたSJか否か			*/
	CriSint8	stopnw_flg;				/*	停止要求フラグ						*/
	ADXSTM	stm;					/*	ADXSTMハンドル						*/
	SJ		sj;						/*	ストリームジョイント				*/
	CriSint32	fnsct;					/*	ファイルサイズ(セクタ)				*/
	CriSint32	skpos;					/*	アクセスポインタの位置				*/
	CriSint32	rdstpos;				/*	読み込み開始位置					*/
	CriSint32	rqsct;					/*	読み込み要求データ量(セクタ)		*/
	CriSint32	rdsct;					/*	読み込んだデータ量(セクタ)			*/
	CriSint8	*buf;					/*	バッファのアドレス(バイト)			*/
	CriSint32	bsize;					/*	バッファのサイズ(バイト)			*/
	CriSint32	rqrdsct;				/*	実際に読み込み要求される量(セクタ)	*/
	/*	ADXSTM_OpenRange -> ADXSTM_OpenFileRange(1999.8.19)	*/
	CriSint32	ofst;					/*	読み込みファイルへのオフセット		*/
	void	*dir;
} ADX_FS;
typedef	ADX_FS	*ADXF;
#endif

/*	パーティション情報		*/
/*	Partition information	*/
#ifndef ADXF_PTINFO_DEFINED
#define ADXF_PTINFO_DEFINED
typedef struct _adxf_ptinfo {
	struct _adxf_ptinfo	*next;		/*	次のパーティション(NULL:オリジナル)	*/
	CriSint32				size;		/*	パーティション情報領域サイズ		*/
	CriSint32				nfile;		/*	パーティション内のファイル数		*/
	CriUint16				nentry;		/*	パーティション内の登録ファイル数	*/
	CriSint8				type;		/*	オリジナルか追記かのフラグ			*/
//	2002.11.05	S.H.	ファイル管理領域サイズ４バイト対応
//	CriSint8				rev;		/*	予約領域							*/
	CriSint8				fmg_type;	/*	ファイル管理サイズ(0=2byte,1=4byte)	*/
	/*	ADXSTM_OpenRange -> ADXSTM_OpenFileRange(1999.8.19)	*/
	CriSint8				fname[ADXF_FNAME_MAX];	/*	AFSファイル名			*/
	void				*curdir;	/*	カレントディレクトリ(NULL:指定なし)	*/
	CriSint32				ofst;		/*	入れ子AFSファイル用					*/
	CriSint32				top;		/*	ファイル情報の先頭					*/
} ADXF_PTINFO;
#endif

/*	追記ファイルの情報				*/
/*	Information of postscript file	*/
#ifndef ADXF_ADD_DEFINED
#define ADXF_ADD_DEFINED
typedef struct _adxf_add_info {
	CriUint16	flid;			/*	ファイルID							*/
	CriUint16	fnsct;			/*	ファイルサイズ						*/
	CriSint32	ofst;			/*	オフセット							*/
} ADXF_ADD_INFO;
#endif

#ifndef ADXF_CMD_HSTRY_DEFINED
#define ADXF_CMD_HSTRY_DEFINED
typedef struct _adxf_cmd_hstry {
	CriUint8	cmdid;			/*	コマンド(関数)ID					*/
	CriUint8	fg;				/*	関数の入り口か出口かを示すフラグ	*/
	CriUint16	ncall;			/*	コマンドの呼び出し回数				*/
	CriSint32	prm[3];					/*	コマンドパラメータ					*/
} ADXF_CMD_HSTRY;
#endif

/****************************************************************************/
/*		変数宣言															*/
/*		Variable Declaration												*/
/****************************************************************************/

/****************************************************************************/
/*		関数の宣言															*/
/*      Function Declaration												*/
/****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================================
		ライブラリの初期化と終了処理
 		Initialize and Finalize of Library
  ==========================================================================*/

/*  ライブラリの初期化
 * [書　式] void ADXF_Init( void );
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ライブラリを初期化する。
 *  Initialization of library
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Initializes library.
 */
void CRIAPI ADXF_Init(void);

/* ライブラリの終了処理
 * [書　式] void ADXF_Finish(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ライブラリの終了処理をする。
 *  Termination of library
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Finalizes library.
 */
void CRIAPI ADXF_Finish(void);

/*  即時復帰型パーティションのロード
 * [書　式] CriSint32 ADXF_LoadPartitionNw(CriSint32 ptid, const CriChar8 *fname, 
 * 													void *dir, void *ptinfo);
 * [入　力] ptid	:パーティションID
 *			fname	:AFSファイルの名前
 *			dir		:ディレクトリ情報 (GDFS:GDFS_DIRREC)
 *			ptinfo	:パーティション情報読み込み領域
 * [出　力] なし
 * [関数値] エラーコード
 * [機　能] AFSファイルを読み込んで、パーティションIDに設定する。
 *  Load of the partition(No wait)
 * [Inputs  ] ptid   :Partition ID
 *			  fname  :Name of AFS file
 *            dir	 :Directory information (GDFS:GDFS_DIRREC)
 *            ptinfo :Area of partition information
 * [Outputs ] None
 * [Return  ] Error code
 * [Function] Loads AFS file and sets partition ID.
 */
CriSint32 CRIAPI ADXF_LoadPartitionNw(CriSint32 ptid, const CriChar8 *fname, void *dir, void *ptinfo);
CriSint32 CRIAPI ADXF_LoadPartitionFmgLongNw(CriSint32 ptid, const CriChar8 *fname, void *dir, void *ptinfo);

/*  即時復帰型パーティションのロード（範囲指定）
 * [書　式] CriSint32 ADXF_LoadPartitionRangeNw(CriSint32 ptid, const CriChar8 *fname, void *dir,
 * 				CriSint32 offset_sct, CriSint32 range_sct, void *ptinfo);
 * [入　力] ptid		:パーティションID
 *			fname		:AFSファイルの名前
 *			dir			:ディレクトリ情報 (GDFS:GDFS_DIRREC)
 *			offset_sct	: ファイル内のオフセット（セクタ単位）
 *			range_sct	: 範囲（セクタ単位）
 *			ptinfo		:パーティション情報読み込み領域
 * [出　力] なし
 * [関数値] エラーコード
 * [機　能] AFSファイルを読み込んで、パーティションIDに設定する。
 *  Load of the partition(No wait)
 * [Inputs  ] ptid   :Partition ID
 *			  fname  :Name of AFS file
 *            dir	 :Directory information (GDFS:GDFS_DIRREC)
 *            ptinfo :Area of partition information
 * [Outputs ] None
 * [Return  ] Error code
 * [Function] Loads AFS file and sets partition ID.
 */
CriSint32 CRIAPI ADXF_LoadPartitionRangeNw(CriSint32 ptid, const CriChar8 *fname, void *dir, CriSint32 offset_sct, CriSint32 range_sct, void *ptinfo);

/*  即時復帰型パーティションのロード(入れ子AFSファイル)
 * [書　式] CriSint32 ADXF_LoadPartitionFromPtNw(CriSint32 set_ptid, CriSint32 rd_ptid, 
 *											CriSint32 rd_flid, void *ptinfo);
 * [入　力] set_ptid	:ロードするAFSファイルに対して設定するパーティションID
 *			rd_ptid		:読み込むAFSファイルのパーティションID
 *			rd_flid		:読み込むAFSファイルのファイルID
 *			ptinfo		:パーティション情報読み込み領域
 * [出　力] なし
 * [関数値] エラーコード
 * [機　能] AFSファイル内のAFSファイルを読み込んで、パーティションIDに設定する。
 *  Load of the partition(No wait)(nested AFS file)
 * [Inputs  ] set_ptid   :Partition ID for loading and setting
 *			  rd_ptid 	 :Partition ID reading AFS file
 *            rd_flid	 :File ID reading AFS file
 *            ptinfo :Area of partition information
 * [Outputs ] None
 * [Return  ] Error code
 * [Function] Loads AFS file in AFS file and sets partition ID.
 */
CriSint32 CRIAPI ADXF_LoadPartitionFromAfsNw(CriSint32 set_ptid, CriSint32 rd_ptid, CriSint32 rd_flid, void *ptinfo);
CriSint32 CRIAPI ADXF_LoadPartitionFromAfsFmgLongNw(CriSint32 set_ptid, CriSint32 rd_ptid, CriSint32 rd_flid, void *ptinfo);

/*  即時復帰型パーティションのロード(読み込みバッファを指定する)
 * [書　式] CriSint32 ADXF_LoadPtNwEx(CriSint32 ptid, const CriChar8 *fname, void *dir, void *ptinfo, 
 *									void *tmpbuf, CriSint32 tbsize);
 * [入　力] ptid	:パーティションID
 *			fname	:AFSファイルの名前
 *			dir		:ディレクトリ情報 (GDFS:GDFS_DIRREC)
 *			ptinfo	:パーティション情報読み込み領域
 *			tmpbuf	:AFSファイルヘッダ読み込みバッファ
 *			tbsize	:AFSファイルヘッダ読み込みバッファサイズ
 * [出　力] なし
 * [関数値] エラーコード
 * [機　能] AFSファイルを読み込んで、パーティションIDに設定する。
 *  Load of the partition(No wait)(specify reading buffer)
 * [Inputs  ] ptid		:Partition ID
 *			  fname		:Name of AFS file
 *            dir	 	:Directory information (GDFS:GDFS_DIRREC)
 *            ptinfo	:Area of partition information
 *			  tmpbuf	:Buffer reading AFS file
 *			  tbsize	:Size of buffer reading AFS file
 * [Outputs ] None
 * [Return  ] Error code
 * [Function] Loads AFS file and sets partition ID.
 */
CriSint32 CRIAPI ADXF_LoadPtNwEx(CriSint32 ptid, const CriChar8 *fname, void *dir, void *ptinfo, 
						void *tmpbuf, CriSint32 tbsize);
CriSint32 CRIAPI ADXF_LoadPtFmgLongNwEx(CriSint32 ptid, const CriChar8 *fname, void *dir, 
						void *ptinfo, void *tmpbuf, CriSint32 tbsize);

/*  即時復帰型パーティションのロード(入れ子AFSファイル)(読み込みバッファを指定する場合)
 * [書　式] CriSint32 ADXF_LoadPtFromPtNwEx(CriSint32 set_ptid, CriSint32 rd_ptid, CriSint32 rd_flid, 
 *										void *ptinfo, void *tmpbuf, CriSint32 tbsize);
 * [入　力] set_ptid	:ロードするAFSファイルに対して設定するパーティションID
 *			rd_ptid		:読み込むAFSファイルのパーティションID
 *			rd_flid		:読み込むAFSファイルのファイルID
 *			ptinfo		:パーティション情報読み込み領域
 *			tmpbuf		:AFSファイルヘッダ読み込みバッファ
 *			tbsize		:AFSファイルヘッダ読み込みバッファサイズ
 * [出　力] なし
 * [関数値] エラーコード
 * [機　能] AFSファイル名を読み込んで、パーティションIDに設定する。
 *  Load of the partition(No wait)(nested AFS file)(specify reading buffer)
 * [Inputs  ] set_ptid	:Partition ID for loading and setting
 *			  rd_ptid	:Partition ID reading AFS file
 *            rd_flid	:File ID reading AFS file
 *            ptinfo	:Area of partition information
 *			  tmpbuf	:Buffer reading AFS file
 *			  tbsize	:Size of buffer reading AFS file
 * [Outputs ] None
 * [Return  ] Error code
 * [Function] Loads AFS file and sets partition ID.
 */
CriSint32 CRIAPI ADXF_LoadPtFromAfsNwEx(CriSint32 set_ptid, CriSint32 rd_ptid, CriSint32 rd_flid, 
						void *ptinfo, void *tmpbuf, CriSint32 tbsize);
CriSint32 CRIAPI ADXF_LoadPtFromAfsFmgLongNwEx(CriSint32 set_ptid, CriSint32 rd_ptid, CriSint32 rd_flid, 
						void *ptinfo, void *tmpbuf, CriSint32 tbsize);

/*  パーティション情報の読み込み状態の取得
 * [書　式] CriSint32 ADXF_GetPtStat(CriSint32 ptid);
 * [入　力] ptid	:パーティションID
 * [出　力] なし
 * [関数値] パーティション情報の読み込み状態
 * [機　能] AFSファイル名を読み込んで、パーティションIDに設定する。
 *  Get partition loading state
 * [Inputs  ] ptid   :Partition ID
 * [Outputs ] None
 * [Return  ] Partition loading state
 * [Function] Gets partition loading state.
 */
CriSint32 CRIAPI ADXF_GetPtStat(CriSint32 ptid);

/*  パーティション情報読み込みの中止
 * [書　式] void ADXF_StopPtLd(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] パーティション情報の読み込みを中止する。
 *  Stop to load partition information
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops to load partition information.
 */
void CRIAPI ADXF_StopPtLd(void);

/*  パーティション情報サイズの取得
 * [書　式] CriSint32 ADXF_GetPtinfoSize(CriSint32 ptid);
 * [入　力] ptid	:パーティションID
 * [出　力] なし
 * [関数値] パーティション情報サイズ(単位：バイト)
 * [機　能] パーティション情報領域のサイズを取得する。
 *  Get size of partition information
 * [Inputs  ] ptid   :Partition ID
 * [Outputs ] None
 * [Return  ] Size of the partition information(byte)
 * [Function] Gets size of the partition information.
 */
CriSint32 CRIAPI ADXF_GetPtinfoSize(CriSint32 ptid);

/* パーティション名の取得 */
CriChar8 * CRIAPI ADXF_GetFnameFromPt(CriSint32 ptid);

/* パーティション内のファイル数の取得 */
CriSint32 CRIAPI ADXF_GetNumFilesFromPt(CriSint32 ptid);

/* パーティション内のファイルサイズの取得 */
CriSint32 CRIAPI ADXF_GetFileSizeFromPt(CriSint32 ptid, CriSint32 fid);

/* AFSファイルのファイル数の取得(AFSファイルの読み込みはアプリで行う) */
CriSint32 CRIAPI ADXF_GetNumFilesFromAfs(void *buf);

/*==========================================================================
		ファイルの読み込み
		Read file
  ==========================================================================*/

/*	ファイルのオープン
 * [書　式] ADXF ADXF_Open(const CriChar8 *fname, void *atr);
 * [入　力] fname	: ファイル名
 *			atr		: ファイルの属性
 * [出　力] なし
 * [関数値] ADXFハンドル、エラーの場合NULL
 * [機　能] 指定のファイルをオープンし、ADXFハンドルを返す。
 *  Open file
 * [Inputs  ] fname  : File name
 *			  atr    : File attribute
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Opens a file.
 */
ADXF CRIAPI ADXF_Open(const CriChar8 *fname, void *atr);

/*	ファイルのオープン（非同期）
 * [書　式] ADXF ADXF_OpenNw(const CriChar8 *fname, void *atr);
 * [入　力] fname	: ファイル名
 *			atr		: ファイルの属性
 * [出　力] なし
 * [関数値] ADXFハンドル、エラーの場合NULL
 * [機　能] 指定ファイルのオープンを開始し、ADXFハンドルを返す。
 *  Begins the file open
 * [Inputs  ] fname  : File name
 *			  atr    : File attribute
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Begins the file open.
 */
ADXF CRIAPI ADXF_OpenNw(const CriChar8 *fname, void *atr);

/*	ファイルのオープン状態をチェック
 * [書　式] CriBool ADXF_IsOpened(ADXF adxf);
 * [入　力] adxf : ADXFハンドル
 * [出　力] なし
 * [関数値] オープン状態（TRUE = オプープンされている、FALSE = オープン処理中）
 * [機　能] ファイルがオープンされているかどうかをチェックする。
 *  Checks the open status
 * [Inputs  ] adxf : ADXF handle
 * [Outputs ] None
 * [Return  ] Open status (TRUE = opened, FALSE = not opened)
 * [Function] Checks whether the file was opened.
 */
CriBool CRIAPI ADXF_IsOpened(ADXF adxf);

/*	ファイルのオープン（セクタ範囲指定付き）
 * [書　式] ADXF ADXF_OpenRange(const CriChar8 *fname, void *atr, CriSint32 offset_sct, CriSint32 range_sct);
 * [入　力] fname		: ファイル名
 *			atr			: ファイルの属性
 *			offset_sct	: ファイル内のオフセット（セクタ単位）
 *			range_sct	: 範囲（セクタ単位）
 * [出　力] なし
 * [関数値] ADXFハンドル、エラーの場合NULL
 * [機　能] 指定のファイルをオープンし、ADXFハンドルを返す。
 *  Open file with internal range
 * [Inputs  ] fname  		: File name
 *			  atr    		: File attribute
 *			  offset_sct	: File offset from top of the file as a sector
 *			  range_sct		: File range as a sector
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Opens a file with internal range.
 */
ADXF CRIAPI ADXF_OpenRange(const CriChar8 *fname, void *atr, CriSint32 offset_sct, CriSint32 range_sct);

/*	ファイルのオープン(AFSフォーマット)
 * [書　式] ADXF ADXF_OpenAfs(CriSint32 ptid, CriSint32 flid);
 * [入　力] ptid	: パーティションID
 *			flid	: ファイルID
 * [出　力] なし
 * [関数値] ADXFハンドル、エラーの場合NULL
 * [機　能] パーティションIDとファイルIDで指定されたAFSファイルをオープンし、
 *			ADXFハンドルを返す。
 *  Open file(AFS format)
 * [Inputs  ] ptid   : Partition ID
 *			  flid   : File ID
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Opens a file of AFS format.
 */
ADXF CRIAPI ADXF_OpenAfs(CriSint32 ptid, CriSint32 flid);

/*	ファイルのオープン（AFSフォーマット、非同期）
 * [書　式] ADXF ADXF_OpenAfsNw(CriSint32 ptid, CriSint32 flid);
 * [入　力] ptid	: パーティションID
 *			flid	: ファイルID
 * [出　力] なし
 * [関数値] ADXFハンドル、エラーの場合NULL
 * [機　能] パーティションIDとファイルIDで指定されたAFSファイルのオープンを開始する。
 *  Begins the file open (AFS format)
 * [Inputs  ] ptid   : Partition ID
 *			  flid   : File ID
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Begins the AFS file open.
 */
ADXF CRIAPI ADXF_OpenAfsNw(CriSint32 ptid, CriSint32 flid);

/* ファイルのクローズ
 * [書　式] void ADXF_Close(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] なし
 * [機　能] 指定されたADXFハンドルのファイルをクローズする。
 *  Close file
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Closes a file of specific ADXF handle.
 */
void CRIAPI ADXF_Close(ADXF adxf);

/* 全てのファイルクローズ
 * [書　式] void ADXF_CloseAll(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] ADXFハンドルのファイルを全てクローズする。
 *  Close file
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Closes all file of ADXF handle.
 */
void CRIAPI ADXF_CloseAll(void);

/*  SJへのデータ読み込み開始
 * [書　式] void ADXF_ReadSj(ADXF adxf, CriSint32 nsct, SJ sj);
 * [入　力] adxf	: ADXFハンドル
 *			nsct	: 読み込みデータの量(単位：セクタ)
 *			SJ		: ストリームジョイント
 * [出　力] なし
 * [関数値] 読み込みデータ量(単位：セクタ)
 * [機　能] ストリームジョイントにデータ読み込みのリクエストを発行する。
 *			ストリームジョイントのバッファサイズは、セクタの整数倍でなければ
 *			ならない。
 *			ストリームジョイントからユーザがデータを読み出すと、自動的にストリー
 *			ムジョイントにデータが読み込まれる。
 *			リクエストしたアクセス動作が完了すると、アクセスポインタは
 *			nsctセクタ進む。
 *  Read-in start to Stream Joint
 * [Inputs  ] adxf   : ADXF handle
 *			  nsct   : Number of read-in sectors to request
 *			  SJ     : Stream Joint
 * [Outputs ] None
 * [Return  ] Number of sectors to read
 * [Function] Requests data read-in to Stream Joint.
 *			  Buffer size of Stream Joint must be multiples of integer.
 *			  If you read data from Stream Joint, data are read into Stream 
 *			  Joint automatically.
 *			  The access pointer moves 'nsct' sector minute when the request 
 *			  was completed.
 */
CriSint32 CRIAPI ADXF_ReadSj(ADXF adxf, CriSint32 nsct, SJ sj);

/* データの読み込み開始
 * [書　式] void ADXF_ReadNw(ADXF adxf, CriSint32 nsct, void *buf);
 * [入　力] adxf	: ADXFハンドル
 *			nsct	: 読み込みデータの量(単位：セクタ)
 *			buf		: 読み込み領域
 * [出　力] なし
 * [関数値] 読み込みデータ量(単位：セクタ)
 * [機　能] データ読み込みのリクエストを発行する。
 *			リクエストしたアクセス動作が完了すると、アクセスポインタは
 *			nsctセクタ進む。
 *  Read-in start
 * [Inputs  ] adxf   : ADXF handle
 *			  nsct   : Number of read-in sectors to request
 *			  buf    : Pointer to buffer into which the data is read
 * [Outputs ] None
 * [Return  ] Number of sectors to read
 * [Function] Requests data read-in to Stream Joint.
 *			  The access pointer moves 'nsct' sector minute when the request 
 *			  was completed.
 */
CriSint32 CRIAPI ADXF_ReadNw(ADXF adxf, CriSint32 nsct, void *buf);

/* データの読み込み中止
 * [書　式] CriSint32 ADXF_Stop(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] 中止した時点でのアクセスポインタの位置
 * [機　能] データの読み込みを中止する。
 *  Read-in stop
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] The location of access pointer when you stop to read
 * [Function] Stops to read data.
 */
CriSint32 CRIAPI ADXF_Stop(ADXF adxf);

/* データの読み込み中止 (即時復帰)
 * [書　式] CriSint32 ADXF_Stop(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] 中止をリクエスト時点でのアクセスポインタの位置
 * [機　能] データの読み込みを中止をリクエストする。
 *			状態はすぐには変らず、読み込みを停止したときにSTOPに変化する
 *  Read-in stop
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] The location of access pointer when you request to stop
 * [Function] Request to Stop.
 */
CriSint32 CRIAPI ADXF_StopNw(ADXF adxf);

/* サーバ関数
 * [書　式] void ADXF_ExecServer(void);
 * [入　力] なし
 * [出　力] なし
 * [関数値] なし
 * [機　能] 内部状態を更新する。
 *  Server function
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status of library.
 */
void CRIAPI ADXF_ExecServer(void);

/*==========================================================================
		アクセスポインタの制御
		Control access pointer
  ==========================================================================*/

/*  アクセスポインタの移動
 * [書　式] CriSint32 ADXF_Seek(ADXF adxf, CriSint32 pos, CriSint32 type);
 * [入　力] adxf	: ADXFハンドル
 *			pos		: アクセスポインタの移動量(単位：セクタ)
 *			type	: 移動基準(シークタイプ：ADXF_SEEK_～)
 * [出　力] なし
 * [関数値] アクセスポインタの位置、負の値はエラーコード
 * [機　能] アクセスポインタをtypeからposセクタ離れた位置に移動させる。
 *  Move access pointer
 * [Inputs  ] adxf   : ADXF handle
 *			  pos    : Offset relative to 'type'
 *			  type   : Specified the origin for the offset
 * [Outputs ] None
 * [Return  ] Location containing new seek pointer
 * [Function] Changes the seek pointer to a new location relative to the 
 *			  beginning of the file, to the end of the file, or to the current 
 *			  seek pointer.
 */
CriSint32 CRIAPI ADXF_Seek(ADXF adxf, CriSint32 pos, CriSint32 type);

/*  アクセスポインタの取得
 * [書　式] CriSint32 ADXF_Tell(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] アクセスポインタの位置
 * [機　能] アクセスポインタの位置を取得する。
 *  Get the access pointer
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] Access pointer
 * [Function] Gets the access pointer.
 */
CriSint32 CRIAPI ADXF_Tell(ADXF adxf);

/*==========================================================================
		情報の取得
		Get information
  ==========================================================================*/

/*  ファイルサイズの取得
 * [書　式] CriSint32 ADXF_GetFsizeSct(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] ファイルサイズ(単位：セクタ)
 * [機　能] 指定されたファイルのサイズをセクタ単位で取得する。
 *  Get file size
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] File size(sector)
 * [Function] Obtains the size of the specified file.
 */
CriSint32 CRIAPI ADXF_GetFsizeSct(ADXF adxf);

/*  ファイルサイズの取得
 * [書　式] CriSint32 ADXF_GetFsizeByte(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] ファイルサイズ(単位：バイト)
 * [機　能] 指定されたファイルのサイズをバイト単位で取得する。
 *  Get file size
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] File size(byte)
 * [Function] Obtains the size of the specified file.
 */
CriSint32 CRIAPI ADXF_GetFsizeByte(ADXF adxf);
CriSint64 CRIAPI ADXF_GetFsizeByte64(ADXF adxf);

/*  読み込み要求情報の取得
 * [書　式] CriSint32 ADXF_GetNumReqSct(ADXF adxf, CriSint32 *seekpos);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] seekpos	: 読み込み位置
 * [関数値] 要求した読み込みデータ量(単位：セクタ)
 * [機　能] 要求した読み込み位置とデータ量を取得する。
 *  Get request information
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] seekpos: location of start to read
 * [Return  ] Request data size(sector)
 * [Function] Obtains read-in start position and the read-in size that 
 *			  demanded.
 */
CriSint32 CRIAPI ADXF_GetNumReqSct(ADXF adxf, CriSint32 *seekpos);

/*  実際に読み込んだセクタ数の取得
 * [書　式] CriSint32 ADXF_GetNumReadSct(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] 読み込んだデータ量(単位：セクタ)
 * [機　能] 実際に読み込んだデータ量を取得する。
 *  Get the number of sectors read
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] Size of read data(sector)
 * [Function] Obtains the size of data that read actually.
 */
CriSint32 CRIAPI ADXF_GetNumReadSct(ADXF adxf);

/*  ハンドルの状態の取得
 * [書　式] CriSint32 ADXF_GetStat(ADXF adxf);
 * [入　力] adxf	: ADXFハンドル
 * [出　力] なし
 * [関数値] ADXFハンドルの内部状態(状態：ADXF_STAT_～)
 * [機　能] ADXFハンドルの内部状態を取得する。
 *  Get status of handle
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] Status of handle
 * [Function] Gets status of handle.
 */
CriSint32 CRIAPI ADXF_GetStat(ADXF adxf);

CriSint32 CRIAPI ADXF_GetFnameRange(CriSint32 ptid, CriSint32 flid, 
	CriChar8 *fname, CriSint32 *ofst, CriSint32 *fnsct);

CriSint32 CRIAPI ADXF_GetFnameRangeEx(CriSint32 ptid, CriSint32 flid, 
	CriChar8 *fname, void **dir, CriSint32 *ofst, CriSint32 *fnsct);

/*　リードリクエストのサイズの設定
 * [書　式] void ADXF_SetReqRdSct(ADXF adxf, CriSint32 nsct);
 * [入　力] adxf    ; ADXFハンドル
 *			nsct    ; セクタ数
 * [出　力] なし
 * [関数値] なし
 * [機　能] 一回に読み込むサイズを設定します。
 *  Set size of read request
 * [Inputs  ] adxf   : ADXF handle
 *			  nsct   : size(sector)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets size of read request(one time).
 */
void CRIAPI ADXF_SetReqRdSct(ADXF adxf, CriSint32 nsct);

/*	ファイル読み込みステータスの取得
 * [書　式] CriSint32 ADXT_GetStatRead(ADXT adxt);
 * [入　力] adxt	: ADXTハンドル
 * [出　力] なし
 * [関数値] 1=読み込み中、0=読み込み停止中
 * [機　能] 読み込み状態を取得する
 *  Get status of data reading
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] 1=Data reading in progress,  0=Data reading stopped
 * [Function] Gets status of data reading
 */
CriSint32 CRIAPI ADXF_GetStatRead(ADXF adxf);

/*	ファイルの有無の確認
 * [書  式] CriBool ADXF_IsExistFile(const CriChar8 *fname);
 * [入  力] fname:ファイル名
 * [出  力] なし
 * [戻り値] 1=ファイルが存在する、0=ファイルが存在しない
 * [説  明] 指定されたファイルが存在するかどうか確認する
 * [補  足] ファイルが見つからない場合でもエラーコールバックは発生しない
 *	Check of file existence
 * [Input   ] fname:file name
 * [Output  ] None
 * [Return  ] 1=file exist, 0=otherwise
 * [Function] Checks whether file exist
 * [Remarks ] This function does not return error callback even if file does not exist
 */
CriBool CRIAPI ADXF_IsExistFile(const CriChar8 *fname);

/*	ファイルサイズの取得
 * [書  式] CriSint32 ADXF_GetFileSize(const Char *fname);
 * [入  力] fname:ファイル名
 * [出  力] なし
 * [戻り値] ファイルサイズ(単位：バイト)
 * [説  明] 指定されたファイル名のファイルサイズをバイト単位で取得します。
 *  Get file size
 * [Input   ] fname:file name
 * [Output  ] None
 * [Return  ] file size (unit : byte)
 * [Function] Gets file size specified file name
 */
CriSint32 CRIAPI ADXF_GetFileSize(const CriChar8 *fname);
CriSint64 CRIAPI ADXF_GetFileSize64(const CriChar8 *fname);

/*==========================================================================
		ROFS用
		for ROFS
  ==========================================================================*/
#if 0

/* 最大文字数 */
#define ADXF_ROFS_VOLNAME_LEN		(8)		/* ボリューム名の文字列長 */
#define ADXF_ROFS_VOLNAME_SIZ		(ADXF_ROFS_VOLNAME_LEN + 1)	/* 文字列サイズ */
#define ADXF_ROFS_FNAME_LEN			(31)	/* ファイル名の文字列長 */
#define ADXF_ROFS_FNAME_SIZ			(ADXF_ROFS_FNAME_LEN + 1)	/* 文字列サイズ */
#define ADXF_ROFS_DIRNAME_LEN		(31)	/* ディレクトリ名の文字列長 */
#define ADXF_RPFS_DIRNAME_SIZ		(ADXF_ROFS_DIRNAME_LEN+1)	/* 文字列サイズ */

/* ライブラリ作業領域サイズ */
#define ADXF_CALC_LIBWORK_SIZ(max_open,max_volume,max_dirent) (sizeof(ADXF_ROFS_WK)+((max_open)+1)*sizeof(ADXF_ROFS_OBJ)+(ADXF_CALC_DIRREC_SIZ(max_dirent)+sizeof(ADXF_ROFS_VOL))*((max_volume)+1))

/* ディレクトリレコード領域サイズ */
#define ADXF_CALC_DIRREC_SIZ(n_dirent) (sizeof(ADXF_ROFS_DIRREC)-sizeof(ADXF_ROFS_DIRRECENT)+(n_dirent)*sizeof(ADXF_ROFS_DIRRECENT))

/* ワーク領域 */
typedef struct ADXF_ROFS_STWK		ADXF_ROFS_WK;

/* プリミティブ関数 */
typedef struct ADXF_ROFS_PFSIFTBL	ADXF_ROFS_PFSTBL;

/* デバイスコントロール関数 */
typedef struct ADXF_ROFS_DEVIFTBLE	ADXF_ROFS_DEVTBL;

/* ライブラリインターフェース */
typedef struct ADXF_ROFS_LIBIFTBLE	ADXF_ROFS_LIFTBL;

/* ディレクトリレコードエントリ */
typedef struct {
	CriSint32	fad;						/* fad */
	CriSint32	fsize;						/* file size */
	CriUint8	flag;						/* file attribute */
	CriUint8	sid;						/* filename search id */
	CriChar8	fname[ADXF_ROFS_FNAME_SIZ];		/* filename */
	CriUint8	pad[6];
} ADXF_ROFS_DIRRECENT;						/* 48 bytes */

/* ディレクトリレコード */
typedef struct {
	CriSint32	dir_num;					/* number of record */
	CriSint32	max_ent;					/* max entry of directory record */
	CriSint32	dir_fad;					/* fad of directory record */
	CriChar8	volname[ADXF_ROFS_FNAME_SIZ];	/* volume name */
	CriSint32	pad;						/* padding */
	ADXF_ROFS_DIRRECENT	dirrec_tbl[1];		/* record table */
} ADXF_ROFS_DIRREC;
typedef ADXF_ROFS_DIRREC	*ADXF_ROFS_DIRRECBUF;

/* ボリューム管理構造体 */
typedef struct {
	CriChar8	volname[ADXF_ROFS_VOLNAME_SIZ];	/* ボリューム名 */
	void	*img_hn;					/* イメージファイルのファイルハンドル */
	CriSint32	zisb;						/* ZONE0イメージデータ開始位置 */
	CriSint32	ptbl_cba;					/* cba of path table */
	CriSint32	ptbl_size;					/* size of path table */
	ADXF_ROFS_DIRRECBUF	curdir;				/* current directory handle */
} ADXF_ROFS_VOL;

/* ROFS File Handle */
typedef struct {
	ADXF_ROFS_WK	*wk;					/* pointer of lib work */
	CriSint32		fid;					/* file id */
	CriSint32		fad;					/* fad */
	CriSint32		ofs;					/* offset */
	CriSint32		fsize;					/* file size */
	CriSint32		fsctsize;				/* sctor size of the file */
	ADXF_ROFS_VOL	*vol;					/* image file volume */
	CriSint32		rsize;					/* reading size */
	CriSint32		trns_seg;				/* 転送済ブロック単位 */
	CriSint32		trns_ofs;				/* 転送済バイト単位 */
	CriSint32		trns_unit;				/* 転送単位(ブロック) */
	CriUint8		*rdadr;					/* 読み込みアドレス */
	CriSint16		used;					/* used flag */
	CriSint16		act;					/* handle act */
	CriSint16		stat;					/* handle status(ADXF_Stat) */
	CriSint16		err;					/* error status */
} ADXF_ROFS_OBJ;
typedef ADXF_ROFS_OBJ	*ADXF_ROFS;

/* ROFS Work Area */
struct ADXF_ROFS_STWK {
	CriBool			f_init;				/* 初期化フラグ */
	CriSint32			max_open;			/* max open files */
	CriSint32			max_volume;
	CriSint32			max_dirent;
	CriUint32			exec_server_cnt;	/* counter */
	ADXF_ROFS			syshdl;				/* handle for system command */
	ADXF_ROFS			hndtbl;				/* handle */
	ADXF_ROFS_VOL		*vollist;			/* ボリューム情報リスト */
	ADXF_ROFS_VOL		*curvol;			/* カレントボリューム情報 */
	ADXF_ROFS_DIRRECBUF	dirbuf;				/* ディレクトリレコードの開始位置 */
	ADXF_ROFS_PFSTBL		*pfs;				/* プリミティブ関数 */
	ADXF_ROFS_DEVTBL		*dev;				/* デバイスコントロール関数 */
	ADXF_ROFS_LIFTBL			*liftbl;			/* ライブラリインターフェース */
	void	(*g_errcb)(void *, CriChar8 *, CriSint32);	/* error callback */
	void			*g_errcb_1st;		/* error callback 1st arg. */
	CriUint32	sctbuf[(ADXF_DEF_SCT_SIZE*2+64)/4];	/* sector buffer */
	ADXF_ROFS_OBJ		hndlist[1];			/* handle table */
}; /* 64 + 4160 + handles */


/* ROFSセットアップ構造体  */
#ifndef ADXF_SPRM_ROFS_DEFINED
#define ADXF_SPRM_ROFS_DEFINED
typedef struct {
	CriSint32	max_open;				/* 同時にオープンできる最大ファイル数 */
	CriSint32	max_volume;				/* 同時に登録できる最大ボリューム数 */
	CriSint32	max_dirent;				/* ディレクトリに格納する最大ファイル数 */
	void	*rofs_work;				/* ライブラリ作業領域の先頭アドレス */
} ADXF_SPRM_ROFS;
#endif

#endif

/* ROFSのセットアップ	*/
/* Setup ROFS			*/
void CRIAPI ADXF_SetupRofs(void *sprmr);

/* ROFSのシャットダウン */
/* Shutdown ROFS		*/
void CRIAPI ADXF_ShutdownRofs(void);

/* ROFSボリュームの追加(スクランブルキー対応)	*/
/* Addition of ROFS volume(with Scramble key)	*/
/* この関数の仕様は変更されました								*/
/* スクランブルキー未使用の場合はscrkeyにNULLを指定してください	*/ 
CriSint32 CRIAPI ADXF_AddRofsVol(CriChar8 *volname, CriChar8 *imgname, CriChar8 *scrkey);

/* ROFSボリュームの削除 */
/* Delete ROFS volume	*/
void CRIAPI ADXF_DelRofsVol(CriChar8 *volname);

/* ROFSボリューム名であるか否か */
/* Check ROFS volume name		*/
CriBool CRIAPI ADXF_IsRofsVol(CriChar8 *volname);

/* デフォルトデバイスの設定 */
/* Setting default device	*/
void CRIAPI ADXF_SetDefDev(CriChar8 *volname);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif	/* _CRI_ADXF_H_INCLUDED */
