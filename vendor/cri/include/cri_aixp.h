#ifndef	_AIXP_H_INCLUDED
#define	_AIXP_H_INCLUDED
/****************************************************************************/
/*																			*/
/*			$title$  AIX �Đ� ���C�u����									*/
/*																			*/
/*				2001.7.29		written M.Oshimi							*/
/*																			*/
/****************************************************************************/

/*	�o�[�W�����ԍ�						*/
/*	Version number of AIXP library 		*/
#define	AIXP_VER	"3.13"

#include "cri_xpt.h"
#include "sj.h"
#include "cri_adxt.h"

/*	�X�g���[���R���g���[��				*/
/*	Stream Controller					*/
#ifndef ADXSTM_DEFINED
#define ADXSTM_DEFINED
typedef	void	*ADXSTM;
#endif

/*	�X�g���[���W���C���g�f�R�[�_		*/
/*	Stream Joint decoder				*/
#ifndef AIXSJD_DEFINED
#define AIXSJD_DEFINED
typedef void	*AIXSJD;
#endif

/*	�I�[�f�B�I�����_��					*/
/*	Audio Renderer						*/
#ifndef ADXRNA_DEFINED
#define ADXRNA_DEFINED
typedef void	*ADXRNA;
#endif

/****************************************************************************/
/*		�萔�}�N��															*/
/*		MACRO CONSTANT														*/
/****************************************************************************/

/*	�ő哯��������						*/
/*	Maximum number of AIXP handle		*/
#define	AIXP_MAX_OBJ		(4)

/*	�ő�g���b�N��						*/
/*	Maximum number of track				*/
#define	AIXP_MAX_TR			(32)

/*	�ő�T���v�����O���g��				*/
/*	Maximum sampling frequency			*/
#define	AIXP_MAX_SFREQ		(48000)

/*	$define$ AIX Talk�̓�����(AIXP_STAT_�`)	*/
/*	Status of AIX Talk							*/
#define	AIXP_STAT_STOP		(0)		/*	��~�� 								*/
									/*	During standstill					*/
#define AIXP_STAT_PREP		(1)		/*	�Đ������� 							*/
									/*	During play preparation				*/
#define AIXP_STAT_PLAYING	(2)		/*	�f�R�[�h���Đ��� 					*/
									/*	During decode and play				*/
#define AIXP_STAT_PLAYEND	(3)		/*	�Đ��I�� 							*/
									/*	Play end							*/
#define AIXP_STAT_ERROR		(4)		/*	�Đ��I�� 							*/
									/*	Play end							*/

/*	�ő�Đ��`�����l����				*/
/*	Maximum number of play channel		*/
#define	AIXP_MAX_NCH		(2)

/*	�}���`�`���l���o�͗p�p�����[�^		*/
/*	Parameters for multi channal output	*/
#define AIXP_MC_NTR			(3)
#define AIXP_MC_NCH			(6)

/*	�T�[�o�֐��̌Ăяo�����p�x�̋K��l�@60(��/�b)				*/
/*	Default value of frequency called server function(times/sec)	*/
#define AIXP_DEF_SVRFREQ	(60)

/*	Default value of output volume(dB)	*/
#define	AIXP_DEF_OUTVOL		(0)

/*	�t�@�C�����̍ő咷	*/
/*	Maximum length of the file path		*/
#define AIXP_MAX_FNAME		(256)

#define AIXP_CALC_WORK_COMMON(nch, nstm, sfreq, ntr)	\
	((ADXT_CALC_IBUFSIZE(nch, nstm, sfreq) + (ADXT_CALC_WORK(nch, ADXT_PLY_MEM, nstm, sfreq) - 64)) * (ntr) + 64)
#define AIXP_CALC_WORK_COMMON_3D(nstm, sfreq, ntr)	\
	((ADXT_CALC_IBUFSIZE_3D(nstm, sfreq) + (ADXT_CALC_WORK_3D(ADXT_PLY_MEM, nstm, sfreq) - 64)) * (ntr) + 64)

/* ���[�N�T�C�Y	*/
/* Work size	*/
#ifdef XPT_TGT_PC
#define AIXP_CALC_WORK(nch, nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON(nch, nstm, sfreq, ntr) + ADXT_CALC_OBUFSIZE(nch))
#else	// #ifdef XPT_TGT_PC
#define AIXP_CALC_WORK(nch, nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON(nch, nstm, sfreq, ntr))
#endif	// #ifdef XPT_TGT_PC

/* ���A���^�C��3D�p���j���O���s���ۂ̃��[�N�T�C�Y				*/
/* Work size to create handle that can use realtime 3D panning	*/
#ifdef XPT_TGT_PC
#define AIXP_CALC_WORK_3D(nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON_3D(nstm, sfreq, ntr) + ADXT_OBUFSIZE_3D)
#else
#define AIXP_CALC_WORK_3D(nstm, sfreq, ntr)	\
	(AIXP_CALC_WORK_COMMON_3D(nstm, sfreq, ntr))
#endif

/* �}���`�t���[�Y�Đ��p���[�N�̈�										*/
/* Work size for multi phrase playback (required on 5 or more phases)	*/
#define AIXP_CALC_PHRASE_WORK(nph)			(((nph) * 16) + 64)

/*	���̓o�b�t�@�̃G�L�X�g���̈�̑傫��	*/
/*	Size of Extra area in input buffer		*/
#define AIXP_IBUF_XLEN (8*1024)

/*	AIX Talk �̏o�̓o�b�t�@�̑傫���@(�T���v���P��)	*/
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

/*	���̓o�b�t�@�ւ̓ǂݍ��ݗ�						*/
/*	Number of sectors to read into the input buffer	*/
#define AIXP_MAX_CDBSCT		(75)
#define AIXP_MIN_CDBSCT		(65)
#define AIXP_PREP_RDSCT		(25)

/*	�p���|�b�g�̐ݒ�l		*/
/*	Panpot parameter		*/
#define	AIXP_PAN_LEFT		(-15)
#define	AIXP_PAN_CENTER		(0)
#define	AIXP_PAN_RIGHT		(15)
#define	AIXP_PAN_AUTO		(-128)	/*	MONO/STE �ɂ���Ď����I�ɐ؂�ւ���	*/
									/*	Changes automatically by data		*/

/*	�X�e���I�Đ����̃`�����l���ԍ�		*/
/*	Channel number playing stereo data	*/
/*	AIXP_CH_L:left, AIXP_CH_R:right		*/
#define	AIXP_CH_L			(0)
#define	AIXP_CH_R			(1)

/*	5.1ch�Đ����̃X�s�[�J�ԍ�			*/
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

/*	�Đ����[�h	*/
/*	Play mode	*/
#define	AIXP_PMODE_FNAME	(0)		/*	Play specified file name	*/
#define	AIXP_PMODE_AFS		(1)		/*	Play specified file ID		*/
#define	AIXP_PMODE_MEM		(2)		/*	Play data from memory		*/
#define	AIXP_PMODE_SJ		(3)		/*	Play data from StreamJoint	*/

