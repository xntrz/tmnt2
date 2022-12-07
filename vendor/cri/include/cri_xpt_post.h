/*****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 1998-2008 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Middleware Library
 * Module   : CRI Common Header / Post-Process
 * File     : cri_xpt_post.h
 * Date     : 2008-02-22
 * Version  : 2.02
 *
 *****************************************************************************/
#ifndef CRI_XPT_POST_H
#define CRI_XPT_POST_H


/*****************************************************************************
 *      �}�N����`
 *      Macro definition 
 *****************************************************************************/

/* �R���p�C�����̐ÓI�A�T�[�g
 *   C++ Boost��STATIC_ASSERT�Ɠ����̋@�\��C����Ŏ�������B
 *   �������^�ł��邱�Ƃ����؂���B�U�̏ꍇ�̓R���p�C���G���[���N�����āA�R��
 *   �p�C�����~������B
 * Static Asart when compiling
 *   This function is equal to STATIC_ASSERT of C++ Boost, and is imprementated
 *   by C language. If the condition is false, a compiler error is caused and
 *   it stop compiling.
 */
#define XPT_STATIC_ASSERT(cond) extern int xpt_static_assert_array[(cond)?1:-1]


/*****************************************************************************
 *      �R���p�C���I�v�V�����̌���
 *      Compiler option test
 *****************************************************************************/

/* enum�^�̌���
 * �@enum�^��int�^�ɐ������邱�Ƃ����؂���B
 * �@�K�؂ȃG���[���b�Z�[�W��\���������̂ŁA�\�Ȃ�R���p�C���I�v�V�����ɂ��
 * �@�`�F�b�N���s���B�s�\�ȏꍇ�A�ėp�I�ȐÓI�A�T�[�g�ɂ��`�F�b�N�������B
 * Enum type test
 *   It is verified that the enum type adjusts to the int type. 
 *   To display an appropriate error message, it checks the compiler option 
 *   if possible. General static asart test works if it is impossible. 
 */
#if defined(__MWERKS__) /* CodeWarrior */
    //#if !__option(enumsalwaysint)
    //    #error ERROR cri_xpt_post.h : Compiler option 'Enums Always Int' should be ON.
    //#endif
#elif defined(__GNUC__)
    //#if Compiler option -fshort-enums is used
    //    #error ERROR cri_xpt_post.h : Compiler option '-fshort-enums' should not be used.
    //#endif
#endif
//typedef enum {XPT_TEST_ENUM_0, XPT_TEST_ENUM_1, XPT_TEST_ENUM_2} XptTestEnum;
//XPT_STATIC_ASSERT(sizeof(XptTestEnum) == sizeof(Uint32));	/* enum should be 4byte. */

/*****************************************************************************
 * �萔�}�N��
 * Macros of constant value
 *****************************************************************************/

/* NULL �|�C���^�^ */
#if !defined(CRI_NULL)
#ifdef __cplusplus
#define CRI_NULL	(0)
#else
#define CRI_NULL	((void *)0)
#endif
#endif

/* �_���萔�P�i�U�A�^�j */
#if !defined(CRI_FALSE)
#define CRI_FALSE	(0)
#endif
#if !defined(CRI_TRUE)
#define CRI_TRUE	(1)
#endif

/* �_���萔�Q�i�X�C�b�`�j */
#if !defined(CRI_OFF)
#define CRI_OFF		(0)
#endif
#if !defined(CRI_ON)
#define CRI_ON		(1)
#endif

/* ���ʔ���̒萔�i�����A���s�j */
#if !defined(CRI_OK)
#define CRI_OK		(0)			/* ���� */
#endif
#if !defined(NG)
#define CRI_NG		(-1)		/* ���s */
#endif

#if !defined(CRI_XPT_DISABLE_UNPREFIXED_TYPE)

/*****************************************************************************
 * �݊����ێ��̂��߂̒萔�}�N��
 * Macros for compatibility with old version
 *****************************************************************************/

/* NULL �|�C���^�^ */
#if !defined(NULL)
#define NULL		(CRI_NULL)
#endif

/* �_���萔�P�i�U�A�^�j */
#if !defined(FALSE)
#define FALSE		(CRI_FALSE)
#endif
#if !defined(TRUE)
#define TRUE		(CRI_TRUE)
#endif

/* �_���萔�Q�i�X�C�b�`�j */
#if !defined(OFF)
#define OFF			(CRI_OFF)
#endif
#if !defined(ON)
#define ON			(CRI_ON)
#endif

/* ���ʔ���̒萔�i�����A���s�j */
#if !defined(OK)
#define OK			(CRI_OK)
#endif
#if !defined(NG)
#define NG			(CRI_NG)
#endif

/*****************************************************************************
 * �݊����ێ��̂��߂̃f�[�^�^�錾
 * Types for compatibility with old version
 *****************************************************************************/

#if !defined(_TYPEDEF_Uint8)
#define _TYPEDEF_Uint8
typedef CriUint8				Uint8;		/* �����Ȃ��P�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Sint8)
#define _TYPEDEF_Sint8
typedef CriSint8				Sint8;		/* �������P�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Uint16)
#define _TYPEDEF_Uint16
typedef CriUint16				Uint16;		/* �����Ȃ��Q�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Sint16)
#define _TYPEDEF_Sint16
typedef CriSint16				Sint16;		/* �������Q�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Uint32)
#define _TYPEDEF_Uint32
typedef CriUint32				Uint32;		/* �����Ȃ��S�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Sint32)
#define _TYPEDEF_Sint32
typedef CriSint32				Sint32;		/* �������S�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Uint64)
#define _TYPEDEF_Uint64
typedef CriUint64				Uint64;		/* �����Ȃ��W�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Sint64)
#define _TYPEDEF_Sint64
typedef CriSint64				Sint64;		/* �������W�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Uint128)
#define _TYPEDEF_Uint128
typedef CriUint128				Uint128;	/* �����Ȃ�16�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Sint128)
#define _TYPEDEF_Sint128
typedef CriSint128				Sint128;	/* ������16�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Float16)
#define _TYPEDEF_Float16
typedef CriFloat16				Float16;	/* �Q�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Float32)
#define _TYPEDEF_Float32
typedef CriFloat32				Float32;	/* �S�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Float64)
#define _TYPEDEF_Float64
typedef CriFloat64				Float64;	/* �W�o�C�g���� */
#endif

#if !defined(_TYPEDEF_Fixed32)
#define _TYPEDEF_Fixed32
typedef CriFixed32				Fixed32;	/* �Œ菬���_32�r�b�g */
#endif

/* X11 API�ixtrapproto.h�j�Ƃ̋������p */
#if !defined(_TYPEDEF_Bool) && !defined(Bool)
#define _TYPEDEF_Bool
typedef CriBool					Bool;		/* �_���^�i�_���萔��l�ɂƂ�j */
#endif

#if !defined(_TYPEDEF_Char8)
#define _TYPEDEF_Char8
typedef CriChar8				Char8;		/* �����^ */
#endif

#if !defined(_TYPEDEF_SintPtr)
#define _TYPEDEF_SintPtr
typedef CriSintPtr				SintPtr;
#endif

#if !defined(_TYPEDEF_UintPtr)
#define _TYPEDEF_UintPtr
typedef CriUintPtr				UintPtr;
#endif

#endif	/* CRI_XPT_DISABLE_UNPREFIXED_TYPE */

#endif	/* CRI_XPT_POST_H */

/* end of file */
