/* $Header$ */

/*	I/O part of em_code module.
	Also contains C_open, C_close
*/
#include <alloc.h>
#include <em_path.h>
#include <em_arith.h>
#include "insert.h"

int		C_ontmpfile = 0;
int		C_sequential = 1;
Part		*C_curr_part;
Part		*C_stable[TABSIZ];
char		*C_tmpdir = TMP_DIR;
int		(*C_outpart)(), (*C_swtout)(), (*C_swttmp)();

#ifdef INCORE
char		*C_BASE;
#endif

File		*C_ofp;

#ifndef INCORE
File		*C_tfr, *C_old_ofp;
char		*C_tmpfile;
char		*strcpy(), *strcat(), *mktemp();
char		*C_ibuf = 0;
long		C_current_out;
#endif

#if BUFSIZ <= 1024 && BIGMACHINE
#define BUFFERSIZ	8*BUFSIZ
#else
#define BUFFERSIZ	BUFSIZ
#endif

static char	obuf[BUFFERSIZ];
char		*C_top = &obuf[BUFFERSIZ];
char		*C_old_top;
char		*C_old_opp;
#ifdef INCORE
char		*C_current_out = obuf;
#else
char		*C_opp = obuf;
#endif

C_flush() {
#ifdef INCORE
	static unsigned int bufsiz;

	if (C_ontmpfile) {
		if (C_BASE == 0) {
			C_BASE = Malloc(BUFFERSIZ);
			bufsiz = BUFFERSIZ;
			C_current_out = C_BASE;
		}
		else {
			C_BASE = Srealloc(C_BASE, (bufsiz << 1));
			C_current_out = C_BASE + bufsiz;
			bufsiz <<= 1;
		}
		C_top = C_BASE + bufsiz;
		return;
	}
#endif
	if (C_opp != obuf && sys_write(C_ofp, obuf, C_opp - obuf) == 0) {
		C_failed();
	}
	C_opp = obuf;
}

#ifndef INCORE
#define Xputbyte(c) if (C_ontmpfile) C_current_out++; put(c)
#else
#define Xputbyte(c) put(c)
#endif

C_putbyte(c)
	int c;
{
	Xputbyte(c);
}

#define C_putbyte Xputbyte

C_init(w, p)
	arith w, p;
{
}

C_open(nm)
	char *nm;
{
	/*	Open file "nm" for output
	*/

	if (nm == 0)
		C_ofp = STDOUT;	/* standard output	*/
	else
	if (sys_open(nm, OP_WRITE, &C_ofp) == 0)
		return 0;
	return 1;
}

C_close()
{
	/*	Finish the code-generation.
	*/

#ifndef INCORE
	C_flush();
	if (C_tmpfile) {
		(*C_swttmp)();
		sys_close(C_ofp);
#else
	if (C_BASE) {
#endif
		if (C_curr_part) {
			C_curr_part->p_parts->pp_end = C_current_out - C_BASE;
		}
		(*C_swtout)();
		if (! C_sequential) {
			(*C_outpart)(0);
		}
#ifndef INCORE
		sys_close(C_tfr);
		sys_remove(C_tmpfile);
		if (C_ibuf) free(C_ibuf);
#else
		free(C_BASE);
#endif
	}
	C_flush();
	if (C_ofp != STDOUT)
		sys_close(C_ofp);
	C_ofp = 0;
}

C_busy()
{
	return C_ofp != 0; /* true if code is being generated */
}