/*	�~�L�T�[���샂�[�h	*/
/*	Mixer mode			*/
#define AIXP_MIX_HARDWARE	(0)
#define AIXP_MIX_SOFTWARE	(1)

/* ���o�[�W�����Ƃ̌݊��p */
/* For compatibility with old version */
#define AIXP_StartFnameDolbyDigital		AIXP_StartFnameMultiChannel
#define AIXP_StartAfsDolbyDigital		AIXP_StartAfsMultiChannel
#define AIXP_StartMemDolbyDigital		AIXP_StartMemMultiChannel
#define AIXP_SetOutVolDolbyDigital		AIXP_SetOutVolMultiChannel

/****************************************************************************/
/*		�f�[�^�^															*/
/*      Data type declaration												*/
/****************************************************************************/

/* �t���[�Y�R���g���[���p�R�[���o�b�N�֐� */
/*	Structure of phrase control           */
typedef CriSint32 (*AIXP_CBFN_PHCTRL)(void *obj, CriSint32 ph_now, CriSint32 ph_max);

#if defined(XPT_TGT_PC)
/* �U�����o�@�\�p�\���� */
typedef struct {
	void *amp;
	SJ sji[AIXP_MAX_NCH];
	SJ sjo[AIXP_MAX_NCH];
	void *context;
} AIXAMP;
#endif

/*	AIX Talk �I�u�W�F�N�g�\����		*/
/*	Structure of AIX Talk object	*/
typedef	struct _aix_ply {
	CriSint8	used;						/*	�g�p�����ۂ�					*/
	CriSint8	stat;						/*	������						*/
	CriSint8	maxnch;						/*	�ő�Đ��`�����l����			*/
	CriSint8	maxntr;						/*	�ő�Đ��g���b�N��				*/
	CriSint8	nch;						/*	���݂̍Đ��`�����l����			*/
	CriSint8	ntr;						/*	���݂̍Đ��g���b�N��			*/
	CriSint8	seek_flag;					/*	�V�[�N�������s�����ǂ���		*/
	CriSint8	rsv1;						/*	�\��							*/
	AIXSJD	sjd;						/*	AIX�X�g���[���W���C���g�f�R�[�_	*/
	ADXSTM	stm;						/*	���̓X�g���[���R���g���[��		*/
	ADXT	adxt[AIXP_MAX_TR];			/*	ADXT�Đ���						*/
	SJ		sjf;						/*	���̓X�g���[���W���C���g		*/
	SJ		sji;						/*	���̓X�g���[���W���C���g		*/
	SJ		sjo[AIXP_MAX_TR];			/*	�o�̓X�g���[���W���C���g		*/
	CriSint8	*ibuf;						/*	���̓o�b�t�@					*/
	CriSint32	ibufbsize;					/*	���̓o�b�t�@�T�C�Y(�o�C�g�P��)	*/
	CriSint32	ibufxsize;					/*	���̓o�b�t�@�G�N�X�g���T�C�Y	*/
	CriSint8	*obuf[AIXP_MAX_TR];			/*	�o�̓o�b�t�@					*/
	CriSint32	obufbsize;					/*	�o�̓o�b�t�@�T�C�Y(�o�C�g�P��)	*/
	CriSint32	obufxsize;					/*	�o�̓o�b�t�@�T�C�Y(�o�C�g�P��)	*/
	CriSint32	pause_flag;					/*	�|�[�Y�t���O					*/
	CriSint32	demux_flag;					/*	�f�}���`�v���N�X�t���O			*/
	CriSint8	lpsw;						/*	���[�v�X�C�b�`					*/
	CriSint8	lnksw;						/*	�A���X�C�b�`					*/
	CriSint8	cmdstart;					/*	�X�^�[�g�R�}���h				*/
	CriSint8	rsv2;						/*	�\��							*/
	CriSint32	lpcnt;						/*	���[�v�J�E���^					*/
	CriSint32	ldph;						/*	��s���ă��[�h����t���[�Y��	*/
	CriSint32	curph;						/*	���ݍĐ����̃t���[�Y�ԍ�		*/
	CriSint32	stph;						/*	�Đ��J�n�t���[�Y�ԍ�			*/
	CriSint32	lpsp;						/*	���[�v�X�^�[�g�t���[�Y�ԍ�		*/
	CriSint32	lpep;						/*	���[�v�G���h�t���[�Y�ԍ�		*/
	CriChar8	*fname_wk;					/*	�t�@�C�����ێ��̈�iwork���j	*/
	CriChar8	*fname;						/*	�t�@�C����						*/
	void	*dir;						/*	�f�B���N�g�����				*/
	CriUint32	ofst;						/*	�t�@�C���I�t�Z�b�g				*/
	CriUint32	nsct;						/*	�t�@�C���T�C�Y(�Z�N�^)			*/
	CriSint32	pmode;						/*	�Đ����[�h						*/
	CriSint32	svrfreq;					/*	�T�[�o�֐��Ăяo���p�x			*/
	CriFloat32 reload_time;				/*	�ēǂݍ��ݎ��� (0:default)		*/
	CriSint32 master_volume;				/*	�S�̃{�����[��					*/
	CriSint32 track_volume[AIXP_MAX_TR];	/*	�g���b�N���Ƃ̃{�����[��		*/
	CriBool	use_3d_panning;				/*  3D�p���j���O���s���邩�ǂ���	*/
	CriSint32	time_offset;				/*	�Đ����A�����I�t�Z�b�g			*/
	CriSint32	time_scale;					/*	�Đ����A�����X�P�[��			*/
	CriSint32	start_smpl_phrase;			/*	�Đ��J�n�T���v���i�t���[�Y���j	*/
	CriSint32	start_smpl;					/*	�Đ��J�n�T���v��				*/
	CriFloat32	start_time;					/*	�Đ��J�n����					*/
	CriSint32	time_offset_seek;			/*	�V�[�N�ɂ�鎞���I�t�Z�b�g		*/
	
	/* �t���[�Y�R���g���[���Đ��p�p�����[�^ */
	AIXP_CBFN_PHCTRL phctrl_cbfn;
	void *phctrl_obj;
	
	/* �}���`�`�����l���I�[�f�B�I�o�͗p�p�����[�^ */
#if defined(XPT_SUPPORT_MULTICHANNEL)
	
#if defined(XPT_TGT_PC)
	/* PC���ŗL�̃p�����[�^ */
	CriSint8 *wkpcm[AIXP_MAX_TR * AIXP_MAX_NCH];
	SJ sjpcm[AIXP_MAX_TR * AIXP_MAX_NCH];
	void *dcd[AIXP_MAX_TR];
	void *mcrna;
	CriBool attached;
	AIXAMP aixamp[AIXP_MAX_TR];
#elif defined(XPT_TGT_XB)
	/* Xbox���ŗL�̃p�����[�^ */
	CriBool dolby_flag[AIXP_TRK_NUM];
	CriSint32 spk_idx[AIXP_SPK_NUM];
#elif defined(XPT_TGT_PS3PPU)
	/* PS3���ŗL�̃p�����[�^ */
	CriSint32 group_id;
#endif
	
	/* �X�s�[�J�[�w��̃{�����[���R���g���[���p */
	CriSint32 spk_vol[AIXP_SPK_NUM];
	
	/* Sofdec�����X�V�p�g���p�����[�^ */
	CriSint32 last_sample;
	CriSint64 last_count;
#endif
} AIX_PLY;
typedef	AIX_PLY	*AIXP;

