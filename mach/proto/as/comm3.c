/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm3.c	1.1 */
/*
 * storage allocation for variables
 */

#include	"comm0.h"

#define	extern	/* empty, to force storage allocation */

#include	"comm1.h"

#undef extern

struct outhead	outhead = {
	O_MAGIC, O_STAMP, 0
};

#include	"y.tab.h"

item_t	keytab[] = {
	{0,	EXTERN,		0,		".define"},
	{0,	EXTERN,		0,		".extern"},
	{0,	DOT,		0,		"."},
	{0,	DATA,		RELO1,	".data1"},
	{0,	DATA,		RELO2,	".data2"},
	{0, DATA,  		RELO3,  ".data3"},
	{0,	DATA,		RELO4,	".data4"},
	{0,	DATA8,		0,	".data8"},
	{0,  DATAF,      4,      ".dataf4"},
	{0,  DATAF,      8,      ".dataf8"},
	{0,	ASCII,		0,		".ascii"},
	{0,	ASCII,		1,		".asciz"},
	{0,	ALIGN,		0,		".align"},
	{0,	ASSERT,		0,		".assert"},
	{0,	SPACE,		0,		".space"},
	{0,	SEEK,		0,		".seek"},
	{0,	COMMON,		0,		".comm"},
	{0,	SECTION,	0,		".sect"},
	{0,	BASE,		0,		".base"},
	{0,	SYMB,		0,		".symb"},
	{0,	SYMD,		0,		".symd"},
	{0,	LINE,		0,		".line"},
	{0,	FILe,		0,		".file"},
#ifdef LISTING
	{0,	LIST,		0,		".nolist"},
	{0,	LIST,		1,		".list"},
#endif
#include	"mach3.c"
	{0,	0,		0,		0}
};
