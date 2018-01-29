/*------------------------------------------------------------------------
 Module:			mytypes.h
 Author:			linxueqin
 Project:			规约平台
 State: 			V1.0
 Creation Date: 	2012-10-8
 Description:		通用类型说明
------------------------------------------------------------------------*/

#ifndef _MYTYPES_H
#define _MYTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#if	!defined(EOF) || (EOF!=(-1))
#define EOF		(-1)
#endif

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
#endif

#if	!defined(EOS) || (EOS!='\0')
#define EOS		'\0'	
#endif

//#if (TYPE_OS == OS_NUCLEUS)
#if 1
#if	!defined(OK) || (OK!=0)
#define OK		0
#endif
#if	!defined(ERROR) || (ERROR!=(-1))
#define ERROR		(-1)
#endif
#ifndef null
#define null	0//((void *)0)
#endif
#ifndef BOOL
typedef bool   BOOL;
#endif
#endif /*(TYPE_OS == OS_NUCLEUS)*/


#if (defined __GNUC__)
#define MYALIGN __attribute__ ((packed))
#else
#define MYALIGN
#endif

//typedef  unsigned char   BOOL;
typedef  unsigned char   BYTE;
typedef  unsigned short  WORD;
typedef  unsigned long   DWORD;
typedef  unsigned int   UINT;

#ifdef __cplusplus
typedef  void (*ENTRYPTR)(...);     /* ptr to function returning void */
#else
typedef  void (*ENTRYPTR) ();	   /* ptr to function returning void */
#endif			/* _cplusplus */



#define STATUS   unsigned char
#define  LOBYTE(w)     ((BYTE)(w))
#define  HIBYTE(w)     ((BYTE)((WORD)(w) >> 8))
#define  LOWORD(l)     ((WORD)(l))
#define  HIWORD(l)     ((WORD)((DWORD)(l) >> 16))
#define  MAKEWORD(low,high)  ((WORD)((BYTE)(low)|(((WORD)((BYTE)(high)))<< 8)))
#define  MAKEDWORD(low,high) ((DWORD)(((WORD)(low))|(((DWORD)((WORD)(high)))<< 16)))
#define  ADJUSTWORD(x)   (MAKEWORD(HIBYTE(x),LOBYTE(x)))
#define  ADJUSTDWORD(x)  (MAKEDWORD(ADJUSTWORD(HIWORD(x)),ADJUSTWORD(LOWORD(x))))
#ifdef __cplusplus
}
#endif


#endif /*_MYTYPES_H*/