/****************************************************************************/
/*		�֐��̐錾															*/
/*      Function Declaration												*/
/****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* $func$ AIXP�̏�����
 * [���@��] void AIXP_Init(void);
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] AIXP������������B
 *			�ϐ��̈�̏��������s���B
 *  Initialization of AIXP
 * [Format  ] void AIXP_Init(void);
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Initializes AIXP. Initializes variable.
 */
void CRIAPI AIXP_Init(void);

/* $func$ AIXP�̏I��
 * [���@��] void AIXP_Finish(void);
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] AIXP �̏I������������B
 *  Termination of AIXP
 * [Format  ] void AIXP_Finish(void);
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Finalizes AIXP.
 */
void CRIAPI AIXP_Finish(void);

/*	$func$ AIXP�n���h�� �̐���
 * [���@��] AIXP AIXP_Create(CriSint32 maxtr, CriSint32 maxnch, void *work, CriSint32 worksize);
 * [���@��] maxntr	: �ő�g���b�N��
 *			maxnch	: �ő�Đ��`�����l����(���m�����݂̂P�F�X�e���I�Q)
 *			work	: ���[�N�̈�
 *			worksize: ���[�N�̈�̃T�C�Y
 * [�o�@��] �Ȃ�
 * [�֐��l] AIXP�n���h��
 * [�@�@�\] AIXP�n���h���𐶐�����B
 *			work�̈�̃T�C�Y�́AAIXP_CALC_WORK�}�N���ɂ���ċ��߂�B
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

/*	$func$ AIXP 3D�n���h�� �̐���
 * [���@��] AIXP AIXP_Create3D(CriSint32 maxtr, void *work, CriSint32 worksize);
 * [���@��] maxntr	: �ő�g���b�N��
 *			work	: ���[�N�̈�
 *			worksize: ���[�N�̈�̃T�C�Y
 * [�o�@��] �Ȃ�
 * [�֐��l] AIXP�n���h��
 * [�@�@�\] ���A���^�C��3D�p���j���O�\��AIXP�n���h���𐶐�����B
 *			work�̈�̃T�C�Y�́AAIXP_CALC_WORK_3D�}�N���ɂ���ċ��߂�B
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

/* $func$ AIXP�n���h�� �̏���
 * [���@��] void AIXP_Destroy(AIXP aixp);
 * [���@��] AIXP	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �w�肳�ꂽ AIXP�n���h������������B
 *  Destroy of AIXP handle
 * [Format  ] void AIXP_Destroy(AIXP aixp);
 * [Inputs  ] AIXP   : AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Destroys specified AIXP handle.
 */
void CRIAPI AIXP_Destroy(AIXP aixp);

/* $func$ �t���[�Y�Đ��p���[�N�̈�̐ݒ�
 * [���@��] void AIXP_SetPhraseWork(AIXP aixp, void *work, CriSint32 wksize);
 * [���@��] AIXP	: AIXP�n���h��
 *			work	: ���[�N�̈�
 *			worksize: ���[�N�̈�̃T�C�Y
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �}���`�t���[�Y��AIX�t�@�C���i5�ȏ�̃t���[�Y���܂�AIX�t�@�C���j
 *          ���Đ�����̂ɕK�v�ȃ��[�N�̈��ݒ肵�܂��B
 *          �t���[�Y����4�ȉ���AIX�t�@�C�����Đ�����ꍇ�ɂ́A
 *          �{�֐����g�p����K�v�͂���܂���B
 * [���@�l] ���[�N�̈�̃T�C�Y�́AAIXP_CALC_PHRASE_WORK�}�N���ɂ���Čv�Z���܂��B
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

/* $func$ �t�@�C�����w��ɂ��Đ��̊J�n
 * [���@��] void AIXP_StartFname(AIXP aixp, CriChar8 *fname, void *atr);
 * [���@��] AIXP	: AIXP�n���h��
 *			fname	: �t�@�C����
 *			atr		: �f�B���N�g�����
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] fname �Ŏw�肳�ꂽAIX�t�@�C���̍Đ����J�n����B
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

/* $func$ �t�@�C���͈͎̔w��ɂ��Đ��̊J�n
 * [���@��] void AIXP_StartFnameRange(AIXP aixp, CriChar8 *fname, void *atr, CriSint32 ofst, CriSint32 nsct);
 * [���@��] AIXP	: AIXP�n���h��
 *			fname	: �t�@�C����
 *			atr		: �f�B���N�g�����
 *			ofst	: �I�t�Z�b�g
 *			nsct	: �T�C�Y(�Z�N�^�P��)
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] fname �t�@�C������ofst��nsct�Ŏw�肳�ꂽAIX�f�[�^�̍Đ����J�n����B
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

/* $func$ FID �w��ɂ��Đ��̊J�n
 * [���@��] void AIXP_StartAfs(AIXP aixp, CriSint32 patid, CriSint32 fid);
 * [���@��] AIXP	: AIXP�n���h��
 *			patid	: �p�[�e�B�V�������ʎq
 *			fid		: �t�@�C�����ʎq
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �p�[�e�B�V�������ʎq�ƃt�@�C�����ʎq�Ŏw�肳�ꂽAIX�t�@�C����
 *			�Đ����J�n����B
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

/* $func$ �X�g���[���W���C���g�ɂ��Đ��̊J�n
 * [���@��] void AIXP_StartSj(AIXP aixp, SJ sj);
 * [���@��] AIXP	: AIXP�n���h��
 *			sj		: �X�g���[���W���C���g
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �X�g���[���W���C���g���瓾����f�[�^���Đ�����B
 *  Play start of AIX data from Stream Joint
 * [Format  ] void AIXP_StartSj(AIXP aixp, SJ sj);
 * [Inputs  ] AIXP	: AIXP handle
 *			  sj	: Stream Joint
 * [Outputs ] None
 * [Return  ] None
 * [Function] Starts to play AIX file when you specify Stream Joint.
 */
void CRIAPI AIXP_StartSj(AIXP aixp, SJ sj);

