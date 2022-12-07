#ifndef	_CRI_ADXF_H_INCLUDED
#define	_CRI_ADXF_H_INCLUDED
/****************************************************************************/
/*																			*/
/*			$title$ �`�c�w�t�@�C���V�X�e�� ���C�u����						*/
/*				ADXF (ADX File System) Library								*/
/*																			*/
/*				1998.7.3		written by S.Hosaka							*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*      �C���N���[�h�t�@�C��												*/
/*      Include file														*/
/****************************************************************************/

#include "cri_xpt.h"
#include "sj.h"

/****************************************************************************/
/*		�萔�}�N��															*/
/*		MACRO CONSTANT														*/
/****************************************************************************/

/*	Version number of ADX File System	*/
#define	ADXF_VER				"7.71"

/*	�t�@�C�����̕�����				*/
/*	Maximum number of file name strings	*/
#define ADXF_FNAME_MAX    		(256)

/*	�ő�p�[�e�B�V������				*/
/*	Maximum number of partition			*/
#define	ADXF_PART_MAX			(256)

/*	�ő�t�@�C����						*/
/*	Maximum number of file				*/
#define	ADXF_FILE_MAX			(65536)

/*	ADXF�n���h���I�u�W�F�N�g�̌�		*/
/*	Maximum number of ADXF handle		*/
#define	ADXF_OBJ_MAX			(16)

/*	�n���h���̏��						*/
/*	Status of handle					*/
#define	ADXF_STAT_STOP			(1)			/*	��~��						*/
											/*	During standstill			*/
#define ADXF_STAT_READING		(2)			/*	�f�[�^�ǂݍ��ݒ�			*/
											/*	During data read-in			*/
#define ADXF_STAT_READEND		(3)			/*	�f�[�^�ǂݍ��ݏI��			*/
											/*	Data read-in end			*/
#define ADXF_STAT_ERROR			(4)			/*	�ǂݍ��݃G���[����			*/
											/*	Read-in error outbreak state*/

/*	�V�[�N�^�C�v						*/
/*	Type of seek						*/
#define	ADXF_SEEK_SET			(0)			/*	�t�@�C���̐擪				*/
											/*	The beginning of file		*/
#define ADXF_SEEK_CUR			(1)			/*	���݂̈ʒu					*/
											/*	The current position		*/
#define ADXF_SEEK_END			(2)			/*	�t�@�C���̏I�[				*/
											/*	The end of file				*/

/*	�G���[�R�[�h						*/
/*	Error code							*/
#define	ADXF_ERR_OK				(0)			/*	����I��					*/
											/*	Successful					*/
#define	ADXF_ERR_FATAL			(-1)		/*	�O���̃G���[				*/
											/*	Fatal error					*/
#define	ADXF_ERR_INTERNAL		(-2)		/*	�����̃G���[				*/
											/*	Internal error				*/
#define	ADXF_ERR_PRM			(-3)		/*	�s���ȃp�����[�^			*/
											/*	Illegal parameter			*/
#define	ADXF_ERR_AFS_FILE		(-4)		/*	�s����AFS�t�@�C��			*/
											/*	Illegal AFS file			*/
#define ADXF_ERR_FSIZE			(-5)		/*	�t�@�C���T�C�Y���擾		*/
											/*	Cannot get file size yet	*/

/*	����l								*/
/*	Default value						*/
#define	ADXF_DEF_SCT_SIZE		(2048)		/*	CD-ROM�̃Z�N�^�T�C�Y		*/
											/*	Sector size in CD-ROM		*/
#ifdef __EE__
#define	ADXF_DEF_DMA_ALIGN		(64)		/*	DMA�̃A���C�����g			*/
											/*	Alignment of DMA transfer	*/
#else
#define	ADXF_DEF_DMA_ALIGN		(32)		/*	DMA�̃A���C�����g			*/
											/*	Alignment of DMA transfer	*/
#endif

#define	ADXF_DEF_REQ_RD_SCT		(512)		/*	���[�h���N�G�X�g�T�C�Y		*/
											/*	read request size			*/

/*	�f�o�b�O�p	*/
/*	for Debug	*/
//#define	ADXF_CMD_HSTRY_MAX		(256)		/*	�R�}���h�����̌�			*/
#define	ADXF_CMD_HSTRY_MAX		(16)		/*	�R�}���h�����̌�			*/
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

/* ���o�[�W�����Ƃ̌݊��p */
/* For compatibility with old version */
#define ADXF_GetPtStatEx		ADXF_GetPtStat
/****************************************************************************/
/*		�񋓒萔															*/
/*		Enumarate Constants													*/
/****************************************************************************/

/****************************************************************************/
/*		�����}�N��															*/
/*		Process MACRO														*/
/****************************************************************************/

/*	�p�[�e�B�V�������ɕK�v�ȗ̈�̌v�Z						*/
/*	Calculation of necessary area for partition information		*/
//	"6"�́Aadxf_ptinfo�̃����o�[��(adxf_ptinfo+size+nfile+nentry+type+rev+curdir+ofst)
#define	ADXF_PTIF_CMN_SZ				((6*sizeof(CriSint32))+(ADXF_FNAME_MAX*sizeof(CriSint8)))
//	���ۂ̃t�@�C���Ǘ��̈�T�C�Y
#define	ADXF_CALC_PTINFO_REAL_SIZE(n)	(ADXF_PTIF_CMN_SZ+(((n)+1)*sizeof(CriSint16)))
//	�S�o�C�g�A���C�����g�ɒ���
#define	ADXF_CALC_PTINFO_SIZE(n)		((ADXF_CALC_PTINFO_REAL_SIZE(n)+sizeof(CriSint16))/4*4)

