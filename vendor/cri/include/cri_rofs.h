/***********************************************************************
 *
 *  Software Library
 *  Copyright (c) 2001-2008 CRI-MW
 *
 *      Read Only File System
 *
 *  Module  : Library Header File
 *  File    : cri_rofs.h
 *  Date    : 2008-11-06
 *  Version : see (ROFS_VERSION_STR)
 *  Notes   : 
 *
 ***********************************************************************/

/*---------------------------------------------------------------------*/
#ifndef _CRI_ROFS_H_
#define _CRI_ROFS_H_

/*****************************************************************************
 *      �C���N���[�h�t�@�C��
 *		Include file
 *****************************************************************************/

/*---------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cri_xpt.h>

#ifndef CRI_XPT_H
	Please include "cri_xpt.h"		/* �\�h */
									/* Prevention */
#endif

/*****************************************************************************
 *      �萔�}�N��
 *		Macro Constant
 *****************************************************************************/

/* �o�[�W�����ԍ� */
/* Version Number */
#define ROFS_VERSION_STR		"2.04"		/* Version number */

/* �Z�N�^�� */
/* Sector Length */
#define ROFS_SCT_LEN			(2048)

/* �t�@�C������ */
/* File Attribute */
#define ROFS_ATR_DIRECTORY		(0x02)		/* �f�B���N�g�� */
											/* Directory */

/* �{�����[���� */
/* Volume name */
#define ROFS_VOLNAME_LEN		(8)
#define ROFS_VOLNAME_SIZ		(ROFS_VOLNAME_LEN + 1)

/* �t�@�C���� */
/* File name */
#define ROFS_FNAME_LEN			(31)
#define ROFS_FNAME_SIZ			(ROFS_FNAME_LEN + 1)

/* �f�B���N�g���� */
/* Directory name */
#define ROFS_DIRNAME_LEN		(31)
#define RPFS_DIRNAME_SIZ		(ROFS_DIRNAME_LEN+1)

/* �{�����[��ID */
/* Volume ID */
#define ROFS_VOLID_SIZ			(8)

/* CVM�^�C���X�^���v */
/* Time stamp of CVM */
#define ROFS_TMSTMP_SIZ			(8)

/* ��ƃo�b�t�@ */
/* Work buffer */
#define ROFS_WKBUF_NUM			(2)
#define ROFS_WKBUF_SIZ			(ROFS_SCT_LEN * ROFS_WKBUF_NUM + 64)


/*============================================================================
 *      ROFS_SetCond�֐��p�̃p�����[�^
 *		Parameters for the ROFS_SetCond function
 *===========================================================================*/
/* �f�B���N�g�����R�[�h�\�[�X	*/
/* Directory record source		*/
#define ROFS_PARM_DIRSRC_DFL	(0)		/* default						*/
#define ROFS_PARM_DIRSRC_CVM	(0)		/* from CVM						*/
#define ROFS_PARM_DIRSRC_MEM	(1)		/* from direcotry record file	*/

/*****************************************************************************
 *      �񋓒萔
 *		Enumarate Constants
 *****************************************************************************/

/* �t�@�C���n���h����� */
/* Status of handle */
typedef enum {
	ROFS_STAT_IDLE		= (0),			/* Idle status */
	ROFS_STAT_COMPLETE	= (1),			/* Tranfer complete status */
	ROFS_STAT_TRANS		= (2),			/* During transfer status */
	ROFS_STAT_ERR		= (3),			/* Error outbreak status */
	ROFS_STAT_NUM
} RofsStat;

/* �V�[�N���[�h */
/* Mode of seek */
typedef enum {
	ROFS_SEEK_SET		= (0),			/* Top of file */
	ROFS_SEEK_CUR		= (1),			/* Current position */
	ROFS_SEEK_END		= (2),			/* End of file */
	ROFS_SEEK_NUM
} RofsSeek;

/* �G���[�R�[�h */
/* Error code */
typedef enum {
	ROFS_ERR_OK			= (0),			/* ����I�� */
										/* Successful */
	ROFS_ERR_NG			= (-1),			/* ���s */
										/* Failure */

	ROFS_ERR_PRM		= (-100),		/* �p�����[�^�̐ݒ�G���[ */
										/* Error of parameter */
	ROFS_ERR_ILLHDL		= (-101),		/* �t�@�C���n���h�����s�� */
										/* Illigal file handle */
	ROFS_ERR_NOHNDL		= (-102),		/* �t�@�C���n���h���ɋ󂫂��Ȃ� */
										/* Too many file handles */
	ROFS_ERR_VOLNAME	= (-103),		/* �s���ȃ{�����[���� */
										/* Illigal volume name */
	ROFS_ERR_REGVOLNAME	= (-104),		/* �o�^�ς݂̃{�����[���� */
										/* Already exist volume name */
	ROFS_ERR_VOLUME		= (-105),		/* �{�����[���̖����G���[ */
										/* Invalid Volume */
	ROFS_ERR_NOTDIR		= (-106),		/* �f�B���N�g���łȂ����̂��w�肵�� */
										/* Not directory*/
	ROFS_ERR_NOTFOUND	= (-107),		/* ���݂��Ȃ��t�@�C�� */
										/* No exist file */
	ROFS_ERR_ILLDIRBUF	= (-108),		/* �s���ȃf�B���N�g�����R�[�h */
										/* Illigal directory record */
	ROFS_ERR_DIROVER	= (-109),		/* �f�B���N�g���G���g���̍ő吔�𒴂��� */
										/* Too many entry of directory */
	ROFS_ERR_NOTFOUND_TOD	= (-110),	/* �f�B���N�g�����R�[�h�̏I�[��������Ȃ� */
										/* Termination of a directory record is not found */
	ROFS_ERR_INVALID_DIRLIST= (-111),	/* �����ȃf�B���N�g�����X�g */
										/* Invalid directory list */

	ROFS_ERR_BUSY		= (-200),		/* ���̃R�}���h�����s�� */
										/* Under processing of other commands */
	ROFS_ERR_TOUT		= (-201),		/* ���������Ń^�C���A�E�g���������� */
										/* Timeout */
	ROFS_ERR_PRIMITIVE	= (-202),		/* �v���~�e�B�u�֐��G���[ */
										/* Primitive function error */
	ROFS_ERR_INIT		= (-203),		/* �������G���[ */
										/* Initialization error */
	ROFS_ERR_REQRD		= (-204),		/* �ǂݍ��ݗv���G���[ */
										/* Read request error */
	ROFS_ERR_SEEK		= (-205),		/* �V�[�N�G���[ */
										/* Seek error */
	ROFS_ERR_OPEN		= (-206),		/* �t�@�C���I�[�v���G���[ */
										/* File open error */
	ROFS_ERR_READ		= (-207),		/* �ǂݍ��݃G���[ */
										/* Read error */

	ROFS_ERR_INTERNAL	= (-1000)		/* �����G���[ */
										/* Internal error */
} RofsErr;

