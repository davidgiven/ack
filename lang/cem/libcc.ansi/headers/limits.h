/*
 * limits.h - implementation limits
 */
/* $Header$ */

#ifndef	_LIMITS_HEADER_
#define	_LIMITS_HEADER_

/*
** Define _SIGNED_CHARS_ for machines with signed characters.
** Define _WORD_32_ for machines with 32 bits integers.
**
** These defines should probably be set by the compiler when the
** -vi option is used.
*/
#define	_SIGNED_CHARS_
#define	_WORD_32_

#define	CHAR_BIT		8
#define	SCHAR_MIN		(-127)
#define	SCHAR_MAX		(+127)
#define	UCHAR_MAX		255
#define	MB_LEN_MAX		1

#define	SHRT_MIN		(-32767)
#define	SHRT_MAX		(+32767)
#define	USHRT_MAX		65535

#define	LONG_MIN		(-2147483647L)
#define	LONG_MAX		(+2147483647L)
#define	ULONG_MAX		4294967295L

#ifdef	_SIGNED_CHARS_
#define	CHAR_MAX		SCHAR_MAX
#define	CHAR_MIN		SCHAR_MIN
#else	/* defined(_UNSIGNED_CHARS_) */
#define	CHAR_MAX		UCHAR_MAX
#define	CHAR_MIN		0
#endif	/* _SIGNED_CHARS */

#ifdef	_WORD_32_
#define	INT_MIN			LONG_MIN
#define	INT_MAX			LONG_MAX
#define	UINT_MAX		ULONG_MAX
#else	/*defined(_WORD_16_)*/
#define	INT_MIN			SHRT_MIN
#define	INT_MAX			SHRT_MAX
#define	UINT_MAX		USHRT_MAX
#endif	/* WORD_32_ */

#endif	/* _LIMITS_HEADER_ */