//	�t�@�C���Ǘ��̈�T�C�Y�ύX(�P�t�@�C��������Q�o�C�g���S�o�C�g)
//	�R�����̈�T�C�Y�͋���
//	���ۂ̃t�@�C���Ǘ��̈�T�C�Y
#define	ADXF_CALC_PTINFO_FMGL_REAL_SIZE(n)	(ADXF_PTIF_CMN_SZ+(((n)+1)*sizeof(CriSint32)))
//	�S�o�C�g�A���C�����g�ɒ���
#define	ADXF_CALC_PTINFO_FMGL_SIZE(n)		((ADXF_CALC_PTINFO_FMGL_REAL_SIZE(n)+sizeof(CriSint32))/4*4)

#define	ADXF_CALC_ADD_PTINFO_SIZE(n)	(ADXF_PTIF_CMN_SZ+((n)*(2*sizeof(CriSint16)+sizeof(CriSint32))))

/*	���[�h�p�[�e�B�V�����p�e���|�����o�b�t�@�̃T�C�Y�v�Z	*/
/*	Calculation of temporary buffer size for load-partition	*/
#define	ADXF_CALC_TMPBUF_SIZE(n)	((((n+1)*8)+(ADXF_DEF_SCT_SIZE-1))/ADXF_DEF_SCT_SIZE*ADXF_DEF_SCT_SIZE)


/****************************************************************************/
/*		�f�[�^�^															*/
/*      Data type declaration												*/
/****************************************************************************/

#ifndef ADXSTM_DEFINED
#define ADXSTM_DEFINED
typedef void *ADXSTM;
#endif

/*	ADXF�n���h��		*/
/*	ADXF handle			*/
#ifndef ADXF_DEFINED
#define	ADXF_DEFINED
typedef	struct _adx_fs {
	CriSint8	used;					/*	�g�p�����ۂ�						*/
	CriSint8	stat;					/*	�n���h���̏��						*/
	CriSint8	sjflag;					/*	�O������ݒ肳�ꂽSJ���ۂ�			*/
	CriSint8	stopnw_flg;				/*	��~�v���t���O						*/
	ADXSTM	stm;					/*	ADXSTM�n���h��						*/
	SJ		sj;						/*	�X�g���[���W���C���g				*/
	CriSint32	fnsct;					/*	�t�@�C���T�C�Y(�Z�N�^)				*/
	CriSint32	skpos;					/*	�A�N�Z�X�|�C���^�̈ʒu				*/
	CriSint32	rdstpos;				/*	�ǂݍ��݊J�n�ʒu					*/
	CriSint32	rqsct;					/*	�ǂݍ��ݗv���f�[�^��(�Z�N�^)		*/
	CriSint32	rdsct;					/*	�ǂݍ��񂾃f�[�^��(�Z�N�^)			*/
	CriSint8	*buf;					/*	�o�b�t�@�̃A�h���X(�o�C�g)			*/
	CriSint32	bsize;					/*	�o�b�t�@�̃T�C�Y(�o�C�g)			*/
	CriSint32	rqrdsct;				/*	���ۂɓǂݍ��ݗv��������(�Z�N�^)	*/
	/*	ADXSTM_OpenRange -> ADXSTM_OpenFileRange(1999.8.19)	*/
	CriSint32	ofst;					/*	�ǂݍ��݃t�@�C���ւ̃I�t�Z�b�g		*/
	void	*dir;
} ADX_FS;
typedef	ADX_FS	*ADXF;
#endif

/*	�p�[�e�B�V�������		*/
/*	Partition information	*/
#ifndef ADXF_PTINFO_DEFINED
#define ADXF_PTINFO_DEFINED
typedef struct _adxf_ptinfo {
	struct _adxf_ptinfo	*next;		/*	���̃p�[�e�B�V����(NULL:�I���W�i��)	*/
	CriSint32				size;		/*	�p�[�e�B�V�������̈�T�C�Y		*/
	CriSint32				nfile;		/*	�p�[�e�B�V�������̃t�@�C����		*/
	CriUint16				nentry;		/*	�p�[�e�B�V�������̓o�^�t�@�C����	*/
	CriSint8				type;		/*	�I���W�i�����ǋL���̃t���O			*/
//	2002.11.05	S.H.	�t�@�C���Ǘ��̈�T�C�Y�S�o�C�g�Ή�
//	CriSint8				rev;		/*	�\��̈�							*/
	CriSint8				fmg_type;	/*	�t�@�C���Ǘ��T�C�Y(0=2byte,1=4byte)	*/
	/*	ADXSTM_OpenRange -> ADXSTM_OpenFileRange(1999.8.19)	*/
	CriSint8				fname[ADXF_FNAME_MAX];	/*	AFS�t�@�C����			*/
	void				*curdir;	/*	�J�����g�f�B���N�g��(NULL:�w��Ȃ�)	*/
	CriSint32				ofst;		/*	����qAFS�t�@�C���p					*/
	CriSint32				top;		/*	�t�@�C�����̐擪					*/
} ADXF_PTINFO;
#endif

/*	�ǋL�t�@�C���̏��				*/
/*	Information of postscript file	*/
#ifndef ADXF_ADD_DEFINED
#define ADXF_ADD_DEFINED
typedef struct _adxf_add_info {
	CriUint16	flid;			/*	�t�@�C��ID							*/
	CriUint16	fnsct;			/*	�t�@�C���T�C�Y						*/
	CriSint32	ofst;			/*	�I�t�Z�b�g							*/
} ADXF_ADD_INFO;
#endif

