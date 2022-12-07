/***************************************************************************
 *
 *	CRI Middleware SDK
 *
 *	Copyright (c) 2008 CRI-MW
 *
 *	Library	: ADX Library
 *	Module	: Functions for XAudio
 *	File	: adx_xaudio2.h
 *
 ***************************************************************************/

/* ���d��`�h�~					*/
/* Prevention of redefinition	*/
#if !defined(_ADXXAUDIO2_H_INCLUDED)
#define _ADXXAUDIO2_H_INCLUDED

/***************************************************************************
 *       �o�[�W�������
 *       Version
 ***************************************************************************/
#define ADXXAUDIO2_VERSION			"1.01"

/***************************************************************************
 *      �C���N���[�h�t�@�C��
 *      Include files
 ***************************************************************************/
#include "cri_xpt.h"
#include "cri_adxt.h"

/* Check of SDK version */
/* ADX library supports XAudio2 functions from June 2008. */
#if defined(XPT_TGT_XBOX360)
	#include <xtl.h>
	#if (_XDK_VER >= 7645)
		#define CRI_ADX_SUPPORT_XAUDIO2
	#endif
#elif defined(XPT_TGT_PC)
	#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		/* NOTE : Following code makes error on DirectX SDK (March 2008).	*/
		/*        You need to modify 'dxsdkver.h' included in DirectX SDK.	*/
		/* See also) http://www.microsoft.com/downloads/details.aspx?FamilyID=572BE8A6-263A-4424-A7FE-69CFF1A5B180&displaylang=en */
		/* ���ӁF�ȉ��̃R�[�h��DirectX SDK (March 2008)���ŃG���[���N�����܂��B	*/
		/* �@�@�@�s��������ɂ́ADirectX SDK����'dxsdkver.h'���C�����Ă��������B	*/
		/* �Q�l�jhttp://www.microsoft.com/japan/msdn/directx/Releasenotes/dxreadmeMar08J.aspx */
		#include <dxsdkver.h>
		#if (_DXSDK_PRODUCT_MAJOR > 9) || \
			((_DXSDK_PRODUCT_MAJOR == 9) && (_DXSDK_PRODUCT_MINOR >= 23))
			#define CRI_ADX_SUPPORT_XAUDIO2
		#endif
	#endif
#endif

/* Checks whether XAudio2 functions are available. */
#if defined(CRI_ADX_SUPPORT_XAUDIO2)

#include <xaudio2.h>

/***************************************************************************
 *      �萔�}�N��
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      �����}�N��
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      �f�[�^�^�錾
 *      Data Type Declarations
 ***************************************************************************/

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
CriSint32 CRIAPI ADXXAUDIO2_SetupSound(IXAudio2 *xa2, IXAudio2Voice *ov);

/* �T�E���h�̃V���b�g�_�E�� */
/* Shutdown of sound */
CriSint32 CRIAPI ADXXAUDIO2_ShutdownSound(void);

/*---------------------------------------------------------------------------
 * �������Ǘ��p�֐�
 * Functions for memory management
 *-------------------------------------------------------------------------*/
/* �T�E���h�o�b�t�@�m�ۗp�������m�ۊ֐��̓o�^ */
/* Sets the custom malloc function for sound buffer allocation */
void CRIAPI ADXXAUDIO2_SetUserMallocFunc(void *(*func)(void *obj, CriUint32 size), void *obj);

/* �T�E���h�o�b�t�@����p����������֐��̓o�^ */
/* Sets the custom free function for sound buffer release */
void CRIAPI ADXXAUDIO2_SetUserFreeFunc(void (*func)(void *obj, void *mem), void *obj);

/*---------------------------------------------------------------------------
 * XAudio2����p�֐�
 * Functions for setting XAudio2 parameters
 *-------------------------------------------------------------------------*/
/* ADXT�n���h������XAudio2SourceVoice���擾���� */
/* Gets the XAudio2SourceVoice from ADXT handle. */
IXAudio2SourceVoice * CRIAPI ADXXAUDIO2_GetSourceVoice(ADXT adxt);

