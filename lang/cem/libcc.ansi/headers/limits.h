/*
 * limits.h - implementation limits
 */
/* $Id$ */

#if	!defined(_LIMITS_H)
#define	_LIMITS_H

#define	CHAR_BIT		8
#define	SCHAR_MIN		-128
#define	SCHAR_MAX		127
#define	UCHAR_MAX		255
#define	MB_LEN_MAX		1

#define	SHRT_MIN		(-32767 - 1)
#define	SHRT_MAX		32767
#define	USHRT_MAX		65535U

#define	LONG_MIN		(-2147483647L - 1L)
#define	LONG_MAX		2147483647L
#define	ULONG_MAX		4294967295UL

/* Assume signed characters (yegh) */
#define	CHAR_MAX		SCHAR_MAX
#define	CHAR_MIN		SCHAR_MIN

#if	_EM_WSIZE == 2
#define	INT_MIN			SHRT_MIN
#define	INT_MAX			SHRT_MAX
#define	UINT_MAX		65535U
#else	/* _EM_WSIZE == 4 */
#define	INT_MIN			(-2147483647 - 1)
#define	INT_MAX			2147483647
#define	UINT_MAX		4294967295U
#endif

#endif	/* _LIMITS_H */