/* �������ID */
/* operating condition id */
typedef enum {
	ROFS_COND_DIRSRC		= (0),	/* �f�B���N�g�����R�[�h�\�[�X	*/
									/* directory record source		*/
	ROFS_COND_ECB_READ		= (1),	/* ���[�h�G���[�R�[���o�b�N		*/
									/* read error call back			*/
	ROFS_COND_ECB_LOADDIR	= (2),	/* ���[�h�f�B���N�g���R�[���o�b�N	*/
									/* load directory error call back	*/
	ROFS_COND_END
} RofsCondId;

/*****************************************************************************
 *      �����}�N��
 *		Process Macro
 *****************************************************************************/
/* ���C�u������Ɨ̈�T�C�Y */
/* Library work area size */
#define ROFS_GET_LIBWORK_SIZ(max_open, max_volume, max_dirent) \
	(sizeof(RofsWork)+((max_open)+1)*sizeof(RofsHandle)+\
	(ROFS_GET_DIRREC_SIZ(max_dirent)+sizeof(RofsVolume))*(max_volume)+8)

/* �f�B���N�g�����R�[�h�̈�T�C�Y */
/* Directory record area size */
#define ROFS_GET_DIRREC_SIZ(n_dirent) \
	(sizeof(RofsDirRec)-sizeof(RofsDirRecEnt)+(n_dirent)*sizeof(RofsDirRecEnt))

/* �f�B���N�g���̔��� */
/* judgment of a directory */
#define ROFS_IS_ATR_DIRECTORY(finf) \
	(((finf)->fatr & ROFS_ATR_DIRECTORY)?TRUE:FALSE)

/* �Z�N�^������o�C�g���ւ̕ϊ� */
/* Conversion to the byte number from the sector number */
#define ROFS_CONV_SCT2BYTE(n_sct)	((n_sct) * ROFS_SCT_LEN)

/* �Z�N�^�ǂݍ��݃o�b�t�@�T�C�Y */
/* Sector reading buffer size */
#define ROFS_GET_SCTBUF_SIZ(n_sct)	(ROFS_CONV_SCT2BYTE(n_sct) + 63)

/* �|�C���^��64byte���E���킹 */
/* 64-byte boundary of a pointer */
#define ROFS_ALIGN_64BYTE(ptr)		(((CriSintPtr)(ptr) + 63) & 0xffffffc0)

/*****************************************************************************
 *      �f�[�^�^�̒�`
 *		Data type declaration
 *****************************************************************************/
/* ���[�N�̈� */
/* Work area */
typedef struct RofsStWrok		RofsWork;

/* �v���~�e�B�u�֐��e�[�u�� */
/* Primitive functions table */
typedef struct RofsPfsIfTbl		RofsPfsTbl;

/* �f�o�C�X�R���g���[���֐� */
/* Device control functions */
typedef struct RfpfsDevIfTbl	RofsDevTbl;

/* ���C�u�����C���^�[�t�F�[�X�֐��e�[�u�� */
/* Library interface functions table */
typedef struct RfmaiLibIfTbl	RofsLif;

/* �f�B���N�g�����R�[�h�G���g�� */
/* Directory record entry */
typedef struct {
	CriUint32	fsize;						/* file size lower */
	CriUint32	fsize_ex;					/* file size upper */
	CriSint32	fad;						/* fad */
	CriUint8	fatr;						/* file attribute */
	CriUint8	sid;						/* filename search id */
	CriChar8	fname[ROFS_FNAME_SIZ];		/* filename */
	CriUint8	pad[2];						/* align padding */
} RofsDirRecEnt;	/* 48 bytes */

/* �f�B���N�g�����R�[�h */
/* Directory record */
typedef struct {
	CriSint32	dir_num;					/* number of record */
	CriSint32	max_ent;					/* max entry of directory record */
	CriSint32	dir_fad;					/* fad of directory record */
	CriChar8	volname[ROFS_VOLNAME_SIZ];	/* volume name */
	CriUint8	pad[3];						/* align padding */
	RofsDirRecEnt	dirrec_tbl[1];		/* record table */
} RofsDirRec;		/* 24 bytes */
typedef RofsDirRec	*RofsDirRecBuf;

/* �{�����[���Ǘ� */
/* Volume management */
typedef struct {
	void	*img_hn;					/* file handle of the CVM file */
	CriSint32	zisb;						/* image start position (0 = DVD-ROM) */
	CriSint32	ptbl_cba;					/* cba of path table */
	CriSint32	ptbl_size;					/* size of path table */
	RofsDirRecBuf	curdir;				/* current directory handle */
	CriSint16	req_cnt;					/* request counter */
	CriChar8	volname[ROFS_VOLNAME_SIZ];	/* volume name */
	CriUint8	pad[1];						/* align padding */
	/* 2002-09-26 Ver.1.30 miko { : CVM�t�H�[�}�b�g�ύX */
	CriUint32	flags;						/* flags */
	CriUint8	vol_id[ROFS_VOLID_SIZ];		/* volume id */
	/* } */
	/* 2003-05-27 Ver.1.60 miko { : dirlst�Ή� */
	CriUint8	vol_cvmtim[ROFS_TMSTMP_SIZ];	/* volume time stamp */
	/* } */
} RofsVolume;		/* 52 bytes */

