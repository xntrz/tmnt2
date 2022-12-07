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
/* �����������g��  */
/* Vsync frequency */
#define MWSFD_VHZ_59_94			(59.94f)
#define MWSFD_VHZ_50_00			(50.00f)
#define MWSFD_VHZ_60_00			(60.00f)

/* �������[�h		*/
/* Composition Mode	*/
#define MWSFD_COMPO_AUTO		(0x00000000)	/* ����						*/
												/* Auto						*/
#define MWSFD_COMPO_OPAQ		(0x00000011)	/* �s����					*/
												/* opacity					*/
#define MWSFD_COMPO_ALPHFULL	(0x00000021)	/* �t��Alpha����			*/
												/* Full Alpha				*/
#define	MWSFD_COMPO_ALPHLUMI	(0x00000031)	/* ���~�i���X�L�[���� 		*/
												/* Luminance key alpha		*/
#define	MWSFD_COMPO_ALPH3		(0x00000041)	/* 3�l�A���t�@ 				*/
												/* 3-value alpha			*/
#define MWSFD_COMPO_Z			(0x00000101)	/* �y�l						*/
												/* Z value					*/

/* �t�@�C���^�C�v		*/
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

/* �o�̓o�b�t�@���̃f�t�H���g�l 	*/
/* Default value of out-buffer size	*/
#define MWSFD_DFL_OUTBUFSIZE	(0)

/* �������[�h	*/
/* Sync Mode	*/
#define MWSFD_SYNCMODE_NONE		(0)		/* ������		*/ /* No sync		*/
#define MWSFD_SYNCMODE_VSYNC	(1)		/* V-Sync����	*/ /* Sync Vint		*/
#define MWSFD_SYNCMODE_AUDIO	(2)		/* Audio ����	*/ /* Sync Audio	*/
#define MWSFD_SYNCMODE_USRTIME	(4)		/* ���[�U�������� */
#define MWSFD_SYNCMODE_EXTCLOCK	(5)		/* �O���N���b�N���� */
#define MWSFD_SYNCMODE_EASYINFINI (6)	/* �ȈՖ����Đ� */ /* Easy infinite video only playback */

/* �������Đ����̃t�@�C���T�C�Y�ȗ�(�T�C�Y�����擾) */
#define MWSFD_FILESIZE_UNKNOWN			(0xFFFFFFFF)

/* PID�̖����l (���w��Ɠ����A���w��̏ꍇ�̓��C�u���������̃}�b�v��͂��L��) */
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

/* �s�N�`�����C�����[�U�f�[�^�p�̃o�b�t�@�T�C�Y�v�Z */
/* Calculate buffer size of user data in picture layer */
#define MWSFD_SIZE_PICUSRBUF(npool, size1pic)	(((npool) + 4) * (size1pic))

/* mwPlyStartIpicture()�p�̍ő�BPS�w��l�}�N�� */
/* �����͍ő�s�N�`���f�[�^�T�C�Y[byte]  */
#define MWSFD_CALC_MAX_BPS_SEEKIPIC(max_picdata_size)	((max_picdata_size + 4096) * 16)

/* �r�f�IPTS�����p���[�N�T�C�Y�v�Z */
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

