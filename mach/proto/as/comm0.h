/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm0.h	1.8 */
/*
 * All preprocessor based options/constants/functions
 */

#ifdef _include
_include	<ctype.h>
_include	<signal.h>
_include	<stdint.h>
_include	<stdio.h>
_include	<stdlib.h>
_include	<string.h>
#else
#include	<ctype.h>
#include	<signal.h>
#include	<stdint.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include    <unistd.h>
#endif

/* ========== ON/OFF options (use #define in mach0.c) ========== */

/*
 * The following options are available, to be set/removed in "mach0.c":
 *	THREE_PASS:	three passes needed for branch optimization
 *	BYTES_REVERSED:	lowest target address for word high-order byte
 *	WORDS_REVERSED:	lowest target address for long high-order byte
 *	LISTING:	include listing facilities
 *	RELOCATION:	relocatable code generated
 *	DEBUG:		for debugging purposes only
 *	TMPDIR:		directory for temporary files
 *	ASLD:		combined assembler/linker
 */

/* ========== constants (use #undef, #define in mach0.c) ========== */

/* table sizes */
#define	STRINGMAX	200	/* <= 256 */
#define	SECTMAX		64
#define	NAMEMAX		80
#define	MEMINCR		2048

/* Some character constants for parsing */
#define	ASC_LPAR	'['
#define	ASC_RPAR	']'
#define	ASC_SQUO	'\''
#define	ASC_DQUO	'"'
#define	ASC_COMM	'!'
#define	ISALPHA(c)	(isalpha(c) || (c) == '_' || (c) == '.')
#define	ISALNUM(c)	(isalnum(c) || (c) == '_')

#define	GENLAB		"I"		/* compiler generated labels */

#define	valu_t		int64_t		/* type of expression values */
#define	uvalu_t		uint64_t	/* unsigned valu_t */
#define	ADDR_T		unsigned short	/* type of dot */
#define	word_t		short		/* type of keyword value */
/*
 * NOTE: word_t is introduced to reduce the tokenfile size for machines
 * with large expressions but smaller opcodes (68000)
 */

#define	ALIGNWORD	1
#define	ALIGNSECT	1

#define	machstart(x)	/* nothing */
#define	machfinish(x)	/* nothing */

#define	SETBASE(sp)	((long)(sp)->s_base)

#define	VALWIDTH	4

/* ========== Machine dependent option/constant settings ========== */

#include	"mach0.c"

/* ========== default option setting ========== */

#ifndef ASLD
#ifndef RELOCATION
separate linker only possible if relocation info produced
#endif /* RELOCATION */
#endif /* ASLD */

#ifndef DEBUG
#define	DEBUG	1
#endif

/* ========== Machine independent type declarations ========== */

#ifdef _include
#ifdef ASLD
_include	"arch.h"
#endif
_include	"out.h"
#else
#ifdef ASLD
#include	"arch.h"
#endif
#include	"out.h"
#endif

/*
 * Define assert().  Disable assertions if DEBUG == 0.
 */
#if DEBUG == 0 && !defined(NDEBUG)
#define NDEBUG
#endif
#ifdef _include
_include <assert.h>
#else
#include <assert.h>
#endif

#define	CTRL(x) 	((x) & 037)

#define	lowb(z)		((int)(z) & 0xFF)
#define	loww(z)		((int)(z) & 0xFFFF)

#define fitx(x, d)  ((((x) + (1<<(d-1))) & ~((int)(1<<(d))-1)) == 0)
#define	fitb(x)		((((x) + 0x80) & ~((int)0xFF)) == 0)
#define	fitw(x)		((((x) + 0x8000L) & ~0xFFFFL) == 0)

#define	fit(x)		if (!(x)) nofit()

#define	PASS_1		0
#define	PASS_2		1
#define	PASS_3		2

#ifdef THREE_PASS
#define	PASS_SYMB	(pass != PASS_1)
#define	PASS_RELO	(pass != PASS_1)
#else
#define	PASS_SYMB	1
#define	PASS_RELO	1
#endif /* THREE_PASS */

