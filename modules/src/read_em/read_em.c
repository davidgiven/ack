/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	Read_em: a module to read either compact or human readable EM code.
	Exported are the following routines:
	EM_open() :	has a parameter representing either a pointer to a
			filename or a null pointer, indicating that input is
			from standard input. This routine initializes for
			reading.
	EM_getinstr() :	Delivers the next EM instruction in a format
			defined in <em_comp.h>.
	Imported are:
	The constants COMPACT (either defined or not defined) and
	CHECKING (again either defined or not defined),
	some routines from the system module. and the em_code module
*/

#include <assert.h>
#include <system.h>
#include <em_label.h>
#include <em_arith.h>
#include <em_pseu.h>
#include <em_spec.h>
#include "em_ptyp.h"
#include "em_comp.h"
#include <em_flag.h>
#include <em_mes.h>

/* Buffered input */

#define getbyte()	(*_ich ? (*_ich++ & 0377) : _fill())
#define ungetbyte(uch)	((uch) >= 0 && (*--_ich = (uch)))
#define init_input()	(_fill(), _ich--)

#define EOF	-1

static File *fd;
static char *_ich;

PRIVATE int
_fill()
{
	static char text[BUFSIZ + 1];
	static int sz;

	if (_ich && _ich < &text[sz]) return _ich++, '\0';
	_ich = text;
	if (sys_read(fd, text, BUFSIZ, &sz) &&
	    sz > 0
	   ) {
		text[sz] = '\0';
		return (*_ich++&0377);
	}
	else {
		sz = 0;
		text[0] = 0;
		return EOF;
	}
}

#define NARGS	3		/* Maximum number of arguments */
#define STRSIZ	256		/* Maximum length of strings */

static struct e_instr emhead;	/* Where we put the head */
static struct e_args emargs[NARGS+2];	/* Where we put the arguments.
					   We need some more because some
					   arguments are constructed
					*/
static struct e_args *i_emargs;
#define argentry()	(i_emargs++)
static struct string {
	int length;
	char str[STRSIZ + 1];
} strings[NARGS];		/* Room for strings */
static struct string *i_strings;		/* Index of last one used */
#define stringentry()	(i_strings++)

static struct e_args *argp;	/* Indicates arguments yet to be
				   delivered
				*/
#ifdef COMPACT
static arith strleft;		/* count # of chars left to read
				   in a string
				*/
#endif

static int state;		/* What state are we in? */
#define CON	01		/* Reading a CON */
#define ROM	02		/* Reading a ROM */
#define MES	03		/* Reading a MES */
#define PSEUMASK 03
#define INSTRING 010		/* Reading a string */

static int EM_initialized;	/* EM_open called? */

static long wordmask[] = {	/* allowed bits in a word */
	0x00000000,
	0x000000FF,
	0x0000FFFF,
	0x00000000,
	0xFFFFFFFF
};

static int wsize, psize;	/* word size and pointer size */
int EM_wordsize, EM_pointersize;

#ifdef CHECKING
static char *argrange = "Argument range error";
#define check(expr) (expr || !EM_error || (EM_error = argrange))
#else not CHECKING
#define check(x)	/* nothing */
#endif CHECKING

/* Error handling
*/

PRIVATE
xerror(s)
	char *s;
{
	if (emhead.em_type != EM_FATAL) emhead.em_type = EM_ERROR;
	if (!EM_error) EM_error = s;
}

#ifdef COMPACT
PRIVATE
xfatal(s)
	char *s;
{
	emhead.em_type = EM_FATAL;
	if (!EM_error) EM_error = s;
}

#include "readk.c"
#else not COMPACT
#include "reade.c"
#endif COMPACT

/* EM_open: Open input file, get magic word if COMPACT.
*/
EXPORT int
EM_open(filename)
	char *filename;
{
	if (EM_initialized) {
		EM_error = "EM_open already called";
		return 0;
	}
	
	if (filename) {
		if (!sys_open(filename, OP_READ, &fd)) {
			EM_error = "Could not open input file";
			return 0;
		}
	}
	else	fd = STDIN;
	EM_filename = filename;
	init_input();

#ifdef COMPACT
	if (get16() != sp_magic) {
		EM_error = "Illegal magic word";
		return 0;
	}
#else not COMPACT
	inithash();	/* initialize hashtable */
#endif COMPACT

	EM_initialized = 1;
	return 1;
}