#ifndef ADXF_CMD_HSTRY_DEFINED
#define ADXF_CMD_HSTRY_DEFINED
typedef struct _adxf_cmd_hstry {
	CriUint8	cmdid;			/*	�R�}���h(�֐�)ID					*/
	CriUint8	fg;				/*	�֐��̓�������o�����������t���O	*/
	CriUint16	ncall;			/*	�R�}���h�̌Ăяo����				*/
	CriSint32	prm[3];					/*	�R�}���h�p�����[�^					*/
} ADXF_CMD_HSTRY;
#endif

/****************************************************************************/
/*		�ϐ��錾															*/
/*		Variable Declaration												*/
/****************************************************************************/

/****************************************************************************/
/*		�֐��̐錾															*/
/*      Function Declaration												*/
/****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================================
		���C�u�����̏������ƏI������
 		Initialize and Finalize of Library
  ==========================================================================*/

/*  ���C�u�����̏�����
 * [���@��] void ADXF_Init( void );
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ���C�u����������������B
 *  Initialization of library
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Initializes library.
 */
void CRIAPI ADXF_Init(void);

/* ���C�u�����̏I������
 * [���@��] void ADXF_Finish(void);
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ���C�u�����̏I������������B
 *  Termination of library
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Finalizes library.
 */
void CRIAPI ADXF_Finish(void);

/*  �������A�^�p�[�e�B�V�����̃��[�h
 * [���@��] CriSint32 ADXF_LoadPartitionNw(CriSint32 ptid, const CriChar8 *fname, 
 * 													void *dir, void *ptinfo);
 * [���@��] ptid	:�p�[�e�B�V����ID
 *			fname	:AFS�t�@�C���̖��O
 *			dir		:�f�B���N�g����� (GDFS:GDFS_DIRREC)
 *			ptinfo	:�p�[�e�B�V�������ǂݍ��ݗ̈�
 * [�o�@��] �Ȃ�
 * [�֐��l] �G���[�R�[�h
 * [�@�@�\] AFS�t�@�C����ǂݍ���ŁA�p�[�e�B�V����ID�ɐݒ肷��B
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

/*  �������A�^�p�[�e�B�V�����̃��[�h�i�͈͎w��j
 * [���@��] CriSint32 ADXF_LoadPartitionRangeNw(CriSint32 ptid, const CriChar8 *fname, void *dir,
 * 				CriSint32 offset_sct, CriSint32 range_sct, void *ptinfo);
 * [���@��] ptid		:�p�[�e�B�V����ID
 *			fname		:AFS�t�@�C���̖��O
 *			dir			:�f�B���N�g����� (GDFS:GDFS_DIRREC)
 *			offset_sct	: �t�@�C�����̃I�t�Z�b�g�i�Z�N�^�P�ʁj
 *			range_sct	: �͈́i�Z�N�^�P�ʁj
 *			ptinfo		:�p�[�e�B�V�������ǂݍ��ݗ̈�
 * [�o�@��] �Ȃ�
 * [�֐��l] �G���[�R�[�h
 * [�@�@�\] AFS�t�@�C����ǂݍ���ŁA�p�[�e�B�V����ID�ɐݒ肷��B
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

/*  �������A�^�p�[�e�B�V�����̃��[�h(����qAFS�t�@�C��)
 * [���@��] CriSint32 ADXF_LoadPartitionFromPtNw(CriSint32 set_ptid, CriSint32 rd_ptid, 
 *											CriSint32 rd_flid, void *ptinfo);
 * [���@��] set_ptid	:���[�h����AFS�t�@�C���ɑ΂��Đݒ肷��p�[�e�B�V����ID
 *			rd_ptid		:�ǂݍ���AFS�t�@�C���̃p�[�e�B�V����ID
 *			rd_flid		:�ǂݍ���AFS�t�@�C���̃t�@�C��ID
 *			ptinfo		:�p�[�e�B�V�������ǂݍ��ݗ̈�
 * [�o�@��] �Ȃ�
 * [�֐��l] �G���[�R�[�h
 * [�@�@�\] AFS�t�@�C������AFS�t�@�C����ǂݍ���ŁA�p�[�e�B�V����ID�ɐݒ肷��B
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

/*  �������A�^�p�[�e�B�V�����̃��[�h(�ǂݍ��݃o�b�t�@���w�肷��)
 * [���@��] CriSint32 ADXF_LoadPtNwEx(CriSint32 ptid, const CriChar8 *fname, void *dir, void *ptinfo, 
 *									void *tmpbuf, CriSint32 tbsize);
 * [���@��] ptid	:�p�[�e�B�V����ID
 *			fname	:AFS�t�@�C���̖��O
 *			dir		:�f�B���N�g����� (GDFS:GDFS_DIRREC)
 *			ptinfo	:�p�[�e�B�V�������ǂݍ��ݗ̈�
 *			tmpbuf	:AFS�t�@�C���w�b�_�ǂݍ��݃o�b�t�@
 *			tbsize	:AFS�t�@�C���w�b�_�ǂݍ��݃o�b�t�@�T�C�Y
 * [�o�@��] �Ȃ�
 * [�֐��l] �G���[�R�[�h
 * [�@�@�\] AFS�t�@�C����ǂݍ���ŁA�p�[�e�B�V����ID�ɐݒ肷��B
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

/*  �������A�^�p�[�e�B�V�����̃��[�h(����qAFS�t�@�C��)(�ǂݍ��݃o�b�t�@���w�肷��ꍇ)
 * [���@��] CriSint32 ADXF_LoadPtFromPtNwEx(CriSint32 set_ptid, CriSint32 rd_ptid, CriSint32 rd_flid, 
 *										void *ptinfo, void *tmpbuf, CriSint32 tbsize);
 * [���@��] set_ptid	:���[�h����AFS�t�@�C���ɑ΂��Đݒ肷��p�[�e�B�V����ID
 *			rd_ptid		:�ǂݍ���AFS�t�@�C���̃p�[�e�B�V����ID
 *			rd_flid		:�ǂݍ���AFS�t�@�C���̃t�@�C��ID
 *			ptinfo		:�p�[�e�B�V�������ǂݍ��ݗ̈�
 *			tmpbuf		:AFS�t�@�C���w�b�_�ǂݍ��݃o�b�t�@
 *			tbsize		:AFS�t�@�C���w�b�_�ǂݍ��݃o�b�t�@�T�C�Y
 * [�o�@��] �Ȃ�
 * [�֐��l] �G���[�R�[�h
 * [�@�@�\] AFS�t�@�C������ǂݍ���ŁA�p�[�e�B�V����ID�ɐݒ肷��B
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

/*  �p�[�e�B�V�������̓ǂݍ��ݏ�Ԃ̎擾
 * [���@��] CriSint32 ADXF_GetPtStat(CriSint32 ptid);
 * [���@��] ptid	:�p�[�e�B�V����ID
 * [�o�@��] �Ȃ�
 * [�֐��l] �p�[�e�B�V�������̓ǂݍ��ݏ��
 * [�@�@�\] AFS�t�@�C������ǂݍ���ŁA�p�[�e�B�V����ID�ɐݒ肷��B
 *  Get partition loading state
 * [Inputs  ] ptid   :Partition ID
 * [Outputs ] None
 * [Return  ] Partition loading state
 * [Function] Gets partition loading state.
 */