/* ROFS�t�@�C���n���h�� */
/* ROFS File Handle */
typedef struct {
	CriUint32		fsize;					/* file size lower */
	CriUint32		fsize_ex;				/* file size upper */
	RofsWork	*wk;					/* pointer of lib work */
	CriSint32		fid;					/* file id */
	CriSint32		fad;					/* fad */
	CriSint32		ofs;					/* offset */
	CriSint32		fsctsize;				/* sctor size of the file */
	RofsVolume	*vol;					/* image file volume */
	CriSint32		rsize;					/* reading size */
	CriSint32		trns_seg;				/* transferred block unit */
	CriSint32		trns_ofs;				/* transferred byte unit */
	CriSint32		trns_unit;				/* transfer unit (block) */
	CriUint8		*rdadr;					/* read address */
	CriSint16		used;					/* used flag */
	CriSint16		act;					/* handle act */
	CriSint16		stat;					/* handle status(RofsStat) */
	CriSint16		err;					/* error status */
} RofsHandle;		/* 72 bytes */
typedef RofsHandle	*Rofs;

/* ROFS��Ɨ̈� */
/* ROFS Work Area */
struct RofsStWrok {
	CriBool			f_init;				/* initialize flag */
	CriSint32			max_open;			/* max open files */
	CriSint32			max_volume;
	CriSint32			max_dirent;
	CriSint32			worksiz;
	CriUint32			exec_server_cnt;	/* counter */
	CriUint32			actflags;
	CriUint32			tout_read_max;
	Rofs			syshdl;				/* handle for system command */
	Rofs			hndtbl;				/* handle */
	RofsVolume		*vollist;			/* Volume information list */
	RofsVolume		*curvol;			/* information of the defalut Volume */
	RofsDirRecBuf	dirbuf;				/* start posision of directory record */
	RofsPfsTbl		*pfs;				/* primitive function table */
	RofsDevTbl		*dev;				/* device control function table */
	RofsLif			*liftbl;			/* library interface */
	void	(*g_errcb)(void *, CriChar8 *, CriSint32);	/* error callback */
	void			*g_errcb_1st;		/* error callback 1st arg. */
	CriUint32			*sctbuf;			/* sector buffer addres */
	CriUint32	sct_load_buf[ROFS_WKBUF_SIZ / 4];	/* sector buffer */
	RofsHandle		hndlist[1];			/* handle table */
}; /* 76 + 4160 + (72 * (handle num + 1)) */

/* ���C�u�������������  */
/* Library initialize information */
typedef struct {
	CriSint32	max_open;			/* �����ɃI�[�v���ł���ő�t�@�C���� */
								/* The number of the maximum files 
								 * which can be opened simultaneously */
	CriSint32	max_volume;			/* �����ɓo�^�ł���ő�{�����[���� */
								/* The number of the maximum volumes 
								 * which can be registered simultaneously */
	CriSint32	max_dirent;			/* �f�B���N�g���Ɋi�[����ő�t�@�C���� */
								/* The number of the maximum files 
								 * stored in a directory */
	void	*rofs_work;			/* ���C�u������Ɨ̈�̐擪�A�h���X */
								/* The head address of a library work domain */
} RofsInitPara;

/* �t�@�C����� */
/* File information */
typedef struct {
	CriUint32	fsize;					/* file size lower */
	CriUint32	fsize_ex;				/* file size upper */
	CriChar8	*fname;					/* file name */
	CriUint8	fatr;					/* file attribute */
	CriUint8	pad[3];					/* align padding */
} RofsFileInf;		/* 16 bytes */

/* �{�����[����� */
/* Volume information */
typedef struct {
	CriChar8	*volname;				/* volume name */
	void	*fhn;					/* file handle of the volume */
} RofsVolumeInf;

/* 2002-10-24 Ver.1.330 miko : add -> 2005-11-15 update */
/* �X�N�����u����� */
/* Scramble information */
typedef struct {
	RofsVolume	*vol;				/* image file volume */
	CriSint32		sctno;				/* sector number */
} RofsScrambleInf;

/* 2002-11-06 Ver.1.400 miko : add */
/* �{�����[���p�����[�^ */
/* Volume parameter */
typedef struct {
	CriChar8	*scrkey;				/* �X�N�����u��������������̃|�C���^ */
									/* The pointer of descramble string */
} RofsVolumePrm;

/* �G���[�R�[���o�b�N�֐� */
/* Error callback function */
typedef void (*RofsErrFunc)(void *obj, CriChar8 *msg, CriSint32 errcode);