/* $func$ �������w��ɂ��Đ��̊J�n
 * [���@��] void AIXP_StartMem(AIXP aixp, void *aixdat, CriSint32 datlen);
 * [���@��] AIXP	: AIXP�n���h��
 *			aixdat	: AIX�f�[�^�̃A�h���X
 *			datlen	: AIX�f�[�^�̑傫��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] aixdata �Ŏw�肳�ꂽAIX�f�[�^���Đ�����B
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

/* $func$ �Đ��̒�~
 * [���@��] void AIXP_Stop(AIXP aixp);
 * [���@��] AIXP	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] AIX�̍Đ����~����B
 *  Play stop
 * [Format  ] void AIXP_Stop(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops to play AIX data.
 */
void CRIAPI AIXP_Stop(AIXP aixp);

/* $func$ ��Ԃ̎擾
 * [���@��] CriSint32 AIXP_GetStat( AIXP aixp );
 * [���@��] AIXP	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] ���݂� AIXP�n���h���̏�Ԃ�\���萔
 * 				AIXP_STAT_STOP	 :	��~��
 *				AIXP_STAT_PREP	 :	�Đ�������
 *				AIXP_STAT_PLAYING:	�f�R�[�h���Đ���
 *				AIXP_STAT_PLAYEND:	�Đ��I��
 *				AIXP_STAT_ERROR	 :	�G���[
 * [�@�@�\] ���݂�AIXP�n���h���̏�Ԃ��擾����B
 *  Get status
 * [Format  ] CriSint32 AIXP_GetStat( AIXP aixp );
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] Status of AIXP handle
 * [Function] Obtains status of AIXP handle.
 */
CriSint32 CRIAPI AIXP_GetStat(AIXP aixp);

/* $func$ �T���v���P�ʂł̍Đ������̎擾
 * [���@��] void AIXP_GetTime( AIXP aixp, CriSint32 *ncount, CriSint32 *tscale );
 * [���@��] aixp    : AIXP�n���h��
 * [�o�@��] ncount	: �Đ��T���v����
 *			tscale	: �T���v�����O���g�� [ Hz ]
 * [�֐��l] �Ȃ�
 * [�@�@�\] �T���v���P�ʂōĐ��������擾����B
 *  Get play time by sample unit
 * [Format  ] void AIXP_GetTime( AIXP aixp, CriSint32 *ncount, CriSint32 *tscale );
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] ncount: Number of play sample
 *			  tscale: Sampling frequency(Hz)
 * [Return  ] None
 * [Function] Obtains play time by sample unit.
 */
void CRIAPI AIXP_GetTime(AIXP aixp, CriSint32 *ncount, CriSint32 *tscale);

/* $func$ ADXT�n���h���̎擾
 * [���@��] ADXT AIXP_GetAdxt(AIXP aixp, CriSint32 trno);
 * [���@��] AIXP	: AIXP�n���h��
 *			trno	: �g���b�N�ԍ�
 * [�o�@��] �Ȃ�
 * [�֐��l] adxt	: ADXT�n���h��
 * [�@�@�\] ADXT�n���h�����擾����B
 *  Getting a ADXT handle of specified track
 * [Format  ] ADXT AIXP_GetAdxt(AIXP aixp, CriSint32 trno);
 * [Inputs  ] AIXP	: AIXP handle
 *			  trno	: Track No.
 * [Outputs ] None
 * [Return  ] ADXT handle
 * [Function] Obtains a ADXT handle of specified track.
 */
ADXT CRIAPI AIXP_GetAdxt(AIXP aixp, CriSint32 trno);

/*	$func$ �ēǂݍ��݊J�n���Ԃ̐ݒ�
 * [���@��] void AIXP_SetReloadTime(AIXP aixp, CriFloat32 time);
 * [���@��] AIXP	: AIXP�n���h��
 *			time	: �ēǂݍ��݊J�n���ԁi�P�ʁF�b�j
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ���̓o�b�t�@�ւ̍ēǂݍ��݊J�n���Ԃ�ݒ肷��B
 *			�o�b�t�@���̃f�[�^�ʂ� time �b������������ꍇ��
 *			�f�B�X�N����f�[�^��ǂݍ��ނ悤�ݒ肵�܂��B
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

/*	$func$ ���̓o�b�t�@���Đ�����
 * [���@��] CriFloat32 AIXP_GetIbufRemainTime(AIXP aixp);
 * [���@��] AIXP	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Đ��\���ԁ@(�P�ʁF�b)
 * [�@�@�\] ���̓o�b�t�@�ɂ���f�[�^�ōĐ��\�Ȏ��Ԃ��擾����B
 *			AIX�f�[�^�̂ݑΉ��B
 * [Format  ] CriFloat32 AIXP_GetIbufRemainTime(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] Playable time (sec)
 * [Function] Obtains playable time using only data in input buffer.
 *			  You can use this function only playing AIX data.
 */
CriFloat32 CRIAPI AIXP_GetIbufRemainTime(AIXP aixp);

/*	$func$ �e�n���h���̃T�[�o�֐��@(������Ԃ̍X�V)
 * [���@��] CriSint32 AIXP_ExecHndl(AIXP aixp);
 * [���@��] AIXP	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �e�n���h���̓�����Ԃ��X�V����B
 *			AIXP_ExecServer������Ăяo�����B
 *  Server function of each handle
 * [Format  ] CriSint32 AIXP_ExecHndl(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status.
 *			  This function is called from 'AIXP_ExecServer' function.
 */
void CRIAPI AIXP_ExecHndl(AIXP aixp);

/*	$func$ �T�[�o�֐��@(������Ԃ̍X�V)
 * [���@��] CriSint32 AIXP_ExecServer(void);
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ���C�u�����̓�����Ԃ��X�V����B
 *			V-Sync ���ɌĂяo���Ȃ���΂Ȃ�Ȃ��B
 *  Server function
 * [Format  ] CriSint32 AIXP_ExecServer(void);
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status of library.
 */
void CRIAPI AIXP_ExecServer(void);

/*	$func$ ���[�v�����񐔂̎擾
 * [���@��] CriSint32 AIXP_GetLpCnt(AIXP aixp);
 * [���@��] AIXP	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] ���[�v������
 * [�@�@�\] ���[�v�����񐔂��擾����B
 *  Get the number of times played a loop
 * [Format  ] CriSint32 AIXP_GetLpCnt(AIXP aixp);
 * [Inputs  ] AIXP	: AIXP handle
 * [Outputs ] None
 * [Return  ] Number of times played a loop
 * [Function] Obtains the number of times that played a loop.
 */
CriSint32 CRIAPI AIXP_GetLpCnt(AIXP aixp);

/*	$func$ ���[�v�X�C�b�`�̐ݒ�
 * [���@��] void AIXP_SetLpSw(AIXP aixp, CriSint32 sw);
 * [���@��] aixp	: AIXP�n���h��
 *			sw		: 1=���[�v����A0=���[�v���Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ���[�v���邩�ۂ���ݒ肷��B
 */
void CRIAPI AIXP_SetLpSw(AIXP aixp, CriSint32 sw);