CriSint32 CRIAPI ADXF_GetPtStat(CriSint32 ptid);

/*  �p�[�e�B�V�������ǂݍ��݂̒��~
 * [���@��] void ADXF_StopPtLd(void);
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �p�[�e�B�V�������̓ǂݍ��݂𒆎~����B
 *  Stop to load partition information
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Stops to load partition information.
 */
void CRIAPI ADXF_StopPtLd(void);

/*  �p�[�e�B�V�������T�C�Y�̎擾
 * [���@��] CriSint32 ADXF_GetPtinfoSize(CriSint32 ptid);
 * [���@��] ptid	:�p�[�e�B�V����ID
 * [�o�@��] �Ȃ�
 * [�֐��l] �p�[�e�B�V�������T�C�Y(�P�ʁF�o�C�g)
 * [�@�@�\] �p�[�e�B�V�������̈�̃T�C�Y���擾����B
 *  Get size of partition information
 * [Inputs  ] ptid   :Partition ID
 * [Outputs ] None
 * [Return  ] Size of the partition information(byte)
 * [Function] Gets size of the partition information.
 */
CriSint32 CRIAPI ADXF_GetPtinfoSize(CriSint32 ptid);

/* �p�[�e�B�V�������̎擾 */
CriChar8 * CRIAPI ADXF_GetFnameFromPt(CriSint32 ptid);

/* �p�[�e�B�V�������̃t�@�C�����̎擾 */
CriSint32 CRIAPI ADXF_GetNumFilesFromPt(CriSint32 ptid);

/* �p�[�e�B�V�������̃t�@�C���T�C�Y�̎擾 */
CriSint32 CRIAPI ADXF_GetFileSizeFromPt(CriSint32 ptid, CriSint32 fid);

/* AFS�t�@�C���̃t�@�C�����̎擾(AFS�t�@�C���̓ǂݍ��݂̓A�v���ōs��) */
CriSint32 CRIAPI ADXF_GetNumFilesFromAfs(void *buf);

/*==========================================================================
		�t�@�C���̓ǂݍ���
		Read file
  ==========================================================================*/

/*	�t�@�C���̃I�[�v��
 * [���@��] ADXF ADXF_Open(const CriChar8 *fname, void *atr);
 * [���@��] fname	: �t�@�C����
 *			atr		: �t�@�C���̑���
 * [�o�@��] �Ȃ�
 * [�֐��l] ADXF�n���h���A�G���[�̏ꍇNULL
 * [�@�@�\] �w��̃t�@�C�����I�[�v�����AADXF�n���h����Ԃ��B
 *  Open file
 * [Inputs  ] fname  : File name
 *			  atr    : File attribute
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Opens a file.
 */
ADXF CRIAPI ADXF_Open(const CriChar8 *fname, void *atr);

/*	�t�@�C���̃I�[�v���i�񓯊��j
 * [���@��] ADXF ADXF_OpenNw(const CriChar8 *fname, void *atr);
 * [���@��] fname	: �t�@�C����
 *			atr		: �t�@�C���̑���
 * [�o�@��] �Ȃ�
 * [�֐��l] ADXF�n���h���A�G���[�̏ꍇNULL
 * [�@�@�\] �w��t�@�C���̃I�[�v�����J�n���AADXF�n���h����Ԃ��B
 *  Begins the file open
 * [Inputs  ] fname  : File name
 *			  atr    : File attribute
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Begins the file open.
 */
ADXF CRIAPI ADXF_OpenNw(const CriChar8 *fname, void *atr);

/*	�t�@�C���̃I�[�v����Ԃ��`�F�b�N
 * [���@��] CriBool ADXF_IsOpened(ADXF adxf);
 * [���@��] adxf : ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �I�[�v����ԁiTRUE = �I�v�[�v������Ă���AFALSE = �I�[�v���������j
 * [�@�@�\] �t�@�C�����I�[�v������Ă��邩�ǂ������`�F�b�N����B
 *  Checks the open status
 * [Inputs  ] adxf : ADXF handle
 * [Outputs ] None
 * [Return  ] Open status (TRUE = opened, FALSE = not opened)
 * [Function] Checks whether the file was opened.
 */
