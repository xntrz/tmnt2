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

/* 多重定義防止					*/
/* Prevention of redefinition	*/
#ifndef _ADXDSOUND8_H_INCLUDED
#define _ADXDSOUND8_H_INCLUDED

/***************************************************************************
 *       バージョン情報
 *       Version
 ***************************************************************************/
#define ADXDSOUND8_VERSION			"1.02"

/***************************************************************************
 *      インクルードファイル
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
#include <dsound.h>

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/*--------------------------------------------------------------------------
 * バッファ作成時の配置位置
 * ハードウェアアクセラレーションの有無に関係
 * Locate mode of DirectSoundBuffer
 * Influences the use of hardware acceleration
 *-------------------------------------------------------------------------*/
typedef enum {
	ADXPC_LOCMODE_SOFTWAREONLY = 0x00,
		/* Softwareバッファ／ミキシングのみ使用					*/
		/* Only software buffer/mixing will be used				*/
	ADXPC_LOCMODE_HARDWAREONLY = 0x01,
		/* Hardwareバッファ／ミキシングのみ使用					*/
		/* Only hardware buffer/mixing will be used				*/
	ADXPC_LOCMODE_HARDWARE = 0x02,
		/* 可能ならHardwareを使用								*/
		/* Hardware buffer/mixing will be used if available		*/
	ADXPC_LOCMODE_DEFER = 0x03,
		/* バッファ確保を再生時まで遅延、可能ならHardwareを使用	*/
		/* The buffer can be assigned to a hardware or software	*/
		/* resource at play time								*/
} ADXPC_LOCMODE;

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* サウンドのセットアップ */
/* Setup of sound */
CriSint32 CRIAPI ADXPC_SetupSoundDirectSound8(LPDIRECTSOUND8 ds);

/* サウンドのシャットダウン */
/* Shutdown of sound */
CriSint32 CRIAPI ADXPC_ShutdownSound(void);

/* 強制ステレオ出力スイッチの設定 */
/* Enable force stereo output mode */
void CRIAPI ADXPC_SetOutputStereo(CriBool flag);

/*---------------------------------------------------------------------------
 * DirectSoundBuffer制御用関数
 * Functions for setting DirectSoundBuffer parameters
 *-------------------------------------------------------------------------*/
/* DirectSoundBuffer8オブジェクトの取得	*/
/* Get the DirectSoundBuffer8 object	*/
LPDIRECTSOUNDBUFFER8 CRIAPI ADXPC_GetDirectSoundBuffer8(ADXT adxt);

/* DirectSoundBufferの配置位置の設定		*/
/* Set the locate mode of DirectSoundBuffer	*/
void CRIAPI ADXPC_SetDsbCapsLocMode(CriSint32 loc_mode);

/* DirectSoundBufferの配置位置の取得		*/
/* Get the locate mode of DirectSoundBuffer	*/
CriSint32 CRIAPI ADXPC_GetDsbCapsLocMode(void);

/* DirectSoundBufferのチャンネル数の設定			*/
/* Set the number of channels of DirectSoundBuffer	*/
void CRIAPI ADXPC_SetDsbNumChannels(CriSint32 num);

/* DirectSoundBufferのチャンネル数の取得			*/
/* Get the number of channels of DirectSoundBuffer	*/
CriSint32 CRIAPI ADXPC_GetDsbNumChannels(void);

/* DirectSoundBufferでバッファリングするサンプル数の設定	*/
/* Set the number of samples that DirectSoundBuffer buffers	*/
void CRIAPI ADXPC_SetDsbNumSamples(CriSint32 nsmpl);

/* DirectSoundBufferでバッファリングするサンプル数の取得	*/
/* Get the number of samples that DirectSoundBuffer buffers	*/
CriSint32 CRIAPI ADXPC_GetDsbNumSamples(void);

/* グローバルフォーカスを持つDirectSoundBufferを作成するかどうかを設定	*/
/* Set DirectSoundBuffer to have global focus							*/
void CRIAPI ADXPC_SetDsbCapsGlobalFocus(CriBool flag);

/* グローバルフォーカスを持つDirectSoundBufferを作成するかどうかを取得	*/
/* Get whether DirectSoundBuffer is having global focus					*/
CriBool CRIAPI ADXPC_GetDsbCapsGlobalFocus(void);

/* 3Dコントロール機能を持つDirectSoundBufferを作成するかどうかを設定	*/
/* Set DirectSoundBuffer to support 3D control							*/
void CRIAPI ADXPC_SetDsbCapsCtrl3D(CriBool flag);

/* 3Dコントロール機能を持つDirectSoundBufferを作成するかどうかを取得	*/
/* Get whether DirectSoundBuffer is supporting 3D control				*/
CriBool CRIAPI ADXPC_GetDsbCapsCtrl3D(void);

/* エフェクト機能を持つDirectSoundBufferを作成するかどうかを設定	*/
/* Set DirectSoundBuffer to support effects processing				*/
void CRIAPI ADXPC_SetDsbCapsCtrlFx(CriBool flag);

/* エフェクト機能を持つDirectSoundBufferを作成するかどうかを取得	*/
/* Get whether DirectSoundBuffer is supporting effects processing	*/
CriBool CRIAPI ADXPC_GetDsbCapsCtrlFx(void);

/* エフェクトを設定する			*/
/* Enable effects on a buffer	*/
CriBool CRIAPI ADXPC_SetDsFx(
	ADXT adxt, CriSint32 count, LPDSEFFECTDESC effect, CriUint32 *result);

#ifdef __cplusplus
}
#endif

/***************************************************************************
 *      旧バージョンとの互換用
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