/* �v���~�e�B�u�֐��e�[�u�� */
/* Primitive functions table */
struct RofsPfsIfTbl {
	void		(*pfs_execserver)(void);
	void		(*pfs_reserve01)(RofsErrFunc, void *);
	CriSint32		(*pfs_reserve02)(CriSint8 *);
	CriSint32		(*pfs_reserve03)(void);
	void		*(*pfs_reserve04)(CriSint8 *, void *, CriSint32);
	void		(*pfs_reserve05)(void *);
	CriSint32		(*pfs_seek)(void *, CriSint32, RofsSeek);
	CriSint32		(*pfs_tell)(void *);
	CriSint32		(*pfs_reqrd)(void *, CriSint32, void *);
	CriSint32		(*pfs_reserve07)(void *, CriSint32, void *);
	void		(*pfs_stoptr)(void *);
	CriSint32		(*pfs_getstat)(void *);
	CriSint32		(*pfs_getsctlen)(void *);
	void		(*pfs_reserve08)(void *, CriSint32);
	CriSint32		(*pfs_getnumtr)(void *);
	CriSint32		(*pfs_reserve09)(CriSint8 *);
	CriSint32		(*pfs_reserve10)(CriSint8 *);
	CriSint32		(*pfs_reserve11)(void);
	CriSint32		(*pfs_reserve12)(CriSint8 *, void *, CriSint32);
	CriSint32		(*pfs_reserve13)(void *);
	CriSint32		(*pfs_reserve14)(CriSint8 *);
	CriSint32		(*pfs_reserve15)(CriSint8 *);
	CriSint32		(*pfs_reserve16)(CriSint8 *);
	CriSint32		(*pfs_reserve17)(CriSint8 *, void *);
	CriSint32		(*pfs_reserve18)(void *, CriSint32, CriSint32, CriSint32);
	CriSint32		(*pfs_reserve19)(void *, CriSint32, CriSint32, CriSint32);
	/* 2002-09-11 Ver.1.22a miko { : 64bit�Ή� */
	CriSint64		(*pfs_getnumtr64)(void *);
	/* } miko */
	/* 2008-02-04 Ver.2.00 Ryo Usuki { : cvFsIsAvailableRetry�Ή� */
	CriBool		(*pfs_isavailableretry)(void *);
	/* } Ryo Usuki */
};

/*****************************************************************************
 *      �ϐ��錾
 *		Variable Declaration
 *****************************************************************************/

/*****************************************************************************
 *      �֐��錾
 *		Function Declaration
 *****************************************************************************/

/*============================================================================
 *      ���C�u�����������^�I��
 *		Initialize and Finalize of Library
 *===========================================================================*/
/* �t�@�C���V�X�e���̏�����
 * [��  ��] init_para	: ���C�u�����������p�����[�^
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) ���C�u������Ɨ̈�� ROFS_GET_LIBWORK_SIZ�}�N�� �ŋ��߂邱�ƁB
 * (b) init_para.max_dirent �́A"."��".."�̂Q�������Z���邱�ƁB
 */
/* Initialize of file system
 * [Inputs ] init_para	: Library initialization parameter
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) Decides the library work area size by ROFS_GET_LIBWORK_SIZ.
 * (b) init_para.max_dirent should add a part for two of "." and "..".
 */
RofsErr CRIAPI		ROFS_Init(RofsInitPara *init_para);

/* �t�@�C���V�X�e���̏I��
 * [��  ��] �Ȃ�
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 */
/* Finalyze of file syste
 * [Inputs ] none
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
void CRIAPI		ROFS_Finish(void);

/* �v���~�e�B�u�֐��̓o�^
 * [��  ��] pfs_tbl		: �v���~�e�B�u�֐��e�[�u��
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) �f�t�H���g�ł�CRI-MW�W���̊֐����o�^����Ă���B
 */
/* Registration of a primitive function
 * [Inputs ] 
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) The standard function for CRI-MW is registered by the default.
 */
RofsErr CRIAPI		ROFS_SetPfsFuncTbl(RofsPfsTbl *pfs_tbl);

/*============================================================================
 *      �{�����[������
 *		Handle Volume
 *===========================================================================*/
/* �{�����[���̒ǉ�
 * [��  ��] volname		: �{�����[����
 *			img_hn		: �I�[�v���ς݃{�����[���̃n���h���i�t�@�C���n���h���j
 *			volprm		: �{�����[���p�����[�^�i�s�v�Ȃ�NULL�j
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) �{�����[����"ROFS"�̓��C�u�����\��Ȃ̂Ŏg�p�s�B
 * (b) volprm �̕s�v�ȃp�����[�^�� 0 �ɂ��Ă������ƁB
 */
/* Addition of volume (Expansion)
 * [Inputs ] volname	: volume name
 *			 img_hn		: Opend Volume handle (file handle)
 *			 volprm		: Volume parameter
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) Volume name "ROFS" is library reservation, it cannot be used.
 * (b) Please set the unnecessary parameter of volprm to 0.
 */
RofsErr CRIAPI		ROFS_AddVolume(CriChar8 *volname, void *img_hn, RofsVolumePrm *volprm);

/* �{�����[���̍폜
 * [��  ��] volname		: �{�����[����
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 */
/* Deletion of volume
 * [Inputs ] volname	: volume name
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a)
 */
RofsErr CRIAPI		ROFS_DelVolume(CriChar8 *volname);

/* �f�t�H���g�{�����[���̐ݒ�
 * [��  ��] volname		: �{�����[����
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 */
/* Setup of default volume
 * [Inputs ] volname	; volume name
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_SetDefVolume(CriChar8 *volname);

/* �f�t�H���g�{�����[���̎擾
 * [��  ��] �Ȃ�
 * [�o  ��] �Ȃ�
 * [�֐��l] �{�����[����
 * [��  �l] 
 */
/* Gets default volume
 * [Inputs ] none
 * [outputs] none
 * [Return ] volume name
 * [Notes  ] 
 */
CriChar8 * CRIAPI		ROFS_GetDefVolume(void);

/* �{�����[���n���h���̐؂�ւ�
 * [��  ��] volname		: �{�����[����
 *			img_hn		: �I�[�v���ς݃{�����[���̃n���h���i�t�@�C���n���h���j
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 */
/* The change of a volume handle
 * [Inputs ] volname	: volume name
 *			 img_hn		: Opend Volume handle (file handle)
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_SwitchImgHn(CriChar8 *volname, void *img_hn);

/*============================================================================
 *      �t�@�C������
 *		Handle File
 *===========================================================================*/
/* �t�@�C���̃I�[�v��
 * [��  ��] fname		: �t�@�C����
 *			dir_buf		: �f�B���N�g�����R�[�h
 * [�o  ��] �Ȃ�
 * [�֐��l] ROFS�n���h��
 * [��  �l] 
 * (a) dir_buf �� NULL �̏ꍇ�́A�J�����g�f�B���N�g�����R�[�h���Q�Ƃ���B
 */
