/* The <ansi.h> header checks whether the compiler claims conformance to ANSI
 * Standard C. If so, the symbol _ANSI is defined as 1, otherwise it is 
 * defined as 0.  Based on the result, a macro
 *
 *	_PROTOTYPE(function, params)
 *
 * is defined.  This macro expands in different ways, generating either
 * ANSI Standard C prototypes or old-style K&R (Kernighan & Ritchie) 
 * prototypes, as needed.  Finally, some programs use _CONST, _VOIDSTAR etc
 * in such a way that they are portable over both ANSI and K&R compilers.
 * The appropriate macros are defined here.
 */

#ifndef _ANSI_H
#define _ANSI_H

/* ANSI C requires __STDC__ to be defined as 1 for an ANSI C compiler.
 * Some half-ANSI compilers define it as 0.  Get around this here.
 */

#define _ANSI              0	/* 0 if compiler is not ANSI C, 1 if it is */

#ifdef __STDC__			/* __STDC__ defined for (near) ANSI compilers*/
#if __STDC__ == 1		/* __STDC__ == 1 for conformant compilers */
#undef _ANSI			/* get rid of above definition */
#define _ANSI              1	/* _ANSI = 1 for ANSI C compilers */
#endif
#endif

/* At this point, _ANSI has been set correctly to 0 or 1. Define the
 * _PROTOTYPE macro to either expand both of its arguments (ANSI prototypes),
 * only the function name (K&R prototypes).
 */

#if _ANSI
#define	_PROTOTYPE(function, params)	function params
#define	_VOIDSTAR	void *
#define	_VOID		void
#define	_CONST		const
#define	_VOLATILE	volatile
#define _SIZET		size_t

#else

#define	_PROTOTYPE(function, params)	function()
#define	_VOIDSTAR	void *
#define	_VOID		void
#define	_CONST
#define	_VOLATILE
#define _SIZET		int

#endif /* _ANSI */

#endif /* ANSI_H */
