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

/* 多重定義防止					*/
/* Prevention of redefinition	*/
#if !defined(_ADXXAUDIO2_H_INCLUDED)
#define _ADXXAUDIO2_H_INCLUDED

/***************************************************************************
 *       バージョン情報
 *       Version
 ***************************************************************************/
#define ADXXAUDIO2_VERSION			"1.01"

/***************************************************************************
 *      インクルードファイル
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
		/* 注意：以下のコードはDirectX SDK (March 2008)環境でエラーを起こします。	*/
		/* 　　　不具合発生時には、DirectX SDK内の'dxsdkver.h'を修正してください。	*/
		/* 参考）http://www.microsoft.com/japan/msdn/directx/Releasenotes/dxreadmeMar08J.aspx */
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
CriSint32 CRIAPI ADXXAUDIO2_SetupSound(IXAudio2 *xa2, IXAudio2Voice *ov);

/* サウンドのシャットダウン */
/* Shutdown of sound */
CriSint32 CRIAPI ADXXAUDIO2_ShutdownSound(void);

/*---------------------------------------------------------------------------
 * メモリ管理用関数
 * Functions for memory management
 *-------------------------------------------------------------------------*/
/* サウンドバッファ確保用メモリ確保関数の登録 */
/* Sets the custom malloc function for sound buffer allocation */
void CRIAPI ADXXAUDIO2_SetUserMallocFunc(void *(*func)(void *obj, CriUint32 size), void *obj);

/* サウンドバッファ解放用メモリ解放関数の登録 */
/* Sets the custom free function for sound buffer release */
void CRIAPI ADXXAUDIO2_SetUserFreeFunc(void (*func)(void *obj, void *mem), void *obj);

/*---------------------------------------------------------------------------
 * XAudio2制御用関数
 * Functions for setting XAudio2 parameters
 *-------------------------------------------------------------------------*/
/* ADXTハンドルからXAudio2SourceVoiceを取得する */
/* Gets the XAudio2SourceVoice from ADXT handle. */
IXAudio2SourceVoice * CRIAPI ADXXAUDIO2_GetSourceVoice(ADXT adxt);

/* XAudio2SourceVoice作成時のフラグを指定する								*/
/* 備考）ADXTハンドルはXAudio2SourceVoiceを内部で作成します。				*/
/*       本関数を使用することで、ADXTハンドルが内部で作成する				*/
/*       XAudio2SourceVoiceに対してフラグを設定することが可能です。			*/
/*       XMPと連動するADXTハンドルを作成する際には、						*/
/*       XAUDIO2_VOICE_MUSICを指定してください。							*/
/*       （デフォルト状態では0が指定さています。）							*/
/* Sets the flags for XAudio2SourceVoice creation.							*/
/* NOTE) ADXT handle creates XAudio2SourceVoice internally.					*/
/*       The specified flags are appended to XAudio2SourceVoice.			*/
/*       The default value is 0.											*/
/*       If you want to create ADXT handle which cooperates with XMP,		*/
/*       please specify XAUDIO2_VOICE_MUSIC.								*/
void CRIAPI ADXXAUDIO2_SetSourceVoiceDefaultFlags(UINT32 Flags);

/* XAudio2SourceVoice作成時に指定されるフラグを取得する */
/* Gets the flags for XAudio2SourceVoice creation. */
UINT32 CRIAPI ADXXAUDIO2_GetSourceVoiceDefaultFlags(void);

/* XAudio2SourceVoice作成時の出力ボイスの最大周波数調整比を指定する			*/
/* 備考）ADXTハンドルはXAudio2SourceVoiceを内部で作成します。				*/
/*       本関数を使用することで、ADXTハンドルが内部で作成する				*/
/*       XAudio2SourceVoiceに対して最大周波数調整比を						*/
/*       設定することが可能です。											*/
/*       （デフォルト状態では1.0fが指定さています。）						*/
/* Sets the max frequency ratio for XAudio2SourceVoice creation.			*/
/* NOTE) ADXT handle creates XAudio2SourceVoice internally.					*/
/*       The specified max frequency ratio is used for						*/
/*       XAudio2SourceVoice creation.										*/
/*       The default value is 1.0f.											*/
void CRIAPI ADXXAUDIO2_SetSourceVoiceDefaultMaxFrequencyRatio(float MaxFrequencyRatio);

/* XAudio2SourceVoice作成時に指定される最大周波数調整比を取得する */
/* Gets the max frequency ratio for XAudio2SourceVoice creation. */
float CRIAPI ADXXAUDIO2_GetSourceVoiceDefaultMaxFrequencyRatio(void);

