/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	This file essentially describes how the mallink info block
	is implemented.
*/

#define	MIN_SIZE	(1<<LOG_MIN_SIZE)
#define	MAX_FLIST	(LOG_MAX_SIZE - LOG_MIN_SIZE)
#if ALIGNMENT != 4 && ALIGNMENT != 8 && ALIGNMENT != 16
#error ALIGNMENT must be 4, 8 or 16
#elif ALIGNMENT % _EM_LSIZE
/* since calloc() does it's initialization in longs */
#error ALIGNMENT must be a multiple of the long size
#endif
#define align(n)	(((n) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

union _inf {
	union _inf *ptr;
	size_type ui;
};

typedef union _inf mallink;
#define	MAL_NULL	((mallink *)0)

/*	Access macros; only these macros know where to find values.
	They are also lvalues.
*/
#ifndef	NON_STANDARD
#define	OFF_SET	0
#else	/* def NON_STANDARD */
#define	OFF_SET	2
#endif	/* NON_STANDARD */

#define	_log_prev_of(ml)	((ml)[-1+OFF_SET]).ptr
#define	_log_next_of(ml)	((ml)[-2+OFF_SET]).ptr
#define	_phys_prev_of(ml)	((ml)[-3+OFF_SET]).ptr
#define	_this_size_of(ml)	((ml)[-4+OFF_SET]).ui
#ifndef	CHECK
#define	N_WORDS			4
#else	/* ifdef	CHECK */
#define	_checksum_of(ml)	((ml)[-5+OFF_SET]).ui
#define	_print_of(ml)		((ml)[-6+OFF_SET]).ui
#define	_mark_of(ml)		((ml)[-7+OFF_SET]).ui
#define	N_WORDS			7
#endif	/* CHECK */

#define	mallink_size()		(size_t) \
	align((N_WORDS - OFF_SET) * sizeof (mallink))

#ifdef	CHECK
#define	set_mark(ml,e)		(_mark_of(ml) = (e))
#define	mark_of(ml)		(_mark_of(ml))

#define	set_checksum(ml,e)	(_checksum_of(ml) = (e))
#define	checksum_of(ml)		(_checksum_of(ml))
#endif	/* CHECK */

#define new_mallink(ml)		( _log_prev_of(ml) = 0, \
				  _log_next_of(ml) = 0, \
				  _phys_prev_of(ml) = 0, \
				  _this_size_of(ml) = 0 )

#define	block_of_mallink(ml)	((void *)ml)
#define	mallink_of_block(addr)	((mallink *)addr)

#define	public	extern
#define	publicdata	extern
#ifndef	EXTERN
#define	private	static
#define	privatedata	static
#else	/* def	EXTERN */
#define	private	extern
#define	privatedata
#endif	/* EXTERN */

#ifdef	ASSERT
public m_assert(const char *fn, int ln);
#define	assert(b)		(!(b) ? m_assert(__FILE__, __LINE__) : 0)
#else	/* ndef	ASSERT */
#define	assert(b)		0
#endif	/* ASSERT */