/* �f�R�[�h�T�[�o */
/* Decode Server  */
typedef enum {
	MWSFD_DEC_SVR_IDLE		= 0,	/* ���C���̗]�莞�ԂŃf�R�[�h����B */
									/* Decode in idel thread            */
	MWSFD_DEC_SVR_MAIN		= 1,	/* ���C���������Ńf�R�[�h����B		*/
									/* Decode in main thread            */

	MWSFD_DEC_SVR_END,
	MWSFD_DEC_SVR_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdDecSvr;

/* �f�R�[�h���ʃo�b�t�@�f�[�^�`��	*/
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

/* ���~�i���X�����̓��߃L�[				*/
/* Luminance alpha's transparence key	*/
typedef enum {
	MWSFD_LUMIKEY_BLACK		= 0,	/* Black key transparence */
	MWSFD_LUMIKEY_WHITE		= 1,	/* White key transparence */

	MWSFD_LUMIKEY_END,
	MWSFD_LUMIKEY_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdLumiKey;

/* �t���[���^�C�v	*/
/* Frame type		*/
typedef enum {
	MWSFD_FRMTYPE_UNKNOWN		= 0,	/* Unknown */
	MWSFD_FRMTYPE_PROGRESSIVE	= 1,	
	MWSFD_FRMTYPE_INTERLACE		= 2,

	MWSFD_FRMTYPE_END,
	MWSFD_FRMTYPE_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdFrmType;

/* �s�N�`���^�C�v	*/
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

/* �t���[����YUV�ϊ����[�h			*/
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

/* �t���[���擾�������[�h			*/
/* Sync mode of mwPlyGetCurFrm()	*/
typedef enum {
	MWSFD_FRMSYNC_TIME		= 0,	/* Sync with time	*/
	MWSFD_FRMSYNC_NONE		= 1,	/* No sync			*/

	MWSFD_FRMSYNC_END,
	MWSFD_FRMSYNC_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdFrmSync;

/* �r�f�I�R�[�f�b�N			*/
/* Codec detail infomation	*/
typedef enum MwsfdVideoCodec {
    MWSFD_VIDEOCODEC_UNKNOWN     = 0,    /* ���m */
    MWSFD_VIDEOCODEC_M1V         = 1,    /* MPEG-1 Video, Sofdec Video */
    MWSFD_VIDEOCODEC_M2V         = 2,    /* MPEG-2 Video */
    MWSFD_VIDEOCODEC_M4V         = 3,    /* MPEG-4 Video */
    MWSFD_VIDEOCODEC_H264        = 4,    /* H.264 */

    MWSFD_VIDEOCODEC_END,
	MWSFD_VIDEOCODEC_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdVideoCodec;

/* �f�B�U���[�h	*/
/* Dither mode	*/
typedef enum MwsfdDitherMode {
	MWSFD_DITHERMODE_AUTO	= 0,    /* ���� */
	MWSFD_DITHERMODE_ON		= 1,    /* �f�B�U�L�� */
	MWSFD_DITHERMODE_OFF	= 2,    /* �f�B�U���� */

	MWSFD_DITHERMODE_END,
	MWSFD_DITHERMODE_ENUM_BE_SINT32 = 0x7FFFFFFF
} MwsfdDitherMode;

/***************************************************************************
 *      Data type declaration
 ***************************************************************************/
/*	MWPLY handle	*/
typedef void* MWPLY;

/* �t���[���v�[���n���h�� */
typedef CriSint32 MwsfdFrmpoolHn;

/* Sofdec�̏������p�����[�^�\����							*/
/* Parameter structure of Sofdec initialization function	*/
typedef struct {
	CriFloat32		vhz;			/* �����������g��[Hz]						*/
								/* Vsync frequency[Hz]						*/
	CriSint32		disp_cycle;		/* (���g�p) �\���X�V����[v]				 	*/
								/* (Not in use) Display update cycle[v]		*/
	CriSint32		disp_latency;	/* (���g�p) �\�����C�e���V[v]				*/
								/* (Not in use) Display latency[v]			*/
	MwsfdDecSvr	dec_svr;		/* �f�R�[�h�T�[�o				 			*/
								/* Decode Server							*/
	CriSint32		rsv[4];			/* �\�� (�S��0��ݒ肵�ĉ�����) 			*/
								/* Reserved(Please set 0 in all of area)	*/
} MwsfdInitPrm, MWS_PLY_INIT_SFD;

/* �n���h�������p�����[�^�\����				*/
/* Parameter structure of handle creation	*/
typedef struct {
	CriSint32	ftype;				/* �Đ�����X�g���[���̎�� 				*/
								/* File type								*/
	CriSint32	max_bps;			/* �ő�̃r�b�g�X�g���[���ʁ@(bit/sec)		*/
								/* Maximum number of bit per second			*/
	CriSint32	max_width;			/* �Đ��摜�T�C�Y�̍ő啝					*/
								/* Maximum width of video stream			*/
	CriSint32	max_height;			/* �Đ��摜�T�C�Y�̍ő卂��					*/
								/* Maximum height of video stream			*/
	CriSint32	nfrm_pool_wk;		/* �V�X�e���̈�̗L���t���[���v�[�����B		*/
								/* �ʏ��(2)�̒l���w�肷��B����͎Q��		*/
								/* �t���[���v�[���������܂܂Ȃ����B			*/
								/* �t���[�����������������ꍇ�́A���̒l��	*/
								/* ���₵�Ă��������B						*/
								/* Number of effective frame pools in 		*/
								/* system memory. Normaly this number is 2.	*/
								/* this number dosen't contain number of 	*/
								/* reference frame pools. If frame is 		*/
								/* droped,you have to increase this number.	*/
	CriSint32	max_stm;			/* �����ǂݍ��݃X�g���[����(ADX�܂�)		*/
								/* �l��0�̏ꍇ�̓f�t�H���g�l(1)�Ƃ݂Ȃ��B	*/
								/* The number of maximum streams playing 	*/
								/* at the same time. 						*/
								/* This number include ADXT/ADXF streams.	*/
								/* If the value is zero then assume 1.		*/
	CriSint8	*work;				/* ���[�N�̈�								*/
								/* Address of working area					*/
	CriSint32	wksize;				/* ���[�N�̈�T�C�Y							*/
								/* Size of working area						*/
	CriSint32  compo_mode;			/* �������[�h								*/
								/* Composition mode							*/
	MwsfdBufFmt		buffmt;		/* �f�R�[�h�o�̓t���[���o�b�t�@�f�[�^�`��	*/
								/* �l��0�̏ꍇ�̓f�t�H���g�`���Ƃ݂Ȃ��B	*/
								/* Buffer format of decoder	output			*/
								/* If the value is 0 then assume defualt.	*/

	/* �ȉ��̃����o�͓���̃^�[�Q�b�g�ł����g�p���Ȃ��B       */
	/* The following members are used with a specific target. */
	CriSint32	outer_frmpool_num;	/* �O���t���[���v�[�����B����͎Q�ƃt���[��	*/
								/* �v�[���������܂߂��l�B					*/
								/* Number of outer frame pools. 			*/
								/* this number contain number of reference	*/
								/* frame pools.								*/
	CriSint32	outer_frmpool_size;	/* �O���t���[���v�[���p�o�b�t�@�T�C�Y		*/
	                            /* �i�P�t���[�����j							*/
								/* Size of outer frame pool.                */
								/* (Size for one frame)                     */
	CriUint8	**outer_frmpool_ptr;/* �O���t���[���v�[���p�o�b�t�@�|�C���^�z�� */
	                            /* �ւ̃|�C���^								*/
								/* Pointer to pointer array for outer frame */
								/* pools.									*/
	MwsfdFrmpoolHn	frmpool_hn;	/* �t���[���v�[���n���h��                   */
	
	CriSint32	rsv[1];			/* �\�� (�S��0��ݒ肵�ĉ�����) 			*/
								/* Reserved(Please set 0 in all of area)	*/
} MwsfdCrePrm, MWS_PLY_CPRM_SFD;

/* �����f�[�^�p�����[�^	*/
/* Subtitle parameter		*/
typedef struct {
	CriBool	update;			/* update data flag								*/
	CriSint32	datasize;		/* size of data [byte]							*/
	CriSint32	tunit;			/* time unit									*/
	CriSint32	intime;			/* start display time.  sec = intime / tunit.	*/
	CriSint32	durtime;		/* duration of display time. sec = intime / tunit.	*/
} MwsfdSbtPrm;

/* MPEG-2�t���[�����		*/
/* MPEG-2 detail infomation	*/
typedef struct {
	CriSint64				pts;						/* PES   :PTS 33bit (-1:PTS���Ȃ�) */
	CriSint32				picture_structure;			/* PIC_C :�s�N�`���\�� 0:�\�� 1:top-field, 2:bottom-f. 3:frame */
	CriSint32				chroma_format;				/* SEQ_X :�F���t�H�[�}�b�g 0:�\�� 1:420 2:422 3:444	*/
	CriSint32				bit_rate;					/* SEQ,X :�r�b�g���[�g 18+12=30bit */
	CriSint32				vbv_buffer_size;			/* SEQ,X :VBV�o�b�t�@�T�C�Y 10+8=18bit */
	CriSint16				display_horizontal_size;	/* SEQ_D :�\���� 14bit */
	CriSint16				display_vertical_size;		/* SEQ_D :�\������ 14bit */
	CriSint8				progressive_frame;			/* PIC_C :0:�C���^���[�X 1:�v���O���b�V�u */
	CriSint8				top_field_first;			/* PIC_C :�g�b�v�t�B�[���h�D��t���O */
	CriSint8				repeat_first_field;			/* PIC_C :�t�B�[���h�����\���t���O */
	CriSint8				aspect_ratio_information;	/* SEQ   :�A�X�y�N�g���� 4bit */
	CriSint8				constrained_parameters_flag;/* SEQ   :����p�����[�^�t���O */
	CriSint8				frame_rate_extension_n;		/* SEQ_X :�t���[�����[�g�g��n 2bit */
	CriSint8				frame_rate_extension_d;		/* SEQ_X :�t���[�����[�g�g��d 5bit */
	CriUint8				profile_and_level_indication;/*SEQ_X :�v���t�@�C���A���x�� 8bit */
	CriSint8				progressive_sequence;		/* SEQ_X :�v���O���b�V�u�V�[�P���X�t���O */
	CriSint8				low_delay;					/* SEQ_X :��x���t���O */
	CriSint8				video_format;				/* SEQ_D :�r�f�I�t�H�[�}�b�g 3bit */
	CriSint8				colour_description;			/* SEQ_D :�F���L�^�t���O 1bit */
	CriUint8				colour_primaries;			/* SEQ_D :��񌹌��F�̐F�x���W 8bit */
	CriUint8				transfer_characteristics;	/* SEQ_D :�ϊ����� 8bit */
	CriUint8				matrix_coefficients;		/* SEQ_D :�}�g���N�X�W�� 8bit */
	CriSint16				num_slices;					/* slice :�X���C�X�� */
} MwsfdFrmM2v;

/* MPEG-4�t���[�����		*/
/* MPEG-4 detail infomation	*/
typedef struct {
	CriSint64				pts;						/* PES   :PTS 33bit (-1:PTS���Ȃ�) */
	void				*header;					/* MPEG-4�r�f�I�̊e��w�b�_��� */
} MwsfdFrmM4v;

/* �R�[�f�b�N�ŗL�̃t���[�����		*/
/* Codec-specific frame information	*/
typedef struct {
	MwsfdVideoCodec		videocodec;		/* �r�f�I�R�[�f�b�N */
	union {
		MwsfdFrmM2v		frmm2v;			/* MPEG-2�t���[�����iMPEG-1, SofdecVideo�ł��L���j */
		MwsfdFrmM4v		frmm4v;			/* MPEG-4�t���[����� */
	} u;
} MwsfdFrmCodec;

/* �t���[�����\���� */
/* Frame Information  */
typedef struct {
	CriUint8			*bufadr;			/* �f�R�[�h���ʃo�b�t�@�A�h���X		*/
										/* Decoder's output buffer address	*/
	CriSint32			frmid;				/* Frame ID */
	MwsfdBufFmt		buffmt;				/* �f�R�[�h���ʃo�b�t�@�f�[�^�`��	*/
										/* Decoder's output buffer format	*/
	CriSint32			width;				/* ���s�N�Z����						*/
										/* Width by the pixel				*/
	CriSint32			height;				/* �c�s�N�Z����						*/
										/* Height by the pixel				*/
	CriSint32			disp_width;			/* �\�����s�N�Z����					*/
										/* Effective Width by the pixel		*/
	CriSint32			disp_height;		/* �\���c�s�N�Z����					*/
										/* Effective Height by the pixel	*/
	CriSint32			x_mb;				/* ���}�N���u���b�N��				*/
										/* Width by the macroblock			*/
	CriSint32			y_mb;				/* �c�}�N���u���b�N��				*/
										/* Height by the macroblock			*/
	MwsfdPtype		ptype;				/* �s�N�`���^�C�v					*/
										/* Picture type						*/
	CriSint32			fps;				/* �t���[�����[�g[fps * 1000]		*/
										/* Frame rate [fps * 1000]			*/
	CriSint32			fno;				/* �t���[���ԍ�						*/
										/* Count of frames					*/
	CriSint32			time;				/* �\������							*/
										/* Time of disp						*/
	CriSint32			tunit;				/* �\�������P��						*/
										/* Unit of display time				*/
	CriSint32			concat_cnt;			/* �A��������						*/
										/* Count of concatenation			*/
	CriSint32			fno_per_file;		/* �t�@�C�����̃t���[���ԍ�			*/
										/* Count of frames per file			*/
	CriSint32			time_per_file;		/* �t�@�C�����̍Đ�����				*/
										/* Playtime per file				*/
	CriSint32			errcnt;				/* �f�[�^�G���[������				*/
										/* Count of data error				*/
	CriSint32			rcvcnt;				/* �f�[�^�G���[�񕜉�				*/
										/* Count of error recovery			*/
	void			*usrdatptr;			/* Picture layer user data address	*/
	CriSint32			usrdatsize;			/* Picture layer user data size		*/
	MwsfdFrmType	frmtype;			/* �t���[���^�C�v 					*/
										/* Frame type		 				*/

	MwsfdYuvMode	yuvmode;			/* YUV�ϊ����[�h					*/
										/* YUV conversion mode				*/
	CriUint32			zmin;				/* Minimum dynamic range for Z movie */
	CriUint32			zmax;				/* Maximum dynamic range for Z movie */

	MwsfdFrmCodec	frmcodec;			/* �R�[�f�b�N�ŗL�̃t���[�����		*/

	void			*sud_ptr;
	CriSint32			sud_size;
} MwsfdFrmObj, MWS_PLY_FRM;

/* �Đ����\���� 		*/
/* Playback information	*/
typedef struct {
	CriSint32		drop_frm_cnt;		/* Count of drop frame 						*/
	CriSint32		skip_dec_cnt;		/* Count of skip decode						*/
	CriSint32		skip_disp_cnt;		/* Count of skip frame was not obtatined	*/
	CriSint32		skip_emptyb_cnt;	/* Count of skip empty-Bpicture 			*/
	CriSint32		no_supply_cnt;		/* Count of data supply shortage			*/
	CriFloat32		average_fps;
} MwsfdPlyInf;

/* �t�@�C���w�b�_���		*/
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

/* YCbCr�v���[�����\���� */
/* YCbCr Plane Information */
typedef struct {
	CriUint8 		*y_ptr;		/* Y  Buffer Pointer */
	CriUint8 		*cb_ptr;	/* Cb Buffer Pointer */
	CriUint8 		*cr_ptr;	/* Cr Buffer Pointer */
	CriSint32		y_width;	/* Y  Buffer width   */
	CriSint32		cb_width;	/* Cb Buffer width   */
	CriSint32		cr_width;	/* Cr Buffer width   */
} MwsfdYccPlane, MWS_PLY_YCC_PLANE;

/* I picture �V�[�N�Đ��p���\����     */
/* I pciture seek playback information  */
typedef struct {
	CriUint32		offset;		/* Byte offset from top of the file */
	CriSint32		fno;		/* Frame number in an I picture movie */
} MwsfdSeekIpic;

/* �y�ϊ��p�R�[���o�b�N�֐� */
/* ���y�o�b�t�@�p�ϊ��e�[�u�����A�v���P�[�V���������삷�邽�߂̊֐��B
 * 	orgtbl : �f�R�[�h���ʂ���A�y�l(0x00000000�`0x7FFFFFFF)�ւ̕ϊ��e�[�u���|�C���^�B
 *           �e�[�u�����̂�MWPLY�n���h���̒��ɂ���B
 *  znear, zfar : �j�A�^�t�@�[
 *  ztbl   : �y�l(0x00000000�`0x7FFFFFFF)����y�o�b�t�@���e�ւ̕ϊ��e�[�u���|�C���^�B
 *           �֐�����CriSint16��CriSint32�ɃL���X�g���Ďg�p����B
 *           �e�[�u�����̂�MWPLY�n���h���̒��ɂ���B
 */
typedef void (*MwsfdMakeZTblCbFn)(CriUint32 *orgtbl, CriFloat32 znear, CriFloat32 zfar, void *ztbl);

/* ���ʃJ�E���^ */
/* Flow counter	*/
typedef struct {
	CriSint64			supplied;		/* �����T�C�Y[byte]                                     */
									/* supplied data size [byte]                            */
	CriSint64			consumed;		/* ����T�C�Y[byte] (�j���T�C�Y���܂�)                  */
									/* consumed data size [byte] (including discarded size) */
	CriSint64			discarded;		/* �j���T�C�Y[byte]                                     */
									/* discarded data size [byte]                           */
} MwsfdFlowCnt;

/* ���ʏ��\���� 	*/
/* Flow information	*/
typedef struct {
	MwsfdFlowCnt	system;		/* system decoder flow counter                                 */
	MwsfdFlowCnt	video;		/* video decoder flow counter ("discarded" is not implemented) */
	MwsfdFlowCnt	audio;		/* audio decoder flow counter ("discarded" is not implemented) */
	MwsfdFlowCnt	adxtin;		/* adxt input sj flow counter ("discarded" is not implemented) */
} MwsfdFlowInf;

/* �O���N���b�N�֐� */
typedef void (*MwsfdExtClockFn)(void *usrobj, CriSint32 *time, CriSint32 *tunit);

/* �t���[���ϊ������R�[���o�b�N�֐� */
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
 *		��{�Đ�����
 *      Basic functions
 *=========================================================================*/
/* MWSFD(Sofdec)���C�u�����̏�����	*/
/* Initialization of Sofdec library	*/
void CRIAPI mwPlyInitSfdFx(MwsfdInitPrm *iprm);

/* MWSFD(Sofdec)���C�u�����̏I��	*/
/* Termination of Sofdec library 	*/
void CRIAPI mwPlyFinishSfdFx(void);

/* ��Ɨ̈�T�C�Y�̌v�Z										*/
/* Calculation of working area size from create parameter	*/
CriSint32 CRIAPI mwPlyCalcWorkCprmSfd(MwsfdCrePrm *cprm);

/* MWSFD�n���h���̐���				*/
/* Create MWPLY handle for Sofdec	*/
MWPLY CRIAPI mwPlyCreateSofdec(MwsfdCrePrm *cprm);

/* �Đ��n���h���̏���						*/
/* Destroy MWPLY handle						*/
void CRIAPI mwPlyDestroy(MWPLY mwply);

/* �Đ��J�n�i�t�@�C������̍Đ��j								*/
/* Start playing by file name 									*/
void CRIAPI mwPlyStartFname(MWPLY mwply, const CriChar8 *fname);

/* �Đ���~									*/
/* Stop playing  							*/
void CRIAPI mwPlyStop(MWPLY mwply);

/* �n���h����Ԃ̎擾								*/
/* Get status of MWPLY handle						*/
MwsfdStat CRIAPI mwPlyGetStat(MWPLY mwply);

/* �t���[���̎擾           */
/* Get frame buffer pointer */
void CRIAPI mwPlyGetCurFrm(MWPLY mwply, MwsfdFrmObj *frm);

/* �t���[���̉�� 			*/
/* Release frame buffer		*/
void CRIAPI mwPlyRelCurFrm(MWPLY mwply);

/* �Đ��T���v�����̎擾														*/
/* Get playing time															*/
/* If playing movie is 30 fps, *tscale is 30 and *ncount is total number of */
/*  displayed frames														*/
void CRIAPI mwPlyGetTime(MWPLY mwply, CriSint32 *ncount, CriSint32 *tscale);

/* �|�[�Y�^�|�[�Y����									*/
/* Set pause switch.  sw=0(Continue), 1(Pause)			*/
void CRIAPI mwPlyPause(MWPLY mwply, CriSint32 sw);

/* �|�[�Y��Ԃ̎擾 */
/* Get pause ON/OFF	*/
CriBool CRIAPI mwPlyIsPause(MWPLY mwply);

/* �R�}����   */
/* Step frame */
void CRIAPI mwPlyStepFrame(MWPLY mwply);

/* <NEW> Get frame */
/* <�V> �t���[���̎擾 */
/* [����]
 *    mwply    :MWPLY�n���h��
 * [�o��]
 *    frmid    :�擾�ł����t���[���̃t���[��ID�i�ϐ����̂̓��[�U����`�j
 *    frm      :�擾�ł����t���[���̃t���[�����\���́i�ϐ����̂̓��[�U����`�j
 * [�֐��l]
 *    TRUE     : �t���[�����擾�ł���
 *    FALSE    : �t���[�����擾�ł��Ȃ�����
 * [���l]
 * �EmwPlyGetCurFrm�֐�/mwPlyRelCurFrm�֐��Ƃ̕��p�͋֎~�B
 * �E�\�������ɂȂ��Ă���t���[�����擾����B
 * �E����t���[���͈�x�����擾�ł���B
 * �E�Â��t���[����������Ȃ��Ă��A�V�����t���[�����擾�ł���B
 * �E�t���[�����e��mwPlyRelFrm�֐��ŉ�����邩�AmwPlyStop�֐����s�܂ŕێ��B
 * �EmwPlyGetFrm�֐���mwPlyRelFrm�֐��͓���X���b�h���Ŏ��s���邱�ƁB
 */
CriBool CRIAPI mwPlyGetFrm(MWPLY mwply, CriSint32 *frmid, MwsfdFrmObj *frm);

/* <NEW> Release frame */
/* <�V> �t���[���̉�� */
/* [����]
 *    mwply    :MWPLY�n���h��
 *    frmid    :�������t���[���̃t���[��ID
 * [���l]
 * �EmwPlyGetCurFrm�֐�/mwPlyRelCurFrm�֐��Ƃ̕��p�͋֎~�B
 * �E����frmid�Ŏw�肵���t���[�����������B
 * �E�����t���[��ID�ɑ΂��Ẳ���͂P�x�̂݁B�Q�x�ڈȍ~�̓G���[�R�[���o�b�N�B
 * �E�t���[�����擾�������ԂƉ�����鏇�Ԃ͈�v���Ȃ��Ă悢�B
 * �E�S�Ă̎擾�ς݃t���[����������Ȃ�����AMWPLY�n���h����PLAYEND��ԂɂȂ�Ȃ��B
 * �EmwPlyGetFrm�֐���mwPlyRelFrm�֐��͓���X���b�h���Ŏ��s���邱�ƁB
 */
void CRIAPI mwPlyRelFrm(MWPLY mwply, CriSint32 frmid);

/*=========================================================================*
 *		�g���t�@�C���Đ�
 *		Extended File Playback
 *=========================================================================*/
/* FID �w��ɂ��Đ��̊J�n 				*/
/* Start AFS palyback by specified file ID	*/
void CRIAPI mwPlyStartAfs(MWPLY mwply, CriSint32 patid, CriSint32 fid);

/* �p�b�N�t�@�C����SFD�f�[�^�̍Đ��J�n */
/* Start playing SFD data in packed file */
/* [Inputs  ] mwply			: MWPLY handle
 *			  fname			: File name
 *			  offset_sct	: File offset from top of the file as a sector
 *			  range_sct		: File range as a sector					 */
void CRIAPI mwPlyStartFnameRange(MWPLY mwply, const CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);

/* SJ�ɂ��Đ��̊J�n								*/
/* Start playing by stream joint					*/
void CRIAPI mwPlyStartSj(MWPLY mwply, SJ sji);

/* �f�[�^�����I���̒ʒm(SJ�Đ��p) */
/* Notify the end of data supply, for mwPlyStartSj() */
void CRIAPI mwPlyTermSupply(MWPLY mwply);

/* �A�h���X�w�胁�����ɂ��Đ��J�n										*/
/* Start playing by memory												*/
void CRIAPI mwPlyStartMem(MWPLY mwply, const void *addr, CriSint32 len);
/* �A�h���X�w�胁�����ɂ�郋�[�v�Đ��J�n								*/
/* Start looping playback by memory										*/
void CRIAPI mwPlyStartMemLp(MWPLY mwply, const void *data, CriSint32 dtlen);

/* �������A���Đ��̊J�n */
/* - �S�Ă̘A���t�@�C����o�^���I�������AmwPlyReleaseMemSeamless�֐����Ă� */
/* - mwPlyReleaseMemSeamless�֐����Ă΂Ȃ��ꍇ�́A�Đ��I����ԂɂȂ�Ȃ� */
void CRIAPI mwPlyStartMemSeamless(MWPLY mwply);
/* �������A���Đ��̓o�^ */
/* - mwPlyStartMemSeamless�֐��̌ďo����ɗL�� */
/* - �����ɓo�^�ł���̂͂P�U�܂� */
void CRIAPI mwPlyEntryMemSeamless(MWPLY mwply, const void *data, CriSint32 dtlen);
/* �������A���Đ��̓o�^�I���錾 */
void CRIAPI mwPlyReleaseMemSeamless(MWPLY mwply);
/* �������A���Đ��̓o�^���擾 */
CriSint32 CRIAPI mwPlyGetNumMemSeamless(MWPLY mwply);

/*=========================================================================*
 *		�V�[�����X���[�v�Đ�
 *		Seamless Loop Playback
 *=========================================================================*/
/* �V�[�����X���[�v�Đ��̊J�n	*/
/* Start semaless loop playback	*/
void CRIAPI mwPlyStartFnameLp(MWPLY mwply, const CriChar8 *fname);

/* �V�[�����X���[�v�Đ��̉���		*/
/* Release semaless loop playback	*/
void CRIAPI mwPlyReleaseLp(MWPLY mwply);

/* FID �w��ɂ��V�[�����X���[�v�Đ��̊J�n	  			*/
/* Start semaless loop playback by specified file ID	*/
void CRIAPI mwPlyStartAfsLp(MWPLY mwply, CriSint32 patid, CriSint32 fid);

/* �p�b�N�t�@�C����SFD�f�[�^�̃V�[�����X���[�v�Đ��̊J�n	*/
/* Start looping playing SFD data in packed file */
void CRIAPI mwPlyStartFnameRangeLp(MWPLY mwply, const CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);

/*=========================================================================*
 *		�V�[�����X�A���Đ�
 *		Seamless Continuous Playback
 *=========================================================================*/
/* �V�[�����X�A���Đ��t�@�C���̓o�^				*/
/* Entry file of Seamless continuous playback	*/
void CRIAPI mwPlyEntryFname(MWPLY mwply,const CriChar8 *fname);

/* �V�[�����X�A���Đ��̊J�n				*/
/* Start seamless continuous playback	*/
void CRIAPI mwPlyStartSeamless(MWPLY mwply);

/* �V�[�����X�A���Đ��̉���				*/
/* Release seamless continuous playback	*/
void CRIAPI mwPlyReleaseSeamless(MWPLY mwply);

/*	�V�[�����X���[�v�Đ��̐ݒ�	*/
/*	Set semaless loop play   	*/
void CRIAPI mwPlySetSeamlessLp(MWPLY mwply, CriSint32 flg);

/* ���ݓo�^����Ă���t�@�C�����̎擾			*/
/* Get number of file entried seamless loop 	*/
CriSint32 CRIAPI mwPlyGetNumSlFiles(MWPLY mwply);

/* �V�[�����X�A���Đ��ɓo�^���Ă���t�@�C�����̎擾	*/
/* Get file name of entried seamless				*/
const CriChar8 * CRIAPI mwPlyGetSlFname(MWPLY mwply, CriSint32 stm_no);

/* �V�[�����X�A���Đ��t�@�C���̓o�^ (AFS)			*/
/* Entry file of Seamless continuous playback (AFS)	*/
void CRIAPI mwPlyEntryAfs(MWPLY mwply, CriSint32 patid, CriSint32 fid);

/* �p�b�N�t�@�C����SFD�f�[�^�̃V�[�����X�A���Đ��o�^				*/
/* Entry SFD data in packed file to seamless continuous playback	*/
void CRIAPI mwPlyEntryFnameRange(MWPLY mwply, const CriChar8 *fname, CriSint32 offset_sct, CriSint32 range_sct);

/*=========================================================================*
 *		�I�[�f�B�I�ݒ�
 *		Audio Configuration
 *=========================================================================*/
/* �I�[�f�B�I�o�̓{�����[���̐ݒ�							*/
/* Set Output Volume (vol= 0 to -960)						*/
void CRIAPI mwPlySetOutVol(MWPLY mwply, CriSint32 vol);

/* �I�[�f�B�I�o�̓{�����[���̎擾				*/
/* Get Output Volume (return value= 0 to -960)	*/
CriSint32 CRIAPI mwPlyGetOutVol(MWPLY mwply);

/* �I�[�f�B�I�o�̓p���|�b�g�̐ݒ�										*/
/* Set output panpot c=0(Mono/Left),1(Right)							*/
/* p = +15(Right),0(Center),-15(Left),-128(Auto)						*/
void CRIAPI mwPlySetOutPan(MWPLY mwply, CriSint32 chno, CriSint32 pan);

/* �I�[�f�B�I�o�̓p���|�b�g�̎擾								*/
/* Get output panpot c=0(Mono/Left),1(Right)					*/
CriSint32 CRIAPI mwPlyGetOutPan(MWPLY mwply, CriSint32 chno);

/* �I�[�f�B�I�X�g���[���`���l�����̎擾		*/
/* Get the number of audio channel streams	*/
CriSint32 CRIAPI mwPlyGetNumAudioCh(MWPLY mwply);

/* �I�[�f�B�I�X�g���[���`���l���̐ݒ�	*/
/* Set audio stream channel 			*/
void CRIAPI mwPlySetAudioCh(MWPLY mwply, CriSint32 ch);

/* �I�[�f�B�I�X�g���[���`���l���̎擾 (-1: ����܂��͍Đ��Ȃ�)	*/
/* Get audio stream channel (-1: pending or out of work) */
CriSint32 CRIAPI mwPlyGetAudioCh(MWPLY mwply);

/*=========================================================================*
 *		�e��I�[�f�B�I�ݒ�
 *		Various Audio Configuration
 *=========================================================================*/
/* �{�C�X�g���b�N�Đ��p���[�N�o�b�t�@�T�C�Y�̎擾 */
/* Get size of work buffer for voice track in movie */
CriSint32 CRIAPI mwPlyGetVoiceWorkSize(void);

/* �{�C�X�g���b�N�Đ��@�\�̒ǉ� */
/* Attach voice track playback function to MWPLY */
void CRIAPI mwPlyAttachVoice(MWPLY mwply, void *wkptr, CriSint32 wksize);

/* �{�C�X�g���b�N�Đ��@�\�̕��� */
/* Detach voice track playback function from MWPLY */
void CRIAPI mwPlyDetachVoice(MWPLY mwply);

/* �{�C�X�g���b�N�`���l���ԍ��̐ݒ� */
/* Set channel number of voice track */
void CRIAPI mwPlySetVoiceCh(MWPLY mwply, CriSint32 ch);

/* AHX�Đ��p���[�N�o�b�t�@�T�C�Y�̎擾 */
/* Get size of work buffer for AHX in movie */
CriSint32 CRIAPI mwPlyGetAhxWorkSize(void);

/* AHX�Đ��@�\�̒ǉ� */
/* Attach AHX to MWPLY */
void CRIAPI mwPlyAttachAhx(MWPLY mwply, void *wkptr, CriSint32 wksize, CriSint32 ch);

/* AHX�Đ��@�\�̕��� */
/* Detach AHX from MWPLY */
void CRIAPI mwPlyDetachAhx(MWPLY mwply);

/* AHX�`���l���ԍ��̐ݒ� */
/* Set channel number of AHX */
void CRIAPI mwPlySetAhxCh(MWPLY mwply, CriSint32 ch);

/*=========================================================================*
 *		�T�u�^�C�g���i�����j
 *		Subtitle
 *=========================================================================*/
/* �����f�[�^�̎擾 */
/* Get subtitle data */
CriSint32 CRIAPI mwPlyGetSubtitle(MWPLY mwply, CriUint8 *bufptr, CriSint32 bufsize, MwsfdSbtPrm *sbtprm);

/* �����f�[�^�̃`���l���ݒ�		*/
/* Set channel of subtitle data	*/
void CRIAPI mwPlySetSubtitleCh(MWPLY mwply, CriSint32 chno);

/* �T�u�^�C�g���`���l�����̎擾 */
/* Get the number of subtitle channel */
CriSint32 CRIAPI mwPlyGetNumSubtitleCh(MWPLY mwply);

/* �����f�[�^�̃V�t�g�^�C���ݒ�		*/
/* Set shift time of subtitle data	*/
void CRIAPI mwPlySetSubtitleScount(MWPLY mwply, CriSint32 scount);

/*=========================================================================*
 *		�t���[���ϊ��ݒ�
 *		FrameConversion Configuration
 *=========================================================================*/

/* �o�̓o�b�t�@�T�C�Y�̐ݒ� */
/* Set size of out buffer 	*/
void CRIAPI mwPlyFxSetOutBufPitchHeight(MWPLY mwply, CriSint32 pitch, CriSint32 height);
/* �o�̓o�b�t�@�T�C�Y�̎擾 */
/* Get size of out buffer 	*/
void CRIAPI mwPlyFxGetOutBufPitchHeight(MWPLY mwply, CriSint32 *pitch, CriSint32 *height);

/* [for Compatibility] */
void CRIAPI mwPlyFxSetOutBufSize(MWPLY mwply, CriSint32 width, CriSint32 height);
void CRIAPI mwPlyFxGetOutBufSize(MWPLY mwply, CriSint32 *width, CriSint32 *height);

/* �������[�h�̐ݒ� 	*/
/* Set composition mode	*/
void CRIAPI mwPlyFxSetCompoMode(MWPLY mwply, CriSint32 mode);
/* �������[�h�̎擾 */
/* Get composition mode	*/
CriSint32 CRIAPI mwPlyFxGetCompoMode(MWPLY mwply);

/* ���~�i���X�L�[�p�����[�^�̐ݒ�		*/
/* Set luminance key alpha parameter	*/
void CRIAPI mwPlyFxSetLumiPrm(MWPLY mwply, CriSint32 in_th, CriSint32 out_th, MwsfdLumiKey key);
/* ���~�i���X�L�[�p�����[�^�̎擾 		*/
/* Get luminance key alpha parameter	*/
void CRIAPI mwPlyFxGetLumiPrm(MWPLY mwply, CriSint32 *in_th, CriSint32 *out_th, MwsfdLumiKey *key);

/* �R�l�A���t�@�p�p�����[�^�ݒ� */
/* Set alpha 3-value parameter */
void CRIAPI mwPlyFxSetAlp3Prm(MWPLY mwply, CriUint8 zero, CriUint8 half, CriUint8 full);
/* �R�l�A���t�@�p�p�����[�^�擾 */
/* Get alpha 3-value parameter */
void CRIAPI mwPlyFxGetAlp3Prm(MWPLY mwply, CriUint8 *zero, CriUint8 *half, CriUint8 *full);

/*=========================================================================*
 *		�y���[�r
 *		Z movie
 *=========================================================================*/
/* 16bit-�y�o�b�t�@�ւ̕ϊ� 		*/
/* Convert to 16bit depth Z-value	*/
void CRIAPI mwPlyFxCnvFrmZ16(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *zout);
/* 32bit-�y�o�b�t�@�ւ̕ϊ� 					*/
/* Convert to 32bit depth Z-value	*/
void CRIAPI mwPlyFxCnvFrmZ32(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *zout);

/* �y�N���b�v�̐ݒ� */
/* Set Z-clip		*/
void CRIAPI mwPlyFxSetZclip(MWPLY mwply, CriFloat32 znear, CriFloat32 zfar);
/* �y�N���b�v�̎擾 */
/* Get Z-clip		*/
void CRIAPI mwPlyFxGetZclip(MWPLY mwply, CriFloat32 *znear, CriFloat32 *zfar);

/* �y�ϊ��e�[�u���쐬�R�[���o�b�N�֐��̓o�^ */
void CRIAPI mwPlyFxSetCbFnMakeZTbl(MWPLY mwply, MwsfdMakeZTblCbFn func);

/*=========================================================================*
 *		���擾
 *		Information
 *=========================================================================*/

/* F/X��ʂ̎擾 */
/* Get F/X type from SFD data */
CriSint32 CRIAPI mwPlyGetFxType(MWPLY mwply);

/* �t�@�C�����̎擾			*/
/* Get file header information	*/
void CRIAPI mwPlyGetHdrInf(CriSint8 *buf, CriSint32 bsize, MwsfdHdrInf *hdrinf);

/* �w�b�_���i�I�[�f�B�I�ڍׁj�̎擾	*/
/* Get audio detail header information	*/
void CRIAPI mwPlyGetHdrAudioDetail(CriSint8 *buf, CriSint32 bsize, MwsfdHdrAudioDetail *audio_detail);

/* 2008-10-02 */
/* �t�@�C�����w��ɂ�� �t�@�C����� �w�b�_���i�I�[�f�B�I�ڍׁj�̎擾	*/
/* Get header informations by filename	*/
CriBool CRIAPI mwPlyReadAndGetHdrInfs(const CriChar8 *fname,MwsfdHdrInf *hdrinf,MwsfdHdrAudioDetail *audio_detail);

/* ���t���[�����̎擾 				*/
/* Get the number of movie frame	*/
CriSint32 CRIAPI mwPlyGetTotalFrmNum(MWPLY mwply);

/*=========================================================================*
 *		����ݒ�
 *		Special Configuration
 *=========================================================================*/
/* Malloc�^Free �֐��̓o�^ */
/* Set memory allocation / free callback function */
void CRIAPI mwPlySetMallocFn(MwsfdMallocFn mallocfn, MwsfdFreeFn freefn, void *obj);

/* �Đ����x�̐ݒ�(�L�����Ŏw��) */
/* Set playback speed by rational number */
void CRIAPI mwPlySetSpeedRational(MWPLY mwply, CriSint32 speed_numerator, CriSint32 speed_denominator);

/* �Đ����x�̐ݒ�(�W�����x = 1000) */
/* Set playback speed (normal speed = 1000) */
void CRIAPI mwPlySetSpeed(MWPLY mwply, CriSint32 speed);

/* �������[�h�̐ݒ�			*/
/* Set synchronization mode	*/
void CRIAPI mwPlySetSyncMode(MWPLY mwply, CriSint32 mode);
/* �������[�h�̎擾			*/
/* Get synchronization mode	*/
CriSint32 CRIAPI mwPlyGetSyncMode(MWPLY mwply);

/* �^�C�}�`���l���̐ݒ� */
/* Set timer channel	*/
void CRIAPI mwPlySetTimerCh(CriSint32 chno);
/* �^�C�}�`���l���̎擾 */
/* Get timer channel	*/
CriSint32 CRIAPI mwPlyGetTimerCh(void);

/* �s�N�`�����C���̃��[�U�f�[�^�p�o�b�t�@�̒ǉ� */
/* Attach buffer for user data in picture layer */
void CRIAPI mwPlyAttachPicUsrBuf(MWPLY mwply, void *bufptr, CriSint32 bufsize, CriSint32 size1pic);

/* �t���[���擾�������[�h�̐ݒ�				*/
/* Set frame obtain synchronization mode	*/
void CRIAPI mwPlySetFrmSync(MWPLY mwply, MwsfdFrmSync frmsync);
/* �t���[���擾�������[�h�̎擾				*/
/* Get frame obtain synchronization mode	*/
MwsfdFrmSync CRIAPI mwPlyGetFrmSync(MWPLY mwply);

/* �O���N���b�N�֐��̓o�^
 * [����]
 *    mwply             :MWPLY�n���h��
 *    extclock_fn       :�O���N���b�N�֐�
 *    extclock_max      :�N���b�N�l�̍ő�l�B(8�r�b�g�̏ꍇ0x000000FF, 32�r�b�g�̏ꍇ0xFFFFFFFF)
 *    extclock_usrobj   :���[�U�I�u�W�F�N�g(�O���N���b�N�֐��̑�1�����Ƃ��ă��[�U�ɓn��)
 * [�O���N���b�N�̒�`]
 * ���[�r�̍Đ������ǂ����Ɋւ�炸�A��ɒP���������鎞��(�J�E���^)�B
 * [���l]
 * �o�^�����N���b�N�֐��́A���肵�ĒP����������l���o�͂���K�v������B
 * ���炩�̃n�[�h�E�F�A���瓾���鐶�̃N���b�N�l�ł��悭�A�Đ����ɃI�[�o�t���[
 * ���邱�Ƃ������Ă��悢���A�I�[�o�t���[��������������͎����X�V���s���������
 * �\���ɑ傫���Ȃ���΂Ȃ�Ȃ��B
 * ���C�u�����͑O��N���b�N�l�Ƃ̍��������l�ƂȂ邱�ƂŃI�[�o�t���[���������o����B
 * �I�[�o�t���[���o���́A�N���b�N�l������ extclock_max + 1 �����Z���ăI�[�o�t���[
 * ��⏞����B
 */
void CRIAPI mwPlySetCbExtClock(MWPLY mwply, MwsfdExtClockFn extclock_fn, CriSint32 extclock_max, void *extclock_usrobj);

/* �����t���[�����̐ݒ� */
void CRIAPI mwPlySetNumPrepFrm(MWPLY mwply, CriSint32 prep_frm);

/* �����t���[�����̎擾 */
CriSint32 CRIAPI mwPlyGetNumPrepFrm(MWPLY mwply);

/*=========================================================================*
 *		�f�o�b�O�p
 *		Debugging
 *=========================================================================*/
/* Sofdec���C�u�����o�[�W����������̎擾 */
/* Get Sofdec library version string */
const CriChar8 * CRIAPI mwPlyGetVersionStr(void);

/* �Đ����ԏ���̐ݒ�i�f�t�H���g�� 9����57�� 0�b�j			*/
/* Set playback limit time (default = 09-57-00(HH-MM-SS))	*/
void CRIAPI mwPlySetLimitTime(MWPLY mwply, CriSint32 sec);

/* �Đ����̎擾 			*/
/* Get playback information	*/
void CRIAPI mwPlyGetPlyInf(MWPLY mwply, MwsfdPlyInf *plyinf);

/* �f�R�[�h�����S�s�N�`�����̎擾	*/
/* Get Number of Decoded Frames 	*/
CriSint32 CRIAPI mwPlyGetNumTotalDec(MWPLY mwply);

/* �f�R�[�h�ςݕێ��t���[�����̎擾	*/
/* Get Number of Pooled Frames 		*/
CriSint32 CRIAPI mwPlyGetNumDecPool(MWPLY mwply);

/* �R�}���������t���[�����̎擾	*/
/* Get number of droped frame	*/
CriSint32 CRIAPI mwPlyGetNumDropFrm(MWPLY mwply);
/* �f�R�[�h���������t���[�����̎擾				*/
/* Get number of frames was skipped to decode	*/
CriSint32 CRIAPI mwPlyGetNumSkipDec(MWPLY mwply);
/* �\���i�̃X�L�b�v�񐔂̎擾					*/
/* Get number of frames was skipped to obtain	*/
CriSint32 CRIAPI mwPlyGetNumSkipDisp(MWPLY mwply);
/* �X�L�b�v�����G���v�e�BB�s�N�`�������̎擾	*/
/* Get the number of skipped empty-B picture	*/
CriSint32 CRIAPI mwPlyGetNumSkipEmptyB(MWPLY mwply);

/* ���ʏ��̎擾           */
/* Get flow information	    */
void CRIAPI mwPlyGetFlowInf(MWPLY mwply, MwsfdFlowInf *flowinf);

/* B-Picture�f�R�[�h�̃X�L�b�v�ݒ� 	*/
/* Set B-Picture skip decode 		*/
void CRIAPI mwPlySetBpicSkip(MWPLY mwply, CriBool sw);
/* �G���v�e�BB�s�N�`���̃f�R�[�h�X�L�b�v�ݒ� 	*/
/* Set empty-B picture skip decode				*/
void CRIAPI mwPlySetEmptyBpicSkip(MWPLY mwply, CriBool sw);
/* P-Picture�f�R�[�h�̃X�L�b�v�ݒ� 	*/
/* Set P-Picture skip decode 		*/
void CRIAPI mwPlySetPpicSkip(MWPLY mwply, CriBool sw);

/* ADXT�n���h���̎擾	*/
/* Get ADXT handle		*/
ADXT CRIAPI mwPlyGetAdxtHn(MWPLY mwply);
/* ���̓X�g���[���W���C���g�̎擾	*/
/* Get SJ of Input          		*/
SJ CRIAPI mwPlyGetInputSj(MWPLY mwply);
/* �r�f�I�f�R�[�_�n���h���̎擾	*/
/* Get Video decoder handle		*/
void * CRIAPI mwPlyGetSfdHn(MWPLY mwply);

/* �c�t���[�����̎擾 (���o���\�ȃt���[�����̎擾) */
CriSint32 CRIAPI mwPlyGetNumRemainFrm(MWPLY mwply);

/* ���O�L�^�@�\�̒ǉ� */
/* Attach Log recording function */
//void CRIAPI mwPlyAttachLog(CriLog log);

/* ���O�L�^�@�\�̕��� */
/* Attach Log recording function */
//void CRIAPI mwPlyDetachLog(void);

/*=========================================================================*
 *		��W���֐�
 *		Nonstandard Functions
 *=========================================================================*/
/* [for Compatibility] */
/* Please call ADXM_WaitVsync(). This function is same function. */
void CRIAPI mwPlySwitchToIdle(void);

/* �T�[�o��؂�₢���킹							*/
/* Determine whether process is in server border	*/
CriBool CRIAPI mwPlyIsSvrBdr(void);

/* YCbCr�v���[���̌v�Z        */
/* Calculation of YCbCr plane */
void CRIAPI mwPlyCalcYccPlane(CriUint8 *bufptr, CriSint32 width, CriSint32 height, 
								MwsfdYccPlane *ycc);

/* �����o�̓X�C�b�`�̐ݒ�			*/
/* Set audio output switch			*/
void CRIAPI mwPlySetAudioSw(MWPLY mwply, CriSint32 sw);

/* �r�f�I�X�g���[���`���l�����̎擾			*/
/* Get the number of video channel streams	*/
CriSint32 CRIAPI mwPlyGetNumVideoCh(MWPLY mwply);
/* �r�f�I�X�g���[���`���l���̐ݒ�	*/
/* Set video stream channel 		*/
void CRIAPI mwPlySetVideoCh(MWPLY mwply, CriSint32 ch);
/* �r�f�I�X�g���[���`���l���̎擾 (-1: ����܂��͍Đ��Ȃ�)	*/
/* Get video stream channel (-1: pending or out of work) */
CriSint32 CRIAPI mwPlyGetVideoCh(MWPLY mwply);

/* �r�f�I�o�̓X�C�b�`�̐ݒ�	*/
/* Set video output switch	*/
void CRIAPI mwPlySetVideoSw(MWPLY mwply, CriSint32 sw);

/* �n���h������Vsync����	*/
/* V-sync function 			*/
void CRIAPI mwPlyVsyncHndl(MWPLY mwply);
/* �n���h�����̃T�[�o���� 		*/
/* Execute decoding one frame	*/
CriBool CRIAPI mwPlyExecSvrHndl(MWPLY mwply);

/* In the case of that no use ADXM_SetupThrd() and ADXM_ExecMain(). */
/* �X���b�h�O���Ǘ����̎����X�V�T�[�o */
/* [�Ԃ�l] TRUE : ���邱�Ƃ��c���Ă���BFALSE: ������邱�Ƃ͖����B */
CriBool CRIAPI mwPlyExecVsyncServer(void);
/* In the case of that no use ADXM_SetupThrd() and ADXM_ExecMain(). */
/* �X���b�h�O���Ǘ����̃f�R�[�h�T�[�o */
/* [�Ԃ�l] TRUE : ���邱�Ƃ��c���Ă���BFALSE: ������邱�Ƃ͖����B */
/* ATTN: �}���`�X���b�h�œ������ꍇ�̓T�[�o��؂菈���ɒ��ӁB */
CriBool CRIAPI mwPlyExecDecServer(void);

/* �L�[���[�h�̐ݒ�(NULL�w��ŃL�[���[�h����) */
void CRIAPI mwPlySetKeyString(MWPLY mwply, const CriChar8 *str);
/* �t�@�C�����̎擾�i�L�[���[�h�w��j	*/
void CRIAPI mwPlyGetHdrInfWithKey(CriSint8 *buf, CriSint32 bsize, const CriChar8 *str, MwsfdHdrInf *hdrinf);

/* �����X�V��VbIn�Ŏ����I�ɍs�����ǂ����̐ݒ�							*/
/* sw = ON	: VbIn�Ŏ����I�ɍX�V�i�f�t�H���g�j							*/
/*    = OFF	: �����I�ɍX�V�͍s��Ȃ��imwPlyUpdateTime�֐��ōX�V����j	*/
void CRIAPI mwPlySetUpdateTimeAuto(MWPLY mwply, CriBool sw);

/* �����̍X�V
 * [���l]
 * mwPlySetUpdateTimeAuto�֐���OFF���w�肵���ꍇ�A����I�ɌĂяo���K�v������B
 * ���C�����[�v��V�����҂��̒���ȂǁAV���Ԓ��̂ł��邾�������^�C�~���O
 * ���]�܂����B
 * �Ăяo���񐔂��Ǘ�����K�v�͂Ȃ��B�܂�A���C�����[�v����������������
 * �ɁA�񐔂�₤���߂ɉ��x���Ăяo���Ȃ��Ă悢�B
 */
void CRIAPI mwPlyUpdateTime(MWPLY mwply);

/*=========================================================================*/
/*=========================================================================*
 *		�t���[���ϊ�
 *		Frame Conversion
 *=========================================================================*/
/*=========================================================================*/
/* ARGB8888�v���[���t�H�[�}�b�g�ւ̕ϊ�	*/
/* Convert to plane ARGB8888 			*/
void CRIAPI mwPlyFxCnvFrmARGB8888(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *out);

/* YUV422�v���[���t�H�[�}�b�g�ϊ�	*/
/* Convert to plane YUV422 			*/
void CRIAPI mwPlyFxCnvFrmYUV422(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *out);

/* Y84C44�v���[���t�H�[�}�b�g�ϊ�	*/
/* Convert to plane Y84C44 			*/
void CRIAPI mwPlyFxCnvFrmY84C44(MWPLY mwply, MwsfdFrmObj *frm, CriUint8 *yout, CriUint8 *cout);

/* ARGB8888�}�N���u���b�N�t�H�[�}�b�g�ւ̕ϊ� */
/* Conver to macloblock ARGB8888 */
void CRIAPI mwPlyFxCnvFrmMbARGB8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

/* RGBA8888�v���[���t�H�[�}�b�g�ւ̕ϊ� */
/* Conver to plane RGBA8888 */
void CRIAPI mwPlyFxCnvFrmRgba8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

/* BGRA8888�v���[���t�H�[�}�b�g�ւ̕ϊ� */
/* Conver to plane BGRA8888 */
void CRIAPI mwPlyFxCnvFrmBgra8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

#if defined(XPT_TGT_PC)
void CRIAPI mwPlyFxCnvFrmAbgr8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);
void CRIAPI mwPlyFxCnvFrmClipAbgr8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
#endif

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*
 * [��`�w��ɂ��t���[���ϊ�����]
 *=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*
 *  (lux, luy) : �ϊ����Ƃ̍�����W�i�����w��j
 *  (w, h)     : �ϊ�����̈�̕��ƍ���
 * [�p�����[�^�w��̗�]
 *  ----+------------------------------------------------------------------
 *      |  �㔼��     ������     ������     �E����     �c�������̒���
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
/* ��`�͈͎w���ARGB8888�ɕϊ� */
/* Convert frame to ARGB8888 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipARGB8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
/* ��`�͈͎w���YUV422�ɕϊ� */
/* Convert frame to YUV422 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipYUV422(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* RGB888�v���[���t�H�[�}�b�g�ւ̕ϊ�	*/
/* Convert to plane RGB888 				*/
void CRIAPI mwPlyFxCnvFrmRGB888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out);

/* ��`�w���RGB565�ϊ��֐�					*/
/* Convert frame to RGB565 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB565(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
/* ��`�w���RGB565�ϊ��֐��i�f�B�U�t���j	*/
/* Convert frame to RGB565 with dithering by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB565WithDither(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* ��`�w���RGB555�ϊ��֐�					*/
/* Convert frame to RGB555 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB555(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);
/* ��`�w���RGB555�ϊ��֐��i�f�B�U�t���j	*/
/* Convert frame to RGB555 with dithering by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRGB555WithDither(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out,
									CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* ��`�͈͎w���RGBA8888�ɕϊ� */
/* Convert frame to RGBA8888 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipRgba8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* ��`�͈͎w���BGRA8888�ɕϊ� */
/* Convert frame to BGRA8888 by clipping rectangle area */
void CRIAPI mwPlyFxCnvFrmClipBgra8888(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, 
								CriSint32 lux, CriSint32 luy, CriSint32 w, CriSint32 h);

/* �A���t�@�̂݃v���[���ւ̕ϊ� */
/* ���ϊ��O�ɃA���t�@�v���[���p�̃s�b�`�ݒ���s���Ă������� */
void CRIAPI mwPlyFxCnvFrmA8(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *alphaout);
/* Y�̂݃v���[���ւ̕ϊ��i�R�l�A���t�@�̂Ƃ��̂ݒl�������ς��j */
/* ���ϊ��O��Y�v���[���p�̃s�b�`�ݒ���s���Ă������� */
void CRIAPI mwPlyFxCnvFrmY8(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *yout);

/*=========================================================================*
 *		�񓯊��̃t���[���ϊ�
 *		Async Frame Conversion
 *=========================================================================*/
void CRIAPI mwPlyFxCnvFrmRGB565Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);
void CRIAPI mwPlyFxCnvFrmARGB1555Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);
void CRIAPI mwPlyFxCnvFrmARGB4444Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);
void CRIAPI mwPlyFxCnvFrmARGB8888Nw(MWPLY mwply, MwsfdFrmObj *mwfrm, CriUint8 *out, MwsfdCnvFrmCbFn cnvfrmcbfn, void *usrobj);


/*=========================================================================*
 *		�񓯊��̃t���[���ϊ��i�N���b�v�j
 *		Async Frame Conversion�iClip�j
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
 *		�ʃv���b�g�t�H�[������
 *		For Each Platform-specific Functions
 *=========================================================================*/

/*-------------------------------------------------------------------------*
 *		�h���r�[�f�W�^���ݒ�
 *		Dolby Digital Configuration
 *-------------------------------------------------------------------------*/
/* DolbyDigital�Đ��p���[�N�o�b�t�@�T�C�Y�̎擾 */
/* Get size of work buffer for DolbyDigital */
CriSint32 CRIAPI mwPlyGetDolbyDigitalWorkSize(void);
/* DolbyDigital�Đ��@�\�̒ǉ� */
/* Attach DolbyDigital */
void CRIAPI mwPlyAttachDolbyDigital(MWPLY mwply, void *wkptr, CriSint32 wksize);
/* DolbyDigital�Đ��@�\�̕��� */
/* Detach DolbyDigital */
void CRIAPI mwPlyDetachDolbyDigital(MWPLY mwply);
/* DolbyDigital�X�g���[���`���l���ԍ��̐ݒ�(�f�t�H���g�̓`���l���P) */
/* Set channel number of DolbyDigital (Default channel = 1) */
void CRIAPI mwPlySetDolbyDigitalCh(MWPLY mwply, CriSint32 ch);

/*-------------------------------------------------------------------------*
 *		�}���`�`�����l���I�[�f�B�I�ݒ�
 *		Multi Channel Audio Configuration
 *-------------------------------------------------------------------------*/
/* �}���`�`�����l���Đ��p���[�N�o�b�t�@�T�C�Y�̎擾 */
/* Get size of work buffer for multi channel audio */
CriSint32 CRIAPI mwPlyGetMultiChannelWorkSize(void);
/* �}���`�`�����l���I�[�f�B�I�Đ��@�\�̒ǉ� */
/* Attach MultiChannel Audio */
void CRIAPI mwPlyAttachMultiChannel(MWPLY mwply, void *wkptr, CriSint32 wksize);
/* �}���`�`�����l���I�[�f�B�I�Đ��@�\�̕��� */
/* Detach MultiChannel Audio */
void CRIAPI mwPlyDetachMultiChannel(MWPLY mwply);
/* �}���`�`���l���I�[�f�B�I�̃X�g���[���`���l���ԍ��̐ݒ�(�f�t�H���g�̓`���l���P) */
/* Set channel number of MultiChannel Audio (Default channel = 1) */
void CRIAPI mwPlySetMultiChannelCh(MWPLY mwply, CriSint32 ch);

/*-------------------------------------------------------------------------*
 *		�v���b�g�t�H�[���ŗL�̊e��g���t�@�C���Đ�
 *		Platform Specific Extended File Playback
 *-------------------------------------------------------------------------*/
/* Movie start by LSN */
void CRIAPI mwPlyStartLsn(MWPLY mwply, CriUint32 lsn, CriUint32 bytesize);
/* Movie start looping playback by LSN */
void CRIAPI mwPlyStartLsnLp(MWPLY mwply, CriUint32 lsn, CriUint32 bytesize);

/*-------------------------------------------------------------------------*
 *		�v���b�g�t�H�[���ŗL�̓���ݒ�
 *		Platform Specific Special Configuration
 *-------------------------------------------------------------------------*/
/* �p�b�N�T�C�Y�̐ݒ�i�f�t�H���g 2048 byte�j */
/* Set system stream pack size (default = 2048)  */
void CRIAPI mwPlySetPackSize(CriSint32 packsize);
/* �T���l�C�����[�h�̐ݒ�iON: 1/8�k���摜���o�͂���j */
/* Set thumbnail mode (ON: Output 1/8 reduced image)  */
void CRIAPI mwPlySetThumbnail(MWPLY mwply, CriSint32 sw);
/* �����F�ϊ������̎w�� ON:���x�D��, OFF:�掿�D��(�f�t�H���g) */
void CRIAPI mwPlySetHighSpeedConversion(int sw);
/* �㉺�t���܂ɕϊ����邩�ǂ�����ݒ肷�� */
void CRIAPI mwPlySetCnvBottomUp(MWPLY mwply, CriBool sw);
/* �㉺�t���܂ɕϊ����邩�ǂ������擾���� */
CriBool CRIAPI mwPlyGetCnvBottomUp(MWPLY mwply);
/* �r�f�IPTS�����Ǘ��̐ݒ� (MPEG-2 TS/PS�Đ��p) */
void CRIAPI mwPlySetVideoPts(MWPLY ply, void *ptswork, CriSint32 ptsworksiz);
/* �r�f�IPID�̐ݒ� (MPEG-2 TS�Đ��p�A�����v���O�����̏ꍇ��PID�w�肪�K�{) */
void CRIAPI mwPlySetVideoPid(MWPLY ply, CriSint32 pid);
/* �I�[�f�B�IPID�̐ݒ� (MPEG-2 TS�Đ��p�A�����v���O�����ꍇ��PID�w�肪�K�{) */
void CRIAPI mwPlySetAudioPid(MWPLY ply, CriSint32 pid);
/* PES�X�g���[���Đ��̐ݒ�iON: PES�X�g���[�����Đ�����AOFF:PS/TS���Đ�����j */
/* Set PES stream playback mode (ON: playback PES,  OFF: playback PS/TS)  */
void CRIAPI mwPlySetPesSw(MWPLY mwply, CriSint32 sw);

/* 
 * ���L�̊֐��͔p�~���A�n���h���쐬�p�����[�^�ŊO���t���[���v�[���p
 * ���[�N�o�b�t�@���w��ł���悤�ɂ����B
 * void CRIAPI mwPlySetDecodeOutBuf(CriSint32 bufnum, CriSint32 bufsize, CriUint8 **bufptr);
 */

/* �O���t���[���v�[���p���[�N�o�b�t�@�T�C�Y�̌v�Z */
CriSint32 CRIAPI mwPlyCalcOuterFrmPool(CriSint32 max_width, CriSint32 max_height);
/* �t���[���ϊ��p���[�N�o�b�t�@�T�C�Y�̌v�Z */
CriSint32 CRIAPI mwPlyCalcCnvFrmWorkSize(CriSint32 max_width, CriSint32 max_height, CriSint32 compo_mode);
/* �t���[���ϊ��p���[�N�o�b�t�@�̐ݒ� */
void CRIAPI mwPlySetCnvFrmWorkBuffer(MWPLY mwply, CriUint8 *workbuf, CriSint32 worksize);
/* �t���[���ϊ��p���[�N�o�b�t�@�̉�� */
void CRIAPI mwPlyReleaseCnvFrmWorkBuffer(MWPLY mwply);

/* �������[�v�Đ��̐ݒ� */
void CRIAPI mwPlySetInfiniteSingleLoop(MWPLY mwply, CriBool sw);
/* �������[�v�Ď��s�񐔂̎擾 */
CriUint32 CRIAPI mwPlyGetInfiniteLoopCount(MWPLY mwply);
/* �Đ����ԏ���̎擾�i�P��[�b]�j */
CriSint32 CRIAPI mwPlyGetLimitTime(MWPLY mwply);

/* �Đ���~�v�� */
/* MEMO: ���ۂ̒�~������ADXM_ExecMain�֐��Ŏ����I�ɓ����A�n���h����Ԃ�STOP�ɂȂ�B */
/*       mwPlyStop/mwPlyDestroy�̕��ב���}���邽�߂̎d�g�݁B */
/*       �n���h����Ԃ�STOP�ɂȂ��Ă���mwPlyDestroy���ĂԂƕ��ׂ�������Ȃ��B*/
void CRIAPI mwPlyRequestStop(MWPLY mwply);

/* �Đ����t�@�C�����̎擾						*/
/* Get header information of playing movie file	*/
void CRIAPI mwPlyGetPlayingHdrInf(MWPLY mwply, MwsfdHdrInf *hdrinf);

/* �ēǂݍ��݊J�n���Ԃ̐ݒ� */
/* Set reload timing */
void CRIAPI mwPlySetReloadTime(MWPLY mwply, CriFloat32 time);

/* �ő�ǂݍ��݃Z�N�^���̐ݒ� */
/* Set maximam read sectors */
void CRIAPI mwPlySetMaxReadSct(MWPLY mwply, CriUint32 nsct);

/* �h�̂݃��[�r�̎w��s�N�`���Đ� */
void CRIAPI mwPlyStartIpicture(MWPLY mwply, const CriChar8 *fname, CriUint32 fno, CriUint32 dec_num, 
						MwsfdSeekIpic *seekipic_tbl, CriUint32 seekipic_num);

/* �t�@�C�����w��ɂ�� �t�@�C����� �w�b�_���i�I�[�f�B�I�ڍׁj�̎擾	*/
/* Get header informations by filename	*/
CriBool mwPlyReadAndGetHdrInf(
	const CriChar8 *fname,
	MwsfdHdrInf *hdrinf,
	MwsfdHdrAudioDetail *audio_detail);

/*=========================================================================*
 *		����J�֐�
 *      Closed functions
 *-------------------------------------------------------------------------*
 *	�݊����̂��߂Ɋ֐��錾���c���Ă���܂��B�ʏ�͎g�p����K�v����܂���B
 *	These function declarations are retained for compatibility.
 *	Usually, it is not necessary to use these functions.
 *=========================================================================*/
/* ���t���[���擾�\�₢���킹				*/
/* Determine whether next frame is ready	*/
CriBool CRIAPI mwPlyIsNextFrmReady(MWPLY mwply);
/* ���t���[���̃s�N�`�����[�U�f�[�^�̎擾 */
/* Get next frame user data in picture layer */
void CRIAPI mwPlyGetNextPicUsr(MWPLY mwply, void **usrdatptr, CriSint32 *usrdatsize);



#ifdef __cplusplus
}
#endif

#endif	/* INCLUDED_CRIMWSFD_H */
