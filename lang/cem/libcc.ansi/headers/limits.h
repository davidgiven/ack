/*
 * limits.h - implementation limits
 */
/* $Header$ */

#if	!defined(__LIMITS_HEADER__)
#define	__LIMITS_HEADER__

#define	CHAR_BIT		8
#define	SCHAR_MIN		(-128)
#define	SCHAR_MAX		(+127)
#define	UCHAR_MAX		255
#define	MB_LEN_MAX		1

#define	SHRT_MIN		(-32767 - 1)
#define	SHRT_MAX		(+32767)
#define	USHRT_MAX		65535U

#define	LONG_MIN		(-2147483647L - 1L)
#define	LONG_MAX		(+2147483647L)
#define	ULONG_MAX		4294967295U

/* Assume signed characters (yegh) */
#define	CHAR_MAX		SCHAR_MAX
#define	CHAR_MIN		SCHAR_MIN

#if	_EM_WSIZE == 4
#define	INT_MIN			LONG_MIN
#define	INT_MAX			LONG_MAX
#define	UINT_MAX		ULONG_MAX
#else	/* _EM_WSIZE == 2 */
#define	INT_MIN			SHRT_MIN
#define	INT_MAX			SHRT_MAX
#define	UINT_MAX		USHRT_MAX
#endif

#endif	/* __LIMITS_HEADER__ */