/* Opening of a file
 * [Inputs ] fname		: File name
 *			 dir_buf	: Directory record
 * [outputs] none
 * [Return ] ROFS handle
 * [Notes  ] 
 * (a) When dir_buf is NULL, a current directory record is referenced.
 */
Rofs CRIAPI		ROFS_Open(CriChar8 *fname, void *dir_buf);

/* �Z�N�^�w��ɂ��t�@�C���̃I�[�v��
 * [��  ��] volname		: �{�����[����
 *			stsct		: �擪�Z�N�^�ԍ�
 *			nsct		: �Z�N�^��
 * [�o  ��] �Ȃ�
 * [�֐��l] ROFS�n���h��
 * [��  �l] 
 */
/* Opening of the file by sector specification
 * [Inputs ] volname	: volume name
 *			 stsct		: Top sector number
 *			 nsct		: The number of sector
 * [outputs] none
 * [Return ] ROFS handle
 * [Notes  ] 
 */
Rofs CRIAPI		ROFS_OpenRange(CriChar8 *volname, CriSint32 stsct, CriSint32 nsct);

/* �t�@�C���̃N���[�Y
 * [��  ��] rofs		: ROFS�n���h��
 * [�o  ��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [��  �l] 
 */
/* Closing of a file
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 */
void CRIAPI		ROFS_Close(Rofs rofs);

/* �t�@�C�����݈ʒu�̈ړ�
 * [��  ��] rofs		: ROFS�n���h��
 *			nsct		: �V�[�N�J�n�ʒu����̈ړ��Z�N�^��
 *			sk_mode		: �V�[�N���[�h
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C���擪����̃Z�N�^��
 * [��  �l] 
 */
/* Movement of a file present position
 * [Inputs ] rofs		: ROFS handle
 *			 nsct		: The number of move sectors from a seeking start position
 *			 sk_mode	: Mode of seek
 * [outputs] none
 * [Return ] The number of sectors from a file top
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_Seek(Rofs rofs, CriSint32 nsct, RofsSeek sk_mode);

/* �t�@�C�����݈ʒu�̎擾
 * [��  ��] rofs		: ROFS�n���h��
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C���擪����̃Z�N�^��
 * [��  �l] 
 */
/* Gets file present position
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] The number of sectors from a file top
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_Tell(Rofs rofs);

/* �ǂݍ��݂̗v��
 * [��  ��] rofs		: ROFS�n���h��
 *			nsct		: �Z�N�^��
 * [�o  ��] buf			: �ǂݍ��ݐ�o�b�t�@
 * [�֐��l] �v�����󂯕t�����Z�N�^��
 * [��  �l] 
 * (a) buf �̐擪���^�[�Q�b�g���w�肷�鋫�E�ɑ����Ă������ƁB
 */
/* Request of reading
 * [Inputs ] rofs		: ROFS handle
 *			 nsct		: The number of sectors
 * [outputs] buf		: Reading buffer
 * [Return ] The number of sectors which received the request
 * [Notes  ] 
 * (a) Must be keep the boundary where a target specifies the address of buf
 */
CriSint32 CRIAPI		ROFS_ReqRd(Rofs rofs, CriSint32 nsct, void *buf);

/* �]���̒�~
 * [��  ��] rofs		: ROFS�n���h��
 * [�o  ��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [��  �l]
 */
/* Stop of transfer
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 */
void CRIAPI		ROFS_StopTr(Rofs rofs);

/* �T�[�o�����̎��s
 * [��  ��] �Ȃ�
 * [�o  ��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [��  �l] 
 * (a) ADX��ADXF_SetupRofs�֐����g���Ă���ꍇ�́A�{�֐����g��Ȃ����ƁB
 *	   �����݂�ADX�Ƃ̕��p�K�{�B�P�̂ł͓��삵�Ȃ��̂ŁA�{�֐��͎g��Ȃ��B
 */
/* Execution of server
 * [Inputs ] none
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 * (a) Don't use this function when use the ADXF_SetupRofs function 
 *     of ADX is being used.
 *     * present : indispensable alone. Combined use with ADX.
 *                 Don't use this function.
 */
void CRIAPI		ROFS_ExecServer(void);

/*============================================================================
 *      ���̎擾
 *		Get Information
 *===========================================================================*/
/* �t�@�C���n���h����Ԃ̎擾
 * [��  ��] rofs		: ROFS�n���h��
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C���n���h���X�e�[�^�X
 * [��  �l] 
 */
/* Gets file handle status
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] file handle status
 * [Notes  ] 
 */
RofsStat CRIAPI	ROFS_GetStat(Rofs rofs);

/* �t�@�C���T�C�Y�̎擾
 * [��  ��] fname		: �t�@�C����
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C���T�C�Y�i�o�C�g���j
 * [��  �l] 
 * (a) ���l�̏ꍇ�̓T�C�Y�ł͖����A�G���[�������B
 * (b) �t�@�C���T�C�Y��2GB�ȏ�̏ꍇ 0x7fffffff ��Ԃ��B
 */
/* Gets file size
 * [Inputs ] fname		: file name
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint32 CRIAPI		ROFS_GetFileSiz(CriChar8 *fname);

/* �t�@�C���T�C�Y�̎擾�i�f�B���N�g�����R�[�h�w��j
 * [��  ��] fname		: �t�@�C����
 *			dir_buf		: �f�B���N�g�����R�[�h
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C���T�C�Y�i�o�C�g���j
 * [��  �l] 
 * (a) ���l�̏ꍇ�̓T�C�Y�ł͖����A�G���[�������B
 * (b) �t�@�C���T�C�Y��2GB�ȏ�̏ꍇ 0x7fffffff ��Ԃ��B
 */
/* Gets file size
 * [Inputs ] fname		: file name
 *			 dir_buf	: directory record
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint32 CRIAPI		ROFS_GetFileSizEx(CriChar8 *fname, void *dir_buf);

/* �Z�N�^���̎擾
 * [��  ��] �Ȃ�
 * [�o  ��] �Ȃ�
 * [�֐��l] �Z�N�^���i�o�C�g���j
 * [��  �l] 
 */