/*	$func$ �t���[�Y�����N�X�C�b�`�̐ݒ�
 * [���@��] void AIXP_SetLnkSw(AIXP aixp, CriSint32 sw);
 * [���@��] aixp	: AIXP�n���h��
 *			sw		: 1=�A������A0=�A�����Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �t���[�Y��A���Đ����邩�ۂ���ݒ肷��B
 */
void CRIAPI AIXP_SetLnkSw(AIXP aixp, CriSint32 sw);

/*	$func$ �Đ��J�n�t���[�Y�ԍ��̐ݒ�
 * [���@��] void AIXP_SetStartPh(AIXP aixp, CriSint32 phno);
 * [���@��] aixp	: AIXP�n���h��
 *			phno	: �t���[�Y�ԍ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �Đ��J�n�t���[�Y�ԍ���ݒ肷��B
 */
void CRIAPI AIXP_SetStartPh(AIXP aixp, CriSint32 phno);

/*	$func$ �Đ��J�n�t���[�Y�ԍ��̎擾
 * [���@��] CriSint32 AIXP_GetStartPh(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �t���[�Y�ԍ�
 * [�@�@�\] �Đ��J�n�t���[�Y�ԍ����擾����B
 */
CriSint32 CRIAPI AIXP_GetStartPh(AIXP aixp);

/*	$func$ ���[�v���̐ݒ�
 * [���@��] void AIXP_SetLpInfo(AIXP aixp, CriSint32 start_phno, CriSint32 length);
 * [���@��] aixp		: AIXP�n���h��
 *			start_phno	: �J�n�t���[�Y�ԍ�
 *			length		: ���[�v����t���[�Y��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ���[�v���(�J�n�t���[�Y�ԍ��ƒ���)��ݒ肷��B
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

/*	$func$ ���[�v���̎擾
 * [���@��] void AIXP_GetLpInfo(AIXP aixp, CriSint32 *start_phno, CriSint32 *length);
 * [���@��] aixp		: AIXP�n���h��
 *			start_phno	: �J�n�t���[�Y�ԍ����i�[����ϐ��ւ̃|�C���^
 *			length		: ���[�v����t���[�Y�����i�[����ϐ��ւ̃|�C���^
 * [�o�@��] �Ȃ�
 * [�֐��l] �t���[�Y�ԍ�
 * [�@�@�\] ���[�v�X�^�[�g�t���[�Y�ԍ����擾����B
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

/*	$func$ �ꎞ��~�̐ݒ�
 * [���@��] void AIXP_Pause(AIXP aixp, CriSint32 sw);
 * [���@��] AIXP	: AIXP�n���h��
 *			sw		: 1=�ꎞ��~�A0=�ĊJ
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �ꎞ��~���邩�ۂ���ݒ肷��B
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

/*	$func$ �ꎞ��~��Ԃ̎擾
 * [���@��] CriSint32 AIXP_GetStatPause(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �ꎞ��~��� 1=�ꎞ��~�A0=��ꎞ��~
 * [�֐��l] �Ȃ�
 * [�@�@�\] �ꎞ��~��Ԃ��擾����B
 *  Pause/Continue
 * [Format  ] CriSint32 AIXP_GetStatPause(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] 1=pause, 0=not pause
 * [Return  ] None
 * [Function] Obtains the status of pause.
 */
CriSint32 CRIAPI AIXP_GetStatPause(AIXP aixp);

/*			�������m�����o�̓X�C�b�`�̐ݒ�	
[��  ��]	void AIXP_SetOutoputMono(CriSint32 flag);
[��  ��]	CriSint32 flag		�������m�����o�̓t���O(OFF:0, ON:1)
[�߂�l]	�Ȃ�					
[�@  �\]	�X�e���I�f�[�^�������I�Ƀ��m�����f�[�^�Ƃ��ďo�͂���B
[���@�l]	
*/
/* 2003-02-26 ���ݖ����� */
/* Not implemented... */
/* void CRIAPI AIXP_SetOutputMono(CriSint32 flag); */

/* $func$ �����f�[�^�̑��T���v�����̎擾
 * [���@��] CriSint32 AIXP_GetTotalNumSmpl(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �����f�[�^�̑��T���v����
 * [�@�@�\] �Đ����� AIX �f�[�^�̑��T���v�������擾����B
 * [���@�l] ��Ԃ��A�Đ���(AIXP_STAT_PLAYING)����Đ��I��(AIXP_STAT_PLAYEND)
 *			�܂ł̎��Ɏ擾�\�B
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

/* $func$ �g���b�N���̎擾
 * [���@��] CriSint32 AIXP_GetNumTrack(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �g���b�N��
 * [�@�@�\] �Đ����� AIX �f�[�^�̃g���b�N�����擾����B
 * [���@�l] ��Ԃ��A�Đ���(AIXP_STAT_PLAYING)����Đ��I��(AIXP_STAT_PLAYEND)
 *			�܂ł̎��Ɏ擾�\�B
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

/* $func$ �t���[�Y���̎擾
 * [���@��] CriSint32 AIXP_GetNumPhrase(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �g���b�N��
 * [�@�@�\] �Đ����� AIX �f�[�^�̃t���[�Y�����擾����B
 * [���@�l] ��Ԃ��A�Đ���(AIXP_STAT_PLAYING)����Đ��I��(AIXP_STAT_PLAYEND)
 *			�܂ł̎��Ɏ擾�\�B
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

/* $func$ �\���O���̎擾
 * [���@��] CriSint32 AIXP_GetNumSong(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �g���b�N��
 * [�@�@�\] �Đ����� AIX �f�[�^�̃\���O�����擾����B
 * [���@�l] ��Ԃ��A�Đ���(AIXP_STAT_PLAYING)����Đ��I��(AIXP_STAT_PLAYEND)
 *			�܂ł̎��Ɏ擾�\�B
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

/*	$func$ �o�̓{�����[���̎擾
 * [���@��] CriSint32 AIXP_GetOutVol(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �������x���@(0:-0dB, -999:-99.9dB)
 * [�@�@�\] �o�̓{�����[�����擾����B
 *  Get volume
 * [Format  ] CriSint32 AIXP_GetOutVol(AIXP aixp);
 * [Inputs  ] aixp	: AIXP handle
 * [Outputs ] None
 * [Return  ] Volume (0:-dB, -999:-99.9dB)
 * [Function] Gets the volume.
 */
CriSint32 CRIAPI AIXP_GetOutVol(AIXP aixp);