#ifdef ASLD
#define	RELOMOVE(a,b)	/* empty */
#else
#define	RELOMOVE(a,b)	{a = b; b = 0;}
#endif

/* symbol table management */
#define	H_SIZE		307		/* hash size, must be od */
#define	H_KEY		(0*H_SIZE)	/* key symbol headers */
#define	H_LOCAL		(1*H_SIZE)	/* module symbol headers */
#ifdef ASLD
#define	H_GLOBAL	(2*H_SIZE)	/* external symbol headers */
#define	H_TOTAL		(3*H_SIZE)
#else
#define	H_TOTAL		(2*H_SIZE)
#endif

/* numeric label table management */
#define	FB_SIZE		10
#define	FB_HEAD		(0*FB_SIZE)
#define	FB_TAIL		(1*FB_SIZE)
#define	FB_BACK		(2*FB_SIZE)
#define	FB_FORW		(3*FB_SIZE)

/* miscellaneous */
#define	KEYDEFINE	0
#define KEYSECT		12
#define	DOTGAIN		DOTSCT->s_gain

/* ========== type declarations ========== */

struct expr_t {
	short	typ;
	valu_t	val;
};

typedef	struct expr_t	expr_t;

struct item_t {
	struct item_t *
		i_next;	/* linked lists with same hash value */
	short	i_type;
	/*
	 * the i_type field is used for two different purposes:
	 *	- the token type for keywords, returned by yylex()
	 *	- the symbol type for IDENT and FBSYM tokens
	 */
	valu_t	i_valu;		/* symbol value */
	char	*i_name;	/* symbol name */
};

struct common_t {
	struct common_t *
		c_next;
	struct item_t *c_it;
#ifndef ASLD
	valu_t	c_size;
#endif
};

typedef struct common_t	common_t;

typedef	struct item_t	item_t;

struct sect_t {
	short	s_flag;		/* some flag bits */
	ADDR_T	s_base;		/* section base */
	ADDR_T	s_size;		/* section size */
	ADDR_T	s_comm;		/* length of commons */
	ADDR_T	s_zero;		/* delayed emit1(0) */
	ADDR_T	s_lign;		/* section alignment */
	long	s_foff;		/* section file offset */
	item_t	*s_item;	/* points to section name */
#ifdef THREE_PASS
	ADDR_T	s_gain;		/* gain in PASS_2 */
#endif
};

typedef	struct sect_t	sect_t;

/* ========== flag field bits ========== */

/* s_flag bits: */
#define	BASED		1	/* at fixed position */

/* sflag bits: */
#define	SYM_EXT		001	/* external symbols */
#define	SYM_LOC		002	/* local symbols */
#define	SYM_LAB		004	/* local, compiler-generated labels */
#define	SYM_SMB		010	/* .symb symbols */
#define	SYM_LIN		020	/* .line and .file */
#define	SYM_SCT		040	/* section names */
#define	SYM_DEF		073	/* default value */

/*
 * extra type bits out of S_ETC, internal use only
 * S_VAR:
 *  - type not known at end of PASS_1 (S_VAR|S_UND)
 *  - value not known at end of PASS_2 (S_VAR|S_ABS)
 * S_DOT:
 *  - dot expression
 */
#define	S_VAR		0x0200
#define	S_DOT		0x0400
/* should be tested by preprocessor
 * due to error in preprocessor it cannot
 * test performed at compiletime by a switch now
 * #if (S_ETC|S_COM|S_VAR|S_DOT) != S_ETC
 * incorrect type bits
 * #endif
 */

/* parts of the a.out file */
#define	PARTEMIT	0
#define	PARTRELO	1
#define	PARTNAME	2
#define	PARTCHAR	3
#define	PARTS		4

#ifdef BYTES_REVERSED
#ifdef WORDS_REVERSED
#define MACHREL_BWR	(RELBR|RELWR)
#else
#define	MACHREL_BWR	(RELBR)
#endif
#else
#ifdef WORDS_REVERSED
#define	MACHREL_BWR	(RELWR)
#else
#define	MACHREL_BWR	(0)
#endif
#endif