CriBool CRIAPI ADXF_IsOpened(ADXF adxf);

/*	�t�@�C���̃I�[�v���i�Z�N�^�͈͎w��t���j
 * [���@��] ADXF ADXF_OpenRange(const CriChar8 *fname, void *atr, CriSint32 offset_sct, CriSint32 range_sct);
 * [���@��] fname		: �t�@�C����
 *			atr			: �t�@�C���̑���
 *			offset_sct	: �t�@�C�����̃I�t�Z�b�g�i�Z�N�^�P�ʁj
 *			range_sct	: �͈́i�Z�N�^�P�ʁj
 * [�o�@��] �Ȃ�
 * [�֐��l] ADXF�n���h���A�G���[�̏ꍇNULL
 * [�@�@�\] �w��̃t�@�C�����I�[�v�����AADXF�n���h����Ԃ��B
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

/*	�t�@�C���̃I�[�v��(AFS�t�H�[�}�b�g)
 * [���@��] ADXF ADXF_OpenAfs(CriSint32 ptid, CriSint32 flid);
 * [���@��] ptid	: �p�[�e�B�V����ID
 *			flid	: �t�@�C��ID
 * [�o�@��] �Ȃ�
 * [�֐��l] ADXF�n���h���A�G���[�̏ꍇNULL
 * [�@�@�\] �p�[�e�B�V����ID�ƃt�@�C��ID�Ŏw�肳�ꂽAFS�t�@�C�����I�[�v�����A
 *			ADXF�n���h����Ԃ��B
 *  Open file(AFS format)
 * [Inputs  ] ptid   : Partition ID
 *			  flid   : File ID
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Opens a file of AFS format.
 */
ADXF CRIAPI ADXF_OpenAfs(CriSint32 ptid, CriSint32 flid);

/*	�t�@�C���̃I�[�v���iAFS�t�H�[�}�b�g�A�񓯊��j
 * [���@��] ADXF ADXF_OpenAfsNw(CriSint32 ptid, CriSint32 flid);
 * [���@��] ptid	: �p�[�e�B�V����ID
 *			flid	: �t�@�C��ID
 * [�o�@��] �Ȃ�
 * [�֐��l] ADXF�n���h���A�G���[�̏ꍇNULL
 * [�@�@�\] �p�[�e�B�V����ID�ƃt�@�C��ID�Ŏw�肳�ꂽAFS�t�@�C���̃I�[�v�����J�n����B
 *  Begins the file open (AFS format)
 * [Inputs  ] ptid   : Partition ID
 *			  flid   : File ID
 * [Outputs ] None
 * [Return  ] ADXF handle
 *			  (If the operation was unsuccessful, this function returns NULL.)
 * [Function] Begins the AFS file open.
 */
ADXF CRIAPI ADXF_OpenAfsNw(CriSint32 ptid, CriSint32 flid);

/* �t�@�C���̃N���[�Y
 * [���@��] void ADXF_Close(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] �w�肳�ꂽADXF�n���h���̃t�@�C�����N���[�Y����B
 *  Close file
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] None
 * [Function] Closes a file of specific ADXF handle.
 */
void CRIAPI ADXF_Close(ADXF adxf);

/* �S�Ẵt�@�C���N���[�Y
 * [���@��] void ADXF_CloseAll(void);
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ADXF�n���h���̃t�@�C����S�ăN���[�Y����B
 *  Close file
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Closes all file of ADXF handle.
 */
void CRIAPI ADXF_CloseAll(void);

/*  SJ�ւ̃f�[�^�ǂݍ��݊J�n
 * [���@��] void ADXF_ReadSj(ADXF adxf, CriSint32 nsct, SJ sj);
 * [���@��] adxf	: ADXF�n���h��
 *			nsct	: �ǂݍ��݃f�[�^�̗�(�P�ʁF�Z�N�^)
 *			SJ		: �X�g���[���W���C���g
 * [�o�@��] �Ȃ�
 * [�֐��l] �ǂݍ��݃f�[�^��(�P�ʁF�Z�N�^)
 * [�@�@�\] �X�g���[���W���C���g�Ƀf�[�^�ǂݍ��݂̃��N�G�X�g�𔭍s����B
 *			�X�g���[���W���C���g�̃o�b�t�@�T�C�Y�́A�Z�N�^�̐����{�łȂ����
 *			�Ȃ�Ȃ��B
 *			�X�g���[���W���C���g���烆�[�U���f�[�^��ǂݏo���ƁA�����I�ɃX�g���[
 *			���W���C���g�Ƀf�[�^���ǂݍ��܂��B
 *			���N�G�X�g�����A�N�Z�X���삪��������ƁA�A�N�Z�X�|�C���^��
 *			nsct�Z�N�^�i�ށB
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

/* �f�[�^�̓ǂݍ��݊J�n
 * [���@��] void ADXF_ReadNw(ADXF adxf, CriSint32 nsct, void *buf);
 * [���@��] adxf	: ADXF�n���h��
 *			nsct	: �ǂݍ��݃f�[�^�̗�(�P�ʁF�Z�N�^)
 *			buf		: �ǂݍ��ݗ̈�
 * [�o�@��] �Ȃ�
 * [�֐��l] �ǂݍ��݃f�[�^��(�P�ʁF�Z�N�^)
 * [�@�@�\] �f�[�^�ǂݍ��݂̃��N�G�X�g�𔭍s����B
 *			���N�G�X�g�����A�N�Z�X���삪��������ƁA�A�N�Z�X�|�C���^��
 *			nsct�Z�N�^�i�ށB
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

/* �f�[�^�̓ǂݍ��ݒ��~
 * [���@��] CriSint32 ADXF_Stop(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] ���~�������_�ł̃A�N�Z�X�|�C���^�̈ʒu
 * [�@�@�\] �f�[�^�̓ǂݍ��݂𒆎~����B
 *  Read-in stop
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] The location of access pointer when you stop to read
 * [Function] Stops to read data.
 */
