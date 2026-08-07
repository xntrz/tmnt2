/***************************************************************************
 *
 *	CRI Middleware SDK
 *
 *	Copyright (c) 2008 CRI-MW
 *
 *	Library	: ADX Library
 *	Module	: Functions for DirectSound8
 *	File	: adx_dsound8.h
 *
 ***************************************************************************/

/* ���d��`�h�~					*/
/* Prevention of redefinition	*/
#ifndef _ADXDSOUND8_H_INCLUDED
#define _ADXDSOUND8_H_INCLUDED

/***************************************************************************
 *       �o�[�W�������
 *       Version
 ***************************************************************************/
#define ADXDSOUND8_VERSION			"1.02"

/***************************************************************************
 *      �C���N���[�h�t�@�C��
 *      Include files
 ***************************************************************************/
/* Disable VC++ warning 
 *   In some cases, including windows.h, dsound.h causes the following warning:
 *       rpcasync.h(45) : warning C4115: '_RPC_ASYNC_STATE' : ...
 */
#if defined(_MSC_VER) && ((_MSC_VER >= 1200) && (_MSC_VER < 1300))
	#pragma warning(disable : 4115)
#endif

#include "cri_xpt.h"
//#include <dsound.h>

/***************************************************************************
 *      �萔�}�N��
 *      Macro Constants
 ***************************************************************************/

#define LPDIRECTSOUNDBUFFER8 void*
#define LPDIRECTSOUND8 void*
#define LPDSEFFECTDESC void*

/***************************************************************************
 *      �����}�N��
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      �f�[�^�^�錾
 *      Data Type Declarations
 ***************************************************************************/
/*--------------------------------------------------------------------------
 * �o�b�t�@�쐬���̔z�u�ʒu
 * �n�[�h�E�F�A�A�N�Z�����[�V�����̗L���Ɋ֌W
 * Locate mode of DirectSoundBuffer
 * Influences the use of hardware acceleration
 *-------------------------------------------------------------------------*/
typedef enum {
	ADXPC_LOCMODE_SOFTWAREONLY = 0x00,
		/* Software�o�b�t�@�^�~�L�V���O�̂ݎg�p					*/
		/* Only software buffer/mixing will be used				*/
	ADXPC_LOCMODE_HARDWAREONLY = 0x01,
		/* Hardware�o�b�t�@�^�~�L�V���O�̂ݎg�p					*/
		/* Only hardware buffer/mixing will be used				*/
	ADXPC_LOCMODE_HARDWARE = 0x02,
		/* �\�Ȃ�Hardware���g�p								*/
		/* Hardware buffer/mixing will be used if available		*/
	ADXPC_LOCMODE_DEFER = 0x03,
		/* �o�b�t�@�m�ۂ��Đ����܂Œx���A�\�Ȃ�Hardware���g�p	*/
		/* The buffer can be assigned to a hardware or software	*/
		/* resource at play time								*/
} ADXPC_LOCMODE;

/***************************************************************************
 *      �ϐ��錾
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      �֐��錾
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* �T�E���h�̃Z�b�g�A�b�v */
/* Setup of sound */
CriSint32 CRIAPI ADXPC_SetupSoundDirectSound8(LPDIRECTSOUND8 ds);

/* �T�E���h�̃V���b�g�_�E�� */
/* Shutdown of sound */
CriSint32 CRIAPI ADXPC_ShutdownSound(void);

/* �����X�e���I�o�̓X�C�b�`�̐ݒ� */
/* Enable force stereo output mode */
void CRIAPI ADXPC_SetOutputStereo(CriBool flag);

/*---------------------------------------------------------------------------
 * DirectSoundBuffer����p�֐�
 * Functions for setting DirectSoundBuffer parameters
 *-------------------------------------------------------------------------*/
/* DirectSoundBuffer8�I�u�W�F�N�g�̎擾	*/
/* Get the DirectSoundBuffer8 object	*/
LPDIRECTSOUNDBUFFER8 CRIAPI ADXPC_GetDirectSoundBuffer8(ADXT adxt);

/* DirectSoundBuffer�̔z�u�ʒu�̐ݒ�		*/
/* Set the locate mode of DirectSoundBuffer	*/
void CRIAPI ADXPC_SetDsbCapsLocMode(CriSint32 loc_mode);

/* DirectSoundBuffer�̔z�u�ʒu�̎擾		*/
/* Get the locate mode of DirectSoundBuffer	*/
CriSint32 CRIAPI ADXPC_GetDsbCapsLocMode(void);

/* DirectSoundBuffer�̃`�����l�����̐ݒ�			*/
/* Set the number of channels of DirectSoundBuffer	*/
void CRIAPI ADXPC_SetDsbNumChannels(CriSint32 num);

/* DirectSoundBuffer�̃`�����l�����̎擾			*/
/* Get the number of channels of DirectSoundBuffer	*/
CriSint32 CRIAPI ADXPC_GetDsbNumChannels(void);