/* XAudio2SourceVoice�쐬���̃t���O���w�肷��								*/
/* ���l�jADXT�n���h����XAudio2SourceVoice������ō쐬���܂��B				*/
/*       �{�֐����g�p���邱�ƂŁAADXT�n���h���������ō쐬����				*/
/*       XAudio2SourceVoice�ɑ΂��ăt���O��ݒ肷�邱�Ƃ��\�ł��B			*/
/*       XMP�ƘA������ADXT�n���h�����쐬����ۂɂ́A						*/
/*       XAUDIO2_VOICE_MUSIC���w�肵�Ă��������B							*/
/*       �i�f�t�H���g��Ԃł�0���w�肳�Ă��܂��B�j							*/
/* Sets the flags for XAudio2SourceVoice creation.							*/
/* NOTE) ADXT handle creates XAudio2SourceVoice internally.					*/
/*       The specified flags are appended to XAudio2SourceVoice.			*/
/*       The default value is 0.											*/
/*       If you want to create ADXT handle which cooperates with XMP,		*/
/*       please specify XAUDIO2_VOICE_MUSIC.								*/
void CRIAPI ADXXAUDIO2_SetSourceVoiceDefaultFlags(UINT32 Flags);

/* XAudio2SourceVoice�쐬���Ɏw�肳���t���O���擾���� */
/* Gets the flags for XAudio2SourceVoice creation. */
UINT32 CRIAPI ADXXAUDIO2_GetSourceVoiceDefaultFlags(void);

/* XAudio2SourceVoice�쐬���̏o�̓{�C�X�̍ő���g����������w�肷��			*/
/* ���l�jADXT�n���h����XAudio2SourceVoice������ō쐬���܂��B				*/
/*       �{�֐����g�p���邱�ƂŁAADXT�n���h���������ō쐬����				*/
/*       XAudio2SourceVoice�ɑ΂��čő���g���������						*/
/*       �ݒ肷�邱�Ƃ��\�ł��B											*/
/*       �i�f�t�H���g��Ԃł�1.0f���w�肳�Ă��܂��B�j						*/
/* Sets the max frequency ratio for XAudio2SourceVoice creation.			*/
/* NOTE) ADXT handle creates XAudio2SourceVoice internally.					*/
/*       The specified max frequency ratio is used for						*/
/*       XAudio2SourceVoice creation.										*/
/*       The default value is 1.0f.											*/
void CRIAPI ADXXAUDIO2_SetSourceVoiceDefaultMaxFrequencyRatio(float MaxFrequencyRatio);

/* XAudio2SourceVoice�쐬���Ɏw�肳���ő���g����������擾���� */
/* Gets the max frequency ratio for XAudio2SourceVoice creation. */
float CRIAPI ADXXAUDIO2_GetSourceVoiceDefaultMaxFrequencyRatio(void);

/* XAudio2SourceVoice�쐬���̏o�̓{�C�X���w�肷��							*/
/* ���l�jADXT�n���h����XAudio2SourceVoice������ō쐬���܂��B				*/
/*       �{�֐����g�p���邱�ƂŁAADXT�n���h���������ō쐬����				*/
/*       XAudio2SourceVoice�ɑ΂��ďo�̓{�C�X��ݒ肷�邱�Ƃ��\�ł��B		*/
/*       �i�f�t�H���g��Ԃł�ADXXAUDIO2_SetupSound�֐��Ŏw�肳�ꂽ			*/
/*       �{�C�X���g�p����܂��B�j											*/
/* Sets the output voice for XAudio2SourceVoice creation.					*/
/* NOTE) ADXT handle creates XAudio2SourceVoice internally.					*/
/*       The specified output voice is used for								*/
/*       XAudio2SourceVoice creation.										*/
/*       The default output voice is the voice specified via				*/
/*       the ADXXAUDIO2_SetupSound function.								*/
void CRIAPI ADXXAUDIO2_SetSourceVoiceDefaultOutputVoice(IXAudio2Voice *pOutput);

/* XAudio2SourceVoice�쐬���Ɏw�肳���o�̓{�C�X���擾���� */
/* Gets the output voice for XAudio2SourceVoice creation. */
IXAudio2Voice *ADXXAUDIO2_GetSourceVoiceDefaultOutputVoice(void);

/* ADX���C�u�������g�p����XAudio2����Z�b�g���ʎq���w�肷��					*/
/* ���l�jADX���C�u�����́A�����Đ���������XAudio2����Z�b�g���g�p���܂��B	*/
/*       �f�t�H���g��Ԃł́A����Z�b�g�̎��ʎq�Ƃ���0x7FFFFFFF��			*/
/*       �g�p����܂��B														*/
/*       �A�v���P�[�V��������XAudio2����Z�b�g���g�p����ꍇ�A				*/
/*       ���̎��ʎq�̎g�p�������K�v������܂��B							*/
/*       ADX���C�u�������g�p���鑀��Z�b�g���ʎq��ύX���������ꍇ�ɁA		*/
/*       �{�֐����g�p���Ď��ʎq��ύX���Ă��������B							*/
/* Sets the XAudio2 Operation Set identifier of the ADX library.			*/
/* NOTE: The ADX library operates some methods as batch by using			*/
/*       XAudio2 Operation Set.												*/
/*       By default, the ADX library uses 0x7FFFFFFF as identifier of		*/
/*       Operation Set.														*/
/*       If you use XAudio2 Operation Set in your application,				*/
/*       you need to avoid using identifier used by the library.			*/
/*       By using this function, you can change the identifier used by		*/
/*       the library.														*/
void CRIAPI ADXXAUDIO2_SetOperationSetIdentifier(UINT32 OperationSet);