CriSint32 CRIAPI ADXF_Stop(ADXF adxf);

/* �f�[�^�̓ǂݍ��ݒ��~ (�������A)
 * [���@��] CriSint32 ADXF_Stop(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] ���~�����N�G�X�g���_�ł̃A�N�Z�X�|�C���^�̈ʒu
 * [�@�@�\] �f�[�^�̓ǂݍ��݂𒆎~�����N�G�X�g����B
 *			��Ԃ͂����ɂ͕ς炸�A�ǂݍ��݂��~�����Ƃ���STOP�ɕω�����
 *  Read-in stop
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] The location of access pointer when you request to stop
 * [Function] Request to Stop.
 */
CriSint32 CRIAPI ADXF_StopNw(ADXF adxf);

/* �T�[�o�֐�
 * [���@��] void ADXF_ExecServer(void);
 * [���@��] �Ȃ�
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ������Ԃ��X�V����B
 *  Server function
 * [Inputs  ] None
 * [Outputs ] None
 * [Return  ] None
 * [Function] Update the inside status of library.
 */
void CRIAPI ADXF_ExecServer(void);

/*==========================================================================
		�A�N�Z�X�|�C���^�̐���
		Control access pointer
  ==========================================================================*/

/*  �A�N�Z�X�|�C���^�̈ړ�
 * [���@��] CriSint32 ADXF_Seek(ADXF adxf, CriSint32 pos, CriSint32 type);
 * [���@��] adxf	: ADXF�n���h��
 *			pos		: �A�N�Z�X�|�C���^�̈ړ���(�P�ʁF�Z�N�^)
 *			type	: �ړ��(�V�[�N�^�C�v�FADXF_SEEK_�`)
 * [�o�@��] �Ȃ�
 * [�֐��l] �A�N�Z�X�|�C���^�̈ʒu�A���̒l�̓G���[�R�[�h
 * [�@�@�\] �A�N�Z�X�|�C���^��type����pos�Z�N�^���ꂽ�ʒu�Ɉړ�������B
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

/*  �A�N�Z�X�|�C���^�̎擾
 * [���@��] CriSint32 ADXF_Tell(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �A�N�Z�X�|�C���^�̈ʒu
 * [�@�@�\] �A�N�Z�X�|�C���^�̈ʒu���擾����B
 *  Get the access pointer
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] Access pointer
 * [Function] Gets the access pointer.
 */
CriSint32 CRIAPI ADXF_Tell(ADXF adxf);

/*==========================================================================
		���̎擾
		Get information
  ==========================================================================*/

/*  �t�@�C���T�C�Y�̎擾
 * [���@��] CriSint32 ADXF_GetFsizeSct(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �t�@�C���T�C�Y(�P�ʁF�Z�N�^)
 * [�@�@�\] �w�肳�ꂽ�t�@�C���̃T�C�Y���Z�N�^�P�ʂŎ擾����B
 *  Get file size
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] File size(sector)
 * [Function] Obtains the size of the specified file.
 */
CriSint32 CRIAPI ADXF_GetFsizeSct(ADXF adxf);

/*  �t�@�C���T�C�Y�̎擾
 * [���@��] CriSint32 ADXF_GetFsizeByte(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �t�@�C���T�C�Y(�P�ʁF�o�C�g)
 * [�@�@�\] �w�肳�ꂽ�t�@�C���̃T�C�Y���o�C�g�P�ʂŎ擾����B
 *  Get file size
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] File size(byte)
 * [Function] Obtains the size of the specified file.
 */
CriSint32 CRIAPI ADXF_GetFsizeByte(ADXF adxf);
CriSint64 CRIAPI ADXF_GetFsizeByte64(ADXF adxf);

/*  �ǂݍ��ݗv�����̎擾
 * [���@��] CriSint32 ADXF_GetNumReqSct(ADXF adxf, CriSint32 *seekpos);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] seekpos	: �ǂݍ��݈ʒu
 * [�֐��l] �v�������ǂݍ��݃f�[�^��(�P�ʁF�Z�N�^)
 * [�@�@�\] �v�������ǂݍ��݈ʒu�ƃf�[�^�ʂ��擾����B
 *  Get request information
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] seekpos: location of start to read
 * [Return  ] Request data size(sector)
 * [Function] Obtains read-in start position and the read-in size that 
 *			  demanded.
 */
CriSint32 CRIAPI ADXF_GetNumReqSct(ADXF adxf, CriSint32 *seekpos);

/*  ���ۂɓǂݍ��񂾃Z�N�^���̎擾
 * [���@��] CriSint32 ADXF_GetNumReadSct(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] �ǂݍ��񂾃f�[�^��(�P�ʁF�Z�N�^)
 * [�@�@�\] ���ۂɓǂݍ��񂾃f�[�^�ʂ��擾����B
 *  Get the number of sectors read
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] Size of read data(sector)
 * [Function] Obtains the size of data that read actually.
 */
CriSint32 CRIAPI ADXF_GetNumReadSct(ADXF adxf);