/* DirectSoundBuffer�Ńo�b�t�@�����O����T���v�����̐ݒ�	*/
/* Set the number of samples that DirectSoundBuffer buffers	*/
void CRIAPI ADXPC_SetDsbNumSamples(CriSint32 nsmpl);

/* DirectSoundBuffer�Ńo�b�t�@�����O����T���v�����̎擾	*/
/* Get the number of samples that DirectSoundBuffer buffers	*/
CriSint32 CRIAPI ADXPC_GetDsbNumSamples(void);

/* �O���[�o���t�H�[�J�X������DirectSoundBuffer���쐬���邩�ǂ�����ݒ�	*/
/* Set DirectSoundBuffer to have global focus							*/
void CRIAPI ADXPC_SetDsbCapsGlobalFocus(CriBool flag);

/* �O���[�o���t�H�[�J�X������DirectSoundBuffer���쐬���邩�ǂ������擾	*/
/* Get whether DirectSoundBuffer is having global focus					*/
CriBool CRIAPI ADXPC_GetDsbCapsGlobalFocus(void);

/* 3D�R���g���[���@�\������DirectSoundBuffer���쐬���邩�ǂ�����ݒ�	*/
/* Set DirectSoundBuffer to support 3D control							*/
void CRIAPI ADXPC_SetDsbCapsCtrl3D(CriBool flag);

/* 3D�R���g���[���@�\������DirectSoundBuffer���쐬���邩�ǂ������擾	*/
/* Get whether DirectSoundBuffer is supporting 3D control				*/
CriBool CRIAPI ADXPC_GetDsbCapsCtrl3D(void);

/* �G�t�F�N�g�@�\������DirectSoundBuffer���쐬���邩�ǂ�����ݒ�	*/
/* Set DirectSoundBuffer to support effects processing				*/
void CRIAPI ADXPC_SetDsbCapsCtrlFx(CriBool flag);

/* �G�t�F�N�g�@�\������DirectSoundBuffer���쐬���邩�ǂ������擾	*/
/* Get whether DirectSoundBuffer is supporting effects processing	*/
CriBool CRIAPI ADXPC_GetDsbCapsCtrlFx(void);

/* �G�t�F�N�g��ݒ肷��			*/
/* Enable effects on a buffer	*/
CriBool CRIAPI ADXPC_SetDsFx(
	ADXT adxt, CriSint32 count, LPDSEFFECTDESC effect, CriUint32 *result);

#ifdef __cplusplus
}
#endif

/***************************************************************************
 *      ���o�[�W�����Ƃ̌݊��p
 *      For compatibility with old version
 ***************************************************************************/
#define ADXPC_SetupSound(obj)			ADXPC_SetupSoundDirectSound8(obj)
#define ADXPC_Setup3DSound()			ADXPC_SetDsbCapsCtrl3D(TRUE)
#define ADXPC_Shutdown3DSound()			ADXPC_SetDsbCapsCtrl3D(FALSE)
#define ADXPC_GetDsb(adxt, dsb)	\
	(*dsb) = (void *)ADXPC_GetDirectSoundBuffer8(adxt);
#define ADXPC_GetDirectSoundBuffer(adxt, dsb)	\
	(*dsb) = (void *)ADXPC_GetDirectSoundBuffer8(adxt);
#define ADXPC_SetSndBufGlobalFocus(mode)	\
	ADXPC_SetDsbCapsFocusMode(mode)
#define ADXPC_GetSndBufGlobalFocus()	ADXPC_GetDsbCapsFocusMode()
#define ADXPC_SetLocMode(mode)			ADXPC_SetDsbCapsLocMode(mode)
#define ADXPC_GetLocMode()				ADXPC_GetDsbCapsLocMode()
#define ADXPC_SPEAKER_FRONT_LEFT		ADXT_SPEAKER_FRONT_LEFT
#define ADXPC_SPEAKER_FRONT_RIGHT		ADXT_SPEAKER_FRONT_RIGHT
#define ADXPC_SPEAKER_FRONT_CENTER		ADXT_SPEAKER_FRONT_CENTER
#define ADXPC_SPEAKER_LOW_FREQUENCY		ADXT_SPEAKER_LOW_FREQUENCY
#define ADXPC_SPEAKER_BACK_LEFT 		ADXT_SPEAKER_BACK_LEFT
#define ADXPC_SPEAKER_BACK_RIGHT		ADXT_SPEAKER_BACK_RIGHT
#define ADXPC_GetSendSpeakerLevel(adxt, ch_id, spk_id)	\
	ADXT_GetSendSpeakerLevel(adxt, ch_id, spk_id)
#define ADXPC_SetSendSpeakerLevel(adxt, ch_id, spk_id, volume)	\
	ADXT_SetSendSpeakerLevel(adxt, ch_id, spk_id, volume)

#endif	/* #ifndef _ADXDSOUND8_H_INCLUDED */

/* --- end of file --- */
