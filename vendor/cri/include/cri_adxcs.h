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

/* ���d��`�h�~					*/
/* Prevention of redefinition	*/
#ifndef _ADXCS_H_INCLUDED
#define _ADXCS_H_INCLUDED

/***************************************************************************
 *       �o�[�W�������
 *       Version
 ***************************************************************************/
#define ADXCS_VER	"1.25"

/***************************************************************************
 *      �C���N���[�h�t�@�C��
 *      Include files
 ***************************************************************************/
#include <cri_xpt.h>
#include <cri_adxt.h>
#include <cri_aixp.h>

/***************************************************************************
 *      �萔�}�N��
 *      Macro Constants
 ***************************************************************************/

/* �����n���h���iADXT�܂���AIXP�j�̍ő吔				*/
/* The maximum number of internal handles(ADXT or AIXP)	*/
#define ADXCS_MAX_INTERNAL_HNDL			(2)


/* �t�F�[�h�A�E�g���Ԃ̃f�t�H���g�l		*/
/* The default value of fade-out time	*/
#define ADXCS_DEF_FADE_OUT_TIME			(30)				/* 0.5sec	*/

/* �t�F�[�h�C�����Ԃ̃f�t�H���g�l		*/
/* The default value of fade-in time	*/
#define ADXCS_DEF_FADE_IN_TIME			(0)

/* �{�����[���ύX���Ԃ̃f�t�H���g�l			*/
/* The default value of volume change time	*/
#define ADXCS_DEF_VOL_CHANGE_TIME		(0)

/* �t�F�[�h�C���J�n�I�t�Z�b�g�̃f�t�H���g�l				*/
/* The default value of offset of fade-in start time	*/
/* from fade-out start time								*/
#define ADXCS_DEF_FADE_IN_START_OFFSET	(0)

/* �t�F�[�h��̃f�B���C���Ԃ̃f�t�H���g�l	*/
/* The default value of fade-end delay		*/
#define ADXCS_DEF_FADE_END_DELAY		(60)				/* 1sec		*/

/* �t�F�[�h���Ԃ̍ŏ��l				*/
/* The minimum value of fading time	*/
#define ADXCS_FADE_TIME_MIN				(0)

/* �t�F�[�h���Ԃ̍ő�l				*/
/* The maximum value of fading time	*/
#define ADXCS_FADE_TIME_MAX				(1800)				/* 30sec	*/


/* �{�C�X�n�C���C�g�̃{�����[���������Ԃ̃f�t�H���g�l			*/
/* The default value of volume decrease time of Voice-Highlight	*/
#define ADXCS_DEF_VHL_DEC_TIME			(30)				/* 0.5sec	*/

/* �{�C�X�n�C���C�g�̃{�����[���������Ԃ̃f�t�H���g�l			*/
/* The default value of volume increase time of Voice-Highlight	*/
#define ADXCS_DEF_VHL_INC_TIME			(60)				/* 1sec		*/

/* �{�C�X�n�C���C�g�̃{�����[�������ʂ̃f�t�H���g�l						*/
/* The default value of quantity of volume decrease of Voice-Highlight	*/
#define ADXCS_DEF_VHL_DEC_VOL			(120)				/* 12dB		*/


/* ADXCS�n���h���̍Đ����			*/
/* Playing state of a ADXCS handle	*/
#define ADXCS_STAT_STOP			(0)	/* ��~��			(Stop)				*/
#define ADXCS_STAT_PREP			(1) /* �Đ�������		(Prepare playing)	*/
#define ADXCS_STAT_PLAYING		(2) /* �f�R�[�h���Đ���	(Decoding & Playing)*/
#define ADXCS_STAT_PLAYEND		(3) /* �Đ��I��			(End of playback)	*/
#define ADXCS_STAT_ERROR		(4) /* �G���[����		(Error)				*/

/* ADXCS�n���h���̃t�F�[�h���		*/
/* Fading state of a ADXCS handle	*/
#define ADXCS_FADE_STAT_IDLE	(0)	/* �x�~���			(Idle state)		*/
#define ADXCS_FADE_STAT_FADING	(1)	/* �t�F�[�h��		(Processing fading)	*/

/* �T�E���h�̎��	*/
/* Type of sound	*/
#define ADXCS_SND_TYPE_NONE		(0)	/* ������			(Independent)		*/
#define ADXCS_SND_TYPE_BGM		(1)	/* BGM				(Background music)	*/
#define ADXCS_SND_TYPE_VOICE	(2)	/* �Z���t			(Voice)				*/
#define ADXCS_SND_TYPE_SE		(3)	/* ���ʉ�			(Sound effect)		*/

