/***************************************************************************
 *
 *  CRI Middleware
 *
 *  Copyright (c) 2001-2007 CRI-MW
 *
 *  Library : ADX Library (CRI ADX for PC)
 *  Module  : Functions for PC
 *  File    : adx_pc.h
 *
 ***************************************************************************/
/* Prevention of redefinition */
#ifndef _ADXPC_H_INCLUDED
#define _ADXPC_H_INCLUDED

/***************************************************************************
 *       �o�[�W�������
 *       Version
 ***************************************************************************/
#define ADXPC_VER					"1.74"

/***************************************************************************
 *      �C���N���[�h�t�@�C��
 *      Include file
 ***************************************************************************/

/* Disable VC++ warning 
 *   In some cases, including windows.h, dsound.h causes the following warning:
 *       rpcasync.h(45) : warning C4115: '_RPC_ASYNC_STATE' : ...
 */
#if defined(_MSC_VER) && ((_MSC_VER >= 1200) && (_MSC_VER < 1300))
#pragma warning(push)
#pragma warning(disable:4115)
#endif

//#include <windows.h>
//#include <dsound.h>

#if defined(_MSC_VER) && ((_MSC_VER >= 1200) && (_MSC_VER < 1300))
#pragma warning(pop)
#endif

#include "cri_xpt.h"
#include "cri_adxt.h"

/***************************************************************************
 *      �萔�}�N��
 *      MACRO CONSTANT
 ***************************************************************************/
/* �f�t�H���g�v���C�I���e�B */
/* Default thread priority */
#define ADXM_PRIO_VSYNC				(THREAD_PRIORITY_HIGHEST)		/* Vsync�X���b�h */
#define ADXM_PRIO_FS				(THREAD_PRIORITY_ABOVE_NORMAL)	/* �t�@�C�������X���b�h */
#define ADXM_PRIO_MWIDLE			(THREAD_PRIORITY_IDLE)			/* �A�C�h���X���b�h */

/* �X�^�b�N�T�C�Y */
/* Thread stack size */
#define	ADXM_STACK_SIZE_VSYNC		(1024 * 1024)
#define	ADXM_STACK_SIZE_FS			(1024 * 1024)
#define	ADXM_STACK_SIZE_MWIDLE		(1024 * 1024)

/* �f�B���N�g���T�����[�h		*/
/* Mode of directory look-up	*/
#define ADXFIC_MODE_ALLDIR			(-1)
#define ADXFIC_MODE_CURRENT			(0)

/* ADXFIC�n���h���X�e�[�^�X	 */
/* Status of ADXFIC handle */
#define ADXFIC_STAT_STOP			(0)
#define ADXFIC_STAT_CACHING			(1)
#define ADXFIC_STAT_CACHEEND		(2)
#define ADXFIC_STAT_ERROR			(3)

/* �f�t�H���g�������g��x100					*/
/* Default of virtual vsync frequency x100	*/
#define ADXPC_DEF_VHZ100			(6000)

/***************************************************************************
 *      �����}�N��
 *      MACRO FUNCTIONS
 ***************************************************************************/
/* ���[�N�̈�ő�T�C�Y�v�Z�}�N�� */
/* Macro that calculates maximum work area size */
#define ADXFIC_CALC_WORK_SIZE(nfiles, max_flen) \
	((nfiles + 1) * (16 + maxflen) + 32)

/***************************************************************************
 *      �f�[�^�^�錾
 *      Data type declaration
 ***************************************************************************/
/*--------------------------------------------------------------------------
 * �t���[�����[�N��ʁiPC�j
 * Type of framework
 *-------------------------------------------------------------------------*/
typedef enum {
	ADXM_FRAMEWORK_DEFAULT			= 0,
	ADXM_FRAMEWORK_PC_SINGLE_THREAD	= 1,
	ADXM_FRAMEWORK_PC_MULTI_THREAD	= 2,
} AdxmFramework;

/*--------------------------------------------------------------------------
 * �X���b�h�Z�b�g�A�b�v�p�����[�^�iPC�j
 * Thread setup parameter for PC
 *-------------------------------------------------------------------------*/
typedef struct {
	/* �v���C�I���e�B */
	CriSint32			priority_vsync;		/* Vsync�X���b�h */
	CriSint32 			priority_fs;		/* �t�@�C�������X���b�h */
	CriSint32 			priority_mwidle;	/* �A�C�h���X���b�h */
} AdxmThreadSprm;

/* �X���b�h�̃Z�b�g�A�b�v���s���ۂ̒��ӓ_									*/
/* (a) �Z�b�g�A�b�v�p�����[�^�́A�t���[�����[�N�Ƃ���						*/
/* �@�@ADXM_FRAMEWORK_Pc_MULTI_THREAD��I�񂾏ꍇ�̂ݗL���ł��B				*/
/* (b) �Z�b�g�A�b�v�p�����[�^�́AADXM_SetupFramework�֐��Ɉ���prm��			*/
/* �@�@���ēn���܂��BNULL���w�肷��ƃf�t�H���g�ݒ肪�g�p����܂��B			*/
/* Notes when setup of thread												*/
/* (a) If you need to apply custom framework setting, set the setup			*/
/*     parameter to ADXM_SetupFramework function as argument "prm".			*/
/*     When NULL is passed, the default setting is used.	 				*/
/* (b) The setup parameter is effective only when you chose 				*/
/*     ADXM_FRAMEWORK_PC_MULTI_THREAD as framework.							*/