/*	$func$ �o�̓{�����[���̐ݒ�
 * [���@��] void AIXP_SetOutVol(AIXP aixp, CriSint32 vol);
 * [���@��] aixp	: AIXP�n���h��
 *			vol		: �������x���@(0:-0dB, -999:-99.9dB)
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �o�̓{�����[����ݒ肷��B
 *			vol�̐ݒ�l	0    :    -0dB  �����Ȃ�
 *						-30  :    -3dB  ��70%
 *						-60  :    -6dB  ��50%
 *						-999 : -99.9dB  �ő�̌�����
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

/*	$func$ �g���b�N���Ƃ̏o�̓{�����[���̎擾
 * [���@��] CriSint32 AIXP_GetTrackVol(AIXP aixp, CriSint32 trno);
 * [���@��] aixp	: AIXP�n���h��
 *          trno    : �g���b�N�ԍ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �������x���@(0:-0dB, -999:-99.9dB)
 * [�@�@�\] �g���b�N�P�ʂ̏o�̓{�����[�����擾����B
 *  Get track volume
 * [Format  ] CriSint32 AIXP_GetTrackVol(AIXP aixp, CriSint32 trno);
 * [Inputs  ] aixp	: AIXP handle
 *            trno  : Track No.
 * [Outputs ] None
 * [Return  ] Volume (0:-dB, -999:-99.9dB)
 * [Function] Gets the volume of specified track.
 */
CriSint32 CRIAPI AIXP_GetTrackVol(AIXP aixp, CriSint32 trno);

/*	$func$ �g���b�N���Ƃ̏o�̓{�����[���̐ݒ�
 * [���@��] void AIXP_SetTrackVol(AIXP aixp, CriSint32 trno, CriSint32 vol);
 * [���@��] aixp	: AIXP�n���h��
 *          trno    : �g���b�N�ԍ�
 *			vol		: �������x���@(0:-0dB, -999:-99.9dB)
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �g���b�N�P�ʂ̏o�̓{�����[����ݒ肷��B
 *			vol�̐ݒ�l	0    :    -0dB  �����Ȃ�
 *						-30  :    -3dB  ��70%
 *						-60  :    -6dB  ��50%
 *						-999 : -99.9dB  �ő�̌�����
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

/* $func$ �f�[�^����(�o�C�g���C�g)�̎擾
 * [���@��] CriSint32 AIXP_GetByteRate(AIXP aixp);
 * [���@��] aixp	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �f�[�^����(�o�C�g���C�g)
 * [�@�@�\] �Đ����� AIX �f�[�^�̃f�[�^���ʂ��擾����B
 * [���@�l] ��Ԃ��A�Đ���(AIXP_STAT_PLAYING)����Đ��I��(AIXP_STAT_PLAYEND)
 *			�܂ł̎��Ɏ擾�\�B
 */
CriSint32 CRIAPI AIXP_GetByteRate(AIXP aixp);

/* $func$ �t���[�Y�R���g���[���p�R�[���o�b�N�֐��̓o�^
 * [���@��] void AIXP_SetCbPhraseControl(AIXP aixp, AIXP_CBFN_PHCTRL cbfunc, void *obj);
 * [���@��] AIXP	: AIXP�n���h��
 *			cbfunc	: �t���[�Y�R���g���[���R�[���o�b�N�֐�
 *			obj		: �C�ӂ̈����n���I�u�W�F�N�g
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �t���[�Y�R���g���[���p�̃R�[���o�b�N�֐���o�^���܂�
 * [���@��] �{�֐���AIXP_SetNumPreloadPhrases�֐��Ƃ𕹗p���邱�Ƃ͂ł��܂���B
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

/* $func$ �v�����[�h�t���[�Y���̐ݒ�
 * [���@��] void AIXP_SetNumPreloadPhrases(AIXP aixp, CriSint32 num);
 * [���@��] AIXP	: AIXP�n���h��
 *			num		: �Đ��J�n���Ɉꊇ���[�h����t���[�Y��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �Đ��J�n���Ɉꊇ���[�h����t���[�Y�̐���ݒ肵�܂�
 * [���@�l] �v�����[�h�t���[�Y���̃f�t�H���g�l��1�ł��B
 *�@�@�@�@�@���[�h����t���[�Y�̐���ύX����ۂɂ́A1�ȏ�̒l��ݒ肵�Ă��������B
 * [���@��] �{�֐���AIXP_SetCbPhraseControl�֐��Ƃ𕹗p���邱�Ƃ͂ł��܂���B
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

/* $func$ �v�����[�h�t���[�Y���̎擾
 * [���@��] CriSint32 AIXP_GetNumPreloadPhrases(AIXP aixp);
 * [���@��] AIXP	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Đ��J�n���Ɉꊇ���[�h����t���[�Y��
 * [�@�@�\] �Đ��J�n���Ɉꊇ���[�h����t���[�Y�̐����擾���܂�
 *  Get the number of preload phrases
 * [Format  ] CriSint32 AIXP_GetNumPreloadPhrases(AIXP aixp);
 * [Inputs  ] AIXP	 : AIXP handle
 * [Outputs ] None
 * [Return  ] Number of preload phrases
 * [Function] Get the number of phrases that will be preloaded at 
 *            the beginning of the playback.
 */
CriSint32 CRIAPI AIXP_GetNumPreloadPhrases(AIXP aixp);

/* $func$ �f�t�H���g�̃T�[�o�������g���̐ݒ�
 * [���@��] void AIXP_SetDefSvrFreq(CriSint32 freq);
 * [���@��] freq : �T�[�o�֐��̌Ăяo���p�x�i1�b������̌Ăяo���񐔁j
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �T�[�o�֐������Ƀf�R�[�h����ʂ𐧌䂵�܂��B
 *�@�@�@�@�@�f�t�H���g�ł́A60�iVsync�j���ݒ肳��Ă��܂��B
 * [���@�l] �{�֐��́AVysnc���g����60Hz�ł͂Ȃ����ŁA�f�R�[�h�ʂ�
 *�@�@�@�@�@�������邽�߂Ɏg�p���܂��B
 */
void CRIAPI AIXP_SetDefSvrFreq(CriSint32 freq);

/* $func$ �T�[�o�������g���̐ݒ�
 * [���@��] void AIXP_SetSvrFreq(AIXP aixp, CriSint32 freq);
 * [���@��] aixp : AIXP�n���h��
 *�@�@�@�@�@freq : �T�[�o�֐��̌Ăяo���p�x�i1�b������̌Ăяo���񐔁j
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �T�[�o�֐������Ƀf�R�[�h����ʂ��n���h���P�ʂŐ��䂵�܂��B
 *�@�@�@�@�@�f�t�H���g�ł́A60�iVsync�j���ݒ肳��Ă��܂��B
 * [���@�l] �{�֐��́A�����̍Đ��s�b�`���グ��ꍇ���A�n���h���P�ʂ�
 *�@�@�@�@�@�f�[�^�̃f�R�[�h�ʂ𒲐��������ꍇ�Ɏg�p���܂��B
 *�@�@�@�@�@����freq�̒l�ɂ́u1�b���̉����f�[�^����Vsync�Ńf�R�[�h���邩�v
 *�@�@�@�@�@���w�肵�܂��B
 *�@�@�@�@�@�Ⴆ�΁A30���w�肵���ꍇ�ɂ́A60Vsync�Ńf�R�[�h���镪�̉���
 *�@�@�@�@�@�f�[�^���A������30Vsync�Ńf�R�[�h���Ă��܂��܂��B
 */