/* Gets sector length
 * [Inputs ] none
 * [outputs] none
 * [Return ] sector length (byte)
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetSctLen(void);

/* �]���ς݃T�C�Y�̎擾
 * [��  ��] rofs		: ROFS�n���h��
 * [�o  ��] �Ȃ�
 * [�֐��l] �]���ς݃f�[�^�T�C�Y�i�o�C�g���j
 * [��  �l] 
 */
/* Gets transferred data size
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] transferred date size (byte)
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetNumTr(Rofs rofs);

/* �{�����[�����̎擾
 * [��  ��] volname		: �{�����[����
 * [�o  ��] volinf		: �{�����[�����
 * [�֐��l] �G���[
 * [��  �l] 
 */
/* Gets valume information
 * [Inputs ] volname	: volume name
 * [outputs] volinf		: Volume information
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_GetVolumeInf(CriChar8 *volname, RofsVolumeInf *volinf);

/*============================================================================
 *      �f�B���N�g������
 *		Handle Directory
 *===========================================================================*/
/* �f�B���N�g�����R�[�h�̓ǂݍ���
 * [��  ��] dirname		: �f�B���N�g����
 *			n_dirent	: ���R�[�h���̏��
 * [�o  ��] dir_buf		: �f�B���N�g�����R�[�h
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) �������A�B
 * (b) n_dirent �����ۂ̃��R�[�h�������̏ꍇ�́AROFS_ERR_DIROVER ���Ԃ�B
 */
/* Reading of a directory record
 * [Inputs ] dirname	: directory name
 *			 n_dirent	: The maximum of the number of records
 * [outputs] dir_buf	: directory record
 * [Return ] error code
 * [Notes  ] 
 * (a) This function is not return, while the function is processing.
 * (b) Return value is ROFS_ERR_DIROVER when n_dirent is under the 
 *     actual number of directory records. */
RofsErr CRIAPI		ROFS_LoadDir(CriChar8 *dirname, void *dir_buf, CriSint32 n_dirent);

/* �J�����g�f�B���N�g���̐ݒ�
 * [��  ��] volname		: �{�����[����
 *			dir_buf		: �f�B���N�g�����R�[�h
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 */
/* Setup of a current directory
 * [Inputs ] volname	: volume name
 *			 dir_buf	: directory record
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_SetDir(CriChar8 *volname, void *dir_buf);

/* �J�����g�f�B���N�g���̐ݒ�(ISO9660�f�B���N�g�����R�[�h) 
 * [��  ��] volname		: �{�����[����
 *			dir_buf		: �f�B���N�g�����R�[�h
 *			nsct		: �f�B���N�g�����R�[�h�̃Z�N�^��
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) �T���v���v���O�����Q��
 */
/* Setup of a current directory (Directory record of the ISO9660 image)
 * [Inputs ] volname	: volume name
 *			 dir_buf	: directory record
 *			 nsct		: the number of sectores of directory record
 * [outputs] 
 * [Return ] error code
 * [Notes  ] 
 * (a) refer to sample program.
 */
RofsErr CRIAPI		ROFS_SetIsoDir(CriChar8 *volname, void *dir_buf, CriSint32 nsct);

/* �J�����g�f�B���N�g���̕ύX
 * [��  ��] dirname		: �f�B���N�g����
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) �������A�B
 * (b) n_dirent �����ۂ̃��R�[�h�������̏ꍇ�́AROFS_ERR_DIROVER ���Ԃ�B
 */
/* Changeing of a current directory
 * [Inputs ] dirname	: directory name
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 * (a) This function is not return, while the function is processing.
 * (b) Return value is ROFS_ERR_DIROVER when n_dirent is under the 
 *     actual number of directory records.
 */
RofsErr CRIAPI		ROFS_ChangeDir(CriChar8 *dirname);

/* �t�@�C�����݂̔���
 * [��  ��] fname		: �t�@�C����
 * [�o  ��] �Ȃ�
 * [�֐��l] �L���iTRUE = �L , FALSE = ���j
 * [��  �l] 
 */
/* Judgment of file existence
 * [Inputs ] fname		: file name
 * [outputs] none
 * [Return ] TRUE = exist, FALSE = not exist
 * [Notes  ] 
 */
CriBool CRIAPI		ROFS_IsExistFile(CriChar8 *fname);

/* �t�@�C�����̎擾
 * [��  ��] volname		: �{�����[����
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C�����i"."��".."�������f�B���N�g���̐����܂ށj
 * [��  �l] 
 */
/* Gets the number of files
 * [Inputs ] volname	: volume name
 * [outputs] none
 * [Return ] the number of files 
 *           (the number of the directories except "." and ".." is included)
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetNumFiles(CriChar8 *volname);

/* �f�B���N�g�����̎擾
 * [��  ��] volname		: �{�����[����
 *			num			: ���R�[�h���̏��
 * [�o  ��] flist		: �t�@�C�����i���R�[�h�����̔z��j
 * [�֐��l] ���R�[�h��
 * [��  �l] 
 */
/* Gets directory information
 * [Inputs ] voname		: volume name
 *			 num		: The maximum of the number of records
 * [outputs] flist		: file information (array of the number of records)
 * [Return ] the number of records
 * [Notes  ] 
 */
CriSint32 CRIAPI		ROFS_GetDirInf(CriChar8 *volname, RofsFileInf *flist, CriSint32 num);

/*============================================================================
 *      �X�N�����u��
 *		Scramble
 *===========================================================================*/
/* �X�N�����u�����̎擾
 * [��  ��] fname		: �t�@�C�����i���̓f�B���N�g�����j
 * [�o  ��] scrinf		: �X�N�����u�����
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) �T���v���v���O�����Q��
 */