/*--------------------------------------------------------------------------
 * �t�@�C���V�X�e���̃Z�b�g�A�b�v�p�����[�^�\����
 * Parameter structure for file system setup function
 *-------------------------------------------------------------------------*/
typedef struct {
	CriChar8	*rtdir;				/* ���[�g�f�B���N�g��	*/
								/* Root directory		*/
} AdxpcSprmFs;

/*--------------------------------------------------------------------------
 * �t�@�C�����L���b�V���n���h���iADXFIC�n���h���j
 * File information cache handle (ADXFIC handle)
 *-------------------------------------------------------------------------*/
typedef void *ADXFIC;

/*--------------------------------------------------------------------------
 * V�����V�X�e���p�I�u�W�F�N�g�̎��
 * Kind of the object for Vsync system
 *-------------------------------------------------------------------------*/
typedef enum {
	ADXPC_VIDEO_NONE = 0x00,
	ADXPC_VIDEO_DDRAW7 = 0x01,		// DirectDraw7
	ADXPC_VIDEO_D3D8 = 0x02,		// Direct3D8
} ADXPC_VIDEO;

/***************************************************************************
 *      �ϐ��錾
 *      Prototype Function 
 ***************************************************************************/

/***************************************************************************
 *      �֐��錾
 *      Prototype Function 
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/***
*			�t���[�����[�N�ݒ�֐�
*			Framework setting function
***/

/* CRI�~�h���E�F�A�̃t���[�����[�N�Z�b�g�A�b�v */
/* Setup the framework */
CriBool CRIAPI ADXM_SetupFramework(AdxmFramework framework, void *prm);

/* CRI�~�h���E�F�A�̃t���[�����[�N�̏I�� */
/* Shutdown the framework */
CriBool CRIAPI ADXM_ShutdownFramework(void);

/***
 *          �X���b�h�֌W
 *          Thread
 ***/
 
/* ADX�T�[�o�֐�(��X���b�h�p)							*/
/* ADX server function(for using ADX without thread)	*/
void CRIAPI ADXPC_ExecServer(void);

/* ADX�T�[�o�֐�(�X���b�h�p)						*/
/* ADX server function(for using ADX with thread)	*/
CriBool CRIAPI ADXPC_ExecServerEx(void *obj);

/***
 *          �t�@�C�������֌W
 *          File proseccing
 ***/

/* �t�@�C���V�X�e���̃Z�b�g�A�b�v	*/
/* Set up the file system			*/
void CRIAPI ADXPC_SetupFileSystem(AdxpcSprmFs *sprmd);

/* �t�@�C���V�X�e���̃V���b�g�_�E��	*/
/* Shut down the file system		*/
void CRIAPI ADXPC_ShutdownFileSystem(void);

/***
 *          V�����֌W
 *          V-Sync system
 ***/

/* V�����V�X�e�����Z�b�g�A�b�v	*/
/* Set up the V-Sync system		*/
CriSint32 CRIAPI ADXPC_SetupVsync(void *video_obj, CriSint32 mode);

/* V�����V�X�e�����V���b�g�_�E��	*/
/* Shut down the V-Sync system		*/
CriBool CRIAPI ADXPC_ShutdownVsync(void);

/* V�����V�X�e�����Đݒ�	*/
/* Reset the V-Sync system	*/
CriSint32 CRIAPI ADXPC_ResetVsync(void);

/* ADX�T�[�o�[������VBlank����ǂꂾ�����炷���ݒ�	*/
/* Set ADX server offset from VBlank				*/
void CRIAPI ADXPC_SetSvrOffset(CriSint32 offset_msec);

/* ���zV���g���̐ݒ�i1/100Hz�P�ʁA60Hz�Ȃ�6000�j				*/
/* Set virtual Vsync frequency	(in unit of 1/100Hz, 60Hz=6000)	*/
void CRIAPI ADXPC_SetVirtualVhz(CriUint32 vhz100);

/* ���zV���g���̎擾			*/
/* Get virtual Vsync frequency	*/
CriUint32 CRIAPI ADXPC_GetVirtualVhz(void);

/* Present�^�C�~���O�̒ʒm */
void CRIAPI ADXPC_NotifyPresentTiming(void);

/* Present�^�C�~���O�}�[�W���̐ݒ� [%] */
void CRIAPI ADXPC_SetPresentTimingMargin(CriSint32 margin);

/***
 *          �t�@�C�����L���b�V���O�p�֐�
 *          Functions for file information cache
 ***/