void CRIAPI AIXP_SetSvrFreq(AIXP aixp, CriSint32 freq);

/* $func$ �f�R�[�h�f�[�^�̔j��
 * [���@��] CriSint32 AIXP_DiscardSmpl(AIXP aixp, CriSint32 nsmpl);
 * [���@��] aixp  : AIXP�n���h��
 *			nsmpl : �j������T���v����
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �f�R�[�h���������f�[�^��j�����܂��B
 *�m���@�l�n�{�֐��́A�Đ��ʒu���X�L�b�v���邽�߂Ɏg�p���܂��B
 * [���@��] ���̊֐���ADXT�n���h���̃X�e�[�^�X��ADXT_STAT_PLAYING�ɂȂ�
 *�@�@�@�@�@�Ă���ĂԕK�v������܂�
 *�@�@�@�@�@�܂��A��ʂ̃f�[�^��j�������ꍇ�A�o�̓f�[�^�̌͊��ɂ��T�E
 *�@�@�@�@�@���h�o�b�t�@���̉����������I�Ƀ��[�v���čĐ������ꍇ������
 *�@�@�@�@�@�܂��B
 */
CriSint32 CRIAPI AIXP_DiscardSmpl(AIXP aixp, CriSint32 nsmpl);


/* $func$ �Đ��J�n�ʒu�̐ݒ�i�b�P�ʁj
 * [���@��] void CRIAPI AIXP_SetStartTime(AIXP aixp, CriFloat32 start_time);
 * [���@��] AIXP		: AIXP�n���h��
 *			start_time	: �Đ��J�n�ʒu�i�b�P�ʁj
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �Đ����J�n����ʒu��b�P�ʂŐݒ肵�܂��B
 * [���@�l] �Đ����iPREP��PLAYEND�܂ށj�ɐݒ肵���ꍇ�A����Đ��J�n���ɓK�p����܂��B
 * [���@��] (a) �{�֐���AIXP_SetStartPh�֐��AAIXP_SetStartSmpl�֐��Ƃ𕹗p���邱�Ƃ͂ł��܂���B�i��ɌĂяo���������D��j
 * 			(b) ���[�v�Đ��ɂ͎g�p�ł��܂���B
 * 			(c) �t���[�Y���܂ރf�[�^�̍Đ��ɂ͎g�p�ł��܂���B
 * 			(d) �������Đ��iAIXP_StartMem�֐��ɂ��Đ��j�ɂ͎g�p�ł��܂���B
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

/* $func$ �Đ��J�n�ʒu�̎擾�i�b�P�ʁj
 * [���@��] CriFloat32 CRIAPI AIXP_GetStartTime(AIXP aixp);
 * [���@��] AIXP		: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Đ��J�n�ʒu�i�b�P�ʁj
 * [�@�@�\] �Đ����J�n����ʒu��b�P�ʂŎ擾���܂��B
 *  Get the start position of a playback by the second
 * [Format  ] CriFloat32 CRIAPI AIXP_GetStartTime(AIXP aixp);
 * [Inputs  ] AIXP			: AIXP handle
 * [Outputs ] None
 * [Return  ] Start position of a playback by the second
 * [Function] This function gets the start position of a playback by the second.
 */
CriFloat32 CRIAPI AIXP_GetStartTime(AIXP aixp);

/* $func$ �Đ��J�n�ʒu�̐ݒ�i�T���v���P�ʁj
 * [���@��] void CRIAPI AIXP_SetStartSmpl(AIXP aixp, CriSint32 start_smpl);
 * [���@��] AIXP		: AIXP�n���h��
 *			start_smpl	: �Đ��J�n�ʒu�i�T���v���P�ʁj
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �Đ����J�n����ʒu���T���v���P�ʂŐݒ肵�܂��B
 * [���@�l] �Đ����iPREP��PLAYEND�܂ށj�ɐݒ肵���ꍇ�A����Đ��J�n���ɓK�p����܂��B
 * [���@��] (a) �{�֐���AIXP_SetStartPh�֐��AAIXP_SetStartTime�֐��Ƃ𕹗p���邱�Ƃ͂ł��܂���B�i��ɌĂяo���������D��j
 * 			(b) ���[�v�Đ��ɂ͎g�p�ł��܂���B
 * 			(c) �t���[�Y���܂ރf�[�^�̍Đ��ɂ͎g�p�ł��܂���B
 * 			(d) �������Đ��iAIXP_StartMem�֐��ɂ��Đ��j�ɂ͎g�p�ł��܂���B
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

/* $func$ �Đ��J�n�ʒu�̎擾�i�T���v���P�ʁj
 * [���@��] CriSint32 CRIAPI AIXP_GetStartSmpl(AIXP aixp);
 * [���@��] AIXP		: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Đ��J�n�ʒu�i�T���v���P�ʁj
 * [�@�@�\] �Đ����J�n����ʒu���T���v���P�ʂŎ擾���܂��B
 *  Get the start position of a playback by the number of samples
 * [Format  ] CriSint32 CRIAPI AIXP_GetStartSmpl(AIXP aixp);
 * [Inputs  ] AIXP			: AIXP handle
 * [Outputs ] None
 * [Return  ] Start position of a playback by the number of samples
 * [Function] This function gets the start position of a playback by the number of samples.
 */
CriSint32 CRIAPI AIXP_GetStartSmpl(AIXP aixp);

/*--------------------------------------------------------------------------*/
/*		�U���擾�p�֐�														*/
/*		Functions for amplitude analysis									*/
/*--------------------------------------------------------------------------*/

/* $func$ �U���擾�p���[�N�̈�T�C�Y�̌v�Z
 * [���@��] CriSint32 AIXP_CalcAmpWork(CriSint32 maxtr, CriSint32 maxnch);
 * [���@��] maxtr	: �ő�g���b�N��
 *          maxnch	: �ő�`�����l����
 * [�o�@��] �Ȃ�
 * [�֐��l] ���[�N�̈�T�C�Y
 * [�@�@�\] �U���擾�@�\���g�p����̂ɕK�v�ȃ��[�N�̈�̃T�C�Y���v�Z���܂��B
 * Work area size calculation
 * [Format  ] CriSint32 AIXP_CalcAmpWork(CriSint32 maxtr, CriSint32 maxnch);
 * [Inputs  ] maxtr	: Number of maximum tracks
 *            maxch	: Number of maximum channels
 * [Outputs ] None
 * [Return  ] Work area size
 * [Function] Calculate work area size used for amplitude analysis.
 */
CriSint32 CRIAPI AIXP_CalcAmpWork(CriSint32 maxtr, CriSint32 maxnch);