/* EM_close: Close input file
*/
EXPORT
EM_close()
{
	
	if (fd != STDIN) {
		sys_close(fd);
		fd = STDIN;
	}
	EM_initialized = 0;
}


/* startmes: handle the start of a message. The only important thing here
	is to get the wordsize and pointer size, and remember that they
	have already been read, not only to check that they are not specified
	again, but also to deliver the arguments on next calls to EM_getinstr.
	This is indicated by the variable "argp".
*/
PRIVATE
startmes(p)
	register struct e_instr *p;
{
	register struct e_args *ap;

	ap = getarg(cst_ptyp);
	p->em_arg = ap;
	state = MES;

	if (ap->em_cst == ms_emx) {
		if (wsize || psize) {
			if (!EM_error) EM_error = "Duplicate ms_emx";
		}
		argp = ap = getarg(cst_ptyp);
		wsize = ap->em_cst;
		EM_wordsize = ap->em_cst;
		ap->em_next = getarg(cst_ptyp);
		ap = ap->em_next;
		psize = ap->em_cst;
		EM_pointersize = ap->em_cst;
	}
}


/* EM_getinstr: read an "EM_line"
*/
EXPORT struct e_instr *
EM_getinstr()
{
	register struct e_instr *p = &emhead;
	register struct e_args *args;

	i_emargs = emargs;
	i_strings = strings;
	EM_error = 0;
#ifdef CHECKING
	if (!EM_initialized) {
		EM_error = "Initialization not done";
		p->em_type = EM_FATAL;
		return p;
	}
#endif CHECKING

	if (argp) {	/* We have some arguments left to deliver */
		args = argp;
		argp = args->em_next;
		p->em_type = EM_MESARG;
		p->em_arg = args;
		args->em_next = 0;
		return p;
	}

	if (!state) {		/* All clear, get a new line */
		p = gethead();
		if (!p) {	/* End of file */
			return p;
		}
		switch(p->em_type) {
		case EM_MNEM: {
			register int i,j;
			register struct e_args *ap;
			extern char em_flag[];
			extern short em_ptyp[];	

			p->em_args = 0;
			j = em_flag[p->em_opcode - sp_fmnem] & EM_PAR;
			i = em_ptyp[j];
			if (j == PAR_NO) {	/* No arguments */
				break;
			}
#ifndef COMPACT
			if (j == PAR_B) i = ptyp(sp_ilb2);
#endif COMPACT
			ap = getarg(i);
#ifndef COMPACT
			if (j == PAR_B) {
				ap->em_cst = ap->em_ilb;
				ap->em_argtype = cst_ptyp;
			}
#endif COMPACT
			/* range checking
			*/
#ifdef CHECKING
			if (wsize <= 4 && psize <= 4) switch(j) {
			case PAR_B:
				check(ap->em_cst <= 32767);
				/* Fall through */
			case PAR_N:
				check(ap->em_cst >= 0);
				break;
			case PAR_G:
				if (ap->em_argtype == cst_ptyp) {
					check(ap->em_cst >= 0);
				}
				/* Fall through */
			case PAR_F:
				/* ??? not in original em_decode or em_encode */
			case PAR_L:
			{	arith m = ap->em_cst >= 0 ? ap->em_cst :
							    - ap->em_cst;

				/* Check that the number fits in a pointer */
				check((m & ~wordmask[psize]) == 0);
				break;
			}
			case PAR_W:
				if (!ap) break;
				check((ap->em_cst & ~wordmask[wsize]) == 0);
				/* Fall through */
			case PAR_S:
				check(ap->em_cst > 0);
				/* Fall through */
			case PAR_Z:
				check(ap->em_cst >= 0 &&
				      ap->em_cst % wsize == 0);
				break;
			case PAR_O:
				check(ap->em_cst > 0 &&
		      		      ( ap->em_cst % wsize == 0 ||
					wsize % ap->em_cst == 0));
				break;
			case PAR_R:
				check(ap->em_cst >= 0 && ap->em_cst <= 2);
				break;
			}
#endif CHECKING
			p->em_args = ap;
#ifndef COMPACT
			checkeol();
#endif COMPACT
			}
			break;
		case EM_PSEU:
			/* handle a pseudo, read possible arguments. CON's and
			   ROM's are handled especially: Only one argument is
			   read, and a mark is set that an argument list of
			   type ROM or CON is in process
			*/
			{
			register struct e_args *ap = 0, *ap1;

			switch(p->em_opcode) {
			case ps_bss:
			case ps_hol:
				ap = getarg(cst_ptyp);
				ap->em_next = ap1 = getarg(par_ptyp);
				ap->em_next->em_next = ap1 = getarg(cst_ptyp);
#ifdef CHECKING
				/* Check that the last value is 0 or 1
				*/
				if (ap1->em_cst != 1 && ap1->em_cst != 0) {
				  if (! EM_error)
				   EM_error="Third argument of hol/bss not 0/1";
				}
#endif CHECKING
				break;
			case ps_exa:
			case ps_ina:
				ap = getarg(lab_ptyp);
				break;
			case ps_exp:
			case ps_inp:
				ap = getarg(pro_ptyp);
				break;
			case ps_exc:
				ap = getarg(cst_ptyp);
				ap->em_next = getarg(cst_ptyp);
				break;
			case ps_pro:
				ap = getarg(pro_ptyp);
				ap->em_next = getarg(cst_ptyp|ptyp(sp_cend));
				break;
			case ps_end:
				ap = getarg(cst_ptyp|ptyp(sp_cend));
				break;
			case ps_con:
				ap = getarg(val_ptyp);
				state |= CON;
				break;
			case ps_rom:
				ap = getarg(val_ptyp);
				state |= ROM;
				break;
			default:
				xerror("Bad pseudo");
				break;
			}
			p->em_args = ap;
			}
#ifndef COMPACT
			if (p->em_opcode != ps_con && p->em_opcode != ps_rom) {
				checkeol();
			}
#endif COMPACT
			break;
		case EM_STARTMES:
			startmes(p);
			break;
		}
		if (!wsize && !EM_error) {
			wsize = 2;
			psize = 2;
			EM_error = "EM code should start with mes 2";
		}
		return p;
	}

	if (state & INSTRING) {	/* We already delivered part of a string.
				   Deliver the next part
				*/
		register struct string *s;
		
		s = getstring(0);
		args = argentry();
		args->em_next = 0;
		args->em_argtype = str_ptyp;
		args->em_str = s->str;
		args->em_size = s->length;
		switch(state & PSEUMASK) {
		default:
			assert(0);
		case MES:
			if (!EM_error)
				EM_error = "String too long in message";
			p->em_type = EM_MESARG;
			p->em_arg = args;
			break;
		case CON:
			p->em_type = EM_PSEU;
			p->em_opcode = ps_con;
			p->em_args = args;
			break;
		case ROM:
			p->em_type = EM_PSEU;
			p->em_opcode = ps_rom;
			p->em_args = args;
			break;
		}
		return p;
	}

	/* Here, we are in a state reading arguments */
	args = getarg(any_ptyp);
	if (EM_error && p->em_type != EM_FATAL) {
		return p;
	}
	if (!args) {	/* No more arguments */
#ifndef COMPACT
		checkeol();
#endif
		if (state == MES) {
			state = 0;
			p->em_type = EM_ENDMES;
			return p;
		}
		/* Here, we have to get the next instruction */
		state = 0;
		return EM_getinstr();
	}

	/* Here, there was an argument */
	if (state == MES) {
		p->em_type = EM_MESARG;
		p->em_arg = args;
		return p;
	}
	p->em_type = EM_PSEU;
	p->em_args = args;
	if (state == CON) p->em_opcode = ps_con;
	else	p->em_opcode = ps_rom;
	return p;
}