/* Gets scramble information
 * [Inputs ] fname		: file name (or directory name)
 * [outputs] scrinf		: scramble information
 * [Return ] error code
 * [Notes  ] 
 * (a) refer to sample program.
 */
RofsErr CRIAPI		ROFS_GetScrambleInf(CriChar8 *fname, RofsScrambleInf *scrinf);

/* �X�N�����u���̉���
 * [��  ��] buf			: �X�N�����u������������f�[�^���������o�b�t�@
 *			sctnum		: ��������Z�N�^��
 *			scrinf		: �X�N�����u�����
 * [�o  ��] scrinf		: �X�N�����u�����i�X�V��j
 * [�֐��l] �G���[
 * [��  �l] 
 * (a) scrinf �̓��e�͍X�V�����B
 * (b) �T���v���v���O�����Q��
 */
/* Descramble
 * [Inputs ] buf		: scrambled data
 *			 sctnum		: buffer size (the number of sector)
 *			 scrinf		: scramble information
 * [outputs] scrinf		: scramble information (after update)
 * [Return ] error code
 * [Notes  ] 
 * (a) The contents of scrinf are updated.
 * (b) refer to sample program.
 */
RofsErr CRIAPI		ROFS_Descramble(void *buf, CriSint32 sctnum, RofsScrambleInf *scrinf);

/*============================================================================
 *      �G���[����
 *		Handle Error
 *===========================================================================*/
/* �G���[�R�[���o�b�N�֐��̓o�^
 * [��  ��] errfunc		: �G���[�R�[���o�b�N�֐�
 *			obj			: �G���[�I�u�W�F�N�g
 * [�o  ��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [��  �l] 
 */
/* Registration of an error callback function
 * [Inputs ] errfunc	: error callack function
 *			 obj		: error object
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 */
void CRIAPI		ROFS_EntryErrFunc(RofsErrFunc errfunc, void *obj);

/* �Ō�ɔ��������G���[���擾����
 * [��  ��] �Ȃ�
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l] 
 */
/* Gets last error code
 * [Inputs ] none
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI		ROFS_GetLastError(void);

/*============================================================================
 *      64bit�Ή�
 *		64bit Operation
 *===========================================================================*/
/* �]���ς݃T�C�Y�̎擾 / 64bit�� / PS2��
 * [��  ��] rofs		: ROFS�n���h��
 * [�o  ��] �Ȃ�
 * [�֐��l] �]���ς݃f�[�^�T�C�Y�i�o�C�g���j
 * [��  �l] 
 */
/* Gets transferred data size / 64bit / for PS2
 * [Inputs ] rofs		: ROFS handle
 * [outputs] none
 * [Return ] transferred date size (byte)
 * [Notes  ] 
 */
CriSint64 CRIAPI		ROFS_GetNumTr64(Rofs rofs);

/* �t�@�C���T�C�Y�̎擾 / 64bit�� / PS2��
 * [��  ��] fname		: �t�@�C����
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C���T�C�Y�i�o�C�g���j
 * (a) ���l�̏ꍇ�̓T�C�Y�ł͖����A�G���[�������B
 * (b) �t�@�C���T�C�Y��2GB�ȏ�̏ꍇ 0x7fffffffffffffff ��Ԃ��B
 */
/* Gets file size / 64bit / for PS2
 * [Inputs ] fname		: file name
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint64 CRIAPI		ROFS_GetFileSiz64(CriChar8 *fname);

/* �t�@�C���T�C�Y�̎擾�i�f�B���N�g�����R�[�h�w��j / 64bit�� / PS2��
 * [��  ��] fname		: �t�@�C����
 *			dir_buf		: �f�B���N�g�����R�[�h
 * [�o  ��] �Ȃ�
 * [�֐��l] �t�@�C���T�C�Y�i�o�C�g���j
 * [��  �l] 
 * (a) ���l�̏ꍇ�̓T�C�Y�ł͖����A�G���[�������B
 * (b) �t�@�C���T�C�Y��2GB�ȏ�̏ꍇ 0x7fffffff ��Ԃ��B
 */
/* Gets file size
 * [Inputs ] fname		: file name
 *			 dir_buf	: directory record
 * [outputs] none
 * [Return ] file size (byte)
 * [Notes  ] 
 * (a) Return value is error code when return value is minus.
 * (b) Return value is 0x7fffffff when file size is over the 2GB.
 */
CriSint64 CRIAPI		ROFS_GetFileSizEx64(CriChar8 *fname, void *dir_buf);

/*============================================================================
 *      �f�B���N�g�����R�[�h�t�@�C���֘A
 *		For directory recored file
 *===========================================================================*/
/* �f�B���N�g���R�[�h�t�@�C����CVM�̐������`�F�b�N
 * [��  ��] vol		: �{�����[����
 *			cvmtim	: CVM�t�@�C���̃^�C���X�^���v�f�[�^�i�r���_���o�͂���j
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l]
 * �f�B���N�g�����R�[�h�t�@�C���́A���ۂɎg��CVM����쐬���Ȃ��Ă͂Ȃ�Ȃ��B
 * �{�֐��ŕs�������N���Ă��Ȃ������`�F�b�N�ł���B
 * �ȉ��̏ꍇ�ɃG���[���Ԃ�B
 * (a) �{�����[�������Ԉ���Ă���B�i��. ���o�^�{�����[���j
 * (b) �f�B���N�g�����X�g�����݂�CVM�t�@�C��������ꂽ���̂ł͂Ȃ��B
 */
/* Check validness of the directory list
 * [Inputs ] vol	: Volume name
 *			 cvmtim	: Time stamp of CVM file (Output by ROFSBLD)
 * [outputs] none
 * [Return ] error code
 * [Notes  ]
 * You have to create a directory record file from actually used CVM.
 * It can be confirmed whether mismatching has occurred with this function.
 * Error code is returned in the following cases.
 * (a) When the volume name is wrong. (EX. non registerd volume)
 * (b) When the directory list is not created from current CVM file.
 */