/*  �n���h���̏�Ԃ̎擾
 * [���@��] CriSint32 ADXF_GetStat(ADXF adxf);
 * [���@��] adxf	: ADXF�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] ADXF�n���h���̓������(��ԁFADXF_STAT_�`)
 * [�@�@�\] ADXF�n���h���̓�����Ԃ��擾����B
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

/*�@���[�h���N�G�X�g�̃T�C�Y�̐ݒ�
 * [���@��] void ADXF_SetReqRdSct(ADXF adxf, CriSint32 nsct);
 * [���@��] adxf    ; ADXF�n���h��
 *			nsct    ; �Z�N�^��
 * [�o�@��] �Ȃ�
 * [�֐��l] �Ȃ�
 * [�@�@�\] ���ɓǂݍ��ރT�C�Y��ݒ肵�܂��B
 *  Set size of read request
 * [Inputs  ] adxf   : ADXF handle
 *			  nsct   : size(sector)
 * [Outputs ] None
 * [Return  ] None
 * [Function] Sets size of read request(one time).
 */
void CRIAPI ADXF_SetReqRdSct(ADXF adxf, CriSint32 nsct);

/*	�t�@�C���ǂݍ��݃X�e�[�^�X�̎擾
 * [���@��] CriSint32 ADXT_GetStatRead(ADXT adxt);
 * [���@��] adxt	: ADXT�n���h��
 * [�o�@��] �Ȃ�
 * [�֐��l] 1=�ǂݍ��ݒ��A0=�ǂݍ��ݒ�~��
 * [�@�@�\] �ǂݍ��ݏ�Ԃ��擾����
 *  Get status of data reading
 * [Inputs  ] adxf   : ADXF handle
 * [Outputs ] None
 * [Return  ] 1=Data reading in progress,  0=Data reading stopped
 * [Function] Gets status of data reading
 */
CriSint32 CRIAPI ADXF_GetStatRead(ADXF adxf);

/*	�t�@�C���̗L���̊m�F
 * [��  ��] CriBool ADXF_IsExistFile(const CriChar8 *fname);
 * [��  ��] fname:�t�@�C����
 * [�o  ��] �Ȃ�
 * [�߂�l] 1=�t�@�C�������݂���A0=�t�@�C�������݂��Ȃ�
 * [��  ��] �w�肳�ꂽ�t�@�C�������݂��邩�ǂ����m�F����
 * [��  ��] �t�@�C����������Ȃ��ꍇ�ł��G���[�R�[���o�b�N�͔������Ȃ�
 *	Check of file existence
 * [Input   ] fname:file name
 * [Output  ] None
 * [Return  ] 1=file exist, 0=otherwise
 * [Function] Checks whether file exist
 * [Remarks ] This function does not return error callback even if file does not exist
 */
CriBool CRIAPI ADXF_IsExistFile(const CriChar8 *fname);

/*	�t�@�C���T�C�Y�̎擾
 * [��  ��] CriSint32 ADXF_GetFileSize(const Char *fname);
 * [��  ��] fname:�t�@�C����
 * [�o  ��] �Ȃ�
 * [�߂�l] �t�@�C���T�C�Y(�P�ʁF�o�C�g)
 * [��  ��] �w�肳�ꂽ�t�@�C�����̃t�@�C���T�C�Y���o�C�g�P�ʂŎ擾���܂��B
 *  Get file size
 * [Input   ] fname:file name
 * [Output  ] None
 * [Return  ] file size (unit : byte)
 * [Function] Gets file size specified file name
 */
CriSint32 CRIAPI ADXF_GetFileSize(const CriChar8 *fname);
CriSint64 CRIAPI ADXF_GetFileSize64(const CriChar8 *fname);

/*==========================================================================
		ROFS�p
		for ROFS
  ==========================================================================*/
#if 0

/* �ő啶���� */
#define ADXF_ROFS_VOLNAME_LEN		(8)		/* �{�����[�����̕����� */
#define ADXF_ROFS_VOLNAME_SIZ		(ADXF_ROFS_VOLNAME_LEN + 1)	/* ������T�C�Y */
#define ADXF_ROFS_FNAME_LEN			(31)	/* �t�@�C�����̕����� */
#define ADXF_ROFS_FNAME_SIZ			(ADXF_ROFS_FNAME_LEN + 1)	/* ������T�C�Y */
#define ADXF_ROFS_DIRNAME_LEN		(31)	/* �f�B���N�g�����̕����� */
#define ADXF_RPFS_DIRNAME_SIZ		(ADXF_ROFS_DIRNAME_LEN+1)	/* ������T�C�Y */

/* ���C�u������Ɨ̈�T�C�Y */
#define ADXF_CALC_LIBWORK_SIZ(max_open,max_volume,max_dirent) (sizeof(ADXF_ROFS_WK)+((max_open)+1)*sizeof(ADXF_ROFS_OBJ)+(ADXF_CALC_DIRREC_SIZ(max_dirent)+sizeof(ADXF_ROFS_VOL))*((max_volume)+1))

/* �f�B���N�g�����R�[�h�̈�T�C�Y */
#define ADXF_CALC_DIRREC_SIZ(n_dirent) (sizeof(ADXF_ROFS_DIRREC)-sizeof(ADXF_ROFS_DIRRECENT)+(n_dirent)*sizeof(ADXF_ROFS_DIRRECENT))

/* ���[�N�̈� */
typedef struct ADXF_ROFS_STWK		ADXF_ROFS_WK;

/* �v���~�e�B�u�֐� */
typedef struct ADXF_ROFS_PFSIFTBL	ADXF_ROFS_PFSTBL;

/* �f�o�C�X�R���g���[���֐� */
typedef struct ADXF_ROFS_DEVIFTBLE	ADXF_ROFS_DEVTBL;

/* ���C�u�����C���^�[�t�F�[�X */
typedef struct ADXF_ROFS_LIBIFTBLE	ADXF_ROFS_LIFTBL;