/* $func$ �U���擾�p��Ɨ̈�̐ݒ�
 * [���@��] void AIXP_SetAmpWork(AIXP aixp, void *work, CriSint32 wksize);
 * [���@��] AIXP	: AIXP�n���h��
 *          work	: ���[�N�̈�
 *          wksize	: ���[�N�̈�T�C�Y
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] AIXP�n���h���ɐU���擾�@�\��t�����܂�
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

/* $func$ �U���̎擾
 * [���@��] CriSint32 AIXP_GetAmplitude(AIXP aixp, CriSint32 tr, CriSint32 ch);
 * [���@��] AIXP	: AIXP�n���h��
 *          tr		: �g���b�N�ԍ�
 *          ch		: �`�����l���ԍ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �Đ����̉����̐U�����擾���܂��B
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

/* $func$ �U���̎擾�i�����I�t�Z�b�g�w��t���j
 * [���@��] CriSint32 AIXP_GetAmplitude2(
 *              AIXP aixp, CriSint32 tr, CriSint32 ch, CriSint32 msec, CriSint32 *msec2);
 * [���@��] AIXP	: AIXP�n���h��
 *          tr		: �g���b�N�ԍ�
 *          ch		: �`�����l���ԍ�
 *          msec	: �v�������imsec�~���b�O�̐U�����擾�j
 *          msec2	: �擾�ł����U���̎����i���ۂɑk�ꂽ�����j
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �Đ��O�̉����̐U�������O�Ɏ擾���܂��B
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
/*		�Í������p�֐�														*/
/*		Functions for encrypted file										*/
/*--------------------------------------------------------------------------*/

/* $func$ �L�[�R�[�h�̐ݒ�
 * [���@��] void AIXP_SetKeyString(AIXP aixp, CriChar8 *str);
 * [���@��] aixp	: AIXP�n���h��
 *          str     : �L�[������
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] AIX�n���h���ɑ΂��ĈÍ��������p�̃L�[�������ݒ肷��B
 * [���@�l] �L�[�����񂪐ݒ肳�ꂽ��Ԃł��A�Í�������Ă��Ȃ�AIX�t�@�C��
 *          �ɂ��Ă͍Đ����\�B
 */
void CRIAPI AIXP_SetKeyString(AIXP aixp, CriChar8 *str);

/*--------------------------------------------------------------------------*/
/*		�}���`�`�����l�������Đ��@�\										*/
/*		Functions for multi-channel audio playback							*/
/*--------------------------------------------------------------------------*/
/*	$func$ �}���`�`�����l���Đ��@�\�̑g�ݍ���
 * [���@��] void AIXP_AttachMultiChannel(AIXP aixp, void *work, CriSint32 wksize);
 * [���@��] aipx	: AIXP�n���h��
 *			work	: �}���`�`�����l���Đ��p���[�N�̈�i=NULL�j
 *			wksize	: �}���`�`�����l���Đ��p���[�N�̈�i=0�j
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] AIXP�n���h���ɑ΂��ă}���`�`�����l���Đ��@�\��ǉ�����B
 *          �{�֐����s��ɍĐ������AIX�����́A�S�ă}���`�`�����l���I�[�f�B�I
 *          �Ƃ��čĐ������B
 * [��  �l] ����A�ǉ��̃��[�N�̈�͕s�v�B
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

/*	$func$ �}���`�`�����l���Đ��@�\�̎��O��
 * [���@��] void AIXP_DetachMultiChannel(AIXP aixp);
 * [���@��] aipx	: AIXP�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] AIXP�n���h������}���`�`�����l���Đ��@�\�����O���B
 *  Detachs multi-channel playback function
 * [Format  ] void AIXP_DetachMultiChannel(AIXP aixp);
 * [Inputs  ] aixp    : AIXP handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Detachs multi-channel playback function.
 */
void CRIAPI AIXP_DetachMultiChannel(AIXP aixp);

/* $func$ �t�@�C�����w��ɂ��Đ��̊J�n(PC�ł̃}���`�`�����l���o�͗p)
 * [���@��] void AIXP_StartFnameMultiChannel(AIXP aixp, CriSint8 *fname);
 * [���@��] AIXP	: AIXP�n���h��
 *			fname	: �t�@�C����
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] fname �Ŏw�肳�ꂽAIX�t�@�C����5.1ch�ōĐ�����B
 * [��  �l] �{�}�N���͋��o�[�W�����Ƃ̌݊����̂��߂ɗp�ӂ���Ă��܂��B
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

/* $func$ FID �w��ɂ��Đ��̊J�n(PC�ł̃}���`�`�����l���o�͗p)
 * [���@��] void AIXP_StartAfsMultiChannel(AIXP aixp, CriSint32 patid, CriSint32 fid);
 * [���@��] AIXP	: AIXP�n���h��
 *			patid	: �p�[�e�B�V�������ʎq
 *			fid		: �t�@�C�����ʎq
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �p�[�e�B�V�������ʎq�ƃt�@�C�����ʎq�Ŏw�肳�ꂽAIX�f�[�^��5.1ch�ōĐ�����B
 * [��  �l] �{�}�N���͋��o�[�W�����Ƃ̌݊����̂��߂ɗp�ӂ���Ă��܂��B
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

/* $func$ �������w��ɂ��Đ��̊J�n(PC�ł̃}���`�`�����l���o�͗p)
 * [���@��] void AIXP_StartMemMultiChannel(AIXP aixp, void *aixdat, CriSint32 datlen);
 * [���@��] AIXP	: AIXP�n���h��
 *			aixdat	: AIX�f�[�^�̃A�h���X
 *			datlen	: AIX�f�[�^�̑傫��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] aixdata �Ŏw�肳�ꂽAIX�f�[�^��5.1ch�ōĐ�����B
 * [��  �l] �{�}�N���͋��o�[�W�����Ƃ̌݊����̂��߂ɗp�ӂ���Ă��܂��B
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

/* $func$ �X�s�[�J�[�ʃ{�����[���w��(PC�ł̃}���`�`�����l���o�͗p)
 * [���@��] void AIXP_SetOutVolMultiChannel(AIXP aixp, CriSint32 spkid, CriSint32 vol);
 * [���@��] AIXP	: AIXP�n���h��
 *			spkid	: �X�s�[�J�[ID
 *			vol		: �{�����[��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] spkid �Ŏw�肳�ꂽ�X�s�[�J�[�̃{�����[����ݒ肷��B
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
/*		PC�p�g���@�\														*/
/*		Functions for PC													*/
/*--------------------------------------------------------------------------*/
/*	$func$ �f�t�H���g�~�L�T�[���[�h�̐ݒ�
 * [���@��] void AIXP_SetDefMixerMode(CriSint32 mode);
 * [���@��] mode	: �~�L�T�[���[�h
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �~�L�T�[�̓��샂�[�h��ݒ肷��B
 *			mode�̐ݒ�l	AIXP_MIX_HARDWARE : �n�[�h�E�F�A�~�L�V���O
 *							AIXP_MIX_SOFTWARE : �\�t�g�E�F�A�~�L�V���O
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