/* �R�[���o�b�N�֐��̎��		*/
/* Type of callback function	*/
#define ADXCS_CB_START_FADE_OUT		(0) /* �t�F�[�h�A�E�g�J�n				*/
										/* Start of fade-out				*/
#define ADXCS_CB_START_FADE_IN		(1) /* �t�F�[�h�C���J�n					*/
										/* Start of fade-in					*/
#define ADXCS_CB_START_VOL_CHANGE	(2) /* �t�F�[�h�{�����[���`�F���W�J�n	*/
										/* Start of volume chage			*/
#define ADXCS_CB_END_FADE_OUT		(3) /* �t�F�[�h�A�E�g�I��				*/
										/* End of fade-out					*/
#define ADXCS_CB_END_FADE_IN		(4) /* �t�F�[�h�C���I��					*/
										/* End of fade-in					*/
#define ADXCS_CB_END_VOL_CHANGE		(5) /* �{�����[���ύX�I��				*/
										/* End of volume change				*/
#define ADXCS_CB_END_FADE_ALL		(6) /* �S�t�F�[�h�I��					*/
										/* End of all process of fading		*/

/***************************************************************************
 *      �����}�N��
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      �f�[�^�^�錾
 *      Data Type Declarations
 ***************************************************************************/
/* ADXCS�n���h��	*/
/* ADXCS Handle		*/
#ifndef ADXCS_DEFINED
#define ADXCS_DEFINED
typedef void *ADXCS;
#endif

/* �n���h���쐬�p�����[�^		*/
/* Handle creation parameter	*/
typedef struct {
	CriSint32	max_stm;		/* �ő哯���Đ��f�B�X�N�X�g���[����				*/
							/* Maximum number of stream which play at the	*/
							/* same time									*/
	CriSint32	max_ch;			/* �ő�`���l����								*/
							/* Maximum number of channel					*/
	CriSint32	max_sfreq;		/* �ő�T���v�����O���g��						*/
							/* Maximum sampling frequency					*/
	CriSint32	max_aix_track;	/* AIX�̍ő�g���b�N��							*/
							/* Maximum number of track of AIX				*/ 
	CriSint32	play_method;	/* �Đ�����(ADXT_PLY_MEM / ADXT_PLY_STM)		*/
							/* Playback method(ADXT_PLY_MEM / ADXT_PLY_STM)	*/
	CriSint8	sound_type;		/* �T�E���h�̎��								*/
							/* Type of sound								*/
	CriBool	use_crossfade;	/* �N���X�t�F�[�h�����邩�ۂ�					*/
							/* Whether handle can use crossfade function	*/
	CriBool	use_aix;		/* AIX���g�p���邩�ۂ�							*/
							/* Whether handle can use AIX playback function	*/
	CriBool	use_ahx;		/* AHX���g�p���邩�ۂ�							*/
							/* Whether handle can use AHX playback function	*/
	CriBool	use_3d_panning;	/* ���A���^�C��3D�p���j���O���g�p���邩�ۂ�		*/
							/* Whether handle can use realtime 3D panning	*/
} AdxcsCrePrm;

/* 
 * ���ӁF
 * ���A���^�C��3D�p���j���O���g�p����ꍇ�iuse_3d_panning��TRUE�j�A�ő�`���l
 * �����͖�������A�v���b�g�t�H�[���ŗL�̒l�ɂȂ�܂��B
 * �܂��A���̂Ƃ����͉\�ȉ����̓��m�����݂̂ł��B
 * Notice:
 * If you want to use 3D panning(use_3d_panning flag is TRUE), maximum number
 * of channel(max_ch) is ignored. It is set to the platform's unique value
 * internally.
 * And then, only a monaural sound can be inputted.
 */

/***************************************************************************
 *      �ϐ��錾
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      �֐��錾
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 *      �������E�I������
 *		Initialization / Finish
 *-------------------------------------------------------------------------*/
/* ����������						*/
/* Initializes the ADXCS library	*/
void CRIAPI ADXCS_Init(void);

/* �I������						*/
/* Finishes	the ADXCS library	*/
void CRIAPI ADXCS_Finish(void);