/* XAudio2SourceVoice作成時の出力ボイスを指定する							*/
/* 備考）ADXTハンドルはXAudio2SourceVoiceを内部で作成します。				*/
/*       本関数を使用することで、ADXTハンドルが内部で作成する				*/
/*       XAudio2SourceVoiceに対して出力ボイスを設定することが可能です。		*/
/*       （デフォルト状態ではADXXAUDIO2_SetupSound関数で指定された			*/
/*       ボイスが使用されます。）											*/
/* Sets the output voice for XAudio2SourceVoice creation.					*/
/* NOTE) ADXT handle creates XAudio2SourceVoice internally.					*/
/*       The specified output voice is used for								*/
/*       XAudio2SourceVoice creation.										*/
/*       The default output voice is the voice specified via				*/
/*       the ADXXAUDIO2_SetupSound function.								*/
void CRIAPI ADXXAUDIO2_SetSourceVoiceDefaultOutputVoice(IXAudio2Voice *pOutput);

/* XAudio2SourceVoice作成時に指定される出力ボイスを取得する */
/* Gets the output voice for XAudio2SourceVoice creation. */
IXAudio2Voice *ADXXAUDIO2_GetSourceVoiceDefaultOutputVoice(void);

/* ADXライブラリが使用するXAudio2操作セット識別子を指定する					*/
/* 備考）ADXライブラリは、音声再生処理時にXAudio2操作セットを使用します。	*/
/*       デフォルト状態では、操作セットの識別子として0x7FFFFFFFが			*/
/*       使用されます。														*/
/*       アプリケーション中でXAudio2操作セットを使用する場合、				*/
/*       この識別子の使用を避ける必要があります。							*/
/*       ADXライブラリが使用する操作セット識別子を変更させたい場合に、		*/
/*       本関数を使用して識別子を変更してください。							*/
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

/* ADXライブラリが使用するXAudio2操作セット識別子を取得する */
/* Gets the XAudio2 Operation Set identifier of the ADX library. */
UINT32 CRIAPI ADXXAUDIO2_GetOperationSetIdentifier(void);

/* ADXTハンドルに対して出力ボイスを設定する */
/* 備考）この関数はIXAudio2Voice_SetOutputVoices関数のラッパー関数です。 */
/* Sets the voice output to ADXT handle. */
/* NOTE: This function calls IXAudio2Voice_SetOutputVoices function. */
HRESULT CRIAPI ADXXAUDIO2_SetOutputVoices(ADXT adxt, const XAUDIO2_VOICE_SENDS *pSendList);

/* ADXTハンドルに対してエフェクトチェーンを設定する						*/
/* 備考）この関数はIXAudio2Voice_SetEffectChain関数のラッパー関数です。	*/
/* Sets the effect chain to ADXT handle.								*/
/* NOTE: This function calls IXAudio2Voice_SetEffectChain function.		*/
HRESULT CRIAPI ADXXAUDIO2_SetEffectChain(
	ADXT adxt, const XAUDIO2_EFFECT_CHAIN *pEffectChain);

/* ADXTハンドルに対してエフェクトパラメータを設定する							*/
/* 備考）この関数はIXAudio2Voice_SetEffectParameters関数のラッパー関数です。	*/
/* Sets the effect paramter to ADXT handle.										*/
/* NOTE: This function calls IXAudio2Voice_SetEffectParameters function.		*/
HRESULT CRIAPI ADXXAUDIO2_SetEffectParameters(ADXT adxt, UINT32 EffectIndex,
	const void *pParameters, UINT32 ParametersByteSize, UINT32 OperationSet);

/* ADXTハンドルからエフェクトパラメータを取得する								*/
/* 備考）この関数はIXAudio2Voice_GetEffectParameters関数のラッパー関数です。	*/
/* Gets the effect paramter from ADXT handle.									*/
/* NOTE: This function calls IXAudio2Voice_GetEffectParameters function.		*/
HRESULT CRIAPI ADXXAUDIO2_GetEffectParameters(ADXT adxt, UINT32 EffectIndex,
	void *pParameters, UINT32 ParametersByteSize);

/* ADXTハンドルに対してエフェクトを有効化する									*/
/* 備考）この関数はIXAudio2SourceVoice_EnableEffect関数のラッパー関数です。		*/
/* Enables the effect to ADXT handle.											*/
/* NOTE: This function calls IXAudio2SourceVoice_EnableEffect function.			*/
HRESULT CRIAPI ADXXAUDIO2_EnableEffect(ADXT adxt, UINT32 EffectIndex, UINT32 OperationSet);

/* ADXTハンドルに対してエフェクトを無効化する									*/
/* 備考）この関数はIXAudio2SourceVoice_DisableEffect関数のラッパー関数です。	*/
/* Disables the effect to ADXT handle.											*/
/* NOTE: This function calls IXAudio2SourceVoice_DisableEffect function.		*/
HRESULT CRIAPI ADXXAUDIO2_DisableEffect(ADXT adxt, UINT32 EffectIndex, UINT32 OperationSet);

#ifdef __cplusplus
}
#endif

/***************************************************************************
 *      旧バージョンとの互換用
 *      For compatibility with old version
 ***************************************************************************/

#endif	/* CRI_ADX_SUPPORT_XAUDIO2 */

#endif	/* _ADXXAUDIO2_H_INCLUDED */

/* --- end of file --- */