/* FIC�̏���������	*/
/* Initialize FIC	*/
void CRIAPI ADXFIC_Init(void);

/* �I������		*/
/* Finish FIC	*/
void CRIAPI ADXFIC_Finish(void);

/* ��Ɨ̈�T�C�Y�̎擾		*/
/* Get size of work area	*/
CriSint32 CRIAPI ADXFIC_GetWorkSize(const CriChar8 *dname, CriSint32 mode);

/* ADXFIC�n���h���̍쐬	*/
/* Create ADXFIC handle	*/
ADXFIC CRIAPI ADXFIC_Create(const CriChar8 *dname, CriSint32 mode, void *work, CriSint32 wksize);

/* ADXFIC�n���h���̏���		*/
/* Destroy ADXFIC handle	*/
void CRIAPI ADXFIC_Destroy(ADXFIC fic);

/* �L���b�V���O���ꂽ�t�@�C�����̎擾	*/
/* Get number of cached files			*/
CriSint32 CRIAPI ADXFIC_GetNumFiles(ADXFIC fic);

/* ���p����Ă����Ɨ̈�T�C�Y�̎擾	*/
/* Get size of used work area			*/
CriSint32 CRIAPI ADXFIC_GetUsedWkSize(ADXFIC fic);

/* �t�@�C���T�����s�����f�B���N�g�����̎擾 */
/* Get directory name for look-up			*/
CriChar8 * CRIAPI ADXFIC_GetDirName(ADXFIC fic);

/* �L���b�V���O���ꂽ�t�@�C���̃T�C�Y���擾�i�Z�N�^�P�ʁj	*/
/* Get size of cached file (sector)							*/
CriSint32 CRIAPI ADXFIC_GetFsizeSct(ADXFIC fic, const CriChar8 *fname);

/* �L���b�V���O���ꂽ�t�@�C���̃T�C�Y���擾�i�o�C�g�P�ʁj	*/
/* Get size of cached file (Byte)							*/
CriSint64 CRIAPI ADXFIC_GetFileSize(ADXFIC fic, const CriChar8 *fname);

/* �t�@�C�����L���b�V������Ă��邩�ǂ����̃`�F�b�N */
/* Check whether the file is cached					*/
CriBool CRIAPI ADXFIC_IsFileCached(const CriChar8 *fname);

/* �w��t�@�C���̃L���b�V��������		*/
/* Disable caching of specified file	*/
void CRIAPI ADXFIC_DisableFile(ADXFIC fic, const CriChar8 *fname);

/* �w��t�@�C���̃L���b�V���L����		*/
/* Enable chaching of specified file	*/
void CRIAPI ADXFIC_EnableFile(ADXFIC fic, const CriChar8 *fname);

/* �X�e�[�^�X�̎擾	*/
/* Get state		*/
CriSint32 CRIAPI ADXFIC_GetStat(ADXFIC fic);

/***
 *          ���̑�
 *          Others
 ***/

/* �o�[�W����������̎擾	*/
/* Get version string		*/
CriChar8 * CRIAPI ADXPC_GetVersion(void);

/***
 *          �f�o�b�O�p
 *          For debugging
 ***/

#ifdef _DEBUG

/* �t�@�C�����̃f�o�b�O�o��	*/
/* Print cached file names	*/
void CRIAPI ADXFIC_DebugPrintFlist(ADXFIC fic);

/* �S�Ẵt�@�C�����̃f�o�b�O�o�� */
/* Print all cached file names	*/
void CRIAPI ADXFIC_DebugPrintFlistAll(void);

#endif	// #ifdef _DEBUG

#ifdef __cplusplus
}
#endif	// __cplusplus

/***************************************************************************
 *      ���o�[�W�����Ƃ̌݊��p
 *      For compatibility with old version
 ***************************************************************************/
#define ADXPC_SetupThrd(tprm)			ADXM_SetupThrd(tprm)
#define ADXPC_SetupThread(tprm)			ADXM_SetupThrd(tprm)
#define ADXPC_ShutdownThread()			ADXM_ShutdownThrd()
#define ADXPC_SPRM_DVD					AdxpcSprmFs
#define ADXPC_SPRM_WINFS				AdxpcSprmFs
#define ADXPC_SPRM_ANSI					AdxpcSprmFs
#define ADXPC_SetupWinFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SetupDvdFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SPRM_WINFS				AdxpcSprmFs
#define ADXPC_SPRM_DVD					AdxpcSprmFs
#define ADXPC_SPRM_ANSI					AdxpcSprmFs
#define ADXPC_SetupWinFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SetupDvdFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_SetupAnsiFs(prm)			ADXPC_SetupFileSystem(prm)
#define ADXPC_ShutdownWinFs()			ADXPC_ShutdownFileSystem()
#define ADXPC_ShutdownDvdFs()			ADXPC_ShutdownFileSystem()
#define ADXPC_ShutdownAnsiFs()			ADXPC_ShutdownFileSystem()

#endif	// #ifndef _ADXPC_H_INCLUDED

/* end of file */