/* ADX���C�u�������g�p����XAudio2����Z�b�g���ʎq���擾���� */
/* Gets the XAudio2 Operation Set identifier of the ADX library. */
UINT32 CRIAPI ADXXAUDIO2_GetOperationSetIdentifier(void);

/* ADXT�n���h���ɑ΂��ďo�̓{�C�X��ݒ肷�� */
/* ���l�j���̊֐���IXAudio2Voice_SetOutputVoices�֐��̃��b�p�[�֐��ł��B */
/* Sets the voice output to ADXT handle. */
/* NOTE: This function calls IXAudio2Voice_SetOutputVoices function. */
HRESULT CRIAPI ADXXAUDIO2_SetOutputVoices(ADXT adxt, const XAUDIO2_VOICE_SENDS *pSendList);

/* ADXT�n���h���ɑ΂��ăG�t�F�N�g�`�F�[����ݒ肷��						*/
/* ���l�j���̊֐���IXAudio2Voice_SetEffectChain�֐��̃��b�p�[�֐��ł��B	*/
/* Sets the effect chain to ADXT handle.								*/
/* NOTE: This function calls IXAudio2Voice_SetEffectChain function.		*/
HRESULT CRIAPI ADXXAUDIO2_SetEffectChain(
	ADXT adxt, const XAUDIO2_EFFECT_CHAIN *pEffectChain);

/* ADXT�n���h���ɑ΂��ăG�t�F�N�g�p�����[�^��ݒ肷��							*/
/* ���l�j���̊֐���IXAudio2Voice_SetEffectParameters�֐��̃��b�p�[�֐��ł��B	*/
/* Sets the effect paramter to ADXT handle.										*/
/* NOTE: This function calls IXAudio2Voice_SetEffectParameters function.		*/
HRESULT CRIAPI ADXXAUDIO2_SetEffectParameters(ADXT adxt, UINT32 EffectIndex,
	const void *pParameters, UINT32 ParametersByteSize, UINT32 OperationSet);

/* ADXT�n���h������G�t�F�N�g�p�����[�^���擾����								*/
/* ���l�j���̊֐���IXAudio2Voice_GetEffectParameters�֐��̃��b�p�[�֐��ł��B	*/
/* Gets the effect paramter from ADXT handle.									*/
/* NOTE: This function calls IXAudio2Voice_GetEffectParameters function.		*/
HRESULT CRIAPI ADXXAUDIO2_GetEffectParameters(ADXT adxt, UINT32 EffectIndex,
	void *pParameters, UINT32 ParametersByteSize);

/* ADXT�n���h���ɑ΂��ăG�t�F�N�g��L��������									*/
/* ���l�j���̊֐���IXAudio2SourceVoice_EnableEffect�֐��̃��b�p�[�֐��ł��B		*/
/* Enables the effect to ADXT handle.											*/
/* NOTE: This function calls IXAudio2SourceVoice_EnableEffect function.			*/
HRESULT CRIAPI ADXXAUDIO2_EnableEffect(ADXT adxt, UINT32 EffectIndex, UINT32 OperationSet);

/* ADXT�n���h���ɑ΂��ăG�t�F�N�g�𖳌�������									*/
/* ���l�j���̊֐���IXAudio2SourceVoice_DisableEffect�֐��̃��b�p�[�֐��ł��B	*/
/* Disables the effect to ADXT handle.											*/
/* NOTE: This function calls IXAudio2SourceVoice_DisableEffect function.		*/
HRESULT CRIAPI ADXXAUDIO2_DisableEffect(ADXT adxt, UINT32 EffectIndex, UINT32 OperationSet);

#ifdef __cplusplus
}
#endif

/***************************************************************************
 *      ���o�[�W�����Ƃ̌݊��p
 *      For compatibility with old version
 ***************************************************************************/

#endif	/* CRI_ADX_SUPPORT_XAUDIO2 */

#endif	/* _ADXXAUDIO2_H_INCLUDED */

/* --- end of file --- */
