/* $Id$ */
/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* debugger symbol table types, as in DBX */

#define	N_GSYM	0x20	/* global symbol: name,,???,address */
#define	N_FUN	0x24	/* routine: name,,???,address */
#define	N_STSYM	0x26	/* static data symbol: name,,???,address */
#define	N_LCSYM	0x28	/* static bss symbol: name,,???,address */
#define N_MAIN  0x2a	/* name of main routine: name,,0,0 */
#define	N_RSYM	0x40	/* register symbol: name,,???,register */
#define	N_SLINE	0x44	/* source line: 0,linenumber,address */
#define	N_SSYM	0x60	/* structure element: name,,???,offset */
#define	N_SO	0x64	/* source file name: name,,0,address */
#define	N_LSYM	0x80	/* local symbol: name,,???,offset from LB */
#define	N_BINCL 0x82	/* header file: name,,0,0 */
#define	N_SOL	0x84	/* #included file name: name,,0,??? */
#define	N_PSYM	0xa0	/* parameter: name,,???,offset from AB */
#define N_EINCL 0xa2	/* end of include file: name,,0,0 */
#define	N_LBRAC	0xc0	/* left bracket: 0,nesting level,address */
#define	N_RBRAC	0xe0	/* right bracket: 0,nesting level,address */
#define N_SCOPE	0xc4	/* scope information */