/* �f�B���N�g�����R�[�h�G���g�� */
typedef struct {
	CriSint32	fad;						/* fad */
	CriSint32	fsize;						/* file size */
	CriUint8	flag;						/* file attribute */
	CriUint8	sid;						/* filename search id */
	CriChar8	fname[ADXF_ROFS_FNAME_SIZ];		/* filename */
	CriUint8	pad[6];
} ADXF_ROFS_DIRRECENT;						/* 48 bytes */

/* �f�B���N�g�����R�[�h */
typedef struct {
	CriSint32	dir_num;					/* number of record */
	CriSint32	max_ent;					/* max entry of directory record */
	CriSint32	dir_fad;					/* fad of directory record */
	CriChar8	volname[ADXF_ROFS_FNAME_SIZ];	/* volume name */
	CriSint32	pad;						/* padding */
	ADXF_ROFS_DIRRECENT	dirrec_tbl[1];		/* record table */
} ADXF_ROFS_DIRREC;
typedef ADXF_ROFS_DIRREC	*ADXF_ROFS_DIRRECBUF;

/* �{�����[���Ǘ��\���� */
typedef struct {
	CriChar8	volname[ADXF_ROFS_VOLNAME_SIZ];	/* �{�����[���� */
	void	*img_hn;					/* �C���[�W�t�@�C���̃t�@�C���n���h�� */
	CriSint32	zisb;						/* ZONE0�C���[�W�f�[�^�J�n�ʒu */
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
	CriSint32		trns_seg;				/* �]���σu���b�N�P�� */
	CriSint32		trns_ofs;				/* �]���σo�C�g�P�� */
	CriSint32		trns_unit;				/* �]���P��(�u���b�N) */
	CriUint8		*rdadr;					/* �ǂݍ��݃A�h���X */
	CriSint16		used;					/* used flag */
	CriSint16		act;					/* handle act */
	CriSint16		stat;					/* handle status(ADXF_Stat) */
	CriSint16		err;					/* error status */
} ADXF_ROFS_OBJ;
typedef ADXF_ROFS_OBJ	*ADXF_ROFS;

/* ROFS Work Area */
struct ADXF_ROFS_STWK {
	CriBool			f_init;				/* �������t���O */
	CriSint32			max_open;			/* max open files */
	CriSint32			max_volume;
	CriSint32			max_dirent;
	CriUint32			exec_server_cnt;	/* counter */
	ADXF_ROFS			syshdl;				/* handle for system command */
	ADXF_ROFS			hndtbl;				/* handle */
	ADXF_ROFS_VOL		*vollist;			/* �{�����[����񃊃X�g */
	ADXF_ROFS_VOL		*curvol;			/* �J�����g�{�����[����� */
	ADXF_ROFS_DIRRECBUF	dirbuf;				/* �f�B���N�g�����R�[�h�̊J�n�ʒu */
	ADXF_ROFS_PFSTBL		*pfs;				/* �v���~�e�B�u�֐� */
	ADXF_ROFS_DEVTBL		*dev;				/* �f�o�C�X�R���g���[���֐� */
	ADXF_ROFS_LIFTBL			*liftbl;			/* ���C�u�����C���^�[�t�F�[�X */
	void	(*g_errcb)(void *, CriChar8 *, CriSint32);	/* error callback */
	void			*g_errcb_1st;		/* error callback 1st arg. */
	CriUint32	sctbuf[(ADXF_DEF_SCT_SIZE*2+64)/4];	/* sector buffer */
	ADXF_ROFS_OBJ		hndlist[1];			/* handle table */
}; /* 64 + 4160 + handles */


/* ROFS�Z�b�g�A�b�v�\����  */
#ifndef ADXF_SPRM_ROFS_DEFINED
#define ADXF_SPRM_ROFS_DEFINED
typedef struct {
	CriSint32	max_open;				/* �����ɃI�[�v���ł���ő�t�@�C���� */
	CriSint32	max_volume;				/* �����ɓo�^�ł���ő�{�����[���� */
	CriSint32	max_dirent;				/* �f�B���N�g���Ɋi�[����ő�t�@�C���� */
	void	*rofs_work;				/* ���C�u������Ɨ̈�̐擪�A�h���X */
} ADXF_SPRM_ROFS;
#endif

#endif

/* ROFS�̃Z�b�g�A�b�v	*/
/* Setup ROFS			*/
void CRIAPI ADXF_SetupRofs(void *sprmr);

/* ROFS�̃V���b�g�_�E�� */
/* Shutdown ROFS		*/
void CRIAPI ADXF_ShutdownRofs(void);

/* ROFS�{�����[���̒ǉ�(�X�N�����u���L�[�Ή�)	*/
/* Addition of ROFS volume(with Scramble key)	*/
/* ���̊֐��̎d�l�͕ύX����܂���								*/
/* �X�N�����u���L�[���g�p�̏ꍇ��scrkey��NULL���w�肵�Ă�������	*/ 
CriSint32 CRIAPI ADXF_AddRofsVol(CriChar8 *volname, CriChar8 *imgname, CriChar8 *scrkey);

/* ROFS�{�����[���̍폜 */
/* Delete ROFS volume	*/
void CRIAPI ADXF_DelRofsVol(CriChar8 *volname);

/* ROFS�{�����[�����ł��邩�ۂ� */
/* Check ROFS volume name		*/
CriBool CRIAPI ADXF_IsRofsVol(CriChar8 *volname);

/* �f�t�H���g�f�o�C�X�̐ݒ� */
/* Setting default device	*/
void CRIAPI ADXF_SetDefDev(CriChar8 *volname);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif	/* _CRI_ADXF_H_INCLUDED */