RofsErr CRIAPI	ROFS_CheckDirrecFile(CriChar8 *volname, void *cvmtim);

/*============================================================================
 *      ���̑�
 *		Other
 *===========================================================================*/
/* �o�[�W����������̎擾
 * [��  ��] �Ȃ�
 * [�o  ��] �Ȃ�
 * [�֐��l] �o�[�W����������̃|�C���^
 * [��  �l]
 */
/* Gets version string
 * [Inputs ] none
 * [outputs] none
 * [Return ] version string
 * [Notes  ] 
 */
const CriChar8	* CRIAPI	ROFS_GetVersionStr(void);

/* �f�[�^�ǂݍ��ݑ҂��^�C���A�E�g�̐ݒ�
 * [��  ��] sec		: �^�C���A�E�g����܂ł̎��ԁi�b�j
 * [�o  ��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [��  �l]
 * (a) �f�[�^�ǂݍ��ݎ��Asec �Őݒ肵�����Ԃ��o�߂��Ă��f�[�^��
 *     �ǂݍ��܂�Ȃ��ꍇ�AROFS_ERR_TOUT ����������B
 * (b) �{�֐��Őݒ肵���҂����Ԃ́A�{�����[���̓o�^����f�B���N�g��
 *     �̓ǂݍ��݂ȂǁA�������A�̓ǂݍ��݂̏ꍇ�ɓK�p�����B
 * (c) �f�t�H���g�ł͖�T�b�ɐݒ肳��Ă���B
 */
/* Setup of the waiting timeout for data reading
 * [Inputs ] sec	: Time until it carries out a timeout (second)
 * [outputs] none
 * [Return ] none
 * [Notes  ] 
 * (a) At the time of data reading, even if the time set up by sec passes,
 *     when data is not read, ROFS_ERR_TOUT occurs. 
 * (b) In readings of a completion return, such as registration of volume,
 *     and a directory, the waiting time set up with this function is applied.
 * (c) Default is about 5 seconds.
 */
void CRIAPI		ROFS_SetTimeoutTime(CriUint32 sec);

/* ��������̐ݒ� 
 * [��  ��] rofs		: ROFS�n���h���iNULL = �S�́j
 *			condid		: �������ID
 *			val			: �ݒ�l
 * [�o  ��] �Ȃ�
 * [�֐��l] �G���[
 * [��  �l]
 */
/* Set operating condition
 * [Inputs ] rofs		: ROFS handle
 *			 condid		: operating condition id
 *			 val		: parameter
 * [outputs] none
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI	ROFS_SetCond(Rofs rofs, RofsCondId condid, CriSint32 val);

/* ��������̎擾
 * [��  ��] rofs		: ROFS�n���h���iNULL = �S�́j
 *			condid		: �������ID
 * [�o  ��] val			: �ݒ�l
 * [�֐��l] �G���[
 * [��  �l]
 */
/* Get operating condition
 * [Inputs ] rofs		: ROFS handle
 *			 condid		: operating condition id
 * [outputs] val		: parameter
 * [Return ] error code
 * [Notes  ] 
 */
RofsErr CRIAPI	ROFS_GetCond(Rofs rofs, RofsCondId condid, CriSint32 *val);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CRI_ROFS_H_ */

/*****************************************************************************
 *      �Z�p���
 *		Technical information
 *****************************************************************************/
#if 0
(1) �f�B���N�g�����R�[�h���[�h
  �@�ʏ�{���C�u������CVM�t�@�C������f�B���N�g�����R�[�h���擾���܂��B
  �f�B���N�g�����R�[�h���[�h�� ROFS_PARM_DIRSRC_MEM �ɐݒ肷�邱�ƂŁA
  �O���Ɏ����Ƃ��\�ł��B
  �@���̃��[�h�ł̓��C�u������Ɨ̈��AddVolume��LoadDir���̃I�[�o�w�b�h
  �����炷���Ƃ��ł��܂��B

  �ݒ���@�j
    err = ROFS_SetCond(NULL, ROFS_COND_DIRSRC, ROFS_PARM_DIRSRC_MEM);

  ���ӎ����j
  (a) ���̃��[�h���g���ɂ́A���炩����ROFSBLD�Ńf�B���N�g���R�[�h�t�@�C����
      �o�͂��Ă����K�v������B
  (b) ROFS_Init�֐��̈����ARofsInitPara�\���̂�max_dirent�����o�͍Œ�R�K�v�B
  (c) ROFS_Init�֐��̒���ɐݒ肷�邱�ƁB
      �������邱�ƂŁAROFS_AddVolume�֐����s���Ƀ��[�g�f�B���N�g���̓ǂݍ���
      �����������Ȃ�̂Ń}�E���g�������Ȃ�B
  (d) ���̃��[�h�ł͈ȉ��̊֐����g���Ă͂Ȃ�Ȃ��B
      ROFS_ChangeDir
  (e) ���̃��[�h�ɂ͖߂��Ȃ��B

/*---------------------------------------------------------------------------*/

(1) Directory record mode
  Usualy this liblary get directory records fom CVM file.
  Supported that change directory with directory record file which is 
  created by rofsbld.exe.
  When AddVolume and LoadDir it can save the liblary work memory and 
  reduce process overhead.

  How to)
    err = ROFS_SetCond(NULL, ROFS_COND_DIRSRC, ROFS_PARM_DIRSRC_MEM);

  Note)
  (a) In order to use this mode, it is necessary to output directory lists
      with ROFSBLD beforehand.
  (b) The argument of a ROFS_Init function and the max_dirent member of a 
      RofsInitPara structure object are at least 3 necessities.
  (c) Please call immediately after a ROFS_Init function.
      By carrying out like this, since reading processing of a 
      root directory is lost at the time of ROFS_AddVolume function 
      execution, mount becomes quick.
  (d) Do not use the following functions.
      ROFS_ChangeDir
  (e) It cannot restore the original mode.
#endif

/* end of file */
