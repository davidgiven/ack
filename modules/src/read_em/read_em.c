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

#define STRSIZ	256		/* Maximum length of strings */

static struct e_instr *emhead;	/* Where we put the head */
static struct e_instr aheads[3];
static int ahead;

static struct string {
	int length;
	char str[STRSIZ + 1];
} string;

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

#ifdef CHECKING
static char *argrange = "Argument range error";
#define check(expr) (expr || EM_error || (EM_error = argrange))
#else not CHECKING
#define check(x)	/* nothing */
#endif CHECKING

/* Error handling
*/

PRIVATE
xerror(s)
	char *s;
{
	if (emhead->em_type != EM_FATAL) emhead->em_type = EM_ERROR;
	if (!EM_error) EM_error = s;
}

#ifdef COMPACT
PRIVATE
xfatal(s)
	char *s;
{
	emhead->em_type = EM_FATAL;
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

	getarg(cst_ptyp, &(p->em_arg));
	state = MES;

	if (p->em_cst == ms_emx) {
		if (wsize || psize) {
			if (!EM_error) EM_error = "Duplicate ms_emx";
		}
		p = &aheads[ahead++];
		getarg(cst_ptyp, &(p->em_arg));
		wsize = p->em_cst;
		EM_wordsize = p->em_cst;
		p->em_type = EM_MESARG;
		p = &aheads[ahead++];
		getarg(cst_ptyp, &(p->em_arg));
		psize = p->em_cst;
		EM_pointersize = p->em_cst;
		p->em_type = EM_MESARG;
	}
}


/* EM_getinstr: read an "EM_line"
*/
EXPORT int
EM_getinstr(p)
	register struct e_instr *p;
{

	EM_error = 0;
	if (ahead) {
		*p = aheads[--ahead];
		return 1;
	}
	emhead = p;
#ifdef CHECKING
	if (!EM_initialized) {
		EM_error = "Initialization not done";
		p->em_type = EM_FATAL;
		return 0;
	}
#endif CHECKING

	if (!state) {		/* All clear, get a new line */
		gethead(p);
		switch(p->em_type) {
		case EM_EOF:
			return EM_error == 0;
		case EM_MNEM: {
			register int i,j;
			extern char em_flag[];
			extern short em_ptyp[];	

			j = em_flag[p->em_opcode - sp_fmnem] & EM_PAR;
			i = em_ptyp[j];
			if (j == PAR_NO) {	/* No arguments */
				p->em_argtype = 0;
				break;
			}
#ifndef COMPACT
			if (j == PAR_B) i = ptyp(sp_ilb2);
#endif COMPACT
			getarg(i, &(p->em_arg));
#ifndef COMPACT
			if (j == PAR_B) {
				p->em_cst = p->em_ilb;
				p->em_argtype = cst_ptyp;
			}
#endif COMPACT
			/* range checking
			*/
#ifdef CHECKING
			if (wsize <= 4 && psize <= 4) switch(j) {
			case PAR_B:
				check(p->em_cst <= 32767);
				/* Fall through */
			case PAR_N:
				check(p->em_cst >= 0);
				break;
			case PAR_G:
				if (p->em_argtype == cst_ptyp) {
					check(p->em_cst >= 0);
				}
				/* Fall through */
			case PAR_F:
				/* ??? not in original em_decode or em_encode */
			case PAR_L:
			{	arith m = p->em_cst >= 0 ? p->em_cst :
							    - p->em_cst;

				/* Check that the number fits in a pointer */
				check((m & ~wordmask[psize]) == 0);
				break;
			}
			case PAR_W:
				if (p->em_argtype == 0) {
					p->em_cst = 0;
					break;
				}
				check((p->em_cst & ~wordmask[wsize]) == 0);
				/* Fall through */
			case PAR_S:
				check(p->em_cst > 0);
				/* Fall through */
			case PAR_Z:
				check(p->em_cst >= 0 &&
				      p->em_cst % wsize == 0);
				break;
			case PAR_O:
				check(p->em_cst > 0 &&
		      		      ( p->em_cst % wsize == 0 ||
					wsize % p->em_cst == 0));
				break;
			case PAR_R:
				check(p->em_cst >= 0 && p->em_cst <= 2);
				break;
			}
#endif CHECKING
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
			struct e_arg dummy;

			switch(p->em_opcode) {
			case ps_bss:
			case ps_hol:
				getarg(cst_ptyp, &dummy);
				EM_holsize = dummy.ema_cst;
				getarg(par_ptyp, &(p->em_arg));
				getarg(cst_ptyp, &dummy);
				EM_holinit = dummy.ema_cst;
#ifdef CHECKING
				/* Check that the last value is 0 or 1
				*/
				if (EM_holinit != 1 && EM_holinit != 0) {
				  if (! EM_error)
				   EM_error="Third argument of hol/bss not 0/1";
				}
#endif CHECKING
				break;
			case ps_exa:
			case ps_ina:
				getarg(lab_ptyp, &(p->em_arg));
				break;
			case ps_exp:
			case ps_inp:
				getarg(pro_ptyp, &(p->em_arg));
				break;
			case ps_exc:
				getarg(cst_ptyp, &dummy);
				p->em_exc1 = dummy.ema_cst;
				getarg(cst_ptyp, &dummy);
				p->em_exc2 = dummy.ema_cst;
				break;
			case ps_pro:
				getarg(pro_ptyp, &(p->em_arg));
				getarg(cst_ptyp|ptyp(sp_cend), &dummy);
				if (dummy.ema_argtype == 0) {
					p->em_nlocals = -1;
				}
				else	p->em_nlocals = dummy.ema_cst;
				break;
			case ps_end:
				getarg(cst_ptyp|ptyp(sp_cend), &(p->em_arg));
				break;
			case ps_con:
				getarg(val_ptyp, &(p->em_arg));
				state |= CON;
				break;
			case ps_rom:
				getarg(val_ptyp, &(p->em_arg));
				state |= ROM;
				break;
			default:
				xerror("Bad pseudo");
				break;
			}
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
		return EM_error == 0;
	}

	if (state & INSTRING) {	/* We already delivered part of a string.
				   Deliver the next part
				*/
		register struct string *s;
		
		s = getstring(0);
		p->em_argtype = str_ptyp;
		p->em_string = s->str;
		p->em_size = s->length;
		switch(state & PSEUMASK) {
		default:
			assert(0);
		case MES:
			if (!EM_error)
				EM_error = "String too long in message";
			p->em_type = EM_MESARG;
			break;
		case CON:
			p->em_type = EM_PSEU;
			p->em_opcode = ps_con;
			break;
		case ROM:
			p->em_type = EM_PSEU;
			p->em_opcode = ps_rom;
			break;
		}
		return EM_error == 0;
	}

	/* Here, we are in a state reading arguments */
	getarg(any_ptyp, &(p->em_arg));
	if (EM_error && p->em_type != EM_FATAL) {
		return 0;
	}
	if (p->em_argtype == 0) {	/* No more arguments */
#ifndef COMPACT
		checkeol();
#endif
		if (state == MES) {
			state = 0;
			p->em_type = EM_ENDMES;
			return EM_error == 0;
		}
		/* Here, we have to get the next instruction */
		state = 0;
		return EM_getinstr(p);
	}

	/* Here, there was an argument */
	if (state == MES) {
		p->em_type = EM_MESARG;
		return EM_error == 0;
	}
	p->em_type = EM_PSEU;
	if (state == CON) p->em_opcode = ps_con;
	else	p->em_opcode = ps_rom;
	return EM_error == 0;
}