/*--------------------------------------------------------------------------
 *      �n���h���̍쐬�E�j��
 *		Handle Creation / Destruction
 *-------------------------------------------------------------------------*/
/* ADXCS�n���h���̍쐬		*/
/* Creates a ADXCS handle	*/
ADXCS CRIAPI ADXCS_Create(const AdxcsCrePrm *cprm,
						  CriSint8 *work, CriSint32 work_size);

/* ADXCS�n���h���̔j��		*/
/* Destroys a ADXCS handle	*/
void CRIAPI ADXCS_Destroy(ADXCS adxcs);


/* ���[�N�̈�T�C�Y���v�Z����			*/
/* Calculates the work size of a handle	*/
CriSint32 CRIAPI ADXCS_CalcWorkCprm(const AdxcsCrePrm *cprm);


/* AHX�Đ��@�\�̑g�ݍ���					*/
/* Attaches the function of AHX playback	*/
void CRIAPI ADXCS_AttachAhx(ADXCS adxcs);

/* AHX�Đ��@�\�̕���						*/
/* Detaches the function of AHX playback	*/
void CRIAPI ADXCS_DetachAhx(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      ��{�Đ�����
 *		Basic Playback
 *-------------------------------------------------------------------------*/
/* �t�@�C�����w��ɂ��Đ��̊J�n			*/
/* Starts a playback by specified filename	*/
void CRIAPI ADXCS_StartFname(ADXCS adxcs, const CriChar8 *fname);

/* �t�@�C�����w��ɂ��Đ��̊J�n�i�͈͎w��t���j					*/
/* Starts a playback by specified filename and the range as sector 	*/
void CRIAPI ADXCS_StartFnameRange(ADXCS adxcs, const CriChar8 *fname, CriSint32 ofst, CriSint32 nsct);

/* AFS�t�@�C������FID�w��ɂ��Đ��̊J�n			*/
/* Starts a playback by specified FID in a AFS file	*/
void CRIAPI ADXCS_StartAfs(ADXCS adxcs, CriSint32 patid, CriSint32 fid);

/* ���������t�@�C���̍Đ��̊J�n									*/
/* Starts a playback by specified address of a file in memory	*/
void CRIAPI ADXCS_StartMem(ADXCS adxcs, void *dat, CriSint32 datlen);

/* �������C���f�b�N�X�w��ɂ��Đ��̊J�n									*/
/* Starts a playback by specified index and address of a ACX data in memory	*/
void CRIAPI ADXCS_StartMemIdx(ADXCS adxcs, void *acx, CriSint32 no);

/* �Đ��̒�~		*/
/* Stops a playback	*/
void CRIAPI ADXCS_Stop(ADXCS adxcs);

/* �ꎞ��~�̐ݒ�				*/
/* Pauses or cancels pausing	*/
void CRIAPI ADXCS_Pause(ADXCS adxcs, CriSint32 sw);

/* �ꎞ��~��Ԃ̎擾	*/
/* Gets pausing state	*/
CriSint32 CRIAPI ADXCS_GetStatPause(ADXCS adxcs);

/* �Đ���Ԃ̎擾		*/
/* Gets playing state	*/
CriSint32 CRIAPI ADXCS_GetStat(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      �p�����[�^�ݒ�
 *		Parameter Settings
 *-------------------------------------------------------------------------*/
/* �n���h���쐬�p�����[�^�̎擾		*/
/* Gets a handle creation parameter	*/
const AdxcsCrePrm *CRIAPI ADXCS_GetCprm(ADXCS adxcs);

/* ���[�v�t���O�̐ݒ�	*/
/* Sets the loop flag	*/
void CRIAPI ADXCS_SetLpFlg(ADXCS adxcs, CriSint32 flg);

/* �T�E���h�̎�ނ̐ݒ�	*/
/* Sets the type of sound	*/
void CRIAPI ADXCS_SetType(ADXCS adxcs, CriSint32 type);

/* �T�E���h�̎�ނ̎擾	*/
/* Gets the type of sound	*/
CriSint32 CRIAPI ADXCS_GetType(ADXCS adxcs);


/* �o�̓{�����[���̐ݒ�	*/
/* Sets a output volume	*/
void CRIAPI ADXCS_SetOutVol(ADXCS adxcs, CriSint32 vol);

/* �o�̓{�����[���̎擾	*/
/* Gets a output volume	*/
CriSint32 CRIAPI ADXCS_GetOutVol(ADXCS adxcs);

/* ����Đ����̏o�̓{�����[���̐ݒ�			*/
/* Sets a output volume of next playback	*/
void CRIAPI ADXCS_SetOutVolNextPlayback(ADXCS adxcs, CriSint32 vol);

/* �g���b�N���Ƃ̏o�̓{�����[���̐ݒ�	*/
/* Sets a output volume of each track	*/
void CRIAPI ADXCS_SetTrackVol(ADXCS adxcs, CriSint32 trno, CriSint32 vol);

/* �g���b�N���Ƃ̏o�̓{�����[���̎擾	*/
/* Gets a output volume of each track	*/
CriSint32 CRIAPI ADXCS_GetTrackVol(ADXCS adxcs, CriSint32 trno);


/* �t�F�[�h�A�E�g���Ԃ̐ݒ�iV-Sync�P�ʁj	*/
/* Sets a time of fade-out(V-Sync)			*/
void CRIAPI ADXCS_SetFadeOutTime(ADXCS adxcs, CriSint32 time);

/* �t�F�[�h�A�E�g���Ԃ̎擾�iV-Sync�P�ʁj	*/
/* Gets a time of fade-out(V-Sync)			*/
CriSint32 CRIAPI ADXCS_GetFadeOutTime(ADXCS adxcs);

/* �t�F�[�h�C�����Ԃ̐ݒ�iV-Sync�P�ʁj	*/
/* Sets a time of fade-in(V-Sync)		*/
void CRIAPI ADXCS_SetFadeInTime(ADXCS adxcs, CriSint32 time);

/* �t�F�[�h�C�����Ԃ̎擾�iV-Sync�P�ʁj	*/
/* Gets a time of fade-in(V-Sync)		*/
CriSint32 CRIAPI ADXCS_GetFadeInTime(ADXCS adxcs);

/* �{�����[���ύX���Ԃ̐ݒ�iV-Sync�P�ʁj	*/
/* Sets a time of volume change(V-Sync)		*/
void CRIAPI ADXCS_SetVolChangeTime(ADXCS adxcs, CriSint32 time);

/* �{�����[���ύX���Ԃ̎擾�iV-Sync�P�ʁj	*/
/* Gets a time of volume change(V-Sync)		*/
CriSint32 CRIAPI ADXCS_GetVolChangeTime(ADXCS adxcs);


/* �t�F�[�h�A�E�g����̑��΃t�F�[�h�C���J�n�����̐ݒ�iV-Sync�P�ʁj			*/
/* Sets a offset of a fade-in start time from a fade-out start time(V-Sync)	*/
void CRIAPI ADXCS_SetFadeInStartOffset(ADXCS adxcs, CriSint32 offset);

/* �t�F�[�h�A�E�g����̑��΃t�F�[�h�C���J�n�����̎擾�iV-Sync�P�ʁj			*/
/* Gets a offset of a fade-in start time from a fade-out start time(V-Sync)	*/
CriSint32 CRIAPI ADXCS_GetFadeInStartOffset(ADXCS adxcs);


/* �t�F�[�h��̃f�B���C���Ԃ̐ݒ�iV-Sync�P�ʁj */
/* Sets a fade-end delay time(V-Sync)			*/
void CRIAPI ADXCS_SetFadeEndDelay(ADXCS adxcs, CriSint32 delay);

/* �t�F�[�h��̃f�B���C���Ԃ̎擾�iV-Sync�P�ʁj */
/* Gets a fade-end delay time(V-Sync)			*/
CriSint32 CRIAPI ADXCS_GetFadeEndDelay(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      �����n���h���̎擾
 *		Getting Internal Handle
 *-------------------------------------------------------------------------*/
/* ���݃A�N�e�B�u�ȓ���ADXT�n���h�����擾			*/
/* Gets a internal ADXT handle which is active now	*/
ADXT CRIAPI ADXCS_GetActiveAdxt(ADXCS adxcs);

/* ���ɐ؂�ւ��\��̓���ADXT�n���h�����擾							*/
/* Gets a internal ADXT handle which will be changed at a next start	*/
ADXT CRIAPI ADXCS_GetNextAdxt(ADXCS adxcs);

/* �C���f�b�N�X�w��œ���ADXT�n���h�����擾	*/
/* Gets a internal ADXT handle from index	*/
ADXT CRIAPI ADXCS_GetAdxt(ADXCS adxcs, CriSint32 index);

/* ����ADXT�n���h���̐����擾				*/
/* Gets the number of internal ADXT handles	*/
CriSint32 CRIAPI ADXCS_GetNumAdxt(ADXCS adxcs);


/* ���݃A�N�e�B�u�ȓ���AIXP�n���h�����擾			*/
/* Gets a internal AIXP handle which is active now	*/
AIXP CRIAPI ADXCS_GetActiveAixp(ADXCS adxcs);

/* ���ɐ؂�ւ��\��̓���AIXP�n���h�����擾							*/
/* Gets a internal AIXP handle which will be changed at a next start	*/
AIXP CRIAPI ADXCS_GetNextAixp(ADXCS adxcs);

/* �C���f�b�N�X�w��œ���AIXP�n���h�����擾	*/
/* Gets a internal AIXP handle from index	*/
AIXP CRIAPI ADXCS_GetAixp(ADXCS adxcs, CriSint32 index);

/* ����AIXP�n���h���̐����擾				*/
/* Gets the number of internal AIXP handles	*/
CriSint32 CRIAPI ADXCS_GetNumAixp(ADXCS adxcs);


/*--------------------------------------------------------------------------
 *      �t�F�[�h����
 *		Fading Process
 *-------------------------------------------------------------------------*/
/* �t�F�[�h�������L�����Z�����Č��̉��ʂɖ߂�			*/
/* Cancels the fading process and turns the volume back	*/
void CRIAPI ADXCS_CancelFading(ADXCS adxcs);

/* �t�F�[�h��Ԃ̎擾		*/
/* Gets the fading state	*/
CriSint32 CRIAPI ADXCS_GetFadeStat(ADXCS adxcs);

/* �t�F�[�h�I���R�[���o�b�N�֐��̓o�^			*/
/* Registers the function of fade-end callback	*/
void CRIAPI ADXCS_SetCbFunc(ADXCS adxcs,
							void (*fn)(ADXCS adxcs, CriSint32 cb_type));

/*--------------------------------------------------------------------------
 *      �{�C�X�n�C���C�g
 *		Voice-Highlight
 *-------------------------------------------------------------------------*/
/* �{�C�X�n�C���C�g�@�\��L���ɂ���		*/
/* Enables the Voice-Highlight function	*/
void CRIAPI ADXCS_EnableVhl(void);

/* �{�C�X�n�C���C�g�@�\�𖳌��ɂ���			*/
/* Disables the Voice-Highlight function	*/
void CRIAPI ADXCS_DisableVhl(void);


/* �{�����[�������ɗv���鎞�Ԃ�ݒ肷��iV-Sync�P�ʁj	*/
/* Sets the time(V-Sync) of the volume decrease			*/
void CRIAPI ADXCS_SetVhlDecTime(CriSint32 dec_time);

/* �{�����[�������ɗv���鎞�Ԃ��擾����iV-Sync�P�ʁj	*/
/* Gets the time(V-Sync) of the volume decrease			*/
CriSint32 CRIAPI ADXCS_GetVhlDecTime(void);

/* �{�����[�������ɗv���鎞�Ԃ�ݒ肷��iV-Sync�P�ʁj	*/
/* Sets the time(V-Sync) of the volume increase			*/
void CRIAPI ADXCS_SetVhlIncTime(CriSint32 inc_time);

/* �{�����[�������ɗv���鎞�Ԃ��擾����iV-Sync�P�ʁj	*/
/* Gets the time(V-Sync) of the volume increase			*/
CriSint32 CRIAPI ADXCS_GetVhlIncTime(void);

/* �{�����[�������ʂ�ݒ肷��		*/
/* Sets the quantity of the volume decrease	*/
void CRIAPI ADXCS_SetVhlDecVol(CriSint32 dec_vol);

/* �{�����[�������ʂ��擾����		*/
/* Gets the quantity of the volume decrease	*/
CriSint32 CRIAPI ADXCS_GetVhlDecVol(void);

/*--------------------------------------------------------------------------
 *      �T�[�o����
 *		Server Process
 *-------------------------------------------------------------------------*/
/* �T�[�o����				*/
/* Executes server process	*/
void CRIAPI ADXCS_ExecServer(void);

/* �n���h�����̃T�[�o����					*/
/* Executes server process of each handle	*/
void CRIAPI ADXCS_ExecHndl(ADXCS adxcs);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef _ADXCS_H_INCLUDED */

/* end of file */
