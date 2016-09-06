/*	$Id: table.c,v 1.4 2016/06/27 11:47:06 ragge Exp $	*/
/*
 * Copyright (c) 2006 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


# include "pass2.h"

# define ANYSIGNED TINT|TLONG|TSHORT|TCHAR
# define ANYUSIGNED TUNSIGNED|TULONG|TUSHORT|TUCHAR
# define ANYFIXED ANYSIGNED|ANYUSIGNED
# define ANYREG (INAREG|INBREG|INCREG)
# define TUWORD TUNSIGNED|TULONG
# define TSWORD TINT|TLONG
# define TWORD TUWORD|TSWORD

struct optab table[] = {
/* First entry must be an empty entry */
{ -1, FOREFF, SANY, TANY, SANY, TANY, 0, 0, "", },

/*
 * Conversions.
 */
/* uchar to (u)int */
{ SCONV,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TINT|TUNSIGNED,
		0,	RLEFT,
		"",	},

/*
 * All ASSIGN entries.
 */
/* reg->reg */
{ ASSIGN,	FOREFF|INAREG|INBREG,
	SAREG|SBREG,	TWORD|TPOINT,
	SAREG|SBREG,	TWORD|TPOINT,
		0,	RDEST,
		"	mov AR,AL\n", },
/* reg->mem */
{ ASSIGN,	FOREFF|INAREG|INBREG,
	SNAME|SOREG,	TWORD|TPOINT,
	SAREG|SBREG,	TWORD|TPOINT,
		0,	RDEST,
		"	sta AR,AL\n", },
/* mem->reg */
{ ASSIGN,	FOREFF|INAREG|INBREG,
	SAREG|SBREG,	TWORD|TPOINT,
	SNAME|SOREG,	TWORD|TPOINT,
		0,	RDEST,
		"	lda AL,AR\n", },

/*
 * LEAF type movements.
 */
/* 0 -> reg */
{ OPLTYPE,	INAREG|INBREG,
	SANY,	TANY,
	SZERO,	TWORD,
		NAREG,	RESC1,
		"	subo A1,A1\n", },

/* 1 -> reg */
{ OPLTYPE,	INAREG|INBREG,
	SANY,	TANY,
	SONE,	TWORD,
		NAREG|NBREG,	RESC1,
		"	subzl A1,A1\n", },

/* constant -> reg */
{ OPLTYPE,	INAREG|INBREG,
	SANY,	TANY,
	SCON,	TWORD|TPOINT,
		NAREG|NBREG,	RESC1,
		"	lda A1,AR\n", },

/* mem -> reg */
{ OPLTYPE,	INAREG,
	SANY,		TANY,
	SNAME|SOREG,	TWORD,
		NAREG,	RESC1,
		"	lda A1,AR\n", },

/* reg -> A-reg */
{ OPLTYPE,	INAREG,
	SANY,		TANY,
	SAREG|SBREG,	TWORD,
		NAREG,	RESC1,
		"	mov AR,A1\n", },

/* reg -> B-reg */
{ OPLTYPE,	INBREG,
	SANY,		TANY,
	SAREG|SBREG,	TWORD|TPOINT,
		NBREG,	RESC1,
		"	mov AR,A1\n", },

{ OPLTYPE,	INBREG,
	SANY,		TANY,
	SNAME|SOREG,	TPOINT,
		NBREG,	RESC1,
		"	lda A1,AR\n", },

{ OPLTYPE,	INBREG,
	SANY,		TANY,
	SLDFPSP,	TANY,
		NBREG,	RESC1,
		"	lda A1,AR\n", },

/*
 * Simple ops.
 */
{ PLUS,		INBREG|INAREG,
	SAREG|SBREG,	TWORD|TPOINT,
	SONE,		TANY,
		0,	RLEFT,
		"	inc AL,AL\n", },

{ OPSIMP,	INBREG|INAREG|FOREFF,
	SAREG|SBREG,	TWORD|TPOINT,
	SAREG|SBREG,	TWORD|TPOINT,
		0,	RLEFT,
		"	O AR,AL\n", },

/*
 * Indirections
 */
#ifdef nova4
{ UMUL,	INAREG,
	SANY,				TPTRTO|TCHAR|TUCHAR,
	SOREG|SAREG|SBREG|SNAME,	TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"	ldb A1,AL\n", },
#endif
{ UMUL,	INAREG,
	SANY,	TPTRTO|TUCHAR,
	SOREG,	TUCHAR,
		NSPECIAL|NAREG,	RESC1,
		"	jsr @lbyt\n", },

{ UMUL,	INAREG,
	SANY,	TPTRTO|TCHAR,
	SOREG,	TCHAR,
		NSPECIAL|NAREG,	RESC1,
		"	jsr @lsbyt\n", },

{ UMUL,	INAREG,
	SANY,	TPOINT|TWORD,
	SOREG,	TPOINT|TWORD,
		NAREG|NASL,	RESC1,
		"	lda A1,AL\n", },

{ UMUL,	INBREG,
	SANY,	TPOINT|TWORD,
	SOREG,	TPOINT|TWORD,
		NBREG|NBSL,	RESC1,
		"	lda A1,AL\n", },

/*
 * logops
 */
{ EQ,	FOREFF,
	SAREG|SBREG,	TANY,
	SZERO,		TANY,
		0,	RNOP,
		"	mov# AL,AL,snr\n	jmp LC\n", },

{ EQ,	FOREFF,
	SAREG|SBREG,	TANY,
	SAREG|SBREG,	TANY,
		0,	RNOP,
		"	sub# AL,AR,snr\n	jmp LC\n", },

{ NE,	FOREFF,
	SAREG|SBREG,	TANY,
	SZERO,		TANY,
		0,	RNOP,
		"	mov# AL,AL,szr\n	jmp LC\n", },

{ NE,	FOREFF,
	SAREG|SBREG,	TANY,
	SAREG|SBREG,	TANY,
		0,	RNOP,
		"	sub# AL,AR,szr\n	jmp LC\n", },

/*
 * Subroutine calls.
 */

{ CALL,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr @CL\n", },

{ UCALL,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr @CL\n", },

{ CALL,		INAREG|FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,
		"	jsr @CL\n", },

{ UCALL,	INAREG|FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,
		"	jsr @CL\n", },

{ GOTO,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jmp @LL\n", },

{ FUNARG,	FOREFF,
	SAREG|SBREG,	TCHAR|TUCHAR|TWORD|TPOINT,
	SANY,		TCHAR|TUCHAR|TWORD|TPOINT,
		NBREG,	RNULL,
		"	lda A1,csp\n	sta AL,ZB\n", },

# define DF(x) FORREW,SANY,TANY,SANY,TANY,REWRITE,x,""

{ UMUL, DF( UMUL ), },

{ ASSIGN, DF(ASSIGN), },

{ STASG, DF(STASG), },

{ FLD, DF(FLD), },

{ OPLEAF, DF(NAME), },

{ OPUNARY, DF(UMINUS), },

{ OPANY, DF(BITYPE), },

{ FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	"help; I'm in trouble\n" },
};

int tablesize = sizeof(table)/sizeof(table[0]);
